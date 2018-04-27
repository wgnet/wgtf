#ifndef META_TYPES_HPP
#define META_TYPES_HPP

#include "core_reflection/metadata/i_meta_type_creator.hpp"
#include "core_dependency_system/depends.hpp"
#include "../direct_base_helper.hpp"

namespace wgt
{
inline IMetaTypeCreator* metaTypeCreator()
{
	return DependsLocal<IMetaTypeCreator>().get<IMetaTypeCreator>();
}

#define MetaEnumFunc(getterFunc) \
	MetaEnum(std::unique_ptr<IEnumGenerator>(new EnumGenerator<SelfType>(&SelfType::getterFunc)))

#define MetaSignalFunc(getterFunc)                                                                      \
	MetaSignal(std::unique_ptr<ISignalProvider>(                                                        \
	new SignalProviderImpl(IBasePropertyPtr(new FunctionProperty<VariantSignal*, SelfType, true, true>( \
	"SignalTypes", &SelfType::getterFunc, NULL, TypeId::getType<VariantSignal*>())))))

#define MetaCommand(commandName, executeFunc)             \
	MetaCommandBase(                                      \
	commandName, new MetaCommandExecutableImpl<SelfType>( \
	reinterpret_cast<MetaCommandExecutableImpl<SelfType>::CommandExecuteFunc>(&SelfType::executeFunc)))

#define MetaDirectBases( ... )\
	MetaBasesHolder(\
		std::unique_ptr< IDirectBaseHelper >(\
			new DirectBaseHelperT< SelfType, __VA_ARGS__ >() ) )\
/**
 *	Deprecated: only for use with EXPOSE macros.
 *	Use nullptr for no metadata where possible.
 */
#define MetaNone() nullptr

/// <summary>
/// Indicates a minimum and maximum valid floating point value the associated property will accept.
/// <para>----------------------------------------------------------------------------------------------------</para>
/// <para>Example:</para>
/// <example>
/// <para>MetaMinMax(0.0f,360.0f)</para>
/// </example>
/// </summary>
/// <param name="min">The minimum value the property will accept.</param>
/// <param name="max">The maximum value the property will accept.</param>
inline MetaData MetaMinMax(float min, float max)
{
	return metaTypeCreator()->MetaMinMax(min, max);
}

/// <summary>
/// Indicates a minimum and maximum valid floating point value the associated property will accept.
/// The mapped values are presented in the UI but mapped to and stored in the range min to max.
/// <para>----------------------------------------------------------------------------------------------------</para>
/// <para>Example:</para>
/// <example>
/// <para>MetaMinMaxMapped(0.0f,100.0f, 0.0f, 1.0f)</para>
/// </example>
/// </summary>
/// <param name="min">The minimum value the property will store.</param>
/// <param name="max">The maximum value the property will store.</param>
/// <param name="mappedMin">The minimum value presented by the UI.</param>
/// <param name="mappedMax">The maximum value presented by the UI.</param>
inline MetaData MetaMinMaxMapped(float min, float max, float mappedMin, float mappedMax)
{
	return metaTypeCreator()->MetaMinMaxMapped(min, max, mappedMin, mappedMax);
}

/// <summary>
/// Indicates the value used by the UI to increment/decrement the current value when stepping the value up/down.
/// <para>----------------------------------------------------------------------------------------------------</para>
/// <para>Example:</para>
/// <example>
/// <para>MetaStepSize(0.01f)</para>
/// </example>
/// </summary>
/// <param name="stepSise">The value to increment/decrement by when stepping.</param>
inline MetaData MetaStepSize(float stepSize)
{
	return metaTypeCreator()->MetaStepSize(stepSize);
}

/// <summary>
/// Indicates the number of decimals digits of precision to use in the UI.
/// <para>----------------------------------------------------------------------------------------------------</para>
/// <para>Example:</para>
/// <example>
/// <para>MetaDecimals(3)</para>
/// </example>
/// </summary>
/// <param name="decimals">The decimal digits of precision to use.</param>
inline MetaData MetaDecimals(int decimals)
{
	return metaTypeCreator()->MetaDecimals(decimals);
}

/// <summary>
/// Indicates the value is an enum which will be presented as a drop down.
/// <para>----------------------------------------------------------------------------------------------------</para>
/// <para>Example:</para>
/// <example>
/// <para>MetaEnum&lt;MyEnum&gt;()</para>
/// </example>
/// </summary>
/// <typeparam name="TEnum">The enum type to use for the options in the drop down.</param>
template<typename TEnum>
inline MetaData MetaEnum()
{
	static_assert(std::is_enum<TEnum>::value, "Type is not an enum");
	return metaTypeCreator()->MetaEnum(std::make_unique<EnumGenerator<TEnum>>());
}

/// <summary>
/// Indicates the value is an enum which will be presented as a drop down.
/// Provide an implementation of the <see cref="IEnumGenerator"/> interface which takes an object
/// and returns a collection of items to be displayed by the drop down.
/// <para>----------------------------------------------------------------------------------------------------</para>
/// <para>Example:</para>
/// <example>
/// <para>MetaEnum(std::make_unique&lt;MyEnumGenerator&gt;())</para>
/// </example>
/// </summary>
/// <param name="enumGenerator">An implementation of the <see cref="IEnumGenerator"/> interface.</param>
inline MetaData MetaEnum(IEnumGeneratorPtr enumGenerator)
{
	return metaTypeCreator()->MetaEnum(std::move(enumGenerator));
}

/// <summary>
/// Indicates the value is an enum which will be presented as a drop down.
/// The provided string indicates the display text and optional values for the item.
/// <para>----------------------------------------------------------------------------------------------------</para>
/// <para>Examples:</para>
/// <example>
/// <para>MetaEnum("Option 1|Option 2|Option3")</para>
/// <para>MetaEnum("Option 1=1|Option 2=2|Option 3=4")</para>
/// </example>
/// </summary>
/// <param name="enumString">A string of display text with optional values to be presented in the drop down.</param>
inline MetaData MetaEnum(const wchar_t* enumString)
{
	return metaTypeCreator()->MetaEnum(enumString);
}

/// <summary>
/// DEPRECATED Used by the BEGIN_EXPOSE_3 macro to indicate the base classes of a reflected type.
/// <para>----------------------------------------------------------------------------------------------------</para>
/// <para>Example:</para>
/// </summary>
inline MetaData MetaBasesHolder(std::unique_ptr< IDirectBaseHelper > & impl)
{
	return metaTypeCreator()->MetaBasesHolder( impl );
}

/// <summary>
/// Indicates the value is to be presented as a slider.
/// Often this will be used with <see cref="MetaMinMax"/> to indicate a range for the slider.
/// <para>----------------------------------------------------------------------------------------------------</para>
/// <para>Example:</para>
/// <example>
/// <para>MetaSlider()</para>
/// </example>
/// </summary>
inline MetaData MetaSlider()
{
	return metaTypeCreator()->MetaSlider();
}

/// <summary>
/// Indicates if the associated property/type is to be grouped under the specified name.
/// <para>----------------------------------------------------------------------------------------------------</para>
/// <para>Example:</para>
/// <example>
/// <para>MetaGroup(L"My Group")</para>
/// </example>
/// </summary>
/// <param name="groupName">The name of the group to which the property/type will be added.</param>
inline MetaData MetaGroup(const wchar_t* groupName)
{
	return metaTypeCreator()->MetaGroup(groupName);
}

/// <summary>
/// Indicates the dynamic name of the group to which the associated property/type will be added.
/// The provided callback takes an object and returns a string.
/// <para>----------------------------------------------------------------------------------------------------</para>
/// <para>Example:</para>
/// <example>
/// <para>MetaGroup([](const ObjectHandle&amp; object) { return MyGroupName(object); })</para>
/// </example>
/// </summary>
/// <param name="callback">The callback function used to retrieve the dyanmic group name to which the property will be added.</param>
inline MetaData MetaGroupCallback(const GetTextCallback& callback)
{
	return metaTypeCreator()->MetaGroupCallback(callback);
}

/// <summary>
/// Indicates the attribute to use as the display name for the associated property.
/// The attribute path is used to bind and invoke the associated property to retrieve the display name.
/// <para>----------------------------------------------------------------------------------------------------</para>
/// <para>Example:</para>
/// <example>
/// <para>MetaAttributeDisplayName(L"myDisplayName")</para>
/// </example>
/// </summary>
/// <param name="attribtePath">The attribute path to bind and invoke on the object to retrieve the display name.</param>
inline MetaData MetaAttributeDisplayName(const char* attributePath)
{
	return metaTypeCreator()->MetaAttributeDisplayName(attributePath);
}

/// <summary>
/// Indicates a user friendly name to display for the associated property/type.
/// <para>----------------------------------------------------------------------------------------------------</para>
/// <para>Example:</para>
/// <example>
/// <para>MetaDisplayName(L"Display Name")</para>
/// </example>
/// </summary>
/// <param name="displayName">The static display name to show in the UI.</param>
inline MetaData MetaDisplayName(const wchar_t* displayName)
{
	return metaTypeCreator()->MetaDisplayName(displayName);
}

/// <summary>
/// Indicates a dynamic user friendly name to display for the associated property/type.
/// <para>----------------------------------------------------------------------------------------------------</para>
/// <para>Example:</para>
/// <example>
/// <para>MetaDisplayName([](const ObjectHandle&amp; object) { return MyDisplayName(object); })</para>
/// </example>
/// </summary>
/// <param name="callback">A callback function that takes an object and returns a string to show in the UI.</param>
inline MetaData MetaDisplayNameCallback(GetTextCallback callback)
{
	return metaTypeCreator()->MetaDisplayNameCallback(callback);
}

/// <summary>
/// Indicates an icon source filename to use for a toolbar button or tool in the tool box.
/// <para>----------------------------------------------------------------------------------------------------</para>
/// <para>Example:</para>
/// <example>
/// <para>MetaIcon(L"qrc:/plg_my_plugin/icons/MyIcon.png"")</para>
/// </example>
/// </summary>
/// <param name="icon">The path to the icon file to use for the toolbar button or tool.</param>
inline MetaData MetaIcon(const char* icon)
{
	return metaTypeCreator()->MetaIcon(icon);
}

/// <summary>
/// Indicates a callback function that takes a property path and the reflected object to retrieve a display name.
/// Similar to <see cref="MetaAttributeDisplayName"/>, but allows the callback to handle how the path is used.
/// <para>----------------------------------------------------------------------------------------------------</para>
/// <para>Example:</para>
/// <example>
/// <para>MetaDisplayPathNameCallback([](const std::string&amp; path, const ObjectHandle&amp; object){ return MyDisplayFromPath(path, object); })</para>
/// </example>
/// </summary>
/// <param name="callback">The callback function taking the path and an object and returns a display name as a string.</param>
inline MetaData MetaDisplayPathNameCallback(GetPathTextCallback callback)
{
	return metaTypeCreator()->MetaDisplayPathNameCallback(callback);
}

/// <summary>
/// Indicates the property's value should display as a static localized string and is stored as an integer identifier.
/// <para>----------------------------------------------------------------------------------------------------</para>
/// <para>Example:</para>
/// <example>
/// <para>MetaStaticString()</para>
/// </example>
/// </summary>
inline MetaData MetaStaticString()
{
	return metaTypeCreator()->MetaStaticString();
}

/// <summary>
/// Indicates the description to be displayed as part of the tooltip for the associated property.
/// Used for displaying history for a reflected method call.
/// <para>----------------------------------------------------------------------------------------------------</para>
/// <para>Example:</para>
/// <example>
/// <para>MetaDescription(L"My description")</para>
/// </example>
/// </summary>
inline MetaData MetaDescription(const wchar_t* displayName)
{
	return metaTypeCreator()->MetaDescription(displayName);
}

inline MetaData MetaPanelLayout(const char* layoutFile, const char* bindingsFile = NULL)
{
	return metaTypeCreator()->MetaPanelLayout(layoutFile, bindingsFile);
}

inline MetaData MetaNoNull()
{
	return metaTypeCreator()->MetaNoNull();
}

/// <summary>
/// Indicates the property should be presented as a color, displaying the RGBA values and a color picker.
/// <para>----------------------------------------------------------------------------------------------------</para>
/// <para>Example:</para>
/// <example>
/// <para>MetaColor()</para>
/// </example>
/// </summary>
inline MetaData MetaColor()
{
	return metaTypeCreator()->MetaColor();
}


/// <summary>
/// Indicates the property should be presented in Kelvins.
/// Optionally specify a converter that converts an integer value into a four component color vector.
/// The color vector values should be floating point values between 0 and 1.
/// <para>----------------------------------------------------------------------------------------------------</para>
/// <para>Example:</para>
/// <example>
/// <para>MetaKelvinColor() // Use default converter </para>
/// <para>MetaKelvinColor([](int value){ return ConvertKelvinToRGB((float)value); })</para>
/// </example>
/// </summary>
/// <param name="converter">A callback function used to convert from Kelvin to a four component color vector.</param>
inline MetaData MetaKelvinColor(std::function<Vector4(int)> converter = nullptr)
{
	return metaTypeCreator()->MetaKelvinColor(converter);
}

/// <summary>
/// Indicates the property should be presented as a HDR color, displaying the RGBA values and a color picker.
/// Optionally specify a tonemap callback that takes a four component color vector and returns a new color vector.
/// <para>----------------------------------------------------------------------------------------------------</para>
/// <para>Examples:</para>
/// <example>
/// <para>MetaHDRColor()</para>
/// <para>MetaHDRColor([](const Vector4&amp; color){ return Tonemap(color); })</para>
/// </example>
/// </summary>
/// <param name="tonemapOperator">A callback function used to tonemap the four component color vector.</param>
inline MetaData MetaHDRColor(std::function<Vector4(const Vector4&)> tonemapOperator = nullptr)
{
	return metaTypeCreator()->MetaHDRColor(tonemapOperator);
}

/// <summary>
/// Indicates that the reflected property will bypass the command system, no Undo/Redo history is created.
/// <para>----------------------------------------------------------------------------------------------------</para>
/// <para>Example:</para>
/// <example>
/// <para>MetaDirectInvoke()</para>
/// </example>
/// </summary>
inline MetaData MetaDirectInvoke()
{
	return metaTypeCreator()->MetaDirectInvoke();
}

/// <summary>
/// Indicates that the property should not be displayed. Optionally specify a predicate callback to dynamically
/// update the visibility of the property.
/// <para>----------------------------------------------------------------------------------------------------</para>
/// <para>Examples:</para>
/// <example>
/// <para>MetaHidden()</para>
/// <para>MetaHidden([](const ObjectHandle&amp; obj) { return IsVisible(obj); } )</para>
/// </example>
/// </summary>
inline MetaData MetaHidden(const Predicate& predicate = Predicate())
{
	return metaTypeCreator()->MetaHidden(predicate);
}

/// <summary>
/// Indicates that the property is a thumbnail image. Optionally specify a width and height;
/// <para>----------------------------------------------------------------------------------------------------</para>
/// <para>Examples:</para>
/// <example>
/// <para>MetaThumbnail()</para>
/// <para>MetaThumbnail(32,32)</para>
/// </example>
/// </summary>
/// <param name="width">The width of the thumbnail.</para>
/// <param name="height">The height of the thumbnail.</para>
inline MetaData MetaThumbnail(int width = -1, int height = -1)
{
	return metaTypeCreator()->MetaThumbnail(width, height);
}

/// <summary>
/// Indicates that the object or container doesn't create a grouping in the UI. All reflected fields and elements
/// are presented in the hierarchy at the level the object would have been presented.
/// <para>----------------------------------------------------------------------------------------------------</para>
/// <para>Example:</para>
/// <example>
/// <para>MetaInPlace()</para>
/// </example>
/// </summary>
/// <param name="propertyName">DEPRECATED</param>

inline MetaData MetaInPlace(const char* propertyName = NULL)
{
	return metaTypeCreator()->MetaInPlace(propertyName);
}

/// <summary>
/// NOT IN USE - Indicates that the property is selected.
/// </summary>
inline MetaData MetaSelected(const char* propertyName)
{
	return metaTypeCreator()->MetaSelected(propertyName);
}

/// <summary>
/// NOT IN USE - Indicates the property to bind and invoke to retrieve the name of a property.
/// </summary>
inline MetaData MetaInPlacePropertyName(const char* propertyName)
{
	return metaTypeCreator()->MetaInPlacePropertyName(propertyName);
}

/// <summary>
/// Indicates that the property is read-only and cannot be modified.
/// Optionally specify a callback predicate to determine dynamically if the property is read-only.
/// <para>----------------------------------------------------------------------------------------------------</para>
/// <para>Examples:</para>
/// <example>
/// <para>MetaReadOnly()</para>
/// <para>MetaReadOnly([](const ObjectHandle&amp; obj) { return IsReadOnly(obj); } )</para>
/// </example>
/// </summary>
/// <param name="predicate">A callback that determines whether the property is read-only.</param>
inline MetaData MetaReadOnly(const Predicate& predicate = Predicate())
{
	return metaTypeCreator()->MetaReadOnly(predicate);
}

/// <summary>
/// Indicates the property is a url and presents a text entry with the ability to launch a browser dialog.
/// Must provide a boolean flag to indicate if a native file dialog or an asset browser dialog is to be presented.
/// Optionally specify a title, start folder, name filters, and a currently selected name filter.
/// <para>----------------------------------------------------------------------------------------------------</para>
/// <para>Examples:</para>
/// <example>
/// <para>MetaUrl(true, "Select a File")</para>
/// <para>MetaUrl(false, "Select an Asset")</para>
/// <para>MetaUrl(false, "Select an Asset", "", "My Files (*.ext *.ext2);;All Files (*.*)")</para>
/// </example>
/// </summary>
/// <param name="native">A flag indicating whether a native file dialog should be used. Otherwise an asset browser dialog is presented.</param>
/// <param name="title">The title of the dialog presented.</param>
/// <param name="folder">The start folder to use when the dialog is presented.</param>
/// <param name="nameFilters">A semi-colon separated string of filters. i.e. "Description (*.ext *.ext2);;Other (*.ext3)"</param>
/// <param name="selectedNameFilter">A string matching one of the filters in <paramref name="nameFilters"/>. i.e. "Description (*.ext *.ext2)"</param>
inline MetaData MetaUrl(bool native, 
						  const std::string& title = "", 
						  const std::string& folder = "",
						  const std::string& nameFilters = "", 
						  const std::string& selectedNameFilter = "")
{
	return metaTypeCreator()->MetaUrl(native, title, folder, nameFilters, selectedNameFilter);
}

/// <summary>
/// Indicates that a button with the provided name should be presented which will invoke the specified action when pressed.
/// <para>----------------------------------------------------------------------------------------------------</para>
/// <para>Example:</para>
/// <example>
/// <para>MetaAction("Do My Action", [](const ObjectHandle&amp; obj) { MyAction(obj); })</para>
/// </example>
/// </summary>
inline MetaData MetaAction(const char* actionName, Action action)
{
	return metaTypeCreator()->MetaAction(actionName, action);
}

/// <summary>
/// NOT IN USE
/// </summary>
inline MetaData MetaCommandBase(const wchar_t* commandName, const IMetaCommandExecutable* executeFunc)
{
	return metaTypeCreator()->MetaCommandBase(commandName, executeFunc);
}

/// <summary>
/// Indicates the specified registered component should be presented for this property.
/// Can be used to override the default component if the component doesn't require any additional input.
/// Components are registered through the <see cref="IUIFramework"/> interface.
/// <para>----------------------------------------------------------------------------------------------------</para>
/// <para>Example:</para>
/// <example>
/// <para>MetaComponent("mycomponent")</para>
/// </example>
/// </summary>
/// <param name="componentType">The type of registered component (i.e. "angle").</param>
/// <seealso cref="registerComponent"/>
inline MetaData MetaComponent(const char* componentType)
{
	return metaTypeCreator()->MetaComponent(componentType);
}

/// <summary>
/// Indicates that the object will not be serialized. <see cref="ObjectManager"/>'s saveObjects method uses this. 
/// <para>----------------------------------------------------------------------------------------------------</para>
/// <para>Example:</para>
/// <example>
/// <para>MetaNoSerialization()</para>
/// </example>
/// </summary>
inline MetaData MetaNoSerialization()
{
	return metaTypeCreator()->MetaNoSerialization();
}

/// <summary>
/// Indicates the unique id this object will use when creating a <see cref="ManagedObject"/>.
/// <para>----------------------------------------------------------------------------------------------------</para>
/// <para>Example:</para>
/// <example>
/// <para>MetaUniqueId("202949DF-25D5-408D-BD16-6DECC50E049B")</para>
/// </example>
/// </summary>
/// <param name="id">A unique identifier for the object.</param>
inline MetaData MetaUniqueId(const char* id)
{
	return metaTypeCreator()->MetaUniqueId(id);
}

/// <summary>
/// NOT IN USE - Indicates the property is created on the stack.
/// </summary>
inline MetaData MetaOnStack()
{
	return metaTypeCreator()->MetaOnStack();
}

/// <summary>
/// See Despair's Prop::Unique()
/// </summary>
inline MetaData MetaUnique()
{
	return metaTypeCreator()->MetaUnique();
}

/// <summary>
/// See Despair's Prop::ParamHelp()
/// </summary>
inline MetaData MetaParamHelp(const char* paramName, 
								const MetaParamTypes::MetaParamType paramType,
                                const char* paramDesc)
{
	return metaTypeCreator()->MetaParamHelp(paramName, paramType, paramDesc);
}

/// <summary>
/// See Despair's Prop::ReturnHelp()
/// </summary>
inline MetaData MetaReturnHelp(const char* returnName, 
								 const MetaParamTypes::MetaParamType returnType,
                                 const char* returnDesc)
{
	return metaTypeCreator()->MetaReturnHelp(returnName, returnType, returnDesc);
}

/// <summary>
/// See Despair's Prop::ConsoleHelp()
/// </summary>
inline MetaData MetaConsoleHelp(const char* text)
{
	return metaTypeCreator()->MetaConsoleHelp(text);
}

/// <summary>
/// See Despair's Prop::ScriptFunctionHelp()
/// </summary>
inline MetaData MetaScriptFunctionHelp(const char* name)
{
	return metaTypeCreator()->MetaScriptFunctionHelp(name);
}

/// <summary>
/// Indicates the tooltip text to display when hovered in the UI.
/// <para>----------------------------------------------------------------------------------------------------</para>
/// <para>Example:</para>
/// <example>
/// <para>MetaTooltip("My Custom Tooltip")</para>
/// </example>
/// </summary>
/// <param name="tooltip">The tooltip to display.</param>
inline MetaData MetaTooltip(const wchar_t* tooltip)
{
	return metaTypeCreator()->MetaTooltip(tooltip);
}

/// <summary>
/// Indicates the dynamic tooltip text to display when hovered in the UI.
/// <para>----------------------------------------------------------------------------------------------------</para>
/// <para>Example:</para>
/// <example>
/// <para>MetaTooltip([](const ObjectHandle&amp; obj) { return GetMyTooltip(obj); })</para>
/// </example>
/// </summary>
/// <param name="callback">The callback function taking an ObjectHandle and returning a wide string.</param>
inline MetaData MetaTooltip(const GetTextCallback& callback)
{
	return metaTypeCreator()->MetaTooltip(callback);
}

/// <summary>
/// Indicates a password text edit field should be displayed in the UI.
/// <para>----------------------------------------------------------------------------------------------------</para>
/// <para>Example:</para>
/// <example>
/// <para>MetaPassword()</para>
/// </example>
/// </summary>
inline MetaData MetaPassword()
{
	return metaTypeCreator()->MetaPassword();
}

/// <summary>
/// Indicates a checkbox should be displayed in the UI.
/// <para>----------------------------------------------------------------------------------------------------</para>
/// <para>Example:</para>
/// <example>
/// <para>MetaCheckBox()</para>
/// </example>
/// </summary>
inline MetaData MetaCheckBox()
{
	return metaTypeCreator()->MetaCheckBox();
}

/// <summary>
/// Indicates a multi-line text field should be displayed in the UI.
/// <para>----------------------------------------------------------------------------------------------------</para>
/// <para>Example:</para>
/// <example>
/// <para>MetaMultiline()</para>
/// </example>
/// </summary>
inline MetaData MetaMultiline()
{
	return metaTypeCreator()->MetaMultiline();
}

/// <summary>
/// Indicates a signal function in C++ that will be triggered when the property is modified.
/// Must provide and implementation of the ISignalProvider interface.
/// <para>----------------------------------------------------------------------------------------------------</para>
/// <para>Example:</para>
/// <example>
/// <para>MetaSignal(std::make_unique&lt;MySignalProvider&gt;())</para>
/// </example>
/// </summary>
/// <param name="signalProvider">An implemenation of the ISignalProvider interface to retrieve the signal function.</param>
inline MetaData MetaSignal(ISignalProviderPtr signalProvider)
{
	return metaTypeCreator()->MetaSignal(std::move(signalProvider));
}

/// <summary>
/// Indicates a numeric edit with a angle dial is to be presented in the UI.
/// Flags indicate if the value shall be displayed and stored in radians or degrees.
/// <para>----------------------------------------------------------------------------------------------------</para>
/// <para>Example:</para>
/// <example>
/// <para>MetaAngle(false, false)</para>
/// </example>
/// </summary>
/// <param name="storeRadians">Determines if the angle will be stored in radians or degrees.</param>
/// <param name="showRadians">Determines if the angle will be presented in radians or degrees.</param>
inline MetaData MetaAngle(bool storeRadians, bool showRadians)
{
	return metaTypeCreator()->MetaAngle(storeRadians, showRadians);
}

/// <summary>
/// Indicates a frame time control shall be displayed showing hours, minutes, seconds, and frames.
/// <para>----------------------------------------------------------------------------------------------------</para>
/// <para>Example:</para>
/// <example>
/// <para>MetaTime()</para>
/// </example>
/// </summary>
inline MetaData MetaTime()
{
	return metaTypeCreator()->MetaTime();
}

/// <summary>
/// Indicates that when this property is modified the object is invalidated, requiring the UI to refresh.
/// <para>----------------------------------------------------------------------------------------------------</para>
/// <para>Example:</para>
/// <example>
/// <para>MetaInvalidatesObject()</para>
/// </example>
/// </summary>
inline MetaData MetaInvalidatesObject()
{
	return metaTypeCreator()->MetaInvalidatesObject();
}

/// <summary>
/// Indicates an action callback to be invoked when this property is modified.
/// The callback function must take a const ObjectHandle&amp;
/// <para>----------------------------------------------------------------------------------------------------</para>
/// <para>Example:</para>
/// <example>
/// <para>MetaCallback(MyType::MyCallbackFunction)</para>
/// </example>
/// </summary>
/// <param name="action">The callback to invoke when the property is modified.</param>
inline MetaData MetaCallback(Action action)
{
	return metaTypeCreator()->MetaCallback(action);
}

/// <summary>
/// Indicates metadata stored for collection items.
/// <para>----------------------------------------------------------------------------------------------------</para>
/// <para>Example:</para>
/// <example>
/// <para>MetaCollectionItemMeta(MetaDisplayPathNameCallback(&GetDisplayName))</para>
/// </example>
/// </summary>
/// <param name="meta">The metadata to be used for the items in the collection.</param>
inline MetaData MetaCollectionItemMeta(MetaData meta)
{
	return metaTypeCreator()->MetaCollectionItemMeta(std::move(meta));
}

/// <summary>
/// Indicates the the property's value shall be converted to a variant for presenting in the UI.
/// Typically you'll want to provide a callback to transform the object into another type, such as a string.
/// </summary>
inline MetaData MetaEnableValueComponentForObject()
{
	return metaTypeCreator()->MetaEnableValueComponentForObject();
}

/// <summary>
/// Indicates the the property's value shall be converted to a variant for presenting in the UI.
/// Typically the callback will transform the object into another type, such as a string.
/// <para>----------------------------------------------------------------------------------------------------</para>
/// <para>Example:</para>
/// <example>
/// <para>MetaEnableValueComponentForObject(MyType::ConvertToStringVariant)</para>
/// </example>
/// </summary>
/// <param name="callback">The callback function that will convert the object to a variant.</param>
inline MetaData MetaEnableValueComponentForObject(std::function<Variant (const ObjectHandle&)> callback)
{
	return metaTypeCreator()->MetaEnableValueComponentForObject(callback);
}

/// <summary>
/// Indicates that the property should be automatically converted into an action to be shown in a menu or on a toolbar.
/// An action .xml with matching identifier can be loaded to specify the action details such as icon, shortcut, etc...
/// In plg_wgs_tools if <see cref="MetaIcon"/> is also provided a default action .xml will be generated
/// and loaded automatically when creating actions for use in the toolbox.
/// The generated xml will be overridden by plg_wgs_tools/resources/actions.xml.
/// <para>----------------------------------------------------------------------------------------------------</para>
/// <para>Example:</para>
/// <example>
/// <para>MetaAutoAction("MyTool_Action")</para>
/// <para>----------------------------------------------------------------------------------------------------</para>
/// <para>Example Actions .xml file:</para>
/// <para>&lt;Actions&gt;</para>
/// <para>&#160;&#160;&#160;&#160;&lt;MyTool_Action text="My Action" shortcut="Ctrl+`" path="MainMenu.Tools;ToolboxToolbar.Tool" icon=":/plg_wgs_tools/icons/mytool.png"/&gt;</para>
/// <para>&lt;/Actions&gt;</para>
/// </example>
/// </summary>
/// <param name="actionId">The unique id used to identify the action in the action manager.</param>
inline MetaData MetaAutoAction(const char* actionId)
{
	return metaTypeCreator()->MetaAutoAction(actionId);
}

/// <summary>
/// Indicates the custom insert functions to use for a container when adding/moving/deleting items.
/// This isn't necessary if the container is a standard container of trivially constructed objects.
/// <para>----------------------------------------------------------------------------------------------------</para>
/// <para>Example:</para>
/// <example>
/// <para>MetaInsert(MyType::MyInsert, MyType::MyCanInsert, MyType::MyCanRemove)</para>
/// </example>
/// </summary>
/// <param name="insertCallback">The callback invoked to insert a new element into the container.</param>
/// <param name="canInsertCallback">The callback invoked to determine if a new element can be inserted into the container.</param>
/// <param name="canRemoveCallback">The callback invoked to determine if an element can be removed from the container.</param>
inline MetaData MetaInsert(InsertCallback insertCallback, CanInsertCallback canInsertCallback, CanRemoveCallback canRemoveCallback)
{
	return metaTypeCreator()->MetaInsert(insertCallback, canInsertCallback, canRemoveCallback);
}

} // end namespace wgt
#endif // META_TYPES_HPP
