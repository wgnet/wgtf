#pragma once
#ifndef PYTHON_DEFINITION_HELPER_HPP
#define PYTHON_DEFINITION_HELPER_HPP

#include "core_reflection/interfaces/i_definition_helper.hpp"

namespace wgt
{
namespace ReflectedPython
{
	class DefinitionHelper : public IDefinitionHelper
	{
	public:
		virtual TypeId typeId() const override;

		virtual IClassDefinition * getDefinition( const ObjectHandle & object ) const override;
	};
}
} // end namespace wgt
#endif
