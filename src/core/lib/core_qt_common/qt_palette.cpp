#include "qt_palette.hpp"
#include <QPalette>
#include <QColor>
#include <QtWidgets/QApplication>
#include <QtWidgets/QStyleFactory>

namespace wgt
{
QtPalette::QtPalette(QQuickItem* parent)
	: QQuickItem( parent ), timerid_( 0 )
{
	// Use the Fusion style instead default OS style
	if (qApp)
	{
		qApp->setDesktopSettingsAware(false);
		qApp->setStyle(QStyleFactory::create("Fusion"));
		QGuiApplication::setFont(QFont("Noto Sans", 9));
	}

	mainWindowColor_ = QColor( 56, 58, 61 );
	highlightColor_ = QColor( 51, 153, 255 );
	highlightTextColor_ = QColor( 255, 255, 255 );

	toolTipColor_ = highlightColor_;
	toolTipColor_.setAlpha( 220 );

	textColor_ = QColor( 255, 255, 255 );

	overlayTextColor_ = QColor( 0, 0, 0 );

	darkText_ = false;
	glowStyle_ = false;

	toolTipTextColor_ = QColor( 255, 255, 255, 220 );
	brightTextColor_ = QColor( 255, 0, 0 );

	lightShade_ = QColor( 255, 255, 255, 16 );
	lighterShade_ = QColor( 255, 255, 255, 32 );
	lightestShade_ = QColor( 255, 255, 255, 48 );
	darkShade_ = QColor( 0, 0, 0, 32 );
	darkerShade_ = QColor( 0, 0, 0, 64 );
	darkestShade_ = QColor( 0, 0, 0, 96 );
	
	overlayLightShade_ = QColor( 255, 255, 255, 128 );
	overlayLighterShade_ = QColor( 255, 255, 255, 204 );
	overlayDarkShade_ = QColor( 0, 0, 0, 128 );
	overlayDarkerShade_ = QColor( 0, 0, 0, 204 );

	highlightShade_ = highlightColor_;
	highlightShade_.setAlpha( 128 );

	darkHeaderColor_ = mainWindowColor_.darker( 125 );
	lightPanelColor_ = mainWindowColor_.lighter( 132 );

	midDarkColor_ = mainWindowColor_.darker( 110 );
	midLightColor_ = mainWindowColor_.lighter( 120 );
	darkColor_ = mainWindowColor_.darker( 150 );

	if (darkText_)
	{
		neutralTextColor_ = textColor_.lighter( 130 );
		disabledTextColor_ = darkestShade_;
		textBoxColor_ = lightestShade_;
		placeholderTextColor_ = darkestShade_;
	}
	else
	{
		neutralTextColor_ = textColor_.darker( 130 );
		disabledTextColor_ = lightestShade_;
		textBoxColor_ = darkerShade_;
		placeholderTextColor_ = lightestShade_;
	}

	onPaletteChanged();
}

QtPalette::QtPalette(QPalette& palette)
	: timerid_(0)
{
	// Use the Fusion style instead default OS style
	if (qApp)
	{
		qApp->setDesktopSettingsAware(false);
		qApp->setStyle(QStyleFactory::create("Fusion"));
		QGuiApplication::setFont(QFont("Noto Sans", 9));
	}

	mainWindowColor_ = palette.color(QPalette::Window);
	highlightColor_ = palette.color(QPalette::Link);
	highlightTextColor_ = palette.color(QPalette::HighlightedText);

	toolTipColor_ = palette.color(QPalette::ToolTipBase);
	toolTipColor_.setAlpha(220);

	textColor_ = palette.color(QPalette::Text);

	overlayTextColor_ = QColor( 0, 0, 0 );

	glowStyle_ = false;

	toolTipTextColor_ = palette.color(QPalette::ToolTipText);
	brightTextColor_ = palette.color(QPalette::BrightText);

	lightShade_ = QColor(255, 255, 255, 16);
	lighterShade_ = QColor(255, 255, 255, 32);
	lightestShade_ = QColor(255, 255, 255, 48);
	darkShade_ = QColor(0, 0, 0, 32);
	darkerShade_ = QColor(0, 0, 0, 64);
	darkestShade_ = QColor(0, 0, 0, 96);
	
	overlayLightShade_ = QColor( 255, 255, 255, 128 );
	overlayLighterShade_ = QColor( 255, 255, 255, 204 );
	overlayDarkShade_ = QColor( 0, 0, 0, 128 );
	overlayDarkerShade_ = QColor( 0, 0, 0, 204 );

	highlightShade_ = highlightColor_;
	highlightShade_.setAlpha(128);

	darkHeaderColor_ = palette.color(QPalette::Mid);
	lightPanelColor_ = palette.color(QPalette::Light);

	midDarkColor_ = palette.color(QPalette::Base);
	midLightColor_ = palette.color(QPalette::Midlight);
	darkColor_ = palette.color(QPalette::Dark);

	neutralTextColor_ = palette.color(QPalette::WindowText);
	disabledTextColor_ = palette.color(QPalette::Disabled, QPalette::WindowText);

	darkText_ = disabledTextColor_ == darkestShade_;

	if (darkText_)
	{
		textBoxColor_ = lightestShade_;
		placeholderTextColor_ = darkestShade_;
	}
	else
	{
		textBoxColor_ = darkerShade_;
		placeholderTextColor_ = lightestShade_;
	}

	onPaletteChanged();
}


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

void QtPalette::setTheme(Theme theme)
{
	theme_ = theme;
	switch (theme)
	{
	case Dark:
		mainWindowColor_ = "#383a3d";
		highlightColor_ = "#3399ff";
		textColor_ = "#FFFFFF";

		lightShade_ = "#10FFFFFF";
		lighterShade_ = "#20FFFFFF";
		lightestShade_ = "#30FFFFFF";

		darkShade_ = "#20000000";
		darkerShade_ = "#40000000";
		darkestShade_ = "#60000000";

		highlightShade_ = QColor::fromRgbF(highlightColor_.redF(), highlightColor_.greenF(), highlightColor_.blueF(), 0.5);

		darkHeaderColor_ = mainWindowColor_.darker(125);
		lightPanelColor_ = mainWindowColor_.lighter(132);

		midDarkColor_ = mainWindowColor_.darker(110);
		midLightColor_ = mainWindowColor_.lighter(120);
		darkColor_ = mainWindowColor_.darker(150);

		neutralTextColor_ = textColor_.darker(130);
		disabledTextColor_ = lightestShade_;
		textBoxColor_ = darkerShade_;
		placeholderTextColor_ = lightestShade_;

		glowStyle_ = false;
		break;

	case Light:
		mainWindowColor_ = "#D8D8D8";
		highlightColor_ = "#438bbf";
		textColor_ = "#000000";

		lightShade_ = "#20FFFFFF";
		lighterShade_ = "#50FFFFFF";
		lightestShade_ = "#80FFFFFF";

		darkShade_ = "#20000000";
		darkerShade_ = "#35000000";
		darkestShade_ = "#50000000";

		highlightShade_ = QColor::fromRgbF(highlightColor_.redF(), highlightColor_.greenF(), highlightColor_.blueF(), 0.5);

		darkHeaderColor_ = "#5693bf";
		lightPanelColor_ = "#E8E8E8";

		midDarkColor_ = mainWindowColor_.darker(110);
		midLightColor_ = mainWindowColor_.lighter(120);
		darkColor_ = mainWindowColor_.darker(160);

		neutralTextColor_ = textColor_.lighter(130);
		disabledTextColor_ = darkestShade_;
		textBoxColor_ = lightestShade_;
		placeholderTextColor_ = darkestShade_;

		glowStyle_ = false;
		break;

	case Wargaming:
		mainWindowColor_ = "#333333";
		highlightColor_ = "#b0302c";
		textColor_ = "#FFFFFF";

		lightShade_ = "#10FFFFFF";
		lighterShade_ = "#20FFFFFF";
		lightestShade_ = "#30FFFFFF";

		darkShade_ = "#20000000";
		darkerShade_ = "#40000000";
		darkestShade_ = "#60000000";

		highlightShade_ = QColor::fromRgbF(highlightColor_.redF(), highlightColor_.greenF(), highlightColor_.blueF(), 0.5);

		darkHeaderColor_ = mainWindowColor_.darker(125);
		lightPanelColor_ = mainWindowColor_.lighter(132);

		midDarkColor_ = mainWindowColor_.darker(110);
		midLightColor_ = mainWindowColor_.lighter(120);
		darkColor_ = mainWindowColor_.darker(150);

		neutralTextColor_ = textColor_.darker(130);
		disabledTextColor_ = lightestShade_;
		textBoxColor_ = darkerShade_;
		placeholderTextColor_ = lightestShade_;

		glowStyle_ = true;
		break;

	case WorldOfTanks:
		mainWindowColor_ = "#2e2c27";
		highlightColor_ = "#e5a139";
		textColor_ = "#FFFFFF";

		lightShade_ = "#10FFFFFF";
		lighterShade_ = "#20FFFFFF";
		lightestShade_ = "#30FFFFFF";

		darkShade_ = "#20000000";
		darkerShade_ = "#40000000";
		darkestShade_ = "#60000000";

		highlightShade_ = QColor::fromRgbF(highlightColor_.redF(), highlightColor_.greenF(), highlightColor_.blueF(), 0.5);

		darkHeaderColor_ = mainWindowColor_.darker(125);
		lightPanelColor_ = mainWindowColor_.lighter(132);

		midDarkColor_ = mainWindowColor_.darker(110);
		midLightColor_ = mainWindowColor_.lighter(120);
		darkColor_ = mainWindowColor_.darker(150);

		neutralTextColor_ = textColor_.darker(130);
		disabledTextColor_ = lightestShade_;
		textBoxColor_ = darkerShade_;
		placeholderTextColor_ = lightestShade_;

		glowStyle_ = true;
		break;

	case WorldOfWarplanes:
		mainWindowColor_ = "#27393d";
		highlightColor_ = "#39b2cf";
		textColor_ = "#FFFFFF";

		lightShade_ = "#10FFFFFF";
		lighterShade_ = "#20FFFFFF";
		lightestShade_ = "#30FFFFFF";

		darkShade_ = "#20000000";
		darkerShade_ = "#40000000";
		darkestShade_ = "#60000000";

		highlightShade_ = QColor::fromRgbF(highlightColor_.redF(), highlightColor_.greenF(), highlightColor_.blueF(), 0.5);

		darkHeaderColor_ = mainWindowColor_.darker(125);
		lightPanelColor_ = mainWindowColor_.lighter(132);

		midDarkColor_ = mainWindowColor_.darker(110);
		midLightColor_ = mainWindowColor_.lighter(120);
		darkColor_ = mainWindowColor_.darker(150);

		neutralTextColor_ = textColor_.darker(130);
		disabledTextColor_ = lightestShade_;
		textBoxColor_ = darkerShade_;
		placeholderTextColor_ = lightestShade_;

		glowStyle_ = true;
		break;

	case WorldOfWarships:
		mainWindowColor_ = "#1d3340";
		highlightColor_ = "#15a3d2";
		textColor_ = "#FFFFFF";

		lightShade_ = "#10FFFFFF";
		lighterShade_ = "#20FFFFFF";
		lightestShade_ = "#30FFFFFF";

		darkShade_ = "#20000000";
		darkerShade_ = "#40000000";
		darkestShade_ = "#60000000";

		highlightShade_ = QColor::fromRgbF(highlightColor_.redF(), highlightColor_.greenF(), highlightColor_.blueF(), 0.5);

		darkHeaderColor_ = mainWindowColor_.darker(125);
		lightPanelColor_ = mainWindowColor_.lighter(132);

		midDarkColor_ = mainWindowColor_.darker(110);
		midLightColor_ = mainWindowColor_.lighter(120);
		darkColor_ = mainWindowColor_.darker(150);

		neutralTextColor_ = textColor_.darker(130);
		disabledTextColor_ = lightestShade_;
		textBoxColor_ = darkerShade_;
		placeholderTextColor_ = lightestShade_;

		glowStyle_ = true;
		break;

	default:
		break;
	}

	glowChanged();
	emit themeChanged(theme_);
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

void QtPalette::onPaletteChanged()
{
	//Assigning QT Widgets Palette Colors.
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
