#ifndef QT_NEW_HANDLER
#define QT_NEW_HANDLER

#include <set>

#define DECLARE_QT_MEMORY_HANDLER                                 \
	\
public:                                                           \
	\
void* operator new(size_t s)                               \
	\
{                                                          \
		return ::operator new(s);                                 \
	\
}                                                          \
	\
\
void operator delete(void* ptr)                                   \
	\
{                                                          \
		auto findIt = wgt::getQtInPlaceNewCollection().find(ptr); \
		if (findIt != wgt::getQtInPlaceNewCollection().end())     \
		{                                                         \
			wgt::getQtInPlaceNewCollection().erase(findIt);       \
			if (wgt::getQtInPlaceNewCollection().empty())         \
			{                                                     \
				wgt::releaseQtInPlaceNewCollection();             \
			}                                                     \
			return;                                               \
		}                                                         \
		::operator delete(ptr);                                   \
	\
}                                                          \
	\
\
void* operator new(size_t s, void* at)                  \
	\
{                                                          \
		wgt::getQtInPlaceNewCollection().insert(at);              \
		return at;                                                \
	\
}                                                          \
	\
\
void operator delete(void* p, void*)                              \
	\
{                                                          \
		wgt::getQtInPlaceNewCollection().erase(p);                \
		if (wgt::getQtInPlaceNewCollection().empty())             \
		{                                                         \
			wgt::releaseQtInPlaceNewCollection();                 \
		}                                                         \
	\
};

namespace wgt
{
std::set<void*>& getQtInPlaceNewCollection();
void releaseQtInPlaceNewCollection();
} // end namespace wgt
#endif // QT_NEW_HANDLER
