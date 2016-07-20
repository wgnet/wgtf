#pragma once

#include "interfaces/i_item_model_controller.hpp"
#include "core_dependency_system/di_ref.hpp"
#include "core_dependency_system/i_interface.hpp"

namespace wgt
{
class ICommandManager;
class IComponentContext;
class IDefinitionManager;

class ItemModelController
	: public Implements< IItemModelController >
{
public:
	ItemModelController( IComponentContext & context );

	virtual bool setValue( AbstractItemModel & model,
		const AbstractItemModel::ItemIndex & index,
		size_t roleId,
		const Variant & data ) override;

private:
	DIRef< ICommandManager > commandManager_;
	DIRef< IDefinitionManager > definitionManager_;
};

} // end namespace wgt
