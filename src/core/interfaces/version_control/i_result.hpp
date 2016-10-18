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
#include <string>
#include <unordered_map>
#include <vector>

namespace wgt
{
typedef std::unordered_map<std::string, std::string> Attributes;
typedef std::vector<Attributes> AttributeResults;
class IResult
{
public:
	virtual ~IResult(){}
	virtual const char* errors() const = 0;
	virtual const char* output() const = 0;

	virtual bool hasErrors() const { return errors() && *errors(); }

	// Overridden to provide grouped results of key value pairs when multiple results are possible
	virtual const AttributeResults& results() const
	{
		static AttributeResults emptyResults;
		return emptyResults;
	}
};

typedef std::unique_ptr<IResult> IResultPtr;
} // end namespace wgt
#endif // I_RESULT_H_
