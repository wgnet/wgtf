#ifndef META_IMPL_HPP
#define META_IMPL_HPP

/*
* Standard meta attribute classes for the reflected system.
* Details: Search for NGT Reflection System on the Wargaming Confluence
* @note for class descriptions see meta_types.hpp
*/
#include "meta_base.hpp"
#include "core_reflection/reflection_dll.hpp"
#include "core_reflection/interfaces/i_signal_provider.hpp"
#include "static_string_database/i_static_string_database.hpp"
#include "core_common/signal.hpp"
#include "core_variant/collection.hpp"
#include "core_reflection/interfaces/i_direct_base_helper.hpp"

namespace wgt
{
class IMetaCommandExecutable;
class Collection;
typedef std::function<void(const ObjectHandle&)> Action;
typedef std::function<bool(const ObjectHandle&)> Predicate;
typedef std::function<std::wstring(const ObjectHandle&)> GetTextCallback;
typedef std::function<std::string(const std::string& path, const ObjectHandle&)> GetPathTextCallback;
typedef std::function<Variant(const ObjectHandle&)> ObjectValueToVariantCallback;
typedef std::function<void(const ObjectHandle&, int64_t index)> InsertCallback;
typedef std::function<bool(const ObjectHandle&)> CanInsertCallback;
typedef std::function<bool(const ObjectHandle&)> CanRemoveCallback;

//==============================================================================
class REFLECTION_DLL MetaMinMaxObj
{
	DECLARE_REFLECTED

public:
	MetaMinMaxObj(float min, float max);
    ~MetaMinMaxObj();

	const float& getMin() const { return min_; }
	const float& getMax() const { return max_; }

private:
	float min_;
	float max_;
};

//==============================================================================
class REFLECTION_DLL MetaMinMaxMappedObj : public MetaMinMaxObj
{
	DECLARE_REFLECTED

public:
	MetaMinMaxMappedObj(float min, float max, float mappedMin, float mappedMax);
	~MetaMinMaxMappedObj() = default;

	const float& getMappedMin() const { return mappedMin_; }
	const float& getMappedMax() const { return mappedMax_; }

private:
	float mappedMin_;
	float mappedMax_;
};

//==============================================================================
class REFLECTION_DLL MetaStepSizeObj
{
	DECLARE_REFLECTED

public:
	static const float DefaultStepSize;

    MetaStepSizeObj();
    explicit MetaStepSizeObj(float stepSize);
    ~MetaStepSizeObj();

	const float& getStepSize() const
	{
		return stepSize_;
	}

private:
	float stepSize_;
};

//==============================================================================
class REFLECTION_DLL MetaDecimalsObj
{
	DECLARE_REFLECTED

public:
	static const int DefaultDecimals;

    MetaDecimalsObj();
    explicit MetaDecimalsObj(int decimals);
    ~MetaDecimalsObj();

	const int& getDecimals() const
	{
		return decimals_;
	}

private:
	int decimals_;
};

class IEnumGenerator;
typedef std::unique_ptr<IEnumGenerator> IEnumGeneratorPtr;

//==============================================================================
class REFLECTION_DLL MetaEnumObj
{
	DECLARE_REFLECTED

public:
	MetaEnumObj();
	explicit MetaEnumObj(IEnumGeneratorPtr enumGenerator);
	explicit MetaEnumObj(const wchar_t* enumString);
	~MetaEnumObj();

	const wchar_t* getEnumString() const;
	Collection generateEnum(const ObjectHandle& provider);
	Collection generateEnum(const ObjectHandle& provider) const;

	template<typename T = void>
	Collection generateEnumDeprecated(const ObjectHandle& provider, const IDefinitionManager& definitionManager) const
	{
		static_assert(!std::is_same<T, T>::value, "This method is now deprecated. Please use generateEnum");
	}

private:
	IEnumGeneratorPtr enumGenerator_;
	const wchar_t* enumString_;
};

//==============================================================================
class REFLECTION_DLL MetaSliderObj
{
	DECLARE_REFLECTED

public:
    MetaSliderObj();
    ~MetaSliderObj();
};

//==============================================================================
class REFLECTION_DLL MetaGroupObj
{
	DECLARE_REFLECTED

public:
    MetaGroupObj();
	explicit MetaGroupObj(const wchar_t* groupName);
    ~MetaGroupObj();

