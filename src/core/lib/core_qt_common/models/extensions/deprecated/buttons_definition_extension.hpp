#ifndef BUTTONS_DEFINITION_EXTENSION_HPP
#define BUTTONS_DEFINITION_EXTENSION_HPP

#include "i_model_extension_old.hpp"

namespace wgt
{

class ButtonsDefinitionExtension: public IModelExtensionOld
{
public:
    QHash<int, QByteArray> roleNames() const override;
    QVariant data(const QModelIndex& index, int role) const override;
    bool setData(const QModelIndex& index, const QVariant& value, int role) override;
};

} // namespace wgt

#endif