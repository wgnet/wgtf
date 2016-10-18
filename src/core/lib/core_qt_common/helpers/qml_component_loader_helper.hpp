#ifndef QML_COMPONENT_LOADER_HELPER_H
#define QML_COMPONENT_LOADER_HELPER_H

#include "core_common/signal.hpp"
#include "core_qt_common/qt_connection_holder.hpp"

#include <chrono>
#include <QUrl>
#include <QQmlComponent>


namespace wgt
{
//==============================================================================
struct QmlComponentLoaderHelper
{
public:
	struct QmlComponentLoaderHelperData
	{
		typedef decltype (std::chrono::high_resolution_clock::now()) TimerCountType;
		TimerCountType startTime_;
		QQmlComponent * qmlComponent_;
		std::string urlString_;
		QUrl url_;
		Signal< void ( QQmlComponent * ) > sig_Loaded_;
        Signal< void ( QQmlComponent * ) > sig_Error_;
		ConnectionHolder connections_;
	};

	QmlComponentLoaderHelper(
		QQmlComponent * qmlComponent,
		const QUrl & url );

	void load( bool async );
	QmlComponentLoaderHelperData * data_;
};
} // end namespace wgt
#endif // QML_COMPONENT_LOADER_HELPER_H
