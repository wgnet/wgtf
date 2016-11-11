#include "wg_dlink.hpp"

namespace
{
#if 0

	class Unit
	{
	public:
		Unit()
		{
			// declare some object that holds a link
			typedef std::pair<int, wg::DLink> Holder;
			Holder holder;

			// declare an empty link that is used to manage list
			wg::DLink list;

			// add object to the list
			list.append(&holder.second);

			// get object pointer from list
			Holder* deducedHolder = dlink_holder(Holder, second, list.next());

			MF_ASSERT(deducedHolder == &holder);
		}

	} unit;

#endif
}

namespace wgt
{
DLink::DLink() : next_(this), prev_(this)
{
}

DLink::DLink(const DLink& v) : next_(this), prev_(this)
{
}

DLink::~DLink()
{
	unlink();
}

DLink& DLink::operator=(const DLink& v)
{
	return *this;
}

bool DLink::isLinked() const
{
	return next_ != this;
}

void DLink::unlink()
{
	prev_->next_ = next_;
	next_->prev_ = prev_;

	prev_ = this;
	next_ = this;
}

void DLink::prepend(DLink* link)
{
	link->unlink();

	prev_->next_ = link;
	link->prev_ = prev_;
	link->next_ = this;
	prev_ = link;
}

void DLink::append(DLink* link)
{
	link->unlink();

	next_->prev_ = link;
	link->next_ = next_;
	link->prev_ = this;
	next_ = link;
}
}
