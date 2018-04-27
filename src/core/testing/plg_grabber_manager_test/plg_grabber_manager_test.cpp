#include "core_generic_plugin/generic_plugin.hpp"
#include "core_object/managed_object.hpp"
#include "core_reflection/reflection_macros.hpp"
#include "core_reflection/i_definition_manager.hpp"
#include "interfaces/grabber_manager/i_grabber.hpp"
#include "interfaces/grabber_manager/i_grabber_manager.hpp"
#include "plugins/plg_grabber_manager/simple_grabber.hpp"
#include "unique_settings_data.hpp"
#include "static_settings_data.hpp"

#include "metadata/unique_settings_data.mpp"
#include "metadata/static_settings_data.mpp"
#include "core_reflection/utilities/reflection_auto_register.hpp"

#include <memory>

namespace wgt
{
	class GrabberManagerTestPlugin : public PluginMain
	{
	public:
		GrabberManagerTestPlugin(IComponentContext& contextManager)
		{
			registerCallback([]( IDefinitionManager & defManager )
			{
				ReflectionAutoRegistration::initAutoRegistration(defManager);
			});
		}

		~GrabberManagerTestPlugin()
		{
		}

		bool PostLoad(IComponentContext& contextManager)
		{
			return true;
		}

		void Initialise(IComponentContext& contextManager)
		{
			grabberManager_ = contextManager.queryInterface<IGrabberManager>();
			uniqueDataObject_ = ManagedObject<UniqueSettingsData>::make_unique();
			staticDataObject_ = ManagedObject<StaticSettingsData>::make_unique();
			uniqueGrabber_.reset(new SimpleGrabber<UniqueSettingsData>(*uniqueDataObject_.get(),
				"Unique Settings", true));			
			staticGrabber_.reset(new SimpleGrabber<StaticSettingsData>(*staticDataObject_.get(),
				"Static Settings", false));
			grabberManager_->registerGrabber(uniqueGrabber_.get());
			grabberManager_->registerGrabber(staticGrabber_.get());
		}

		bool Finalise(IComponentContext& contextManager)
		{
			grabberManager_->deregisterGrabber(uniqueGrabber_.get());
			uniqueGrabber_.reset(nullptr);
			grabberManager_->deregisterGrabber(staticGrabber_.get());
			staticGrabber_.reset(nullptr);
			return true;
		}

		void Unload(IComponentContext& contextManager)
		{
		}

	private:
		IGrabberManager* grabberManager_;
		UniqueSettingsData uniqueData_;
		std::unique_ptr<ManagedObject<UniqueSettingsData>> uniqueDataObject_;
		std::unique_ptr<SimpleGrabber<UniqueSettingsData>> uniqueGrabber_;
		StaticSettingsData staticData_;
		std::unique_ptr<ManagedObject<StaticSettingsData>> staticDataObject_;
		std::unique_ptr<SimpleGrabber<StaticSettingsData>> staticGrabber_;
	};

	PLG_CALLBACK_FUNC(GrabberManagerTestPlugin)
} // end namespace wgt