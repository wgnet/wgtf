
#ifndef __GRAPHEDITOR_ACTION_H__
#define __GRAPHEDITOR_ACTION_H__

#include <core_reflection/object_handle.hpp>

#include <string>
#include <functional>

namespace wgt
{
class Action
{
public:
    typedef std::function<void(float x, float y, size_t)> TOnTrigger;
    void SetParams(std::string const& title, TOnTrigger const& callback);

    std::string const& GetTitle() const;
    void Trigger(float x, float y, size_t objectUid);

private:
    std::string title;
    TOnTrigger callback;
};
} // end namespace wgt
#endif // __GRAPHEDITOR_ACTION_H__
