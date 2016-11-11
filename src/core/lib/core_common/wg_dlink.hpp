#ifndef WG_DLINK_HPP_INCLUDED
#define WG_DLINK_HPP_INCLUDED

#include <cstddef>

/**
Calculate pointer to holder of the link.

Example:
@code
// declare some object that holds a link
typedef std::pair<int, DLink> Holder;
Holder holder;

// declare an empty link that is used to manage list
DLink list;

// add object to the list
list.append(&holder.second);

// get object pointer from list
Holder* deducedHolder = dlink_holder(Holder, second, list.next());

assert(deducedHolder == &holder);
@endcode
*/
#define dlink_holder(HolderType, member, link) \
	reinterpret_cast<HolderType*>(reinterpret_cast<char*>(link) - offsetof(HolderType, member))

namespace wgt
{
/**
Utility class to link objects into lists.

Class invariants:
@li both next() and prev() are always set and valid;
@li this->next()->prev() == this->prev()->next() == this
*/
class DLink
{
public:
	DLink();
	DLink(const DLink& v);
	~DLink();

	DLink& operator=(const DLink& v);

	/**
	Check if this link is linked with anything else but self.
	*/
	bool isLinked() const;

	/**
	Remove this link from list.
	*/
	void unlink();

	/**
	Make the specified link to go before this one.
	*/
	void prepend(DLink* link);

	/**
	Make the specified link to go after this one.
	*/
	void append(DLink* link);

	DLink* next() const
	{
		return next_;
	}

	DLink* prev() const
	{
		return prev_;
	}

private:
	DLink* next_;
	DLink* prev_;
};
}

#endif
