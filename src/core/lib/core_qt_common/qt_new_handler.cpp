#include "qt_new_handler.hpp"
#include <assert.h>

namespace wgt
{
static std::set<void*>* s_QtInPlaceNewed = nullptr;

std::set<void*>& getQtInPlaceNewCollection()
{
	if (s_QtInPlaceNewed == nullptr)
	{
		s_QtInPlaceNewed = new std::set<void*>;
	}
	return *s_QtInPlaceNewed;
}

void releaseQtInPlaceNewCollection()
{
	assert(s_QtInPlaceNewed && s_QtInPlaceNewed->empty());
	delete s_QtInPlaceNewed;
	s_QtInPlaceNewed = nullptr;
}
} // end namespace wgt
