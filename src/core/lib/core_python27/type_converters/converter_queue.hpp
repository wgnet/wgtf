#pragma once
#ifndef _PYTHON_TYPE_CONVERTER_QUEUE_HPP
#define _PYTHON_TYPE_CONVERTER_QUEUE_HPP

#include "converters.hpp"
#include "default_converter.hpp"
#include "dict_converter.hpp"
#include "list_converter.hpp"
#include "primitive_converter.hpp"
#include "tuple_converter.hpp"
#include "none_converter.hpp"

#include <longintrepr.h>

#include <stack>
#include <string>

namespace wgt
{
namespace PythonType
{
/**
 *	Converts the name of a Python type to a C++ type.
 *	@param scriptObject script object from which to get the type.
 *	@return equivalent C++ type name, if supported by type converters.
 */
const TypeId& scriptTypeToTypeId(const PyScript::ScriptObject& scriptObject);

/**
 *	Queue for converting ScriptObject<->Variant.
 */
class ConverterQueue
{
public:
	ConverterQueue(IComponentContext& context);

	void init();
	void fini();

private:
	IComponentContext& context_;

	BasicTypeConverters basicTypeConverters_;
	ParentTypeConverters parentTypeConverters_;
	DefaultConverter defaultTypeConverter_;
	Converters allConverters_;

	NoneConverter noneTypeConverter_;
	PrimitiveConverter<int> intTypeConverter_;
	PrimitiveConverter<digit> longTypeConverter_;
	PrimitiveConverter<double> floatTypeConverter_;
	PrimitiveConverter<std::string> strTypeConverter_;
	PrimitiveConverter<std::wstring> unicodeTypeConverter_;
	ListConverter listTypeConverter_;
	TupleConverter tupleTypeConverter_;
	DictConverter dictTypeConverter_;

	InterfacePtr pTypeConvertersInterface_;
};

} // namespace PythonType
} // end namespace wgt
#endif // PYTHON_TYPE_CONVERTER_HPP
