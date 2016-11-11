//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//  perforce_result.cpp
//
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  Copyright (c) Wargaming.net. All rights reserved.
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "perforce_result.hpp"

#include <sstream>

namespace wgt
{
PerforceResult::PerforceResult(const char* output, const char* errors, AttributeResults&& results)
    : output_(output), errors_(errors), results_(std::move(results))
{
}

const char* PerforceResult::errors() const
{
	return errors_.c_str();
}

const char* PerforceResult::output() const
{
	return output_.c_str();
}

const AttributeResults& PerforceResult::results() const
{
	return results_;
}

void MultiResult::appendResult(IResultPtr result)
{
	results_.emplace_back(std::move(result));
	output_.clear();
	errors_.clear();
	attributeResults_.clear();
}

const char* MultiResult::errors() const
{
	if (errors_.empty())
	{
		std::stringstream stream;
		for (auto& result : results_)
		{
			if (result->hasErrors())
			{
				stream << result->errors() << std::endl << std::endl;
			}
		}
		errors_ = stream.str();
	}
	return errors_.c_str();
}

const char* MultiResult::output() const
{
	if (output_.empty())
	{
		std::stringstream stream;
		for (auto& result : results_)
		{
			if (result->output() && *result->output())
			{
				stream << result->output() << std::endl << std::endl;
			}
		}
		output_ = stream.str();
	}
	return output_.c_str();
}

const AttributeResults& MultiResult::results() const
{
	if (attributeResults_.empty())
	{
		for (auto& result : results_)
		{
			for (auto& attributeResult : result->results())
			{
				attributeResults_.emplace_back(attributeResult);
			}
		}
	}
	return attributeResults_;
}
} // end namespace wgt
