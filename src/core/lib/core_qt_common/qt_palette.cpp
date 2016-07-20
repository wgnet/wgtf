#include "qt_palette.hpp"
#include <QPalette>
#include <QColor>
#include <QtWidgets/QApplication>
#include <QtWidgets/QStyleFactory>

namespace wgt
{


	QtPalette::QtPalette(QQuickItem* parent)
		: QQuickItem(parent), timerid_(0)
	{
		createDefaultPalette();
		generateStyle();
	}

	QtPalette::QtPalette(QPalette& palette)
		: timerid_(0)
	{
		createDefaultPalette();
		generateStyle();
	}

	void QtPalette::createDefaultPalette(){
		// Use the Fusion style instead default OS style
		if (qApp)
		{
			qApp->setDesktopSettingsAware(false);
			qApp->setStyle(QStyleFactory::create("Fusion"));

			// Change the default font for the entire app
			QGuiApplication::setFont(QFont("Noto Sans", 9));
		}

		/*************************** STYLING ***************************

		General Tip:
		Edit the values at the top of this file for bigger, obvious, predictable changes. The further you go down the more subtle and potentially unpredictable these changes will be.


		*************************** MAIN COLORS ***************************

		Most styling changes can be made by changing just these two colors.

		mainWindowColor_ is the background of most of the UI and should probably be very dark OR very light
		highlightColor_ is the primary contrast color of the UI for mouseovers, highlights and other contrasts. It works best as a bright saturated color - not too light */

		mainWindowColor_ = QColor(56, 58, 61);
		highlightColor_ = QColor(51, 153, 255);

		/*************************** TEXT COLOR ***************************

		darkText_ is a simple variable for changing the text theme from dark on light to light on dark

		In most cases you only want to change this unless you want text that isn't subtle shades of pure black or white

		If mainWindowColor_ is very dark, set this to false
		If mainWindowColor_ is very light, set this to true */

		darkText_ = false;
	}


	void QtPalette::generateStyle(){
		/*************************** GENERATED COLORS ***************************

		/*	If you want the text to be some other variation of pure white or pure black, edit these values.

		These values are modified later on in this file for disabled/placeholder states etc. */
		if (darkText_)
		{
			textColor_ = QColor(0, 0, 0);
		}
		else
		{
			textColor_ = QColor(255, 255, 255);
		}

		/*	highlightTextColor_ is the color of text on top of the highlightColor_ and probably unrelated to the state of darkText_
		it should almost always be left white unless the highlight color is also a very light color such as bright yellow) */
		highlightTextColor_ = QColor(255, 255, 255);

		/*	toolTipColor_ is the background of a popup tooltip. By default it is a transparent shade of the highlightColor_
		toolTipTextColor_ is the text on top of a tooltip and generally should be the same as highlightTextColor_ unless toolTipColor_ is changed */

		toolTipColor_ = highlightColor_;
		toolTipColor_.setAlpha(220);

		toolTipTextColor_ = highlightTextColor_;
		toolTipTextColor_.setAlpha(220);


		/*************************** SHADES ***************************

		Dark and light shades generally overlay mainWindowColor_ to provide subtle raised and sunken areas, borders etc.
		The dark shades are black at various transparencies, light shades are white at varying transparencies.

		In general you want more light contrast for light mainWindowColors and more dark contrast for dark mainWindowColors
		Edit these values if you want more contrast from your UI.

		Increasing the values provides more contrast, lowering them provides less contrast */
		
		if (theme_ != Custom)
		{
			if (darkText_)
			{
				darkContrast_ = 16;
				lightContrast_ = 32;
			}
			else
			{
				darkContrast_ = 32;
				lightContrast_ = 16;
			}
		}

		/*	It is HIGHLY recommended you do not change the basic shade colors from black or white.

		Shading a color with something other than pure black or pure white does not make much sense. */

		lightShade_ = QColor(255, 255, 255, lightContrast_);
		lighterShade_ = QColor(255, 255, 255, lightContrast_ * 2);
		lightestShade_ = QColor(255, 255, 255, lightContrast_ * 3);

		darkShade_ = QColor(0, 0, 0, darkContrast_);
		darkerShade_ = QColor(0, 0, 0, darkContrast_ * 2);
		darkestShade_ = QColor(0, 0, 0, darkContrast_ * 3);


		/*	highlightShade_ is used for highlighting elements where the full highlightColor_ is too bright such as for text highlighting and menu selection
		By default it is highlightColor_ at half opacity */

		highlightShade_ = highlightColor_;
		highlightShade_.setAlpha(128);

		/*	disabledTextColor_ is a transparent shade of textColor_ used for disabled buttons, disabled text boxes etc.
		placeholderTextColor_ is only used in textBoxes with placeholder text and is the same as disabledText (the background of the text box is different)
		neutralTextColor_ is used for a few controls like buttons in a neutral state. textColor_ is then used when hovered
		textBoxColor_ is a shade used for sunken areas such as textBoxes that contrasts with textColor_*/

		if (darkText_)
		{

			disabledTextColor_ = textColor_;
			disabledTextColor_.setAlpha(darkContrast_ * 3);
			placeholderTextColor_ = disabledTextColor_;

			neutralTextColor_ = textColor_.lighter(130);

			textBoxColor_ = lighterShade_;
		}
		else
		{
			neutralTextColor_ = textColor_.darker(130);

			disabledTextColor_ = textColor_;
			disabledTextColor_.setAlpha(lightContrast_ * 3);
			placeholderTextColor_ = disabledTextColor_;

			textBoxColor_ = darkerShade_;
		}

		/*	brightTextColor_ is a throwback to Qt Widgets intended for error/warning text but is not used anywhere in WGTF.
		It's default is bright red */

		brightTextColor_ = QColor(255, 0, 0);

		/*  These panel colors are used in WGSubPanel and a few other areas to provide additional contrast without using overlapping transparency

		darkHeaderColor_ is possibly a good option for creating a three color scheme if used carefully */
		darkHeaderColor_ = mainWindowColor_.darker(125);
		lightPanelColor_ = mainWindowColor_.lighter(132);

		/*	The following solid colors are not used often and are a bit of a throwback to Qt Widgets

		They are used in some areas where transparent colors do not render properly*/
		midDarkColor_ = mainWindowColor_.darker(110);
		midLightColor_ = mainWindowColor_.lighter(120);
		darkColor_ = mainWindowColor_.darker(150);


		/*	The overlay colors are used in the Overlay styles intended to be for transparent controls overlaid something like a viewport

		Editing them has no affect on most of the UI */
		overlayTextColor_ = QColor(0, 0, 0);

		overlayLightShade_ = QColor(255, 255, 255, 128);
		overlayLighterShade_ = QColor(255, 255, 255, 204);
		overlayDarkShade_ = QColor(0, 0, 0, 128);
		overlayDarkerShade_ = QColor(0, 0, 0, 204);


		/*	This was used to create an alternate styling on some controls but support for this has been discontinued */
		glowStyle_ = false;

		connect(this, SIGNAL(colorChanged()), this, SLOT(onColorChanged()));

		colorChanged();

		onPaletteChanged();
	}

