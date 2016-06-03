#ifndef CLASS_DEFINITION_MODEL_HPP
#define CLASS_DEFINITION_MODEL_HPP

#include "core_data_model/i_list_model.hpp"
#include <vector>

namespace wgt
{
class IClassDefinition;
class IDefinitionManager;

class ClassDefinitionModel : public IListModel
{
public:
	ClassDefinitionModel( const IClassDefinition * definition, const IDefinitionManager & definitionManager );
	virtual ~ClassDefinitionModel();

	IItem * item( size_t index ) const override;
	size_t index( const IItem * item ) const override;

	bool empty() const override;
	size_t size() const override;
	int columnCount() const override;

private:
	std::vector< IItem * > items_;
};
} // end namespace wgt
#endif // CLASS_DEFINITION_MODEL_HPP
