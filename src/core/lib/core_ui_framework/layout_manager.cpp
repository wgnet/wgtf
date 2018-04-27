#include "layout_manager.hpp"
#include "layout_hint.hpp"

#include "i_action.hpp"
#include "i_menu.hpp"
#include "i_region.hpp"
#include "i_status_bar.hpp"
#include "i_view.hpp"
#include "i_window.hpp"

#include "core_common/assert.hpp"

#include <algorithm>

namespace wgt
{
namespace
{
const char* safe_str(const char* str)
{
	return str ? str : "";
}
}

bool LayoutManager::matchMenu(IMenu& menu, const char* path)
{
	auto menuPath = menu.path();
	auto menuPathLen = strlen(menuPath);

	auto chr = strchr(path, '|');
	auto minPathLen = chr != nullptr ? static_cast<size_t>(chr - path) : 0;
	auto optPath = chr != nullptr ? chr + 1 : path;

	if (menuPathLen < minPathLen)
	{
		return false;
	}

	if (minPathLen > 0 && strncmp(path, menuPath, minPathLen) != 0)
	{
		return false;
	}

	if (menuPathLen > 0 && strncmp(optPath, menuPath + minPathLen, menuPathLen - minPathLen) != 0)
	{
		return false;
	}

	if (menuPathLen > 0 && optPath[menuPathLen - minPathLen] != '.' && optPath[menuPathLen - minPathLen] != '\0')
	{
		return false;
	}

	return true;
}

std::vector<IMenu*> LayoutManager::findAllMenus(IWindow& window, const char* path)
{
	// create a collection of all menus associated with this window.
	auto& windowMenus = window.menus();
	std::vector<IMenu*> menus(windowMenus.size() + dynamicMenus_.size());

	// copy all the menus owned by the window
	auto it = std::transform(windowMenus.begin(), windowMenus.end(), menus.begin(),
	                         [](const std::unique_ptr<IMenu>& menu) { return menu.get(); });

	// copy all the menus dynamically registered for this window
	auto windowId = safe_str(window.id());
	it = std::copy_if(dynamicMenus_.begin(), dynamicMenus_.end(), it, [&](IMenu* menu) {
		auto menuWindowId = safe_str(menu->windowId());
		return strcmp(windowId, menuWindowId) == 0;
	});

	// resize the collection to the actual number of menus that were collected
	menus.erase(it, menus.end());

	for (auto it = menus.begin(); it != menus.end();)
	{
		if (matchMenu(**it, path))
		{
			++it;
			continue;
		}

		it = menus.erase(it);
	}

	return menus;
}

IRegion* LayoutManager::findBestRegion(IWindow& window, const LayoutHint& hint)
{
	// find the region who's tags have the greatest correlation to the passed in hint
	IRegion* bestRegion = nullptr;
	float bestRegionScore = 0.f;
	auto& regions = window.regions();
	for (auto& region : regions)
	{
		auto& regionTags = region->tags();
		auto regionScore = hint.match(regionTags);
		if (regionScore > bestRegionScore)
		{
			bestRegion = region.get();
			bestRegionScore = regionScore;
		}
	}
	return bestRegion;
}

void LayoutManager::addMenuPath(const char* path, const char* windowId)
{
	auto window = getWindow(windowId);
	if (window == nullptr)
	{
		return;
	}

	auto menus = findAllMenus(*window, path);

	for (auto& menu : menus)
	{
		menu->addPath(path);
	}
}

void LayoutManager::addAction(ManagedAction& action, IWindow& window)
{
	for (auto& path : action.action_->paths())
	{
		auto menus = findAllMenus(window, path.c_str());
		for (auto& menu : menus)
		{
			menu->addAction(*action.action_, path.c_str());
			action.menus_.insert(menu);
		}
	}
}

void LayoutManager::addView(IView& view, IWindow& window)
{
	IRegion* bestRegion = findBestRegion(window, view.hint());
	if (bestRegion == nullptr)
	{
		bestRegion = findBestRegion(window, LayoutHint("default"));
	}
	if (bestRegion == nullptr)
	{
		return;
	}

	const auto menus = view.menus();
	for (auto& menu : menus)
	{
		addMenu(*menu);
	}

	// add the view to the best region
	bestRegion->addView(view);
	views_[&view] = bestRegion;
}

void LayoutManager::refreshActions(IWindow& window)
{
	// go through every action associated with this window and try to re add it.
	// this will move the action between menus where appropriate
	for (auto actionIt = actions_.begin(); actionIt != actions_.end(); ++actionIt)
	{
		auto action = actionIt->action_;
		if (getWindow(action->windowId()) != &window)
		{
			continue;
		}

		TF_ASSERT(actionIt->menus_.empty());
		addAction(*actionIt, window);
	}
}

void LayoutManager::refreshViews(IWindow& window)
{
	// go through every view associated with this window and try to re add it.
	// this will move the view between regions where appropriate
	for (auto viewIt = views_.begin(); viewIt != views_.end(); ++viewIt)
	{
		auto view = viewIt->first;
		if (getWindow(view->windowId()) != &window)
		{
			continue;
		}

		TF_ASSERT(views_[view] == nullptr);
		addView(*view, window);
	}
}

void LayoutManager::removeActions(IWindow& window)
{
	for (auto actionIt = actions_.begin(); actionIt != actions_.end(); ++actionIt)
	{
		auto action = actionIt->action_;
		if (getWindow(action->windowId()) != &window)
		{
			continue;
		}

		for (auto& menu : actionIt->menus_)
		{
			menu->removeAction(*action);
		}
		actionIt->menus_.clear();
	}
}

void LayoutManager::removeViews(IWindow& window)
{
	for (auto viewIt = views_.begin(); viewIt != views_.end(); ++viewIt)
	{
		auto view = viewIt->first;
		if (getWindow(view->windowId()) != &window)
		{
			continue;
		}

		const auto menus = view->menus();
		for (auto& menu : menus)
		{
			removeMenu(*menu);
		}

		viewIt->second->removeView(*view);
		viewIt->second = nullptr;
	}
}

IWindow* LayoutManager::getWindow(const char* windowId)
{
	auto windowIt = windows_.find(safe_str(windowId));
	if (windowIt == windows_.end())
	{
		return nullptr;
	}
	return windowIt->second;
}

LayoutManager::LayoutManager()
{
}

LayoutManager::~LayoutManager()
{
}

void LayoutManager::update() const
{
	/* TODO: remove the need for this */
	for (auto& view : views_)
	{
		view.first->update();
	}

	for (auto& window : windows_)
	{
		window.second->update();
	}
	/* ------------------------------ */
}

void LayoutManager::addAction(IAction& action)
{
	actions_.push_back(&action);

	auto window = getWindow(action.windowId());
	if (window == nullptr)
	{
		return;
	}

	addAction(actions_.back(), *window);
}

void LayoutManager::addMenu(IMenu& menu)
{
	dynamicMenus_.push_back(&menu);

	auto window = getWindow(menu.windowId());
	if (window == nullptr)
	{
		return;
	}

	for (auto actionIt = actions_.begin(); actionIt != actions_.end(); ++actionIt)
	{
		auto action = actionIt->action_;
		if (getWindow(action->windowId()) != window)
		{
			continue;
		}

		for (auto& path : action->paths())
		{
			if (matchMenu(menu, path.c_str()))
			{
				menu.addAction(*action, path.c_str());
				actionIt->menus_.insert(&menu);
			}
		}
	}
}

void LayoutManager::addView(IView& view)
{
	views_[&view] = nullptr;

	auto window = getWindow(view.windowId());
	if (window == nullptr)
	{
		return;
	}

	addView(view, *window);
}

void LayoutManager::addWindow(IWindow& window)
{
	auto windowId = safe_str(window.id());

	auto windowIt = windows_.find(windowId);
	if (windowIt != windows_.end())
	{
		// error?
		return;
	}

	windows_[windowId] = &window;

	// after adding a window we need to reevaluate all the actions and views that we registered
	// against this window as they now all need to be added to the UI
	refreshActions(window);
	refreshViews(window);
}

void LayoutManager::removeAction(IAction& action)
{
	auto actionIt =
	std::find_if(actions_.begin(), actions_.end(), [&](ManagedAction& item) { return item.action_ == &action; });
	if (actionIt == actions_.end())
	{
		return;
	}

	for (auto& menu : actionIt->menus_)
	{
		menu->removeAction(action);
	}
	actions_.erase(actionIt);
}

void LayoutManager::removeMenu(IMenu& menu)
{
	auto menuIt = std::find(dynamicMenus_.begin(), dynamicMenus_.end(), &menu);
	if (menuIt == dynamicMenus_.end())
	{
		return;
	}

	for (auto actionIt = actions_.begin(); actionIt != actions_.end(); ++actionIt)
	{
		for (auto menuIt = actionIt->menus_.begin(); menuIt != actionIt->menus_.end();)
		{
			if (*menuIt != &menu)
			{
				++menuIt;
				continue;
			}

			menu.removeAction(*actionIt->action_);
			menuIt = actionIt->menus_.erase(menuIt);
		}
	}

	dynamicMenus_.erase(menuIt);
}

void LayoutManager::removeView(IView& view)
{
	auto viewIt = views_.find(&view);
	if (viewIt == views_.end())
	{
		return;
	}

	auto region = viewIt->second;
	if (region == nullptr)
	{
		views_.erase(viewIt);
		return;
	}

	const auto menus = view.menus();
	for (auto& menu : menus)
	{
		removeMenu(*menu);
	}

	region->removeView(view);
	views_.erase(viewIt);
}

void LayoutManager::removeWindow(IWindow& window)
{
	auto windowId = safe_str(window.id());

	auto windowIt = windows_.find(windowId);
	if (windowIt == windows_.end())
	{
		return;
	}

	removeActions(window);
	removeViews(window);

	windows_.erase(windowId);
}

void LayoutManager::setWindowIcon(const char* path, const char* windowId)
{
	if (windowId == nullptr)
	{
		windowId = "";
	}

	auto windowIt = windows_.find(windowId);
	if (windowIt == windows_.end())
	{
		return;
	}

	windowIt->second->setIcon(path);
}

void LayoutManager::setStatusMessage(const char* message, int timeout)
{
	IStatusBar* statusBar = nullptr;
	for (const auto& windowPair : windows())
	{
		statusBar = windowPair.second->statusBar();
		if ( statusBar != nullptr )
		{
			break;
		}
	}

	if ( statusBar )
	{
		statusBar->showMessage(message, timeout);
	}
}

void LayoutManager::saveWindowPreferences()
{
	for(auto windowIt = windows_.begin(); windowIt != windows_.end(); ++windowIt)
	{
		windowIt->second->savePreference();
	}
}

void LayoutManager::loadWindowPreferences()
{
	for(auto windowIt = windows_.begin(); windowIt != windows_.end(); ++windowIt)
	{
		windowIt->second->loadPreference();
	}
}

const Windows& LayoutManager::windows() const
{
	return windows_;
}
} // end namespace wgt
