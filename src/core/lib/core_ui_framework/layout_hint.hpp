#ifndef LAYOUT_HINT_HPP
#define LAYOUT_HINT_HPP

#include <map>
#include <string>

namespace wgt
{
struct LayoutTags;

struct LayoutHint
{
public:
	LayoutHint();
	LayoutHint( const char * hint, float strength = 1.0f );
	LayoutHint operator + ( const LayoutHint & other );
	LayoutHint & operator += ( const LayoutHint & other );
	void clear();
	float match( const LayoutTags & tags ) const;

private:
	std::map< std::string, float > hints_;
};
} // end namespace wgt
#endif//LAYOUT_HINT_HPP
