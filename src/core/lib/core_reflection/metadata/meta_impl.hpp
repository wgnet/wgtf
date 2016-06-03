#ifndef META_IMPL_HPP
#define META_IMPL_HPP

/*
Standard meta attribute classes for the reflected system.
Details: Search for NGT Reflection System on the Wargaming Confluence
*/

#include "meta_base.hpp"

namespace wgt
{
class Collection;

//==============================================================================
class MetaNoneObj
	: public MetaBase
{
	DECLARE_REFLECTED

public:
	MetaNoneObj() {}
	~MetaNoneObj() {}
};

//==============================================================================
class MetaMinMaxObj
	: public MetaBase
{
	DECLARE_REFLECTED

public:
	MetaMinMaxObj(){}
	MetaMinMaxObj( float min, float max );

	const float & getMin() const { return min_; }
	const float & getMax() const { return max_; }

private:
	float min_;
	float max_;
};

//==============================================================================
class MetaStepSizeObj
	: public MetaBase
{
	DECLARE_REFLECTED

public:
	static const float DefaultStepSize;
	
	MetaStepSizeObj() : stepSize_(DefaultStepSize){}
	MetaStepSizeObj(float stepSize);
	~MetaStepSizeObj() {}
	
	const float & getStepSize() const { return stepSize_; }

private:
	float stepSize_;
};

//==============================================================================
class MetaDecimalsObj
	: public MetaBase
{
	DECLARE_REFLECTED

public:
	static const int DefaultDecimals;

	MetaDecimalsObj() : decimals_(DefaultDecimals){}
	MetaDecimalsObj(int decimals);
	~MetaDecimalsObj() {}

	const int & getDecimals() const { return decimals_; }

private:
	int decimals_;
};

class IEnumGenerator;
typedef std::unique_ptr<IEnumGenerator> IEnumGeneratorPtr;
//==============================================================================
class MetaEnumObj
	: public MetaBase
{
	DECLARE_REFLECTED

public:
	MetaEnumObj();
	explicit MetaEnumObj( IEnumGeneratorPtr enumGenerator );
	explicit MetaEnumObj( const wchar_t * enumString );

	~MetaEnumObj();

	const wchar_t * getEnumString() const;

	Collection generateEnum( const ObjectHandle & provider, const IDefinitionManager & definitionManager ) const;

private:
	IEnumGeneratorPtr	enumGenerator_;
	const wchar_t *		enumString_;
};


//==============================================================================
class MetaSliderObj
	: public MetaBase
{
	DECLARE_REFLECTED

public:
	MetaSliderObj() {}
	~MetaSliderObj() {}
};


//==============================================================================
class MetaGroupObj
	: public MetaBase
{
	DECLARE_REFLECTED

public:
	MetaGroupObj()
		: groupName_( NULL )
		, groupNameHash_( 0 )
	{
	}

	explicit MetaGroupObj( const wchar_t * groupName );
	~MetaGroupObj() {}

	const wchar_t * getGroupName() const;
	uint64_t getGroupNameHash() const;

private:
	const wchar_t * groupName_;
	uint64_t groupNameHash_;
};


//==============================================================================
class MetaAttributeDisplayNameObj
	: public MetaBase
{
	DECLARE_REFLECTED

public:
	MetaAttributeDisplayNameObj()
		: attributeName_( NULL )
	{
	}

	explicit MetaAttributeDisplayNameObj( const char * attributeName );
	~MetaAttributeDisplayNameObj() {}

	const char * getAttributeName() const;

private:
	const char * attributeName_;
};


//==============================================================================
class MetaDisplayNameObj
	: public MetaBase
{
	DECLARE_REFLECTED

public:
	MetaDisplayNameObj()
		: displayName_( NULL )
	{
	}

	explicit MetaDisplayNameObj( const wchar_t * displayName );
	~MetaDisplayNameObj() {}

	const wchar_t * getDisplayName() const;

private:
	const wchar_t * displayName_;
};


//==============================================================================
class MetaDescriptionObj
	: public MetaBase
{
	DECLARE_REFLECTED

public:
	MetaDescriptionObj()
		: description_( NULL )
	{
	}

	explicit MetaDescriptionObj( const wchar_t * description );
	~MetaDescriptionObj() {}

	const wchar_t * getDescription() const;

private:
	const wchar_t * description_;
};


//==============================================================================
class MetaPanelLayoutObj
	: public MetaBase
{
	DECLARE_REFLECTED

public:
	explicit MetaPanelLayoutObj( 
		const char * layoutFile = NULL, 
		const char * bindingsFile = NULL) :
		layoutFile_(layoutFile),
		bindingsFile_(bindingsFile)
	{
	}

	~MetaPanelLayoutObj() {}

	const char * getLayoutFile() const { return layoutFile_; }
	const char * getBindingsFile() const { return bindingsFile_; }

private:
	const char * layoutFile_;
	const char * bindingsFile_;
};


//==============================================================================
class MetaNoNullObj
	: public MetaBase
{
	DECLARE_REFLECTED

public:
	MetaNoNullObj() {}
	~MetaNoNullObj() {}
};


//==============================================================================
class MetaColorObj
	: public MetaBase
{
	DECLARE_REFLECTED

public:
	MetaColorObj() {}
	~MetaColorObj() {}
};


//==============================================================================
class MetaHiddenObj
	: public MetaBase
{
	DECLARE_REFLECTED

public:
	MetaHiddenObj() {}
	~MetaHiddenObj() {}
};


//==============================================================================
class MetaThumbnailObj
	: public MetaBase
{
	DECLARE_REFLECTED

public:
	MetaThumbnailObj( int width = -1, int height = -1 )
		: width_( width )
		, height_( height )
	{
	}

	~MetaThumbnailObj() {}

	int getWidth() const { return width_; }
	int getHeight() const { return height_; }

private:
	int width_;
	int height_;
};


//==============================================================================
class MetaInPlaceObj
	: public MetaBase
{
	DECLARE_REFLECTED

public:
	explicit MetaInPlaceObj( const char * propName = NULL )
		: propName_( propName )
	{
	}

	const char * getPropName() const { return propName_; }

private:
	const char * propName_;
};


//==============================================================================
class MetaSelectedObj
	: public MetaBase
{
	DECLARE_REFLECTED

public:
	explicit MetaSelectedObj( const char * propName = NULL )
		: propName_( propName )
	{
	}

	const char * getPropName() const { return propName_; }

private:
	const char * propName_;
};


class IMetaCommandExecutable;

//==============================================================================
class MetaCommandObj
	: public MetaBase
{
	DECLARE_REFLECTED

public:
	MetaCommandObj();

	explicit MetaCommandObj(
		const wchar_t * commandName,
		const IMetaCommandExecutable * commandExecutable )
		: commandName_( commandName )
		, commandExecutable_( commandExecutable )
	{
	}

	~MetaCommandObj();

	const wchar_t * getCommandName() const { return commandName_; }

	void execute( void * pBase, void * arguments ) const;

private:
	const wchar_t *					commandName_;
	const IMetaCommandExecutable *	commandExecutable_;
};

//==============================================================================
class MetaNoSerializationObj
	: public MetaBase
{
	DECLARE_REFLECTED
	
public:
	MetaNoSerializationObj() {}
	~MetaNoSerializationObj() {}
};

//==============================================================================
class MetaUniqueIdObj
	: public MetaBase
{
	DECLARE_REFLECTED
public:
	MetaUniqueIdObj( const char * id = nullptr )
		: id_( id )
	{
	}
	~MetaUniqueIdObj() {}

	const char * getId() const;
private:
	const char * id_;
};

//==============================================================================
class MetaOnStackObj
	: public MetaBase
{
	DECLARE_REFLECTED

public:
	MetaOnStackObj() {}
	~MetaOnStackObj() {}
};

//==============================================================================
class MetaInPlacePropertyNameObj
	: public MetaBase
{
public:
	MetaInPlacePropertyNameObj( const char * propertyName = nullptr )
		: propName_( propertyName )
	{
	}
	~MetaInPlacePropertyNameObj()
	{
	}
	const char * getPropertyName() const;
private:
	const char * propName_;
};

//==============================================================================
class MetaReadOnlyObj : public MetaBase
{
	DECLARE_REFLECTED

public:
	MetaReadOnlyObj() {}
	~MetaReadOnlyObj() {}
};

class MetaUrlObj : public MetaBase
{
	DECLARE_REFLECTED

public:
	MetaUrlObj( bool isAssetBrowserDialog = false,
				const char * urlDlgTitle = nullptr, 
				const char * urlDlgDefaultFolder = nullptr,
				int urlDlgModality = 1,
				const char * urlDlgNameFilters = nullptr, 
				const char * urlDlgSelectedNameFilter = nullptr);
	~MetaUrlObj() {}

