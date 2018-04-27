#ifndef TEST_OBJECTS_HPP
#define TEST_OBJECTS_HPP

#include "test_structure.hpp"

namespace wgt
{
class TestObjects
{
public:
	TestStructure& getTestStructure();

private:
	TestStructure structure_;
};
} // end namespace wgt
#endif // TEST_OBJECTS_HPP
