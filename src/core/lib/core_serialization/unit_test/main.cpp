#include "pch.hpp"
#include <stdlib.h>
#include "core_variant/variant.hpp"
#include "core_reflection/object_handle.hpp"

int main( int argc, char* argv[] )
{
	using namespace wgt;
#ifdef _WIN32
	_set_error_mode(_OUT_TO_STDERR);
	_set_abort_behavior( 0, _WRITE_ABORT_MSG);
#endif // _WIN32
	std::vector<std::unique_ptr<MetaType>> metaTypes;
	metaTypes.emplace_back(new MetaTypeImpl<ObjectHandle>("object"));
	for(const auto& m: metaTypes)
	{
		Variant::registerType(m.get());
	}

	int result = BWUnitTest::runTest(
			"serialization test", argc, argv );
	return result;
}

// main.cpp
