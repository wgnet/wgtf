#include "shared_library.hpp"


namespace wgt
{
SharedLibrary::SharedLibrary():
	lib_(nullptr)
{
}


SharedLibrary::SharedLibrary(SharedLibrary&& that):
	lib_(that.lib_)
{
	that.lib_ = nullptr;
}


SharedLibrary::SharedLibrary(const char* fileName):
	lib_(nullptr)
{
	load(fileName);
}


SharedLibrary::~SharedLibrary()
{
	unload();
}


SharedLibrary& SharedLibrary::operator=(SharedLibrary&& that)
{
	if(lib_ != that.lib_)
	{
		unload();
		lib_ = that.lib_;
		that.lib_ = nullptr;
	}

	return *this;
}


bool SharedLibrary::isValid() const
{
	return lib_ != nullptr;
}
} // end namespace wgt
