#ifndef I_COMPONENT_CONTEXT_CREATOR_HPP
#define I_COMPONENT_CONTEXT_CREATOR_HPP

namespace wgt
{
class IInterface;

class IComponentContextCreator
{
public:
	virtual ~IComponentContextCreator() {}
	virtual const char * getType() const = 0;
	virtual IInterface * createContext( const wchar_t * contextId ) = 0;
};
} // end namespace wgt
#endif ///I_COMPONENT_CONTEXT_CREATOR_HPP
