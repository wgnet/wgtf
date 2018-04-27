#pragma once

#include "grabber_utils.hpp"
#include "interfaces/grabber_manager/i_grabber.hpp"
#include "core_common/assert.hpp"
#include "core_reflection/object_handle.hpp"
#include "core_reflection/class_definition.hpp"
#include "core_reflection/i_definition_manager.hpp"
#include "core_reflection/metadata/meta_base.hpp"
#include "core_reflection/metadata/meta_impl.hpp"
#include "core_dependency_system/depends.hpp"
#include <vector>

namespace wgt
{
	template <typename T>
	class SimpleGrabber : public IGrabber, public Depends<IDefinitionManager>
	{
	public:
		SimpleGrabber(ManagedObject<T>& managedObject, 
		              const char* title = nullptr, 
		              bool allUnique = false) :
			propertiesObject_(managedObject.getHandle())
		{
			init(title, allUnique);
		}

		SimpleGrabber(ObjectHandle objectHandle,
			          const char* title = nullptr,
			          bool allUnique = false) :
			propertiesObject_(objectHandle)
		{
			init(title, allUnique);
		}

		void init(const char* title, bool allUnique)
		{
			IDefinitionManager* definitionManager = get<IDefinitionManager>();
			IClassDefinition* definition = definitionManager->getObjectDefinition(propertiesObject_);
			TF_ASSERT(definition != nullptr);

			settingsTitle_ = title != nullptr ? title :
				GrabberUtils::findTitle(*definitionManager, propertiesObject_);

			if(allUnique)
			{
				for(IBasePropertyPtr prop : definition->allProperties())
				{
					if(!prop->isValue())
					{
						continue;
					}

					uniquePropertyNames_.push_back(prop->getName());
				}
			}
		}

		virtual ~SimpleGrabber() {}

		ObjectHandle getPropertiesObject() override { return propertiesObject_; }
		const ObjectHandle getPropertiesObject() const override { return propertiesObject_; }
		const char* getTitle() const override { return settingsTitle_.c_str(); }
		const char* getId() const override { return typeid(T).name(); }
		bool isSupportedEnvironment(const EnvironmentId& id) const override { return true; }
		const std::vector<std::string>& getUniquePropertyNames() const override { return uniquePropertyNames_; }

	private:
		ObjectHandle propertiesObject_;
		std::vector<std::string> uniquePropertyNames_;
		std::string settingsTitle_;
	};
} // end namespace wgt