#ifndef I_TEST_DATA_PROVIDER_HPP
#define I_TEST_DATA_PROVIDER_HPP

#include <memory>

namespace wgt
{

class IStringList;

class ITestDataProvider
{
public:
	virtual ~ITestDataProvider() {}
	virtual std::unique_ptr< IStringList > getStringList(bool isShort) const = 0;
};

} // end namespace wgt

#endif // I_TEST_DATA_PROVIDER_HPP