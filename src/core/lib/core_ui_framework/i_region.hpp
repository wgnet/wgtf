#ifndef I_REGION_HPP
#define I_REGION_HPP

namespace wgt
{
class IView;
struct LayoutTags;

class IRegion
{
public:
	virtual ~IRegion()
	{
	}

	virtual const LayoutTags& tags() const = 0;

	virtual void addView(IView& view) = 0;
	virtual void removeView(IView& view) = 0;
};
} // end namespace wgt
#endif