	/*	This is a function to choose from a range of sample themes

		These themes aren't official in any way but are just some examples roughly based off the game UI's */
	

	void QtPalette::setTheme(Theme theme)
	{
		theme_ = theme;
		switch (theme)
		{
		case Dark:

			mainWindowColor_ = QColor(56, 58, 61);
			highlightColor_ = QColor(51, 153, 255);

			darkContrast_ = 32;
			lightContrast_ = 16;

			darkText_ = false;

			break;

		case Light:

			mainWindowColor_ = QColor(216, 216, 216);
			highlightColor_ = QColor(67, 139, 191);

			darkContrast_ = 16;
			lightContrast_ = 32;

			darkText_ = true;

			break;

		case BattleRed:

			mainWindowColor_ = QColor(51, 51, 51);
			highlightColor_ = QColor(176, 48, 44);

			darkContrast_ = 32;
			lightContrast_ = 16;

			darkText_ = false;

			break;

		case ArmyBrown:

			mainWindowColor_ = QColor(46, 44, 39);
			highlightColor_ = QColor(229, 161, 57);

			darkContrast_ = 32;
			lightContrast_ = 16;

			darkText_ = false;

			break;

		case AirForceGreen:

			mainWindowColor_ = QColor(39, 57, 61);
			highlightColor_ = QColor(57, 178, 207);

			darkContrast_ = 32;
			lightContrast_ = 16;

			darkText_ = false;

			break;

		case NavyBlue:

			mainWindowColor_ = QColor(29, 51, 64);
			highlightColor_ = QColor(21, 163, 210);

			darkContrast_ = 32;
			lightContrast_ = 16;

			darkText_ = false;

			break;

		case Custom:

			mainWindowColor_ = customWindowColor_;
			highlightColor_ = customHighlightColor_;

			break;

		default:
			break;
		}

		generateStyle();

		emit themeChanged(theme_);
	}

	void QtPalette::onColorChanged()
	{
		// Start a timer so we update our palette on the next update
		killTimer(timerid_);
		timerid_ = startTimer(0);
	}

	void QtPalette::timerEvent(QTimerEvent* event)
	{
		if (timerid_ == event->timerId())
		{
			onPaletteChanged();
			killTimer(timerid_);
			timerid_ = 0;
		}
		QQuickItem::timerEvent(event);
	}


