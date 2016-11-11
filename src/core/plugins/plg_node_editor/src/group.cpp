#include "group.hpp"

namespace wgt
{
Group::Group() : m_rectangle(0.0f, 0.0f, 100.0f, 100.0f), m_name("New Group"), m_color(1.0f, 1.0f, 1.0f, 1.0f)
{
}

Group::Group(const Vector4& rectangle, const std::string& name, const Vector4& color)
    : m_rectangle(rectangle), m_name(name), m_color(color)
{
}

Group::~Group()
{
}

const Vector4& Group::getRectangle() const /* override */
{
	return m_rectangle;
}

void Group::setRectangle(const Vector4& value) /* override */
{
	m_rectangle = value;
}

const std::string& Group::getName() const /* override */
{
	return m_name;
}

void Group::setName(const std::string& value) /* override */
{
	m_name = value;
}

const Vector4& Group::getColor() const /* override */
{
	return m_color;
}

void Group::setColor(const Vector4& value) /* override */
{
	m_color = value;
}

} // end namespace wgt
