#ifndef TOKENIZED_STRING_FILTER_HPP
#define TOKENIZED_STRING_FILTER_HPP

#include "i_item_filter.hpp"
#include <memory>

namespace wgt
{
/**
 *	TokenizedStringFilter
 *  A filter implementation, which uses a vector of strings to compare text against.
 */
class TokenizedStringFilter : public IItemFilter
{
public:
	TokenizedStringFilter();
	virtual ~TokenizedStringFilter();

	virtual void setRole( ItemRole::Id roleId ) override;
	
	virtual bool checkFilter( const IItem * item ) override;

	void updateFilterTokens( const char * filterText );
	const char* getFilterText();

	void setSplitterChar( const char * splitter );
	const char* getSplitterChar();

private:
	struct Implementation;
	std::unique_ptr<Implementation> impl_;
};
} // end namespace wgt
#endif // TOKENIZED_STRING_FILTER_HPP
