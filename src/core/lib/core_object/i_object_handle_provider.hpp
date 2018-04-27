#pragma once

namespace wgt
{
class ObjectHandle;
class IDefinitionManager;

class IObjectHandleProvider
{
public:
    virtual ~IObjectHandleProvider() {};

private:
    friend class ObjectManager;
    virtual void setHandle(const ObjectHandle& handle, const IDefinitionManager& manager) = 0;
};
} // end namespace wgt
