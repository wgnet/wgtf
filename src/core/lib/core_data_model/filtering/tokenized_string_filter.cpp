#include "tokenized_string_filter.hpp"
#include "../i_item.hpp"
#include "../i_item_role.hpp"
#include <iostream>
#include <sstream>
#include <mutex>

namespace wgt
{
struct TokenizedStringFilter::Implementation
{
	Implementation( TokenizedStringFilter & self );
	
	TokenizedStringFilter & self_;
	std::vector< std::string > filterTokens_;
	std::string sourceFilterText_;
	std::string splitter_;
	unsigned int roleId_;
	std::mutex filterTokensLock_;
};

TokenizedStringFilter::Implementation::Implementation( TokenizedStringFilter & self )
	: self_( self )
	, sourceFilterText_( "" )
	, splitter_( " " )
	, roleId_( 0 )
{
}

TokenizedStringFilter::TokenizedStringFilter()
	: impl_( new Implementation( *this ) )
{
}

TokenizedStringFilter::~TokenizedStringFilter()
{
}

void TokenizedStringFilter::updateFilterTokens( const char * filterText )
{
	impl_->sourceFilterText_ = filterText;

	std::lock_guard<std::mutex>( impl_->filterTokensLock_ );
	impl_->filterTokens_.clear();

	std::istringstream stream( filterText );
	std::string token;

	char splitter = ' ';
	if (impl_->splitter_.length() > 0)
	{
		splitter = impl_->splitter_[0];
	}
		
	while (std::getline( stream, token, splitter ))
	{
		if (token.length() > 0)
		{
			std::transform( token.begin(), token.end(), token.begin(), ::tolower );
			impl_->filterTokens_.push_back( token );
		}
	}
}

const char* TokenizedStringFilter::getFilterText()
{
	return impl_->sourceFilterText_.c_str();
}

void TokenizedStringFilter::setSplitterChar( const char * splitter )
{
	impl_->splitter_ = splitter;
}

const char* TokenizedStringFilter::getSplitterChar()
{
	return impl_->splitter_.c_str();
}

void TokenizedStringFilter::setRole( unsigned int roleId )
{
	impl_->roleId_ = roleId;
}

bool TokenizedStringFilter::checkFilter( const IItem* item )
{
	if (impl_->filterTokens_.size() < 1)
	{
		return true;
	}

	std::string haystack = "";

	if (impl_->roleId_ == 0)
	{
		haystack = item->getDisplayText( 0 );
	}
	else 
	{
		auto data = item->getData( 0, impl_->roleId_ );
		bool result = data.tryCast( haystack );
		if (!result)
		{
			// The developer should provide a roleId that corresponds to string data
			return false;
		}
	}

	if (haystack.length() == 0)
	{
		return false;
	}
	
	std::transform( haystack.begin(), haystack.end(), haystack.begin(), ::tolower );
	std::lock_guard<std::mutex>( impl_->filterTokensLock_ );

	for (auto & filter : impl_->filterTokens_)
	{
		if (haystack.find( filter ) == std::string::npos)
		{
			return false;
		}
	}

	return true;
}
} // end namespace wgt