	//Assigning QT Widgets Palette Colors. Do not recommend editing these, they have unpredictable results
	QPalette QtPalette::toQPalette() const
	{
		QPalette palette;

		palette.setColor(QPalette::Window, mainWindowColor_);
		palette.setColor(QPalette::WindowText, neutralTextColor_);
		palette.setColor(QPalette::Base, midDarkColor_);
		palette.setColor(QPalette::AlternateBase, mainWindowColor_);
		palette.setColor(QPalette::ToolTipBase, toolTipColor_);
		palette.setColor(QPalette::Text, textColor_);
		palette.setColor(QPalette::ToolTipText, toolTipTextColor_);
		palette.setColor(QPalette::Button, mainWindowColor_);
		palette.setColor(QPalette::ButtonText, neutralTextColor_);
		palette.setColor(QPalette::BrightText, brightTextColor_);
		palette.setColor(QPalette::Link, highlightColor_);
		palette.setColor(QPalette::Light, lightPanelColor_);
		palette.setColor(QPalette::Midlight, midLightColor_);
		palette.setColor(QPalette::Mid, darkHeaderColor_);
		palette.setColor(QPalette::Dark, darkColor_);
		palette.setColor(QPalette::Highlight, highlightColor_);
		palette.setColor(QPalette::HighlightedText, highlightTextColor_);
		palette.setColor(QPalette::Disabled, QPalette::Window, mainWindowColor_);
		palette.setColor(QPalette::Disabled, QPalette::WindowText, disabledTextColor_);
		palette.setColor(QPalette::Disabled, QPalette::Base, midDarkColor_);
		palette.setColor(QPalette::Disabled, QPalette::AlternateBase, mainWindowColor_);
		palette.setColor(QPalette::Disabled, QPalette::ToolTipBase, toolTipColor_);
		palette.setColor(QPalette::Disabled, QPalette::Text, disabledTextColor_);
		palette.setColor(QPalette::Disabled, QPalette::Button, mainWindowColor_);
		palette.setColor(QPalette::Disabled, QPalette::ButtonText, disabledTextColor_);
		palette.setColor(QPalette::Disabled, QPalette::BrightText, brightTextColor_);
		palette.setColor(QPalette::Disabled, QPalette::Link, disabledTextColor_);
		palette.setColor(QPalette::Disabled, QPalette::Highlight, mainWindowColor_);
		palette.setColor(QPalette::Disabled, QPalette::HighlightedText, disabledTextColor_);

		return palette;
	}

	void QtPalette::onPaletteChanged()
	{
		QPalette palette;
		palette.setColor(QPalette::Window, mainWindowColor_);
		palette.setColor(QPalette::WindowText, neutralTextColor_);
		palette.setColor(QPalette::Base, midDarkColor_);
		palette.setColor(QPalette::AlternateBase, mainWindowColor_);
		palette.setColor(QPalette::ToolTipBase, toolTipColor_);
		palette.setColor(QPalette::Text, textColor_);
		palette.setColor(QPalette::ToolTipText, toolTipTextColor_);
		palette.setColor(QPalette::Button, mainWindowColor_);
		palette.setColor(QPalette::ButtonText, neutralTextColor_);
		palette.setColor(QPalette::BrightText, brightTextColor_);
		palette.setColor(QPalette::Link, highlightColor_);
		palette.setColor(QPalette::Light, lightPanelColor_);
		palette.setColor(QPalette::Midlight, midLightColor_);
		palette.setColor(QPalette::Mid, darkHeaderColor_);
		palette.setColor(QPalette::Dark, darkColor_);

		palette.setColor(QPalette::Highlight, highlightColor_);
		palette.setColor(QPalette::HighlightedText, highlightTextColor_);

		// Disabled
		palette.setColor(QPalette::Disabled, QPalette::Window, mainWindowColor_);
		palette.setColor(QPalette::Disabled, QPalette::WindowText, disabledTextColor_);
		palette.setColor(QPalette::Disabled, QPalette::Base, midDarkColor_);
		palette.setColor(QPalette::Disabled, QPalette::AlternateBase, mainWindowColor_);
		palette.setColor(QPalette::Disabled, QPalette::ToolTipBase, toolTipColor_);
		palette.setColor(QPalette::Disabled, QPalette::Text, disabledTextColor_);
		palette.setColor(QPalette::Disabled, QPalette::Button, mainWindowColor_);
		palette.setColor(QPalette::Disabled, QPalette::ButtonText, disabledTextColor_);
		palette.setColor(QPalette::Disabled, QPalette::BrightText, brightTextColor_);
		palette.setColor(QPalette::Disabled, QPalette::Link, disabledTextColor_);

		palette.setColor(QPalette::Disabled, QPalette::Highlight, mainWindowColor_);
		palette.setColor(QPalette::Disabled, QPalette::HighlightedText, disabledTextColor_);

		if (qApp)
		{
			qApp->setPalette(palette);
			// Not sure why the signal isn't emitted when setPalette is called need to manually emit the signal
			emit qApp->paletteChanged(palette);
		}
	}
} // end namespace wgt
