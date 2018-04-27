#ifndef MACROS_MODEL_HPP
#define MACROS_MODEL_HPP

#include "core_data_model/collection_model.hpp"

namespace wgt
{
class IComponentContext;
/** Data model for wrapping a macro display info in a list model. */
class MacrosModel : public CollectionModel
{
public:
	MacrosModel();
	~MacrosModel();
	virtual AbstractItem* item(int index) const override;
};

/** Data model for wrapping a macro steps display info in a list model. */
class MacroStepsModel : public CollectionModel
{
public:
	MacroStepsModel();
	~MacroStepsModel();
	virtual AbstractItem* item(int index) const override;

	void iterateRoles(const std::function<void(const char*)>&) const override;
	virtual std::vector<std::string> roles() const override;
};
} // end namespace wgt
#endif // MACROS_MODEL_HPP
