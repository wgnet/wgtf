#include "shared_string.hpp"

namespace wgt
{

bool downcast(wgt::SharedString * v, const wgt::SharedString & storage)
{
	*v = storage;
	return true;
}

} //end namespace wgt