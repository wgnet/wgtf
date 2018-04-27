#include "resource_log_listener.hpp"

#include "core_base_editor/log_listener.hpp"
#include "core_common/assert.hpp"
#include "core_dependency_system/depends.hpp"
#include "core_logging_system/log_message.hpp"
#include "interfaces/i_loading_event_source.hpp"
#include "core_ui_framework/i_ui_framework.hpp"
#include <sstream>

namespace wgt
{

	struct ResourceLogListener::Impl
	{
		Impl() : listening_(false) {}
		virtual ~Impl() {
			// Fix for TITAN-1409 Crash at Load/unload_world while selecting sa_lakeview.streaming from asset browser
			// Sometimes events are coming in during destruction phase, set to false to ignore them.
			// This is still unsafe but should prevent from crashes in most cases.
			listening_ = false;
		}

		virtual void startListening(LogLevel logLevel)
		{
			if (listening_)
				return;
			listening_ = true;
			logListener_.startListening(logLevel);
		}

		virtual std::vector<LogMessage> stopListening()
		{
			if (!listening_)
				return std::vector<LogMessage>();
			listening_ = true;
			return logListener_.stopListening();
		}

		void notifyCompleted()
		{
			auto errors = stopListening();
			onCompleted_(errors);
		}
		Signal<OnOperationCompleted> onCompleted_;
		LogListener logListener_;
		bool listening_;
	};

	class LoadEventDrivenLogListener : public ResourceLogListener
	{
		typedef ResourceLogListener super;
	public:
		LoadEventDrivenLogListener(const std::string& resourceName, LogLevel logLevel = LOG_ERROR)
			: super(std::make_unique<Impl>(resourceName), logLevel)
		{
		}
	private:
		struct Impl
			: public super::Impl
			, public Depends<ILoadingEventSource>
		{
			typedef ResourceLogListener::Impl super;

			Impl(const std::string& resource)
				: watchResource_(resource)
			{
			}

			~Impl()
			{
				if (listening_)
				{
					ILoadingEventSource* evtSource = get<ILoadingEventSource>();
					if (evtSource)
					{
						evtSource->endListening();
					}
				}
			}

			void onEndResource(ILoadingEventSource::Stage stage, const std::string& filename)
			{
				// Not listening anyway
				if (!listening_)
					return;

				if (watchResource_.empty() || filename != watchResource_)
				{
					// Ignore this resource
					return;
				}

				// have we finished loading?
				if (stage != ILoadingEventSource::Stage::Initialize)
				{
					// Still need initialization
					return;
				}

				// If we've finished loading, then lets exit
				notifyCompleted();
			}

			virtual void startListening(LogLevel logLevel) override
			{
				if (listening_)
					return;
				listening_ = true;

				ILoadingEventSource* evtSource = get<ILoadingEventSource>();
				if (evtSource)
				{
					evtSourceConnection_.add(evtSource->connectEnd([this](ILoadingEventSource::Stage stage, const std::string& filename)
					{
						onEndResource(stage, filename);
					}));
					evtSource->beginListening();
				}
				super::startListening(logLevel);
			}

			std::vector<LogMessage> stopListening()
			{
				if (!listening_)
					return std::vector<LogMessage>();
				listening_ = false;
				return super::stopListening();
			}

			std::string watchResource_;
			ConnectionHolder evtSourceConnection_;
		};

	};

	class SimpleLogListener : public ResourceLogListener
	{
		typedef ResourceLogListener super;
	public:
		SimpleLogListener(LogLevel logLevel)
			: super(std::make_unique<ResourceLogListener::Impl>(), logLevel)
		{
		}
	};

	ResourceLogListener::ResourceLogListener(std::unique_ptr<Impl>&& impl, LogLevel logLevel)
		: impl_(std::move(impl))
	{
		impl_->startListening(logLevel);
	}

	ResourceLogListener::~ResourceLogListener()
	{
		impl_->stopListening();
	}

