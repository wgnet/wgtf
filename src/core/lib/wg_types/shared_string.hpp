#ifndef SHARED_STRING_HPP
#define SHARED_STRING_HPP

#include <string>
#include <memory>

namespace wgt
{

class SharedString
{
public:
	SharedString()
		: storage_(std::make_shared< std::string >())
	{
	}

	SharedString(const char * str)
		: storage_( std::make_shared< std::string >( str ) )
	{
	}

	SharedString(const std::string & str)
		: storage_(std::make_shared< std::string >(str))
	{
	}

	SharedString( std::string && str)
		: storage_(std::make_shared< std::string >( std::move( str ) ))
	{
	}

	const std::string & str() const
	{
		return *storage_.get();
	}

	const std::string * operator->() const
	{
		return storage_.get();
	}

	//operator std::string() const
	//{
	//	return *storage_.get();
	//}

	//operator const char *() const
	//{
	//	return storage_->c_str();
	//}

	std::shared_ptr< const std::string > getStorage() const
	{
		return storage_;
	}

	SharedString & operator+= (const std::string & other)
	{
		*storage_ += other;
		return *this;
	}

private:
	std::shared_ptr< std::string > storage_;
};

bool downcast(wgt::SharedString * v, const wgt::SharedString & storage);

}

#endif //SHARED_STRING_HPP