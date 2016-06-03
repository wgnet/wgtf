#ifndef META_TYPES_HPP
#define META_TYPES_HPP

#include "meta_base.hpp"

#include "../types/reflected_collection.hpp"

namespace wgt
{
class IEnumGenerator;
typedef std::unique_ptr<IEnumGenerator> IEnumGeneratorPtr;

/**
 *	Depricated: only for use with EXPOSE macros.
 *	Use nullptr for no metadata where possible.
 */
MetaHandle MetaNone();
MetaHandle MetaMinMax( float min, float max );
MetaHandle MetaStepSize( float stepSize );
MetaHandle MetaDecimals( int decimals );


#define MetaEnumFunc( getterFunc ) \
	MetaEnum(\
		std::unique_ptr<IEnumGenerator>(new ReflectedCollectionImpl(\
			IBasePropertyPtr(\
				new FunctionProperty< std::map< int, std::wstring >,SelfType, true, true >(\
					"EnumTypes", &SelfType::getterFunc, NULL,\
					TypeId::getType< std::map< int, std::wstring > >() ) ) ) ) )

MetaHandle MetaEnum( IEnumGeneratorPtr enumGenerator );
MetaHandle MetaEnum( const wchar_t * enumString );

MetaHandle MetaSlider();

MetaHandle MetaGroup( const wchar_t * groupName );

MetaHandle MetaAttributeDisplayName( const char * attributePath );

MetaHandle MetaDisplayName( const wchar_t * displayName );

MetaHandle MetaDescription( const wchar_t * displayName );

MetaHandle MetaPanelLayout( const char * layoutFile, const char * bindingsFile = NULL );

MetaHandle MetaNoNull();

MetaHandle MetaColor();

MetaHandle MetaHidden();

MetaHandle MetaThumbnail( int width = -1, int height = -1 );

MetaHandle MetaInPlace( const char * propertyName = NULL );

MetaHandle MetaSelected( const char * propertyName );

MetaHandle MetaInPlacePropertyName( const char * propertyName );

MetaHandle MetaReadOnly();

MetaHandle MetaUrl(bool isAssetBrowserDialog = false,
				   const char * urlDlgTitle = nullptr, 
				   const char * urlDlgDefaultFolder = nullptr,
				   int urlDlgModality = 1,
				   const char * urlDlgNameFilters = nullptr, 
				   const char * urlDlgSelectedNameFilter = nullptr );

class IMetaCommandExecutable
{
public:
	virtual ~IMetaCommandExecutable() {}
	virtual void execute( void * pBase, const void * arguments ) const = 0;
};

template< typename SelfType >
class MetaCommandExecutableImpl
	: public IMetaCommandExecutable
{
public:
	typedef void (SelfType::*CommandExecuteFunc)( const void * );

	MetaCommandExecutableImpl( CommandExecuteFunc commandExecuteFunc )
		: commandExecuteFunc_( commandExecuteFunc )
	{
	}

	virtual void execute( void * pBase, const void * arguments ) const
	{
		typedef void (SelfType::*ActualFuncSignature)( const void * );
		ActualFuncSignature actualFunc = 
			reinterpret_cast< ActualFuncSignature >( commandExecuteFunc_ );
		( ( ( SelfType* ) pBase )->*actualFunc )( arguments );
	}

private:
	CommandExecuteFunc commandExecuteFunc_;
};

#define MetaCommand( commandName, executeFunc ) \
	MetaCommandBase( commandName,\
		new MetaCommandExecutableImpl< SelfType >(\
			reinterpret_cast< MetaCommandExecutableImpl< SelfType >::CommandExecuteFunc >(\
			&SelfType::executeFunc ) ) )

MetaHandle MetaCommandBase(
	const wchar_t * commandName, const IMetaCommandExecutable * executeFunc );

MetaHandle MetaNoSerialization();

MetaHandle MetaUniqueId( const char * id );

MetaHandle MetaOnStack();

MetaHandle MetaUnique();

MetaHandle MetaParamHelp( const char* paramName, const MetaParamTypes::MetaParamType paramType, const char* paramDesc );

MetaHandle MetaReturnHelp( const char* returnName, const MetaParamTypes::MetaParamType returnType, const char* returnDesc );

MetaHandle MetaConsoleHelp( const char* text );

MetaHandle MetaScriptFunctionHelp( const char* name );

MetaHandle MetaTooltip( const char* tooltip );

MetaHandle MetaPassword();

MetaHandle MetaMultiline();
} // end namespace wgt
#endif //META_TYPES_HPP