	bool isAssetBrowserDialog() const;
	const char * getDialogTitle() const;
	const char * getDialogDefaultFolder() const;
	int getDialogModality() const;
	const char * getDialogNameFilters() const;
	const char * getDialogSelectedNameFilter() const;

private:
	bool isAssetBrowserDialog_;
	const char * title_;
	const char * defaultFolder_;
	int modality_;
	const char * nameFilters_;
	const char * selectedNameFilter_;
};

//==============================================================================
class MetaUniqueObj : public MetaBase
{
	DECLARE_REFLECTED
};

//==============================================================================
class MetaParamHelpObj : public MetaBase 
{
	DECLARE_REFLECTED

public:

	MetaParamHelpObj( const char* paramName, const MetaParamTypes::MetaParamType paramType, const char* paramDesc )
		: name_(paramName)
		, desc_(paramDesc)
		, type_(paramType)
	{
	}

	const char*		getDesc() const { return desc_; }
	const char*		getName() const { return name_; }
	MetaParamTypes::MetaParamType	getType() const { return type_; }
	const char*		getTypeName() const
	{
		switch ( type_ )
		{
		case MetaParamTypes::kBoolean:		return "Boolean";
		case MetaParamTypes::kInteger:		return "Integer";
		case MetaParamTypes::kFloat:		return "Float";
		case MetaParamTypes::kString:		return "String";
		case MetaParamTypes::kEnum:			return "Enum";
		case MetaParamTypes::kHandle:		return "Handle";
		case MetaParamTypes::kHandleList:	return "HandleList";
		case MetaParamTypes::kTable:		return "Table";
		case MetaParamTypes::kFunction:		return "Function";
		case MetaParamTypes::kImportName:	return "ImportName";
		default:							return "Undefined";
		}
	}

private:
	const char*		name_;
	const char*		desc_;
	MetaParamTypes::MetaParamType	type_;
};

//==============================================================================
class MetaReturnHelpObj : public MetaBase
{
	DECLARE_REFLECTED

public:

