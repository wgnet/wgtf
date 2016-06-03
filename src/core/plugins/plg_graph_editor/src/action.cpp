
#include "action.h"
#include "Metadata/Action.mpp"

namespace wgt
{
void Action::SetParams(std::string const& title_, TOnTrigger const& callback_)
{
    title = title_;
    callback = callback_;
}

std::string const& Action::GetTitle() const
{
    return title;
}

void Action::Trigger(float x, float y, size_t objectUid)
{
    if (callback)
    {
        callback(x, y, objectUid);
    }
}
} // end namespace wgt