	Connection ResourceLogListener::connectOnCompleted(OnCompletedCallback callback)
	{
		return impl_->onCompleted_.connect(callback);
	}

	void ResourceLogListener::notifyCompleted()
	{
		impl_->notifyCompleted();
	}

	std::unique_ptr<ResourceLogListener> ResourceLogListener::createLogListener(LogLevel logLevel)
	{
		return std::make_unique<SimpleLogListener>(logLevel);
	}

	std::unique_ptr<ResourceLogListener> ResourceLogListener::createLoadingEventSourceListener(const std::string& filename, LogLevel logLevel)
	{
		return std::make_unique<LoadEventDrivenLogListener>(filename, logLevel);
	}

	std::unique_ptr<ResourceLogListener> ResourceLogListener::createLoadListenerWithDefaultHandler(
		const std::string& filename, 
		bool isLoadingEventListener,
		LogLevel logLevel)
	{

		auto result = isLoadingEventListener ? createLoadingEventSourceListener(filename, logLevel) : createLogListener(logLevel);
		result->connectOnCompleted([filename](const std::vector<LogMessage>& errors) 
		{
			if (errors.empty())
			{
				// No errors, we don't need a message
				return;
			}

			std::stringstream errorTitle;
			errorTitle << "Error loading asset: " << filename;

			std::stringstream errorMsg;
			errorMsg << "Important!" << std::endl << std::endl;
			errorMsg << "There were ";
			errorMsg << errors.size();
			errorMsg << " errors loading this file. "
				"You will still be able to save this file, but there is the possibility of data loss! "
				"The errors listed in the Details should be addressed as soon as possible.";

			std::stringstream errorDetail;
			for (const auto& error : errors)
			{
				errorDetail << error.getLevelString() << ":\t" << error.c_str() << "\n\n";
			}

			DependsLocal<IUIFramework> deps;
			auto ui = deps.get<IUIFramework>();
			TF_ASSERT(ui);
			ui->displayMessageBox(errorTitle.str().c_str(),
				errorMsg.str().c_str(), IUIFramework::Ok, IUIFramework::MessageBoxIcon::Error, errorDetail.str().c_str());
		});

		return std::move(result);
	}

	std::unique_ptr<ResourceLogListener> ResourceLogListener::createSaveListenerWithDefaultHandler(const std::string& filename, std::function<bool()> successCheck, LogLevel logLevel)
	{
		auto result = createLogListener(logLevel);

		result->connectOnCompleted([filename, successCheck](const std::vector<LogMessage>& errors) 
		{
			if (errors.empty())
			{
				// No errors, we don't need a message
				return;
			}

			std::stringstream errorMsg;
			std::stringstream errorTitle;
			if (!successCheck())
			{				
				errorTitle << "Asset Not Saved: " << filename;
				errorMsg << "Important!" << std::endl << std::endl;
				errorMsg << "There were ";
				errorMsg << errors.size();
				errorMsg << " errors attempting to save this file. " << std::endl <<
					"The errors listed in the Details are preventing the asset from being saved.";
			}
			else
			{
				errorTitle << "Errors encountered while saving asset: " << filename;
				errorMsg << "Important!" << std::endl << std::endl;
				errorMsg << "The file was saved but with ";
				errorMsg << errors.size();
				errorMsg << " errors. There is a possibility of data loss." << std::endl <<
					"The errors listed in the Details should ideally be resolved and the asset re-saved.";
			}
			
			std::stringstream errorDetail;
			for (const auto& error : errors)
			{
				errorDetail << error.getLevelString() << ":\t" << error.c_str() << "\n\n";
			}

			DependsLocal<IUIFramework> deps;
			auto ui = deps.get<IUIFramework>();
			TF_ASSERT(ui);
			ui->displayMessageBox(errorTitle.str().c_str(),
				errorMsg.str().c_str(), IUIFramework::Ok, IUIFramework::MessageBoxIcon::Error, errorDetail.str().c_str());
		});
		return std::move(result);
	}
}