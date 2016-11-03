#include "invoke_reflected_method_command.hpp"

#include "core_variant/variant.hpp"
#include "core_reflection/i_object_manager.hpp"
#include "core_command_system/i_command_manager.hpp"
#include "core_reflection/reflected_method_parameters.hpp"
#include "core_reflection/property_accessor.hpp"
#include "core_data_model/variant_list.hpp"
#include "core_reflection/interfaces/i_base_property.hpp"
#include "core_reflection/reflected_method.hpp"
#include "core_reflection/metadata/meta_utilities.hpp"
#include "core_reflection/metadata/meta_impl.hpp"
#include "core_reflection/base_property_with_metadata.hpp"
#include "core_reflection/generic/generic_object.hpp"

namespace wgt
{
struct ReflectedMethodCommandParameters::Implementation
{
	Implementation(
		ReflectedMethodCommandParameters& self,
		RefObjectId id = std::string(),
		std::string path = "",
		ReflectedMethodParameters parameters = ReflectedMethodParameters() )
		: self_( self )
		, id_( id )
		, path_( path )
		, parameters_( parameters )
	{}

	ReflectedMethodCommandParameters& self_;
	RefObjectId id_;
	std::string path_;
	ReflectedMethodParameters parameters_;

	static const char* s_IdName;
	static const char* s_PathName;
	static const char* s_ParametersName;
};


const char* ReflectedMethodCommandParameters::Implementation::s_IdName = "MethodContextId";
const char* ReflectedMethodCommandParameters::Implementation::s_PathName = "MethodPath";
const char* ReflectedMethodCommandParameters::Implementation::s_ParametersName = "MethodParameters";


const char* ReflectedMethodCommandParameters::idName()
{
	return Implementation::s_IdName;
}


const char* ReflectedMethodCommandParameters::pathName()
{
	return Implementation::s_PathName;
}


const char* ReflectedMethodCommandParameters::parametersName()
{
	return Implementation::s_ParametersName;
}


ReflectedMethodCommandParameters::ReflectedMethodCommandParameters()
	: impl_( new Implementation( *this ) )
{
}


ReflectedMethodCommandParameters::ReflectedMethodCommandParameters( const ReflectedMethodCommandParameters& rhs )
	: impl_( new Implementation( *this, rhs.impl_->id_, rhs.impl_->path_, rhs.impl_->parameters_ ) )
{
}


ReflectedMethodCommandParameters& ReflectedMethodCommandParameters::operator=(
	const ReflectedMethodCommandParameters& rhs )
{
	if (this != &rhs)
	{
		impl_->id_ = rhs.impl_->id_;
		impl_->path_ = rhs.impl_->path_;
		impl_->parameters_ = rhs.impl_->parameters_;
	}

	return *this;
}


ReflectedMethodCommandParameters::~ReflectedMethodCommandParameters()
{
}


const RefObjectId& ReflectedMethodCommandParameters::getId() const
{
	return impl_->id_;
}


const char* ReflectedMethodCommandParameters::getPath() const
{
	return impl_->path_.c_str();
}


const ReflectedMethodParameters& ReflectedMethodCommandParameters::getParameters() const
{
	return impl_->parameters_;
}

ReflectedMethodParameters& ReflectedMethodCommandParameters::getParametersRef() const
{
	return impl_->parameters_;
}

void ReflectedMethodCommandParameters::setId( const RefObjectId& id )
{
	impl_->id_ = id;
}


void ReflectedMethodCommandParameters::setPath( const char* path )
{
	impl_->path_ = path;
}


void ReflectedMethodCommandParameters::setParameters( const ReflectedMethodParameters& parameters )
{
	impl_->parameters_ = parameters;
}


struct InvokeReflectedMethodCommand::Implementation
{
	Implementation(InvokeReflectedMethodCommand& self, IDefinitionManager& definitionManager)
	    : self_(self)
	    , definitionManager_(definitionManager)
	{}

	IBasePropertyPtr getProperty(const ObjectHandle& arguments) const
	{
		auto objectManager = definitionManager_.getObjectManager();
		assert(objectManager != nullptr);

		auto commandParameters = arguments.getBase<ReflectedMethodCommandParameters>();
		const ObjectHandle& object = objectManager->getObject(commandParameters->getId());
		auto definition = object.getDefinition(definitionManager_);

		PropertyAccessor methodAccessor = definition->bindProperty(commandParameters->getPath(), object);
		IBasePropertyPtr classMember = methodAccessor.getProperty();
		assert(classMember->isMethod());

		return classMember;
	}