	static const int FULL_GROUP_LEVEL = -1;
	virtual const wchar_t *getGroupName(const ObjectHandle& object, int subGroupIndex = FULL_GROUP_LEVEL) const;
	virtual uint64_t getGroupNameHash  (const ObjectHandle& object, int subGroupIndex = FULL_GROUP_LEVEL) const;
	const int getNumSubGroups() const;

private:
	struct NameAndHash
	{
		std::wstring name;
		uint64_t hash;
	};

	std::vector<NameAndHash>  groupNames_; // Name split up by their subgroup, index 0 stores the full name
	mutable const wchar_t *groupName_;
	mutable uint64_t groupNameHash_;

	const int normalizeSubGroupIndex(const int index) const;
};


//==============================================================================
class REFLECTION_DLL MetaGroupCallbackObj : public MetaGroupObj
{
	DECLARE_REFLECTED;

public:
	MetaGroupCallbackObj(const GetTextCallback& callback);
    ~MetaGroupCallbackObj();

	virtual const wchar_t *getGroupName(const ObjectHandle& object, int level = FULL_GROUP_LEVEL) const;
	virtual uint64_t getGroupNameHash(const ObjectHandle& object,   int level = FULL_GROUP_LEVEL) const;

private:
	GetTextCallback callback_;
	mutable std::wstring groupNameString_;
};

//==============================================================================
class REFLECTION_DLL MetaAttributeDisplayNameObj
{
	DECLARE_REFLECTED

public:
    MetaAttributeDisplayNameObj();
	explicit MetaAttributeDisplayNameObj(const char* attributeName);
    ~MetaAttributeDisplayNameObj();

	const char* getAttributeName() const;

private:
	const char* attributeName_;
};

//==============================================================================
class REFLECTION_DLL MetaDisplayNameObj
{
	DECLARE_REFLECTED

public:
	explicit MetaDisplayNameObj(const wchar_t* displayName);
    ~MetaDisplayNameObj();

	virtual const wchar_t* getDisplayName() const;
	virtual const wchar_t* getDisplayName(const ObjectHandle& handle) const;

protected:
	const wchar_t* displayName_ = nullptr;
};

//==============================================================================
class REFLECTION_DLL MetaDisplayNameCallbackObj : public MetaDisplayNameObj
{
	DECLARE_REFLECTED

public:
    explicit MetaDisplayNameCallbackObj(GetTextCallback action);
    ~MetaDisplayNameCallbackObj();

	virtual const wchar_t* getDisplayName() const override;
	virtual const wchar_t* getDisplayName(const ObjectHandle& handle) const override;

	void setDisplayName(const wchar_t* displayName)
	{
		displayName_ = displayName;
	}

private:
	GetTextCallback callback_;
	mutable std::wstring displayNameString_;
};

//==============================================================================
class REFLECTION_DLL MetaDisplayPathNameCallbackObj
{
	DECLARE_REFLECTED

public:
    explicit MetaDisplayPathNameCallbackObj(GetPathTextCallback action);
    ~MetaDisplayPathNameCallbackObj();

	/**
	* Asks stored callback for display value.
	* @param path Property path
	* @param handle Object handle
	* @return Display name from stored callback.
	**/
	virtual const std::string getDisplayName(const std::string& path, const ObjectHandle& handle) const;

private:
	GetPathTextCallback callback_;
};

//==============================================================================
class REFLECTION_DLL MetaIconObj
{
	DECLARE_REFLECTED

public:
	explicit MetaIconObj(const char* displayName);
	~MetaIconObj() = default;

	virtual const char* getIcon() const;

protected:
	const char* icon_ = nullptr;
};

//==============================================================================
class REFLECTION_DLL MetaStaticStringObj
{
	DECLARE_REFLECTED

public:
    explicit MetaStaticStringObj(const IStaticStringDatabase* staticStringDatabase);
    ~MetaStaticStringObj();

	/**
	 * Gets string for an integer property.
	 * @param staticStringDatabase Static string database interface.
	 * @param stringId String id.
	 * @return String for the numeric property or nullptr if property is not integer or string does not exist.
	**/
	const char* getStaticString(uint64_t stringId) const;

	/**
	* Gets id to string map.
	* @param staticStringDatabase Static string database interface.
	* @return A collection of strings indexed by ids.
	**/
	Collection getAllStaticStringIds() const;
private:
	const char* getStaticStringExpose(uint64_t stringId);
	Collection getAllStaticStringIdsExpose();
	Variant getStaticStringId(const std::string& str);

