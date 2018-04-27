#pragma once
#ifndef ISERIALIZERNEW_HPP
#define ISERIALIZERNEW_HPP
#include "v0/ISerializerNew_v0.hpp"

namespace wgt
{
enum SerializationFormat : int
{
	DEFAULT,
	XML,
	END
};

DECLARE_LATEST_VERSION(ISerializerNew, 0, 0)
} // end namespace wgt

#endif // ISERIALIZERNEW_HPP