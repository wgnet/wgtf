
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//  qt_qlist_memory_fix.cpp
//
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  Copyright (c) Wargaming.net. All rights reserved.
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "qglobal.h"
#include "qlist.h"
#include "qvariant.h"

/************************************************************************/
/* This fix works around an issue where memory is allocated on a heap   */
/* in a plugin but deallocated on a different heap in Qt Core.          */
/* This specialization forces the node to be allocated on the same heap */
/* so when Qt deallocates it using the default heap it doesn't complain */
/*                                                                      */
/* A Qt bug for this issue can be found here but wont be fixed until v6 */
/*  https://bugreports.qt.io/browse/QTBUG-37395                         */
/* A proposed fix for this issue can be found here.                     */
/*  https://codereview.qt-project.org/#/c/80535/                        */
/*                                                                      */
/************************************************************************/

static_assert(QT_VERSION < QT_VERSION_CHECK(6, 0, 0), "Are QList<> specializations still needed this version of Qt?");

#define SPECIALIZE_QLIST(TNode)                                                                                   \
	template <>                                                                                                   \
	Q_INLINE_TEMPLATE void QList<TNode>::node_construct(Node* n, const TNode& t)                                  \
	\
{                                                                                                          \
		static_assert(QTypeInfo<TNode>::isLarge || QTypeInfo<TNode>::isStatic, "Invalid QList<> Specialization"); \
		void* memory = ::malloc(sizeof(TNode));                                                                   \
		n->v = new (memory) TNode(t);                                                                             \
	\
}                                                                                                          \
	\
template<>                                                                                                        \
	Q_INLINE_TEMPLATE void QList<TNode>::node_destruct(Node* n)                                                   \
	\
{                                                                                                          \
		static_assert(QTypeInfo<TNode>::isLarge || QTypeInfo<TNode>::isStatic, "Invalid QList<> Specialization"); \
		reinterpret_cast<TNode*>(n->v)->~TNode();                                                                 \
		::free(n->v);                                                                                             \
	\
}                                                                                                          \
	\
template<>                                                                                                        \
	Q_INLINE_TEMPLATE void QList<TNode>::node_copy(Node* from, Node* to, Node* src)                               \
	\
{                                                                                                          \
		Node* current = from;                                                                                     \
		static_assert(QTypeInfo<TNode>::isLarge || QTypeInfo<TNode>::isStatic, "Invalid QList<> Specialization"); \
		QT_TRY                                                                                                    \
		{                                                                                                         \
			while (current != to)                                                                                 \
			{                                                                                                     \
				void* memory = ::malloc(sizeof(TNode));                                                           \
				current->v = new (memory) TNode(*reinterpret_cast<TNode*>(src->v));                               \
				++current;                                                                                        \
				++src;                                                                                            \
			}                                                                                                     \
		}                                                                                                         \
		QT_CATCH(...)                                                                                             \
		{                                                                                                         \
			while (current-- != from)                                                                             \
			{                                                                                                     \
				reinterpret_cast<TNode*>(current->v)->~TNode();                                                   \
				::free(current->v);                                                                               \
			}                                                                                                     \
			QT_RETHROW;                                                                                           \
		}                                                                                                         \
	\
}                                                                                                          \
	\
template<>                                                                                                        \
	Q_INLINE_TEMPLATE void QList<TNode>::node_destruct(Node* from, Node* to)                                      \
	\
{                                                                                                          \
		static_assert(QTypeInfo<TNode>::isLarge || QTypeInfo<TNode>::isStatic, "Invalid QList<> Specialization"); \
		while (from != to)                                                                                        \
		{                                                                                                         \
			--to;                                                                                                 \
			reinterpret_cast<TNode*>(to->v)->~TNode();                                                            \
			::free(to->v);                                                                                        \
		}                                                                                                         \
	\
}