	const IStaticStringDatabase* staticStringDatabase_;
};

//==============================================================================
class REFLECTION_DLL MetaDescriptionObj
{
	DECLARE_REFLECTED

public:
	explicit MetaDescriptionObj(const wchar_t* description);
    ~MetaDescriptionObj();

	const wchar_t* getDescription() const;

private:
	const wchar_t* description_ = nullptr;
};

//==============================================================================
class REFLECTION_DLL MetaPanelLayoutObj
{
	DECLARE_REFLECTED

public:
    explicit MetaPanelLayoutObj(const char* layoutFile = NULL, const char* bindingsFile = NULL);
    ~MetaPanelLayoutObj();

	const char* getLayoutFile() const
	{
		return layoutFile_;
	}
	const char* getBindingsFile() const
	{
		return bindingsFile_;
	}

private:
	const char* layoutFile_;
	const char* bindingsFile_;
};

//==============================================================================
class REFLECTION_DLL MetaNoNullObj
{
	DECLARE_REFLECTED

public:
    MetaNoNullObj();
    ~MetaNoNullObj();
};

//==============================================================================
class REFLECTION_DLL MetaComponentObj
{
	DECLARE_REFLECTED

public:
	/*! Indicates the decorated property should use the component identified by the component name
	    The custom component will need to be registered with the IUIFramework::registerComponent
	*/
	MetaComponentObj(const char* componentName);
    ~MetaComponentObj();

	/*!
	*/
	const char* getComponentName() const
	{
		return componentName_;
	}

private:
	const char* componentName_;
};

//==============================================================================
class REFLECTION_DLL MetaColorObj
{
	DECLARE_REFLECTED

public:
	MetaColorObj();
	~MetaColorObj();
};

//==============================================================================
class REFLECTION_DLL MetaKelvinColorObj : public MetaColorObj, public MetaComponentObj
{
	DECLARE_REFLECTED

public:
	explicit MetaKelvinColorObj(std::function<Vector4(int)> converter);
    ~MetaKelvinColorObj();

private:
	Vector4 convert(int);

	std::function<Vector4(int)> converter_ = nullptr;
};

//==============================================================================
class REFLECTION_DLL MetaHDRColorObj : public MetaColorObj, public MetaComponentObj
{
	DECLARE_REFLECTED

public:
	explicit MetaHDRColorObj(std::function<Vector4(const Vector4&)> tonemapOperator);
	~MetaHDRColorObj();

private:
	Vector4 tonemap(const Vector4& v);

	std::function<Vector4(const Vector4&)> tonemapOperator_ = nullptr;
};

//==============================================================================
class REFLECTION_DLL MetaHDRColorReinhardTonemapObj : public MetaHDRColorObj
{
	DECLARE_REFLECTED

public:
	MetaHDRColorReinhardTonemapObj();
    ~MetaHDRColorReinhardTonemapObj();
	static float getLuminanceWhite();
	static void setLuminanceWhite(float v);

private:
	static Vector4 tonemap(const Vector4& v);
	static float luminanceWhite_;
};

//==============================================================================
/**
 *	Interface for changing the value of properties over time.
 *	The value changing can be any type; float, vector4 etc.
 *	@note the IMetaAnimatedObj implementation must not store data because
 *		it is attached per-type, not per-instance.
 *		All functions require passing in a handle to the object to which the
 *		meta data is attached.
 */
class REFLECTION_DLL IMetaAnimatedObj
{
	DECLARE_REFLECTED

public:
	typedef float anim_time_t; /// Type for animation timeline

	/**
	 *	@param handle to the object on which the meta data is attached.
	 *	@param path property path to the object on which the meta data is attached.
	 *	@return if an animation is activated for the property.
	 */
	virtual bool isActive(const ObjectHandle& handle,
		const std::string& path) = 0;
	/**
	 *	@param handle to the object on which the meta data is attached.
	 *	@param path property path to the object on which the meta data is attached.
	 *	@param active if an animation is activated for the property.
	 *	@return true on success.
	 */
	virtual bool setActive(ObjectHandle& handle,
		const std::string& path,
		bool active) = 0;

