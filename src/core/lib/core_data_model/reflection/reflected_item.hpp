#ifndef REFLECTED_ITEM_HPP
#define REFLECTED_ITEM_HPP

#include "core_data_model/generic_tree_item.hpp"
#include "core_reflection/reflected_object.hpp"
#include "core_variant/collection.hpp"

namespace wgt
{
class PropertyAccessor;
class IReflectionController;
class ObjectHandle;
class IDefinitionManager;
class IBaseProperty;
typedef std::shared_ptr< IBaseProperty > IBasePropertyPtr;

/**
 *	Base class for adding a reflected item to a tree.
 */
class ReflectedItem : public GenericTreeItem
{
public:
	ReflectedItem( ReflectedItem * parent, const char * path ) 
		: parent_( parent )
		, path_( path )
		, controller_( nullptr )
		, definitionManager_( nullptr ) {}
	ReflectedItem( ReflectedItem * parent, const std::string & path ) 
		: parent_( parent )
		, path_( path )
		, controller_( nullptr )
		, definitionManager_( nullptr ) {}
	virtual ~ReflectedItem() {}

	virtual const ObjectHandle & getRootObject() const = 0;
    virtual const ObjectHandle & getObject() const = 0;
	virtual const IClassDefinition * getDefinition() const;

	const std::string & getPath() const { return path_; }
	IReflectionController * getController() const;
	void setController( IReflectionController * controller );
	IDefinitionManager * getDefinitionManager() const;
	void setDefinitionManager( IDefinitionManager * definitionManager );

	// IItem
	ThumbnailData getThumbnail( int column ) const { return nullptr; }
	Variant getData( int column, size_t roleId ) const { return Variant(); }
	bool setData( int column, size_t roleId, const Variant & data ) { return false; }

	// GenericTreeItem
	GenericTreeItem * getParent() const { return parent_; }

	virtual bool preSetValue( const PropertyAccessor & accessor, const Variant & value ) = 0;
	virtual bool postSetValue( const PropertyAccessor & accessor, const Variant & value ) = 0;

protected:
	ReflectedItem * parent_;
	std::string path_;
	IReflectionController * controller_;
	IDefinitionManager * definitionManager_;

	typedef std::function<bool(IBasePropertyPtr, const std::string &)> PropertyCallback;
	bool EnumerateVisibleProperties(const PropertyCallback& callback) const;

private:
	static bool EnumerateVisibleProperties(ObjectHandle object, const IDefinitionManager & definitionManager, const std::string & inplacePath, const PropertyCallback& callback);
};
} // end namespace wgt
#endif //REFLECTED_ITEM_HPP
