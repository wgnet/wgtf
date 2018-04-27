#pragma once
#include <stdint.h>

namespace wgt
{
	class StaticSettingsData
	{
	public:
		void setFirst(const int64_t& newFirst) { first_ = newFirst; }
		void setSecond(const int64_t& newSecond) { second_ = newSecond; }

		int64_t getFirst() const { return first_; }
		int64_t getSecond() const { return second_; }
		int64_t getResult() const { return result_; }

		void calculateResult() { result_ = first_ * second_; }

	private:
		int64_t first_ = 2;
		int64_t second_ = 3;
		int64_t result_ = 0;
	};
} // end namespace wgt