	/**
	 *	@param handle to the object on which the meta data is attached.
	 *	@param path property path to the object on which the meta data is attached.
	 *	@return if the animation is looping.
	 */
	virtual bool isLooping(const ObjectHandle& handle,
		const std::string& path) = 0;
	/**
	 *	@param handle to the object on which the meta data is attached.
	 *	@param path property path to the object on which the meta data is attached.
	 *	@param looping if the animation is looping.
	 *	@return true on success.
	 */
	virtual bool setLooping(ObjectHandle& handle,
		const std::string& path,
		bool looping) = 0;

	/**
	 *	@param handle to the object on which the meta data is attached.
	 *	@param path property path to the object on which the meta data is attached.
	 *	@return the duration of the animation.
	 */
	virtual anim_time_t getDuration(const ObjectHandle& handle,
		const std::string& path) = 0;
	/**
	 *	@param handle to the object on which the meta data is attached.
	 *	@param path property path to the object on which the meta data is attached.
	 *	@return number of keyframes on animation.
	 */
	virtual size_t getNumberOfKeyframes(const ObjectHandle& handle,
		const std::string& path) = 0;
	/**
	 *	Insert a value at a given time.
	 *	
	 *	@post this may invalidate indexes to other keyframes.
	 *	
	 *	@param handle to the object on which the meta data is attached.
	 *	@param path property path to the object on which the meta data is attached.
	 *	@param value to be inserted.
	 *	@param time at which to insert the value.
	 *	@return index of keyframe at point of insertion
	 *		or max size_t on failure.
	 */
	virtual size_t insertKeyframe(ObjectHandle& handle,
		const std::string& path,
		const Variant& value,
		anim_time_t time) = 0;
	/**
	 *	Insert a value at a given time.
	 *	
	 *	@pre keyframeIndex is valid.
	 *	@post this may invalidate indexes to other keyframes.
	 *	
	 *	@param handle to the object on which the meta data is attached.
	 *	@param path property path to the object on which the meta data is attached.
	 *	@param keyframeIndex of keyframe to remove.
	 *	@return true on success.
	 */
	virtual bool removeKeyframe(ObjectHandle& handle,
		const std::string& path,
		size_t keyframeIndex) = 0;

	/**
	 *	@pre keyframeIndex is valid.
	 *	
	 *	@param handle to the object on which the meta data is attached.
	 *	@param path property path to the object on which the meta data is attached.
	 *	@param keyframeIndex index of keyframe.
	 *	@return time of the given keyframe
	 *		or max anim_time_t on failure.
	 */
	virtual anim_time_t getKeyframeTime(const ObjectHandle& handle,
		const std::string& path,
		size_t keyframeIndex) = 0;
	/**
	 *	@pre keyframeIndex is valid.
	 *	@post this may invalidate indexes to other keyframes.
	 *	
	 *	@param handle to the object on which the meta data is attached.
	 *	@param path property path to the object on which the meta data is attached.
	 *	@param keyframeIndex index of keyframe.
	 *	@param time of the given keyframe.
	 *	@return true on success.
	 */
	virtual bool setKeyframeTime(ObjectHandle& handle,
		const std::string& path,
		size_t keyframeIndex,
		anim_time_t time) = 0;

	/**
	 *	@pre keyframeIndex is valid.
	 *	
	 *	@param handle to the object on which the meta data is attached.
	 *	@param path property path to the object on which the meta data is attached.
	 *	@param keyframeIndex index of keyframe.
	 *	@return value of the given keyframe
	 *		or empty Variant on failure.
	 */
	virtual Variant getKeyframeValue(const ObjectHandle& handle,
		const std::string& path,
		size_t keyframeIndex) = 0;
	/**
	 *	@pre keyframeIndex is valid.
	 *	
	 *	@param handle to the object on which the meta data is attached.
	 *	@param path property path to the object on which the meta data is attached.
	 *	@param keyframeIndex index of keyframe.
	 *	@param value of the given keyframe.
	 *	@return true on success.
	 */
	virtual bool setKeyframeValue(const ObjectHandle& handle,
		const std::string& path,
		size_t keyframeIndex,
		const Variant& value) = 0;

	/**
	 *	@param handle to the object on which the meta data is attached.
	 *	@param path property path to the object on which the meta data is attached.
	 *	@param time at which to get the value.
	 *		If the time is not that of a keyframe, the value will be extrapolated.
	 *	@return value at the given time
	 *		or empty variant if there are no keyframes.
	 */
	virtual Variant extrapolateValue(const ObjectHandle& handle,
		const std::string& path,
		anim_time_t time) = 0;
};

//==============================================================================
class REFLECTION_DLL MetaHiddenObj
{
	DECLARE_REFLECTED

public:
    explicit MetaHiddenObj(Predicate predicate = Predicate());
    ~MetaHiddenObj();

