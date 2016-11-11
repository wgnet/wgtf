#ifndef META_TYPES_HPP
#define META_TYPES_HPP

#include "meta_base.hpp"

#include "../types/reflected_collection.hpp"
#include "../reflection_dll.hpp"
namespace wgt
{
class IEnumGenerator;
typedef std::unique_ptr<IEnumGenerator> IEnumGeneratorPtr;
typedef std::function<void(ObjectHandle)> Action;
typedef std::function<const wchar_t*(ObjectHandle)> GetTextCallback;
/**
 *	Deprecated: only for use with EXPOSE macros.
 *	Use nullptr for no metadata where possible.
 */
REFLECTION_DLL MetaHandle MetaNone();
REFLECTION_DLL MetaHandle MetaMinMax(float min, float max);
REFLECTION_DLL MetaHandle MetaStepSize(float stepSize);
REFLECTION_DLL MetaHandle MetaDecimals(int decimals);

#define MetaEnumFunc(getterFunc)                                                              \
	MetaEnum(std::unique_ptr<IEnumGenerator>(new ReflectedCollectionImpl(                     \
	IBasePropertyPtr(new FunctionProperty<std::map<int, std::wstring>, SelfType, true, true>( \
	"EnumTypes", &SelfType::getterFunc, NULL, TypeId::getType<std::map<int, std::wstring>>())))))

REFLECTION_DLL MetaHandle MetaEnum(IEnumGeneratorPtr enumGenerator);
REFLECTION_DLL MetaHandle MetaEnum(const wchar_t* enumString);

/**
* Notifies that the property is a slider
*/
REFLECTION_DLL MetaHandle MetaSlider();

REFLECTION_DLL MetaHandle MetaGroup(const wchar_t* groupName);

REFLECTION_DLL MetaHandle MetaAttributeDisplayName(const char* attributePath);

REFLECTION_DLL MetaHandle MetaDisplayName(const wchar_t* displayName);

REFLECTION_DLL MetaHandle MetaDisplayNameCallback(GetTextCallback callback);

/**
* User defined description
* @note used for displaying history for a reflected method call
*/
REFLECTION_DLL MetaHandle MetaDescription(const wchar_t* displayName);

REFLECTION_DLL MetaHandle MetaPanelLayout(const char* layoutFile, const char* bindingsFile = NULL);

REFLECTION_DLL MetaHandle MetaNoNull();

/**
* Notifies that the property is a colour
*/
REFLECTION_DLL MetaHandle MetaColor();

/**
* Allows large or blocking function calls to bypass the command system
* @note bypassing means no history is saved for the call
*/
REFLECTION_DLL MetaHandle MetaDirectInvoke();

/**
* Notifies that the property should not be displayed
*/
REFLECTION_DLL MetaHandle MetaHidden();

/**
* Notifies that the property is a thumbnail image
*/
REFLECTION_DLL MetaHandle MetaThumbnail(int width = -1, int height = -1);

REFLECTION_DLL MetaHandle MetaInPlace(const char* propertyName = NULL);

REFLECTION_DLL MetaHandle MetaSelected(const char* propertyName);

REFLECTION_DLL MetaHandle MetaInPlacePropertyName(const char* propertyName);

/**
* Notifies that the property cannot be interacted with
*/
REFLECTION_DLL MetaHandle MetaReadOnly();

REFLECTION_DLL MetaHandle MetaUrl(bool native, const std::string& title = "", const std::string& folder = "",
                                  const std::string& nameFilters = "", const std::string& selectedNameFilter = "");

class IMetaCommandExecutable
{
public:
	virtual ~IMetaCommandExecutable()
	{
	}
	virtual void execute(void* pBase, const void* arguments) const = 0;
};

template <typename SelfType>
class MetaCommandExecutableImpl : public IMetaCommandExecutable
{
public:
	typedef void (SelfType::*CommandExecuteFunc)(const void*);

	MetaCommandExecutableImpl(CommandExecuteFunc commandExecuteFunc) : commandExecuteFunc_(commandExecuteFunc)
	{
	}

	virtual void execute(void* pBase, const void* arguments) const
	{
		typedef void (SelfType::*ActualFuncSignature)(const void*);
		ActualFuncSignature actualFunc = reinterpret_cast<ActualFuncSignature>(commandExecuteFunc_);
		(((SelfType*)pBase)->*actualFunc)(arguments);
	}

private:
	CommandExecuteFunc commandExecuteFunc_;
};

REFLECTION_DLL MetaHandle MetaAction(const char* actionName, Action action);

#define MetaCommand(commandName, executeFunc)             \
	MetaCommandBase(                                      \
	commandName, new MetaCommandExecutableImpl<SelfType>( \
	             reinterpret_cast<MetaCommandExecutableImpl<SelfType>::CommandExecuteFunc>(&SelfType::executeFunc)))

REFLECTION_DLL MetaHandle MetaCommandBase(const wchar_t* commandName, const IMetaCommandExecutable* executeFunc);

REFLECTION_DLL MetaHandle MetaNoSerialization();

REFLECTION_DLL MetaHandle MetaUniqueId(const char* id);

REFLECTION_DLL MetaHandle MetaOnStack();

REFLECTION_DLL MetaHandle MetaUnique();

REFLECTION_DLL MetaHandle MetaParamHelp(const char* paramName, const MetaParamTypes::MetaParamType paramType,
                                        const char* paramDesc);

REFLECTION_DLL MetaHandle MetaReturnHelp(const char* returnName, const MetaParamTypes::MetaParamType returnType,
                                         const char* returnDesc);

REFLECTION_DLL MetaHandle MetaConsoleHelp(const char* text);

REFLECTION_DLL MetaHandle MetaScriptFunctionHelp(const char* name);

REFLECTION_DLL MetaHandle MetaTooltip(const char* tooltip);

REFLECTION_DLL MetaHandle MetaPassword();

REFLECTION_DLL MetaHandle MetaMultiline();

REFLECTION_DLL MetaHandle MetaOnPropertyChanged(Action action);
} // end namespace wgt
#endif // META_TYPES_HPP
