#ifndef I_METHOD_HPP
#define I_METHOD_HPP

#include <cstddef>

#include "core_reflection/reflection_dll.hpp"

namespace wgt
{
class ObjectHandle;
class IDefinitionManager;
class ReflectedMethodParameters;
class Variant;

class REFLECTION_DLL IMethod
{
public:
	virtual ~IMethod() {}
	virtual Variant invoke(
		const ObjectHandle& object,
		const IDefinitionManager & definitionManager,
		const ReflectedMethodParameters& parameters ) = 0;
	// TODO return a collection of the arg types
	virtual std::size_t parameterCount() const = 0;
};
} // end namespace wgt
#endif // I_METHOD_HPP