	bool isHidden(const ObjectHandle& handle) const
	{
		if (predicate_)
		{
			return predicate_(handle);
		}
		return true;
	}

	bool isDynamic() const
	{
		return static_cast<bool>(predicate_);
	}

private:
	Predicate predicate_;
};

//==============================================================================
class REFLECTION_DLL MetaThumbnailObj
{
	DECLARE_REFLECTED

public:
    MetaThumbnailObj(int width = -1, int height = -1);
    ~MetaThumbnailObj();

	int getWidth() const
	{
		return width_;
	}
	int getHeight() const
	{
		return height_;
	}

private:
	int width_;
	int height_;
};

//==============================================================================
class REFLECTION_DLL MetaInPlaceObj
{
	DECLARE_REFLECTED

public:
    explicit MetaInPlaceObj(const char* propName = NULL);
    ~MetaInPlaceObj();

	const char* getPropName() const
	{
		return propName_;
	}

private:
	const char* propName_;
};

//==============================================================================
class REFLECTION_DLL MetaSelectedObj
{
	DECLARE_REFLECTED

public:
    explicit MetaSelectedObj(const char* propName = NULL);
    ~MetaSelectedObj();

	const char* getPropName() const
	{
		return propName_;
	}

private:
	const char* propName_;
};

//==============================================================================
class REFLECTION_DLL MetaActionObj
{
	DECLARE_REFLECTED

public:
    MetaActionObj(const char* actionName, Action action, IDefinitionManager* definitionManager);
    ~MetaActionObj();

	const char* getActionName() const
	{
		return actionName_;
	}

	void execute(const ObjectHandle& handle, const std::string& path);

private:
	const char* actionName_;
	Action action_;
	IDefinitionManager* definitionManager_;
};

//==============================================================================
class REFLECTION_DLL MetaCommandObj
{
	DECLARE_REFLECTED

public:
	MetaCommandObj();
    MetaCommandObj(const wchar_t* commandName, const IMetaCommandExecutable* commandExecutable);
	~MetaCommandObj();

	const wchar_t* getCommandName() const
	{
		return commandName_;
	}

	void execute(void* pBase, void* arguments) const;

private:
	const wchar_t* commandName_;
	const IMetaCommandExecutable* commandExecutable_;
};

//==============================================================================
class REFLECTION_DLL MetaNoSerializationObj
{
	DECLARE_REFLECTED

public:
    MetaNoSerializationObj();
    ~MetaNoSerializationObj();
};

//==============================================================================
class REFLECTION_DLL MetaUniqueIdObj
{
	DECLARE_REFLECTED
public:
    MetaUniqueIdObj(const char* id = nullptr);
    ~MetaUniqueIdObj();

	const char* getId() const;

private:
	const char* id_;
};

//==============================================================================
class REFLECTION_DLL MetaOnStackObj
{
	DECLARE_REFLECTED

public:
    MetaOnStackObj();
    ~MetaOnStackObj();
};

//==============================================================================
class REFLECTION_DLL MetaInPlacePropertyNameObj
{
public:
    explicit MetaInPlacePropertyNameObj(const char* propertyName = nullptr);
    ~MetaInPlacePropertyNameObj();
	const char* getPropertyName() const;

private:
	const char* propName_;
};

//==============================================================================
class REFLECTION_DLL MetaReadOnlyObj
{
	DECLARE_REFLECTED

public:
    explicit MetaReadOnlyObj(Predicate predicate = Predicate());
    ~MetaReadOnlyObj();

	bool isReadOnly(const ObjectHandle& handle) const
	{
		if(predicate_)
		{
			return predicate_(handle);
		}
		return true;
	}

private:
	Predicate predicate_;
};

//==============================================================================
class REFLECTION_DLL MetaUrlObj
{
	DECLARE_REFLECTED

public:
	MetaUrlObj(bool native = true, const std::string& title = "", const std::string& folder = "",
	           const std::string& nameFilters = "", const std::string& selectedNameFilter = "");
    ~MetaUrlObj();

	bool native() const;
	const std::string& title() const;
	const std::string& folder() const;
	const std::string& nameFilters() const;
	const std::string& selectedNameFilter() const;

