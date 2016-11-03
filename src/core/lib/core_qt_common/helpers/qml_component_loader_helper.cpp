#include "qml_component_loader_helper.hpp"

#include <sstream>
#include "core_common/signal.hpp"
#include "core_logging/logging.hpp"

#include <QQmlComponent>

namespace wgt
{
namespace QmlComponentLoaderHelperPrivate
{
	//------------------------------------------------------------------------------
	void handleLoaded(QmlComponentLoaderHelper::QmlComponentLoaderHelperData * data )
	{
		std::stringstream stream;
		stream << "Loaded ";
		stream << data->urlString_;
		stream << " in ";
		stream << std::chrono::duration_cast<std::chrono::seconds>(
			(std::chrono::high_resolution_clock::now() - data->startTime_)).count();
		stream << " s." << std::endl;
		auto endValue = stream.str();
		NGT_TRACE_MSG("%s", endValue.c_str());
		data->sig_Loaded_( data->qmlComponent_ );
	}
}

//------------------------------------------------------------------------------
QmlComponentLoaderHelper::QmlComponentLoaderHelper(
	QQmlComponent * qmlComponent,
	const QUrl & url )
{
	//Will be deleted when component fails / succeeds to load
	auto data = new QmlComponentLoaderHelperData();
	data_ = data;
	data->qmlComponent_ = qmlComponent;

	QObject::connect(
		data->qmlComponent_, &QQmlComponent::statusChanged,
		[data](QQmlComponent::Status status )
	{
		switch (status)
		{
		case QQmlComponent::Error:
			NGT_WARNING_MSG("Error loading control %s\n",
				qPrintable(data->qmlComponent_->errorString()));
            data->sig_Error_( data->qmlComponent_ );
			delete data;
			return;

		case QQmlComponent::Ready:
			QmlComponentLoaderHelperPrivate::handleLoaded( data );
			delete data;
			return;
		}
	});

	data->startTime_ = std::chrono::high_resolution_clock::now();
	data->url_ = url;
	data->urlString_ = url.fileName().toUtf8().constData();
	NGT_TRACE_MSG("Queuing load of %s\n", data->urlString_.c_str() );
}

//------------------------------------------------------------------------------
void QmlComponentLoaderHelper::load( bool async )
{
	data_->qmlComponent_->loadUrl( data_->url_, async ? QQmlComponent::Asynchronous : QQmlComponent::PreferSynchronous );
}
} // end namespace wgt
