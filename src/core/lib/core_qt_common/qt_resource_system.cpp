#include "qt_resource_system.hpp"

#include "core_common/assert.hpp"
#include "wg_types/binary_block.hpp"
#include "core_logging/logging.hpp"
#include <QFile>

namespace wgt
{
QtResourceSystem::QtResourceSystem()
{
}

QtResourceSystem::~QtResourceSystem()
{
}

bool QtResourceSystem::exists(const char* resource) const
{
	if (!QFile::exists(resource))
	{
		return false;
	}
	return true;
}

IResourceSystem::BinaryBlockPtr QtResourceSystem::readBinaryContent(const char* resource) const
{
	std::unique_ptr<QIODevice> device;
	if (!QFile::exists(resource))
	{
		return nullptr;
	}
	device.reset(new QFile(resource));
	device->open(QFile::ReadOnly);

	TF_ASSERT(device != nullptr);
	auto size = device->size();
	auto data = device->readAll();
	device->close();
	if (data.isEmpty())
	{
		NGT_WARNING_MSG("Read action data error from %s.\n", resource);
		return nullptr;
	}
	auto buffer = data.constData();

	return std::unique_ptr<BinaryBlock>(new BinaryBlock(buffer, size, false));
}

} // end namespace wgt