	// DEPRECATED
	bool isAssetBrowserDialog() const;
	const char* getDialogTitle() const;
	const char* getDialogDefaultFolder() const;
	const char* getDialogNameFilters() const;
	const char* getDialogSelectedNameFilter() const;

private:
	bool native_;
	std::string title_;
	std::string folder_;
	std::string nameFilters_;
	std::string selectedNameFilter_;
	std::string rootDirectory_;
};

//==============================================================================
class REFLECTION_DLL MetaUniqueObj
{
	DECLARE_REFLECTED

public:
    MetaUniqueObj();
    ~MetaUniqueObj();
};

//==============================================================================
class REFLECTION_DLL MetaParamHelpObj
{
	DECLARE_REFLECTED

public:
    MetaParamHelpObj(const char* paramName, const MetaParamTypes::MetaParamType paramType, const char* paramDesc);
    ~MetaParamHelpObj();

	const char* getDesc() const
	{
		return desc_;
	}
	const char* getName() const
	{
		return name_;
	}
	MetaParamTypes::MetaParamType getType() const
	{
		return type_;
	}
	const char* getTypeName() const
	{
		switch (type_)
		{
		case MetaParamTypes::kBoolean:
			return "Boolean";
		case MetaParamTypes::kInteger:
			return "Integer";
		case MetaParamTypes::kFloat:
			return "Float";
		case MetaParamTypes::kString:
			return "String";
		case MetaParamTypes::kEnum:
			return "Enum";
		case MetaParamTypes::kHandle:
			return "Handle";
		case MetaParamTypes::kHandleList:
			return "HandleList";
		case MetaParamTypes::kTable:
			return "Table";
		case MetaParamTypes::kFunction:
			return "Function";
		case MetaParamTypes::kImportName:
			return "ImportName";
		default:
			return "Undefined";
		}
	}

private:
	const char* name_;
	const char* desc_;
	MetaParamTypes::MetaParamType type_;
};

//==============================================================================
class REFLECTION_DLL MetaReturnHelpObj
{
	DECLARE_REFLECTED

public:
    MetaReturnHelpObj(const char* returnName, const MetaParamTypes::MetaParamType returnType, const char* returnDesc);
    ~MetaReturnHelpObj();

	const char* getDesc() const
	{
		return desc_;
	}
	const char* getName() const
	{
		return name_;
	}
	MetaParamTypes::MetaParamType getType() const
	{
		return type_;
	}

private:
	const char* name_;
	const char* desc_;
	MetaParamTypes::MetaParamType type_;
};

//==============================================================================
class REFLECTION_DLL MetaConsoleHelpObj
{
	DECLARE_REFLECTED

public:
    MetaConsoleHelpObj(const char* text);
    ~MetaConsoleHelpObj();

	const char* getText() const
	{
		return text_;
	}

private:
	const char* text_;
};

//==============================================================================
class REFLECTION_DLL MetaScriptFunctionHelpObj
{
	DECLARE_REFLECTED

public:
    MetaScriptFunctionHelpObj(const char* name);
    ~MetaScriptFunctionHelpObj();

	const char* getName() const
	{
		return name_;
	}

private:
	const char* name_;
};

//==============================================================================
class REFLECTION_DLL MetaTooltipObj
{
	DECLARE_REFLECTED

public:
    MetaTooltipObj(const wchar_t* tooltip);
    MetaTooltipObj(const GetTextCallback& callback);
    ~MetaTooltipObj();

