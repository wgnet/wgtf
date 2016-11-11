#ifndef QT_RESOURCE_SYSTEM_HPP
#define QT_RESOURCE_SYSTEM_HPP

#include "core_dependency_system/i_interface.hpp"
#include "core_serialization/i_resource_system.hpp"

namespace wgt
{
class QtResourceSystem : public Implements<IResourceSystem>
{
public:
	QtResourceSystem();
	virtual ~QtResourceSystem();

	virtual bool exists(const char* resource) const override;
	virtual BinaryBlockPtr readBinaryContent(const char* resource) const override;
};
} // end namespace wgt
#endif
