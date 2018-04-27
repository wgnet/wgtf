#include "qt_new_handler.hpp"
#include <assert.h>

namespace wgt
{
static std::set<void*>* s_QtInPlaceNewed = nullptr;

void insertItemQtInPlaceNewCollection(void* ptr)
{
    if (s_QtInPlaceNewed == nullptr)
    {
        s_QtInPlaceNewed = new std::set<void*>;
    }
    s_QtInPlaceNewed->insert(ptr);
}

bool eraseItemQtInPlaceNewCollection(void* ptr)
{
    bool erased = false;
    if (s_QtInPlaceNewed)
    {
        auto findIt = s_QtInPlaceNewed->find(ptr);
        if (findIt != s_QtInPlaceNewed->end())
        {
            s_QtInPlaceNewed->erase(ptr);
            erased = true;
        }

        if (s_QtInPlaceNewed->empty())
        {
            delete s_QtInPlaceNewed;
            s_QtInPlaceNewed = nullptr;
        }
    }

    return erased;
}

} // end namespace wgt