	InvokeReflectedMethodCommand& self_;
	IDefinitionManager& definitionManager_;
};

InvokeReflectedMethodCommand::InvokeReflectedMethodCommand(IDefinitionManager& definitionManager)
    : impl_(new Implementation(*this, definitionManager))
{
}


InvokeReflectedMethodCommand::InvokeReflectedMethodCommand( const InvokeReflectedMethodCommand& rhs )
	: impl_( new Implementation( *this, rhs.impl_->definitionManager_ ) )
{
}


InvokeReflectedMethodCommand& InvokeReflectedMethodCommand::operator=(
	const InvokeReflectedMethodCommand& rhs )
{
	return *this;
}


InvokeReflectedMethodCommand::~InvokeReflectedMethodCommand()
{
}


const char* InvokeReflectedMethodCommand::getId() const
{
	static const char* s_Id = getClassIdentifier<InvokeReflectedMethodCommand>();
	return s_Id;
}

bool InvokeReflectedMethodCommand::validateArguments(const ObjectHandle& arguments ) const 
{
	auto objectManager = impl_->definitionManager_.getObjectManager();
	if ( objectManager == nullptr ) 
	{
		return false;
	}

	auto commandParameters = arguments.getBase<ReflectedMethodCommandParameters>();
	if ( commandParameters == nullptr ) 
	{
		return false;
	}

	const ObjectHandle& object = objectManager->getObject( commandParameters->getId() );
	if ( !object.isValid() )
	{
		return false;
	}

	auto defintion = object.getDefinition( impl_->definitionManager_ );
	if ( defintion == nullptr ) 
	{
		return false;
	}

	PropertyAccessor methodAccessor = defintion->bindProperty( commandParameters->getPath(), object );
	if ( !methodAccessor.isValid() ) 
	{
		return false;
	}
	

	return true;
}


ObjectHandle InvokeReflectedMethodCommand::execute( const ObjectHandle& arguments ) const
{
	auto objectManager = impl_->definitionManager_.getObjectManager();
	assert( objectManager != nullptr );

	auto commandParameters = arguments.getBase<ReflectedMethodCommandParameters>();
	const ObjectHandle& object = objectManager->getObject( commandParameters->getId() );
	auto defintion = object.getDefinition( impl_->definitionManager_ );
	PropertyAccessor methodAccessor = defintion->bindProperty( commandParameters->getPath(), object );

	if (!methodAccessor.isValid())
	{
		return CommandErrorCode::INVALID_ARGUMENTS;
	}

	const ReflectedMethodParameters& parameters = commandParameters->getParameters();
	Variant result = methodAccessor.invoke( parameters );
	return result;
}


CommandThreadAffinity InvokeReflectedMethodCommand::threadAffinity() const
{
	return CommandThreadAffinity::UI_THREAD;
}

ObjectHandle InvokeReflectedMethodCommand::getCommandDescription(const ObjectHandle& arguments) const
{
	IBasePropertyPtr classMember = impl_->getProperty(arguments);

	// Check if a method with meta data to use for the description
	auto methodWithMetaData = dynamic_cast<BasePropertyWithMetaData*>(classMember.get());
	if (methodWithMetaData != nullptr)
	{
		auto description = findFirstMetaData<MetaDescriptionObj>(
		methodWithMetaData->getMetaData(), impl_->definitionManager_);

		if (description != nullptr && description->getDescription() != nullptr)
		{
			auto object = GenericObject::create(impl_->definitionManager_);
			assert(object != nullptr);
			object->set("Name", std::wstring(description->getDescription()));
			object->set("Type", "Unknown");
			return object;
		}
	}

	return ObjectHandle();
}

bool InvokeReflectedMethodCommand::canUndo(const ObjectHandle& arguments) const
{
	IBasePropertyPtr classMember = impl_->getProperty(arguments);

	// Bad cast if classMember is a ReflectedPython::Property*
	auto method = dynamic_cast<ReflectedMethod*>( classMember.get() );
	if (method != nullptr)
	{
		return method->getUndoMethod() != nullptr;
	}

	// Check if a method with meta data
	auto methodWithMetaData = dynamic_cast<BasePropertyWithMetaData*>(classMember.get());
	if (methodWithMetaData != nullptr && methodWithMetaData->baseProperty())
	{
		auto baseMethod = dynamic_cast<ReflectedMethod*>(methodWithMetaData->baseProperty().get());
		return baseMethod->isMethod() && baseMethod->getUndoMethod() != nullptr;
	}

	return false;
}
} // end namespace wgt
