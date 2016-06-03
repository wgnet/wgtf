//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//  perforce_result.h
//
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  Copyright (c) Wargaming.net. All rights reserved.
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#ifndef PERFORCE_RESULT_H_
#define PERFORCE_RESULT_H_

#pragma once

#include "version_control/i_result.hpp"

#include <string>
#include <vector>

namespace wgt
{
class PerforceResult : public IResult
{
public:
	PerforceResult(const char* output, const char* errors);

	virtual const char* errors() const override;
	virtual const char* output() const override;

private:
	std::string output_;
	std::string errors_;
};

class MultiResult : public IResult
{
public:
	void appendResult(IResultPtr result);

	virtual const char* errors() const override;
	virtual const char* output() const override;

private:
	std::vector<IResultPtr> results_;
	mutable std::string output_;
	mutable std::string errors_;
};
} // end namespace wgt
#endif // PERFORCE_RESULT_H_
