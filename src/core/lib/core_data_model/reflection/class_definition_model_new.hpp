#pragma once
#ifndef _CLASS_DEFINITION_MODEL_NEW_HPP
#define _CLASS_DEFINITION_MODEL_NEW_HPP

#include "core_data_model/abstract_item_model.hpp"

#include <memory>
#include <vector>

namespace wgt
{
class IClassDefinition;
class IDefinitionManager;

/**
 *	Expose IClassDefinition as a list model.
 */
class ClassDefinitionModelNew : public AbstractListModel
{
public:
	ClassDefinitionModelNew(const IClassDefinition* definition, const IDefinitionManager& definitionManager);
	virtual ~ClassDefinitionModelNew();

	virtual AbstractItem* item(int row) const override;
	virtual int index(const AbstractItem* item) const override;

	virtual int rowCount() const override;
	virtual int columnCount() const override;

private:
	std::vector<std::unique_ptr<AbstractItem>> items_;
};
} // end namespace wgt
#endif // _CLASS_DEFINITION_MODEL_NEW_HPP
