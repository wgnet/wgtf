#ifndef QT_MODEL_MACROS_HPP
#define QT_MODEL_MACROS_HPP


/**
 *	Typical model event function declarations.
 */
#define EVENT_DECL( InterfaceName ) \
	void onPreItemDataChanged( const InterfaceName* sender, \
		const InterfaceName::PreItemDataChangedArgs& args ); \
	void onPostItemDataChanged( const InterfaceName* sender, \
		const InterfaceName::PostItemDataChangedArgs& args ); \
	void onPreItemsInserted( const InterfaceName* sender, \
		const InterfaceName::PreItemsInsertedArgs& args ); \
	void onPostItemsInserted( const InterfaceName* sender, \
		const InterfaceName::PostItemsInsertedArgs& args ); \
	void onPreItemsRemoved( const InterfaceName* sender, \
		const InterfaceName::PreItemsRemovedArgs& args ); \
	void onPostItemsRemoved( const InterfaceName* sender, \
		const InterfaceName::PostItemsRemovedArgs& args ); \
	void onDestructing( const InterfaceName* sender, \
		const InterfaceName::DestructingArgs& args ); \


/**
 *	Use this for data change events.
 *	E.g. onPreDataChanged/onPostDataChanged.
 */
#define EVENT_IMPL1( ClassName, InterfaceName, FunctionName, Action ) \
void ClassName::onPre##FunctionName( const InterfaceName* sender, \
	const InterfaceName::Pre##FunctionName##Args& args ) \
{ \
	assert( getModel() != nullptr ); \
	assert( sender == getModel() ); \
 \
	const auto pItem = args.item_; \
	if (pItem == nullptr) \
	{ \
		return; \
	} \
 \
	const int role = QtModelHelpers::encodeRole( \
		args.roleId_, impl_->extensions_ ); \
	if (role < Qt::UserRole) \
	{ \
		return; \
	} \
 \
	const int column = args.column_; \
	const QModelIndex parentIndex = \
		Impl::calculateParentIndex( *this, pItem, column ); \
	const auto value = QtHelpers::toQVariant( args.data_ ); \
	this->begin##Action( parentIndex, role, value ); \
} \
void ClassName::onPost##FunctionName( const InterfaceName* sender, \
	const InterfaceName::Post##FunctionName##Args& args ) \
{ \
	assert( getModel() != nullptr ); \
	assert( sender == getModel() ); \
 \
	const auto pItem = args.item_; \
	if (pItem == nullptr) \
	{ \
		return; \
	} \
 \
	const int role = QtModelHelpers::encodeRole( \
		args.roleId_, impl_->extensions_ ); \
	if (role < Qt::UserRole) \
	{ \
		return; \
	} \
 \
	const int column = args.column_; \
	const QModelIndex parentIndex = \
		Impl::calculateParentIndex( *this, pItem, column ); \
	const auto value = QtHelpers::toQVariant( args.data_ ); \
	this->end##Action( parentIndex, role, value ); \
}


/**
 *	Used for implementing row change events.
 *	E.g. onPreItemsInserted/onPostItemsInserted.
 */
#define EVENT_IMPL2( ClassName, InterfaceName, FunctionName, Action ) \
void ClassName::onPre##FunctionName( const InterfaceName* sender, \
	const InterfaceName::Pre##FunctionName##Args & args ) \
{ \
	assert( getModel() != nullptr ); \
	assert( sender == getModel() ); \
 \
	auto pParentItem = args.item_; \
	const int column = 0; \
	const QModelIndex parentIndex = \
		Impl::calculateParentIndex( *this, pParentItem, column ); \
	const int first = QtModelHelpers::calculateFirst( args.index_ ); \
	const int last = QtModelHelpers::calculateLast( args.index_, args.count_ ); \
	this->begin##Action( parentIndex, first, last );\
} \
void ClassName::onPost##FunctionName( const InterfaceName* sender, \
	const InterfaceName::Post##FunctionName##Args & args ) \
{ \
	assert( getModel() != nullptr ); \
	assert( sender == getModel() ); \
 \
	auto pParentItem = args.item_; \
	const int column = 0; \
	const QModelIndex parentIndex = \
		Impl::calculateParentIndex( *this, pParentItem, column ); \
	const int first = QtModelHelpers::calculateFirst( args.index_ ); \
	const int last = QtModelHelpers::calculateLast( args.index_, args.count_ ); \
	this->end##Action( parentIndex, first, last ); \
}


/**
 *	Function declarations for typical model signals.
 */
#define EMIT_DECL \
	void beginChangeData( \
		const QModelIndex &index, int role, const QVariant &value ); \
	void endChangeData( \
		const QModelIndex &index, int role, const QVariant &value ); \
	void beginInsertRows( const QModelIndex &parent, int first, int last ); \
	void endInsertRows( const QModelIndex &parent, int first, int last ); \
	void beginRemoveRows( const QModelIndex &parent, int first, int last ); \
	void endRemoveRows( const QModelIndex &parent, int first, int last ); \


/**
 *	Use this for emitting data change signals.
 *	E.g. beginChangeData/changeData.
 */
#define EMIT_IMPL1( ClassName, Noun1, Verb1, Noun2, Verb2 ) \
void ClassName::begin##Verb1##Noun1( const QModelIndex& index, \
	int role, \
	const QVariant& value ) \
{ \
	if (QThread::currentThread() == QCoreApplication::instance()->thread()) \
	{ \
		emit Noun2##AboutToBe##Verb2( index, role, value ); \
	} \
	else \
	{ \
		emit Noun2##AboutToBe##Verb2##Thread( \
			index, role, value, QPrivateSignal() ); \
	} \
} \
void ClassName::end##Verb1##Noun1( const QModelIndex &index, \
	int role, \
	const QVariant &value ) \
{ \
	if (QThread::currentThread() == QCoreApplication::instance()->thread()) \
	{ \
		emit Noun2##Verb2( index, role, value ); \
	} \
	else \
	{ \
		emit Noun2##Verb2##Thread( \
			index, role, value, QPrivateSignal() ); \
	} \
}


/**
 *	Use this for emitting row change signals.
 *	E.g. beginInsertRows/endInsertRows.
 */
#define EMIT_IMPL2( ClassName, BaseClassName, Noun1, Verb1, Noun2, Verb2 ) \
void ClassName::begin##Noun1##Verb1( const QModelIndex &parent, \
	int first, \
	int last ) \
{ \
	if (QThread::currentThread() == QCoreApplication::instance()->thread()) \
	{ \
		emit BaseClassName::begin##Noun1##Verb1( parent, first, last ); \
	} \
	else \
	{ \
		emit Noun2##AboutToBe##Verb2##Thread( \
			parent, first, last, QPrivateSignal() ); \
	} \
} \
void ClassName::end##Noun1##Verb1( const QModelIndex &parent, \
	int first, \
	int last ) \
{ \
	if (QThread::currentThread() == QCoreApplication::instance()->thread()) \
	{ \
		emit BaseClassName::end##Noun1##Verb1(); \
	} \
	else \
	{ \
		emit Noun2##Verb2##Thread( \
			parent, first, last, QPrivateSignal() ); \
	} \
}


#endif // QT_MODEL_MACROS_HPP
