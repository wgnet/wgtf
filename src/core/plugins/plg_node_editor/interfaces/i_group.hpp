#ifndef __I_GROUP_H__
#define __I_GROUP_H__

#include "core_reflection/reflected_object.hpp"
#include <string>

namespace wgt
{
class Vector4;

/*!
* \class IGroup
*
* \brief Interface for grouping nodes together.
*/
class IGroup
{
	DECLARE_REFLECTED
public:
	virtual const Vector4& getRectangle() const = 0;
	virtual void setRectangle(const Vector4& value) = 0;
	virtual const std::string& getName() const = 0;
	virtual void setName(const std::string& value) = 0;
	virtual const Vector4& getColor() const = 0;
	virtual void setColor(const Vector4& value) = 0;
};

} // end namespace wgt
#endif // __I_GROUP_H__
