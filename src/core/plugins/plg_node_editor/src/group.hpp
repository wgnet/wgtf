#ifndef __GROUP_H__
#define __GROUP_H__

#include "interfaces/i_group.hpp"

#include "core_dependency_system/i_interface.hpp"
#include "wg_types/vector4.hpp"

namespace wgt
{
class INode;

class Group : public Implements<IGroup>
{
	DECLARE_REFLECTED
public:
	Group();
	Group(const Vector4& rectangle, const std::string& name, const Vector4& color);
	virtual ~Group();

	virtual const Vector4& getRectangle() const override;
	virtual void setRectangle(const Vector4& value) override;
	virtual const std::string& getName() const override;
	virtual void setName(const std::string& value) override;
	virtual const Vector4& getColor() const override;
	virtual void setColor(const Vector4& value) override;

private:
	Vector4 m_rectangle;
	std::string m_name;
	Vector4 m_color;
};

} // end namespace wgt
#endif // __GROUP_H__
