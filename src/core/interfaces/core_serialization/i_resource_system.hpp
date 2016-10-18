//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//  i_resource_system.hpp
//
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  Copyright (c) Wargaming.net. All rights reserved.
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#ifndef I_RESOURCE_SYSTEM_HPP_
#define I_RESOURCE_SYSTEM_HPP_

#pragma once
#include <memory>

namespace wgt
{
class BinaryBlock;
class IResourceSystem
{
public:
	typedef std::unique_ptr<BinaryBlock> BinaryBlockPtr;

	virtual ~IResourceSystem(){}
	virtual bool exists(const char* resource) const = 0;
	virtual BinaryBlockPtr readBinaryContent(const char* resource) const = 0;
};
} // end namespace wgt
#endif // I_RESOURCE_SYSTEM_HPP_
