#include "meta_types.hpp"
#include "meta_impl.hpp"
#include "../utilities/definition_helpers.hpp"

namespace wgt
{
#define SET_UP_OBJECT( type, ctor_parameters ) \
	ObjectHandleT< type > handle( std::move( new type ctor_parameters ) ); \
	return staticCast< MetaBase, type >( handle );


//==============================================================================
MetaHandle MetaNone()
{
	SET_UP_OBJECT( MetaNoneObj, () )
}


//==============================================================================
MetaHandle MetaMinMax( float min, float max )
{
	SET_UP_OBJECT( MetaMinMaxObj, ( min, max ) )
}


//==============================================================================
MetaHandle MetaEnum( IEnumGeneratorPtr enumGenerator )
{
	SET_UP_OBJECT( MetaEnumObj, ( std::move(enumGenerator) ) )
}

//==============================================================================
MetaHandle MetaEnum( const wchar_t * enumString )
{
	SET_UP_OBJECT( MetaEnumObj, ( enumString ) )
}

//==============================================================================
MetaHandle MetaSlider()
{
	SET_UP_OBJECT( MetaSliderObj, () )
}


//==============================================================================
MetaHandle MetaGroup( const wchar_t * groupName )
{
	SET_UP_OBJECT( MetaGroupObj, ( groupName ) )
}


//==============================================================================
MetaHandle MetaAttributeDisplayName( const char * attributePath )
{
	SET_UP_OBJECT( MetaAttributeDisplayNameObj, ( attributePath ) )
}


//==============================================================================
MetaHandle MetaDisplayName( const wchar_t * displayName )
{
	SET_UP_OBJECT( MetaDisplayNameObj, ( displayName ) )
}


//==============================================================================
MetaHandle MetaDescription(const wchar_t * description)
{
	SET_UP_OBJECT(MetaDescriptionObj, ( description ))
}


//==============================================================================
MetaHandle MetaPanelLayout( const char * layoutFile, const char * bindingsFile )
{
	SET_UP_OBJECT( MetaPanelLayoutObj, ( layoutFile, bindingsFile ) )
}


//==============================================================================
MetaHandle MetaNoNull()
{
	SET_UP_OBJECT( MetaNoNullObj, () )
}


//==============================================================================
MetaHandle MetaColor()
{
	SET_UP_OBJECT( MetaColorObj, () )
}


//==============================================================================
MetaHandle MetaStepSize(float stepSize)
{
	SET_UP_OBJECT(MetaStepSizeObj, ( stepSize ))
}


//==============================================================================
MetaHandle MetaDecimals(int decimals)
{
	SET_UP_OBJECT(MetaDecimalsObj, ( decimals ))
}


//==============================================================================
MetaHandle MetaHidden()
{
	SET_UP_OBJECT( MetaHiddenObj, () )
}


//==============================================================================
MetaHandle MetaThumbnail( int width, int height )
{
	SET_UP_OBJECT( MetaThumbnailObj, ( width, height ) )
}


//==============================================================================
MetaHandle MetaInPlace( const char * propertyName )
{
	SET_UP_OBJECT( MetaInPlaceObj, ( propertyName ) )
}


//==============================================================================
MetaHandle MetaSelected( const char * propertyName )
{
	SET_UP_OBJECT( MetaSelectedObj, ( propertyName ) )
}

//==============================================================================
MetaHandle MetaInPlacePropertyName( const char * propertyName )
{
	SET_UP_OBJECT( MetaInPlacePropertyNameObj, ( propertyName ) )
}

//==============================================================================
MetaHandle MetaCommandBase(
	const wchar_t * commandName, const IMetaCommandExecutable * commandExecutable )
{
	SET_UP_OBJECT( MetaCommandObj,  ( commandName, commandExecutable ) );
}

//==============================================================================
MetaHandle MetaNoSerialization()
{
	SET_UP_OBJECT( MetaNoSerializationObj, () )
}

//==============================================================================
MetaHandle MetaUniqueId( const char * id )
{
	SET_UP_OBJECT( MetaUniqueIdObj, ( id ) );
}

//==============================================================================
MetaHandle MetaOnStack()
{
	SET_UP_OBJECT( MetaOnStackObj, () );
}

//==============================================================================
MetaHandle MetaReadOnly()
{
	SET_UP_OBJECT( MetaReadOnlyObj, () );
}

//==============================================================================
MetaHandle MetaUrl(bool isAssetBrowserDialog,
				   const char * urlDlgTitle, 
				   const char * urlDlgDefaultFolder,
				   int urlDlgModality,
				   const char * urlDlgNameFilters, 
				   const char * urlDlgSelectedNameFilter)
{
	SET_UP_OBJECT( MetaUrlObj, 
		(isAssetBrowserDialog, urlDlgTitle, urlDlgDefaultFolder, 
		urlDlgModality, urlDlgNameFilters, urlDlgSelectedNameFilter ) );
}

//==============================================================================
MetaHandle MetaUnique()
{
	SET_UP_OBJECT( MetaUniqueObj, ( ) );
}

//==============================================================================
MetaHandle MetaParamHelp(const char* paramName, const MetaParamTypes::MetaParamType paramType, const char* paramDesc )
{
	SET_UP_OBJECT( MetaParamHelpObj, ( paramName, paramType, paramDesc  ) );
}

//==============================================================================
MetaHandle MetaReturnHelp(const char* returnName, const MetaParamTypes::MetaParamType returnType, const char* returnDesc)
{
	SET_UP_OBJECT( MetaReturnHelpObj, ( returnName, returnType, returnDesc ) );
}

//==============================================================================
MetaHandle MetaConsoleHelp(const char* text)
{
	SET_UP_OBJECT( MetaConsoleHelpObj, ( text ) );
}

//==============================================================================
MetaHandle MetaScriptFunctionHelp(const char* name)
{
	SET_UP_OBJECT( MetaScriptFunctionHelpObj, ( name ) );
}

//==============================================================================
MetaHandle MetaTooltip(const char* tooltip)
{
	SET_UP_OBJECT( MetaTooltipObj, ( tooltip ) );
}

//==============================================================================
MetaHandle MetaPassword()
{
	SET_UP_OBJECT(MetaPasswordObj, ( ));
}

//==============================================================================
MetaHandle MetaMultiline()
{
	SET_UP_OBJECT(MetaMultilineObj, ( ));
}
} // end namespace wgt
