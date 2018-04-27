#pragma once
#include <stdint.h>

namespace wgt
{
	class UniqueSettingsData
	{
	public:
		int64_t getFirstValue() const { return firstValue_; }
		int64_t getSecondValue() const { return secondValue_; }
		bool getFirstCondition() const { return firstCondition_; }
		bool getSecondCondition() const { return secondCondition_; }

		void setFirstValue(const int64_t& i) { firstValue_ = i; }
		void setSecondValue(const int64_t& i) { secondValue_ = i; }
		void setFirstCondition(const bool& i) { firstCondition_ = i; }
		void setSecondCondition(const bool& i) { secondCondition_ = i; }

	private:
		int64_t firstValue_ = 7;
		int64_t secondValue_ = 42;
		bool firstCondition_ = true;
		bool secondCondition_ = false;
	};
} // end namespace wgt