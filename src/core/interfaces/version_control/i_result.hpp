//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//  i_result.h
//
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  Copyright (c) Wargaming.net. All rights reserved.
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#ifndef I_RESULT_H_
#define I_RESULT_H_

#pragma once

#include <memory>

namespace wgt
{
class IResult
{
public:
	virtual ~IResult(){}
	virtual const char* errors() const = 0;
	virtual const char* output() const = 0;

	virtual bool hasErrors() const { return errors() && *errors(); }
};

typedef std::unique_ptr<IResult> IResultPtr;
} // end namespace wgt
#endif // I_RESULT_H_
