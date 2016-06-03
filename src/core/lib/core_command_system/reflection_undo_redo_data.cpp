#include "reflection_undo_redo_data.hpp"
#include "command_instance.hpp"
#include "core_reflection_utils/commands/reflectedproperty_undoredo_helper.hpp"
#include "core_reflection/generic/generic_object.hpp"
#include "core_reflection/property_accessor_listener.hpp"
#include "core_reflection/property_accessor.hpp"
#include "core_serialization/serializer/xml_serializer.hpp"
#include "core_reflection/metadata/meta_utilities.hpp"
#include "core_reflection/metadata/meta_impl.hpp"
#include "core_reflection/i_object_manager.hpp"
#include "wg_types/binary_block.hpp"

namespace wgt
{
namespace RPURU = ReflectedPropertyUndoRedoUtility;
namespace
{

	void initReflectedMethodInDisplayObject(GenericObject& object, RPURU::ReflectedClassMemberUndoRedoHelper* helper)
	{
		object.set("Type", "Method");
		auto methodHelper = static_cast<RPURU::ReflectedMethodUndoRedoHelper*>(helper);
		size_t max = methodHelper->parameters_.size();
		char parameterName[32] = "Parameter";
		char * counterPointer = &parameterName[strlen(parameterName)];

		for (size_t i = 0; i < max; ++i)
		{
#if defined(_MSC_VER) && ( _MSC_VER < 1900 )
			sprintf(counterPointer, "%d", i);
#else
			sprintf(counterPointer, "%zd", i);
#endif
			object.set(parameterName, methodHelper->parameters_[i]);
		}
	}    
}


class PropertyAccessorWrapper
	: public PropertyAccessorListener
{
public:
	PropertyAccessorWrapper( RPURU::UndoRedoHelperList & undoRedoHelperList )
		: undoRedoHelperList_( undoRedoHelperList )
	{
	}


	~PropertyAccessorWrapper()
	{
	}


	void preSetValue(
		const PropertyAccessor & accessor, const Variant & value ) override
	{
		const auto & obj = accessor.getRootObject();
		assert( obj != nullptr );
		RefObjectId id;
		bool ok = obj.getId( id );
		if (!ok)
		{
			NGT_ERROR_MSG( "Trying to create undo/redo helper for unmanaged object\n" );
			// evgenys: we have to split notifications for managed and unmanaged objects
			return;
		}
		const char * propertyPath = accessor.getFullPath();
		const TypeId type = accessor.getType();
		Variant prevalue = accessor.getValue();
		auto pHelper = this->findUndoRedoHelper( id, propertyPath );
		if (pHelper != nullptr)
		{
			return;
		}
		auto helper = new RPURU::ReflectedPropertyUndoRedoHelper();
		helper->objectId_ = id;
		helper->path_ = propertyPath;
		helper->typeName_ = type.getName();
		helper->preValue_ = std::move( prevalue );
		undoRedoHelperList_.emplace_back( helper );
	}


	void postSetValue(
		const PropertyAccessor & accessor, const Variant & value ) override
	{
		const auto & obj = accessor.getRootObject();
		assert( obj != nullptr );
		RefObjectId id;
		bool ok = obj.getId( id );
		if (!ok)
		{
			NGT_ERROR_MSG( "Trying to create undo/redo helper for unmanaged object\n" );
			// evgenys: we have to split notifications for managed and unmanaged objects
			return;
		}
		const char * propertyPath = accessor.getFullPath();
		Variant postValue = accessor.getValue();
		RPURU::ReflectedPropertyUndoRedoHelper* pHelper = static_cast<RPURU::ReflectedPropertyUndoRedoHelper*>(
			this->findUndoRedoHelper( id, propertyPath ) );
		assert( pHelper != nullptr );
		pHelper->postValue_ = std::move( postValue );
	}


	void preInvoke(
		const PropertyAccessor & accessor, const ReflectedMethodParameters& parameters, bool undo ) override
	{
		const char* path = accessor.getFullPath();
		const auto& object = accessor.getRootObject();
		assert( object != nullptr );

		RefObjectId id;
		bool ok = object.getId( id );
		assert(ok);

		RPURU::ReflectedMethodUndoRedoHelper* helper = static_cast<RPURU::ReflectedMethodUndoRedoHelper*>(
			this->findUndoRedoHelper( id, path ) );

		if (helper == nullptr)
		{
			auto helper = new RPURU::ReflectedMethodUndoRedoHelper();
			helper->objectId_ = id;
			helper->path_ = path;
			helper->parameters_ = parameters;
			undoRedoHelperList_.emplace_back( helper );
		}
		else
		{
			helper->parameters_ = parameters;
		}
	}

	void postInvoke( const PropertyAccessor & accessor, Variant result, bool undo ) override
	{
		const auto& object = accessor.getRootObject();
		assert( object != nullptr );

		RefObjectId id;
		bool ok = object.getId( id );
		assert( ok );

		if (undoRedoHelperList_.size() > 0)
		{
			auto helper = undoRedoHelperList_.back().get();
			if (helper->isMethod())
			{
				auto methodHelper = (RPURU::ReflectedMethodUndoRedoHelper*)helper;

				if (methodHelper->objectId_ == id)
				{
					methodHelper->result_ = result;
				}
			}
		}
	}


private:
	RPURU::ReflectedClassMemberUndoRedoHelper* findUndoRedoHelper( 
		const RefObjectId & id, const char * propertyPath )
	{
		RPURU::ReflectedClassMemberUndoRedoHelper* helper = nullptr;
		for (auto& findIt : undoRedoHelperList_)
		{
			if (findIt->objectId_ == id && findIt->path_ == propertyPath)
			{
				helper = findIt.get();
				break;
			}
		}
		return helper;
	}
private:
	RPURU::UndoRedoHelperList &	undoRedoHelperList_;
};


ReflectionUndoRedoData::ReflectionUndoRedoData( CommandInstance & commandInstance )
	: commandInstance_( commandInstance )
	, paListener_( new PropertyAccessorWrapper( undoRedoHelperList_ ) )
{

}


void ReflectionUndoRedoData::connect()
{
	auto definitionManager = commandInstance_.defManager_;
	assert( definitionManager != nullptr );
	definitionManager->registerPropertyAccessorListener( paListener_ );
}


void ReflectionUndoRedoData::disconnect()
{
	auto definitionManager = commandInstance_.defManager_;
	assert( definitionManager != nullptr );
	definitionManager->deregisterPropertyAccessorListener( paListener_ );
}

void ReflectionUndoRedoData::consolidate()
{
	auto definitionManager = commandInstance_.defManager_;
	assert( definitionManager != nullptr );

	XMLSerializer undoSerializer( undoData_, *definitionManager );
	undoSerializer.serialize( RPURU::getUndoStreamHeaderTag() );
	undoSerializer.serialize( undoRedoHelperList_.size() );

	XMLSerializer redoSerializer( redoData_, *definitionManager );
	redoSerializer.serialize( RPURU::getRedoStreamHeaderTag() );
	redoSerializer.serialize( undoRedoHelperList_.size() );

	for (const auto& helper : undoRedoHelperList_)
	{
		RPURU::saveUndoData( undoSerializer, *helper );
		RPURU::saveRedoData( redoSerializer, *helper );
	}

	undoRedoHelperList_.clear();
}


void ReflectionUndoRedoData::undo()
{
	auto definitionManager = commandInstance_.defManager_;
	assert( definitionManager != nullptr );
	const auto pObjectManager = definitionManager->getObjectManager();
	assert( pObjectManager != nullptr );

	if (!undoData_.buffer().empty())
	{
		undoData_.seek( 0 );
		XMLSerializer serializer( undoData_, *definitionManager );
		RPURU::performReflectedUndo( serializer, *pObjectManager, *definitionManager );
	}
}


void ReflectionUndoRedoData::redo()
{
	auto definitionManager = commandInstance_.defManager_;
	assert( definitionManager != nullptr );
	const auto pObjectManager = definitionManager->getObjectManager();
	assert( pObjectManager != nullptr );
	if (!redoData_.buffer().empty())
	{
		redoData_.seek( 0 );
		XMLSerializer serializer( redoData_, *definitionManager );
		RPURU::performReflectedRedo( serializer, *pObjectManager, *definitionManager );
	}
}


ObjectHandle ReflectionUndoRedoData::getCommandDescription() const
{
	auto definitionManager = commandInstance_.defManager_;
	assert( definitionManager != nullptr );
	auto pObjectManager = definitionManager->getObjectManager();
	assert(pObjectManager != nullptr);
	auto& objectManager = (*pObjectManager);
	const char * undoStreamHeaderTag = RPURU::getUndoStreamHeaderTag();
	const char * redoStreamHeaderTag = RPURU::getRedoStreamHeaderTag();

	ObjectHandle result;
	RPURU::UndoRedoHelperList propertyCache;
	{
		// Need to read undo/redo data separately and then consolidate it into
		// propertyCache.

		// Make a copy because this function should not modify stream contents
		// TODO ResizingMemoryStream const read implementation
		ResizingMemoryStream undoStream(undoData_.buffer());
		assert(!undoStream.buffer().empty());
		XMLSerializer undoSerializer(undoStream, *definitionManager);

		// Read property header
		std::string undoHeader;
		undoHeader.reserve(strlen(undoStreamHeaderTag));
		undoSerializer.deserialize(undoHeader);
		assert(undoHeader == undoStreamHeaderTag);

		// Make a copy because this function should not modify stream contents
		// TODO ResizingMemoryStream const read implementation
		ResizingMemoryStream redoStream(redoData_.buffer());
		assert(!redoStream.buffer().empty());
		XMLSerializer redoSerializer(redoStream, *definitionManager);

		// Read property header
		std::string redoHeader;
		redoHeader.reserve(strlen(redoStreamHeaderTag));
		redoSerializer.deserialize(redoHeader);
		assert(redoHeader == redoStreamHeaderTag);

		// Read properties into cache
		const bool reflectedPropertiesLoaded = loadReflectedProperties(
			propertyCache,
			undoSerializer,
			redoSerializer,
			objectManager,
			*definitionManager);
		if (!reflectedPropertiesLoaded)
		{
			return ObjectHandle();
		}
	}

	// Create display object from cache
	{
		auto handle = GenericObject::create(*definitionManager);
		assert(handle.get() != nullptr);
		auto& genericObject = (*handle);

		// command with no property change
		if (propertyCache.empty())
		{
			genericObject.set("Name", "Unknown");
			genericObject.set("Type", "Unknown");
		}
		// Single command
		else if (propertyCache.size() == 1)
		{
			auto& helper = propertyCache.at(0);
			// TODO: Refactor this and the section below as they do the same thing.
			genericObject.set("Id", helper->objectId_);
			ObjectHandle object = objectManager.getObject(helper->objectId_);

			if (object == nullptr)
			{
				genericObject.set("Name", helper->path_);
			}
			else
			{
				assert(object != nullptr);
				PropertyAccessor pa(object.getDefinition(*definitionManager)->bindProperty(
					helper->path_.c_str(), object));
				auto metaData = findFirstMetaData< MetaInPlacePropertyNameObj >(pa, *definitionManager);
				if (metaData != nullptr)
				{
					const char * propName = metaData->getPropertyName();
					pa = object.getDefinition(*definitionManager)->bindProperty(propName, object);
					auto value = pa.getValue();
					std::string name;
					bool isOk = value.tryCast(name);
					assert(isOk);
					genericObject.set("Name", name);
				}
				else
				{
					genericObject.set("Name", helper->path_);
				}
			}

			if (helper->isMethod())
			{
				initReflectedMethodInDisplayObject(genericObject, helper.get());
			}
			else
			{
				auto propertyHelper = static_cast<RPURU::ReflectedPropertyUndoRedoHelper*>(helper.get());
				genericObject.set("Type", propertyHelper->typeName_);
				genericObject.set("PreValue", propertyHelper->preValue_);
				genericObject.set("PostValue", propertyHelper->postValue_);
			}
		}
		else
		{
			genericObject.set("Name", "Batch");
			genericObject.set("Type", "Batch");

			// Need to create a CollectionHolder, otherwise
			// genericObject.set( "Children", children );
			// is unsafe, because it takes a reference
			// which will be deleted when children goes out of scope
			typedef std::vector< GenericObjectPtr > Children;
			auto collectionHolder =
				std::make_shared< CollectionHolder< Children > >();

			Children& children = collectionHolder->storage();
			children.reserve(propertyCache.size());

			for (const auto& helper : propertyCache)
			{
				auto childHandle = GenericObject::create(*definitionManager);
				assert(childHandle.get() != nullptr);

				auto& childObject = (*childHandle);
				// TODO: Refactor this and the section above as they do the same thing.
				childObject.set("Id", helper->objectId_);
				ObjectHandle object = objectManager.getObject(helper->objectId_);

				if (object == nullptr)
				{
					genericObject.set("Name", helper->path_);
				}
				else
				{
					PropertyAccessor pa(object.getDefinition(*definitionManager)->bindProperty(
						helper->path_.c_str(), object));
					auto metaData = findFirstMetaData< MetaInPlacePropertyNameObj >(pa, *definitionManager);
					if (metaData != nullptr)
					{
						const char * propName = metaData->getPropertyName();
						pa = object.getDefinition(*definitionManager)->bindProperty(propName, object);
						auto value = pa.getValue();
						std::string name;
						bool isOk = value.tryCast(name);
						assert(isOk);
						childObject.set("Name", name);
					}
					else
					{
						genericObject.set("Name", helper->path_);
					}
				}

				if (helper->isMethod())
				{
					initReflectedMethodInDisplayObject(genericObject, helper.get());
				}
				else
				{
					auto propertyHelper = static_cast<RPURU::ReflectedPropertyUndoRedoHelper*>(helper.get());
					childObject.set("Type", propertyHelper->typeName_);
					childObject.set("PreValue", propertyHelper->preValue_);
					childObject.set("PostValue", propertyHelper->postValue_);
				}

				children.push_back(childHandle);
			}

			// Convert CollectionHolder to Collection
			Collection childrenCollection(collectionHolder);
			genericObject.set("Children", childrenCollection);
		}

		result = ObjectHandle(std::move(handle));
	}

	return result;
}

std::shared_ptr<BinaryBlock> ReflectionUndoRedoData::getUndoData() const
{
    return std::make_shared<BinaryBlock>( undoData_.buffer().c_str(), undoData_.buffer().length(), true );
}

std::shared_ptr<BinaryBlock> ReflectionUndoRedoData::getRedoData() const
{
    return std::make_shared<BinaryBlock>( redoData_.buffer().c_str(), redoData_.buffer().length(), true );
}

void ReflectionUndoRedoData::setUndoData( const std::shared_ptr<BinaryBlock> & undoData )
{
     undoData_.setBuffer( std::string( undoData->cdata(), undoData->length() ) );
}

void ReflectionUndoRedoData::setRedoData( const std::shared_ptr<BinaryBlock> & redoData )
{
    redoData_.setBuffer( std::string( redoData->cdata(), redoData->length() ) );
}
} // end namespace wgt