	const wchar_t* getTooltip(const ObjectHandle& handle) const
	{
		if(callback_ != nullptr)
		{
			tooltip_ = callback_(handle);
		}
		return tooltip_.c_str();
	}

private:
	mutable std::wstring tooltip_;
	GetTextCallback callback_;
};

//==============================================================================
class REFLECTION_DLL MetaCheckBoxObj : public MetaComponentObj
{
	DECLARE_REFLECTED
public:
	MetaCheckBoxObj();
	~MetaCheckBoxObj();
};

//==============================================================================
class REFLECTION_DLL MetaPasswordObj
{
	DECLARE_REFLECTED
public:
    MetaPasswordObj();
    ~MetaPasswordObj();
};

//==============================================================================
class REFLECTION_DLL MetaMultilineObj
{
	DECLARE_REFLECTED
public:
    MetaMultilineObj();
    ~MetaMultilineObj();
};

//==============================================================================
class REFLECTION_DLL MetaDirectInvokeObj
{
	DECLARE_REFLECTED
public:
    MetaDirectInvokeObj();
    ~MetaDirectInvokeObj();
};

//==============================================================================
class REFLECTION_DLL MetaAngleObj : public MetaComponentObj
{
	DECLARE_REFLECTED

public:
    MetaAngleObj(bool storeRadians, bool showRadians);
    ~MetaAngleObj();

private:
	bool storeRadians_;
	bool showRadians_;
};

//==============================================================================
class REFLECTION_DLL MetaTimeObj : public MetaComponentObj
{
	DECLARE_REFLECTED

public:
    MetaTimeObj();
    ~MetaTimeObj();
};

//==============================================================================
class REFLECTION_DLL MetaSignalObj
{
	DECLARE_REFLECTED

public:
    MetaSignalObj();
	explicit MetaSignalObj(ISignalProviderPtr signalProvider);
	~MetaSignalObj();
	Signal<void(Variant&)>* getSignal(const ObjectHandle& provider, const IDefinitionManager& definitionManager) const;

private:
	ISignalProviderPtr signalProvider_;
};

//==============================================================================
class REFLECTION_DLL MetaInvalidatesObjectObj
{
	DECLARE_REFLECTED

public:
    MetaInvalidatesObjectObj() = default;
    ~MetaInvalidatesObjectObj() = default;
};

//==============================================================================
class REFLECTION_DLL MetaCallbackObj
{
	DECLARE_REFLECTED

public:
    explicit MetaCallbackObj(Action action);
    ~MetaCallbackObj();

	void invoke(const ObjectHandle& handle) const
	{
		if (action_)
		{
			action_(handle);
		}
	}

private:
	Action action_;
};

//==============================================================================
class REFLECTION_DLL MetaCollectionItemMetaObj
{
	DECLARE_REFLECTED

public:
    explicit MetaCollectionItemMetaObj(MetaData meta);
    ~MetaCollectionItemMetaObj();

	void appendMetaData(MetaData&& meta);

	/**
	* Asks for stored metadata.
	* @return Display name from stored callback.
	**/
	const MetaData & getMetaData() const;

private:
    MetaData meta_;
};

//==============================================================================
class REFLECTION_DLL MetaDropHitTestObj
{
	DECLARE_REFLECTED

public:
	virtual ~MetaDropHitTestObj();
	virtual const char* getHitTestMethodPath() const = 0;
};

//==============================================================================
class REFLECTION_DLL MetaEnableValueComponentForObjectObj : public MetaComponentObj
{
	DECLARE_REFLECTED
public:
	MetaEnableValueComponentForObjectObj(ObjectValueToVariantCallback callback);
	MetaEnableValueComponentForObjectObj(const char* componentName = "");
	virtual ~MetaEnableValueComponentForObjectObj();
	virtual const char* getComponentType(const ObjectHandle& handle) const;
	virtual Variant getComponentValue(const ObjectHandle& handle) const;
private:
	ObjectValueToVariantCallback callback_;
};


//==============================================================================
class REFLECTION_DLL MetaBasesHolderObj
{
	DECLARE_REFLECTED
public:
	MetaBasesHolderObj(std::unique_ptr< IDirectBaseHelper > & impl)
		: impl_(std::move(impl))
	{
	}
	virtual ~MetaBasesHolderObj() {}

	const IDirectBaseHelper::ParentCollection & getParents() const;
	const IDirectBaseHelper::CasterCollection & getCasters() const;
private:
	std::unique_ptr< IDirectBaseHelper > impl_;
};

//==============================================================================
class REFLECTION_DLL MetaAutoActionObj
{
	DECLARE_REFLECTED
public:
	explicit MetaAutoActionObj(const char* actionId)
		: actionId_(actionId)
	{

	}

	const char* getActionId()
	{
		return actionId_;
	}

private:
	const char* actionId_;
};

//==============================================================================
class REFLECTION_DLL MetaInsertObj
{
	DECLARE_REFLECTED
public:
	MetaInsertObj(InsertCallback insertCallback, CanInsertCallback canInsertCallback, CanRemoveCallback canRemoveCallback);
	void insert(const ObjectHandle& handle, int64_t index) const;
	bool canInsert(const ObjectHandle& handle) const;
	bool canRemove(const ObjectHandle& handle) const;

private:
	InsertCallback insertCallback_;
	CanInsertCallback canInsertCallback_;
	CanRemoveCallback canRemoveCallback_;
};

} // end namespace wgt
#endif // META_IMPL_HPP
