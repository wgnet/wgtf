#pragma once
#include "core_environment_system/i_env_system.hpp"
#include "interfaces/grabber_manager/i_grabber_manager.hpp"
#include "core_dependency_system/i_interface.hpp"

#include <memory>

namespace wgt
{
	struct IGrabber;

	class GrabberManager : public Implements<IGrabberManager>
	{
	public:
		GrabberManager();
		~GrabberManager();

		virtual void registerGrabber(IGrabber* grabber,
			                         std::function<bool()> iconVisibleFn = nullptr) override;

		virtual void deregisterGrabber(IGrabber* grabber) override;
		virtual void refreshGrabber(IGrabber* grabber) override;

		virtual void postDraw() override;
		virtual void draw() override;
		virtual void postLoad() override;
		virtual void init() override;
		virtual void fini() override;

		virtual bool onMouseButtonDown(MouseButton button, int x, int y) override;
		virtual bool onMouseButtonUp(MouseButton button, int x, int y) override;
		virtual bool onMouseMove(int x, int y) override;
		virtual void onMouseFocusChanged(bool leavingFocus) override;

	private:
		void update();

		struct Implementation;
		friend Implementation;
		std::unique_ptr<Implementation> impl_;
	};
}