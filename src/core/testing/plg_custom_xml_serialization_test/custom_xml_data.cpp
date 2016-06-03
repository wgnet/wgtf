#include "custom_xml_data.hpp"

namespace wgt
{
bool CustomXmlData::operator==(const CustomXmlData& tdo) const
{
	if (name_ != tdo.name_)
		return false;

	if (filename_ != tdo.filename_)
		return false;
	
	if (createdBy_ != tdo.createdBy_)
		return false;

	if (visibility_ != tdo.visibility_)
		return false;

	if (position_ != tdo.position_)
		return false;

	return true;
}
} // end namespace wgt
