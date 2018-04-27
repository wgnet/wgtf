#ifndef REFLECTED_ENUM_MODEL_HPP
#define REFLECTED_ENUM_MODEL_HPP

#include "core_data_model/i_list_model.hpp"
#include <vector>

namespace wgt
{
class PropertyAccessor;
class MetaEnumObj;
template< class T > class ObjectHandleT;

class ReflectedEnumModel : public IListModel
{
public:
	ReflectedEnumModel(const MetaEnumObj* enumObj);
	ReflectedEnumModel(const PropertyAccessor&, ObjectHandleT<MetaEnumObj> enumObj);
	virtual ~ReflectedEnumModel();

	IItem* item(size_t index) const override;
	size_t index(const IItem* item) const override;

	bool empty() const override;
	size_t size() const override;
	int columnCount() const override;

private:
	std::vector<IItem*> items_;
};
} // end namespace wgt
#endif // REFLECTED_ENUM_MODEL_HPP
