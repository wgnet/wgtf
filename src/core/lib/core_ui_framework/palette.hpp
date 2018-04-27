#pragma once

namespace wgt
{
namespace Palette
{
	/**
	* Palette color options
	*/
	enum Color
	{
		MainWindowColor,
		LightColor,
		LightPanelColor,
		LightMidColor,
		DarkColor,
		DarkHeaderColor,
		DarkMidColor,
		CustomWindowColor,
		CustomHighlightColor,
		CustomReadonlyColor,
	};

	/**
	* Palette theme options
	* @note when adding new entries, 
	* add to end as values cannot change
	*/
	enum Theme
	{
		Dark = 0,
		Light = 1,
		BattleRed = 2,
		ArmyBrown = 3,
		AirForceGreen = 4,
		NavyBlue = 5,
		Custom = 6,
		MaxThemes
	};
};
} // end namespace wgt
