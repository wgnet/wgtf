#ifndef INTERNAL_CONTEXT_QUERY_HPP
#define INTERNAL_CONTEXT_QUERY_HPP

namespace wgt
{

class IComponentContext;
class IDefinitionManager;

//This functions are meant for core_reflection internal use only.
namespace ReflectionShared
{

extern IComponentContext & getComponentContext();
extern IDefinitionManager & getDefinitionManager();

}// end namespace ReflectionShared

} // end namespace wgt

#endif INTERNAL_CONTEXT_QUERY_HPP