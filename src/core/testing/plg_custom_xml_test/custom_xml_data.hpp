#ifndef CUSTOM_XML_DATA_HPP
#define CUSTOM_XML_DATA_HPP
#include <string>
#include "tf_types/vector3.hpp"
#include "core_variant/variant.hpp"

namespace wgt
{
struct CustomXmlData
{
	std::string name_;
	std::string filename_;
	std::string createdBy_;
	bool visibility_;
	Vector3 position_;

	bool operator==(const CustomXmlData& tdo) const;
};

} // end namespace wgt

META_TYPE_NAME(wgt::CustomXmlData, "CustomXmlData")

#endif // CUSTOM_XML_DATA_HPP
