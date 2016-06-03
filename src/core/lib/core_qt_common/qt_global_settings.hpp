#ifndef QT_GLOBAL_SETTINGS_HPP
#define QT_GLOBAL_SETTINGS_HPP

#include <QtQuick/QQuickItem>

namespace wgt
{
class QtGlobalSettings: public QQuickItem
{
	Q_OBJECT

public:
	QtGlobalSettings()
	{
		dragLocked_ = true;
		wgCopyableEnabled_ = false;
		wgNativeRendering_ = false;
	}

	Q_PROPERTY( bool dragLocked MEMBER dragLocked_ NOTIFY dragLockedChanged )
	Q_PROPERTY( bool wgCopyableEnabled MEMBER wgCopyableEnabled_ NOTIFY wgCopyableEnabledChanged )
	Q_PROPERTY(bool wgNativeRendering MEMBER wgNativeRendering_ CONSTANT )

    void firePrePreferenceChangeEvent()
    {
        emit prePreferencesChanged();
    }

    void firePostPreferenceChangeEvent()
    {
        emit postPreferencesChanged();
    }

    void firePrePreferenceSavedEvent()
    {
        emit prePreferencesSaved();
    }

signals:
	void dragLockedChanged();
	void wgCopyableEnabledChanged();
    void prePreferencesChanged();
    void postPreferencesChanged();
    void prePreferencesSaved();

private:
	bool dragLocked_;
	bool wgCopyableEnabled_;
	bool wgNativeRendering_;
};
} // end namespace wgt
#endif
