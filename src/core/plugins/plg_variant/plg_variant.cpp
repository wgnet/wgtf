#include "core_generic_plugin/generic_plugin.hpp"
#include "core_variant/variant.hpp"
#include "core_variant/default_meta_type_manager.hpp"


namespace wgt
{
//==============================================================================
class TypeSystemHolder
{
public:
	//==========================================================================
	TypeSystemHolder()
		: typeManager_( new DefaultMetaTypeManager )
	{
	}


	//==========================================================================
	~TypeSystemHolder()
	{
		typeManager_.reset();
	}

	DefaultMetaTypeManager * getTypeManager()
	{
		return typeManager_.get();
	}

private:
	std::unique_ptr< DefaultMetaTypeManager > typeManager_;
};

//==============================================================================
class TypeSystemPlugin
	: public PluginMain
{
public:
	//==========================================================================
	TypeSystemPlugin( IComponentContext & contextManager )
		: typeSystemHolder_( new TypeSystemHolder )
	{ 
		types_.push_back(
			contextManager.registerInterface( typeSystemHolder_->getTypeManager(), false ) );
		Variant::setMetaTypeManager( typeSystemHolder_->getTypeManager() );
	}

	//==========================================================================
	void Unload( IComponentContext & contextManager ) override
	{
		for( auto type : types_ )
		{
			contextManager.deregisterInterface( type );
		}
	}
private:
	std::unique_ptr< TypeSystemHolder >	typeSystemHolder_;
	std::vector< IInterface * > types_;
};

PLG_CALLBACK_FUNC(TypeSystemPlugin)
} // end namespace wgt
