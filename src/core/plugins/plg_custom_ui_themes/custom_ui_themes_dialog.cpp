#include "custom_ui_themes_dialog.hpp"
#include "core_logging/logging.hpp"
#include "core_variant/variant.hpp"

namespace wgt
{
bool CustomUIThemesDialog::createDialog()
{
	auto uiFramework = this->get<IUIFramework>();
	auto uiApplication = this->get<IUIApplication>();

	if ((uiFramework == nullptr) || (uiApplication == nullptr))
	{
		return false;
	}

	uiFramework->loadActionData(":/WGCustomUIThemes/actions.xml", IUIFramework::ResourceType::File);

	actions_.push_back(uiFramework->createAction("showThemeDialog",
		std::bind(&CustomUIThemesDialog::actionShowDialog, this, std::placeholders::_1)));

	actions_.push_back(uiFramework->createAction("darkThemeAction",
		std::bind(&CustomUIThemesDialog::setTheme, this, std::placeholders::_1, Palette::Dark),
		[](const IAction*) { return true; },
		std::bind(&CustomUIThemesDialog::getIsCurrentTheme, this, std::placeholders::_1, Palette::Dark)
		));

	actions_.push_back(uiFramework->createAction("lightThemeAction",
		std::bind(&CustomUIThemesDialog::setTheme, this, std::placeholders::_1, Palette::Light),
		[](const IAction*) { return true; },
		std::bind(&CustomUIThemesDialog::getIsCurrentTheme, this, std::placeholders::_1, Palette::Light)
	));

	actions_.push_back(uiFramework->createAction("battleRedThemeAction",
		std::bind(&CustomUIThemesDialog::setTheme, this, std::placeholders::_1, Palette::BattleRed),
		[](const IAction*) { return true; },
		std::bind(&CustomUIThemesDialog::getIsCurrentTheme, this, std::placeholders::_1, Palette::BattleRed)
	));

	actions_.push_back(uiFramework->createAction("armyBrownThemeAction",
		std::bind(&CustomUIThemesDialog::setTheme, this, std::placeholders::_1, Palette::ArmyBrown),
		[](const IAction*) { return true; },
		std::bind(&CustomUIThemesDialog::getIsCurrentTheme, this, std::placeholders::_1, Palette::ArmyBrown)
	));

	actions_.push_back(uiFramework->createAction("airForceGreenThemeAction",
		std::bind(&CustomUIThemesDialog::setTheme, this, std::placeholders::_1, Palette::AirForceGreen),
		[](const IAction*) { return true; },
		std::bind(&CustomUIThemesDialog::getIsCurrentTheme, this, std::placeholders::_1, Palette::AirForceGreen)
	));

	actions_.push_back(uiFramework->createAction("navyBlueThemeAction",
		std::bind(&CustomUIThemesDialog::setTheme, this, std::placeholders::_1, Palette::NavyBlue),
		[](const IAction*) { return true; },
		std::bind(&CustomUIThemesDialog::getIsCurrentTheme, this, std::placeholders::_1, Palette::NavyBlue)
	));

	actions_.push_back(uiFramework->createAction("customThemeAction",
		std::bind(&CustomUIThemesDialog::setTheme, this, std::placeholders::_1, Palette::Custom),
		[](const IAction*) { return true; },
		std::bind(&CustomUIThemesDialog::getIsCurrentTheme, this, std::placeholders::_1, Palette::Custom)
	));

	for (auto& action : actions_)
	{
		uiApplication->addAction(*action);
	}

	return true;
}

void CustomUIThemesDialog::destroyDialog()
{
	auto uiApplication = this->get<IUIApplication>();
	if (uiApplication == nullptr)
	{
		return;
	}

	if (!actions_.empty())
	{
		for (auto& action : actions_)
		{
			uiApplication->removeAction(*action);
		}
		actions_.clear();
	}
}
void CustomUIThemesDialog::actionShowDialog(const IAction* action)
{
	auto uiFramework = this->get<IUIFramework>();
	uiFramework->showDialog("WGCustomUIThemes/CustomUIThemesDialog.qml",IDialog::Mode::MODELESS);
}


void CustomUIThemesDialog::setTheme(const IAction* action, Palette::Theme theme)
{
	auto uiFramework = this->get<IUIFramework>();
	uiFramework->setPaletteTheme(theme);
}

bool CustomUIThemesDialog::getIsCurrentTheme(const IAction* action, Palette::Theme theme) const
{
	auto uiFramework = this->get<IUIFramework>();
	return uiFramework->getPaletteTheme() == theme;
}

}