	MetaReturnHelpObj(const char* returnName, const MetaParamTypes::MetaParamType returnType, const char* returnDesc)
		: name_(returnName)
		, desc_(returnDesc)
		, type_(returnType)
	{
	}

	const char*		getDesc() const { return desc_; }
	const char*		getName() const { return name_; }
	MetaParamTypes::MetaParamType	getType() const { return type_; }

private:
	const char* name_;
	const char* desc_;
	MetaParamTypes::MetaParamType type_;
};

//==============================================================================
class MetaConsoleHelpObj : public MetaBase
{
	DECLARE_REFLECTED

public:

	MetaConsoleHelpObj(const char* text)
		: text_(text)
	{
	}

	const char* getText() const { return text_; }

private:
	const char* text_;
};

//==============================================================================
class MetaScriptFunctionHelpObj : public MetaBase
{
	DECLARE_REFLECTED

public:

	MetaScriptFunctionHelpObj(const char* name)
		: name_(name)
	{
	}

	const char* getName() const { return name_; }

private:
	const char* name_;
};

//==============================================================================
class MetaTooltipObj : public MetaBase
{
	DECLARE_REFLECTED

public:

	MetaTooltipObj(const char* tooltip)
		: tooltip_(tooltip)
	{
	}

	const char* getTooltip() const { return tooltip_; }

private:
	const char* tooltip_;
};

//==============================================================================
class MetaPasswordObj : public MetaBase
{
	DECLARE_REFLECTED
};

//==============================================================================
class MetaMultilineObj : public MetaBase
{
	DECLARE_REFLECTED
};
} // end namespace wgt
#endif //META_IMPL_HPP
