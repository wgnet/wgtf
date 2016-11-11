#ifndef SHARED_LIBRARY_HPP_INCLUDED
#define SHARED_LIBRARY_HPP_INCLUDED

namespace wgt
{
class SharedLibrary
{
public:
	SharedLibrary();
	SharedLibrary(SharedLibrary&& that);
	explicit SharedLibrary(const char* fileName);
	~SharedLibrary();

	SharedLibrary& operator=(SharedLibrary&& that);

	bool isValid() const;

	bool load(const char* fileName);
	void unload();
	void* findRawSymbol(const char* name) const;

	template <typename T>
	T* findSymbol(const char* name) const
	{
		return reinterpret_cast<T*>(findRawSymbol(name));
	}

	template <typename T>
	bool findSymbol(const char* name, T** ptr) const
	{
		*ptr = findSymbol<T>(name);
		return *ptr != nullptr;
	}

private:
	SharedLibrary(const SharedLibrary&);
	SharedLibrary& operator=(const SharedLibrary&);

	void* lib_;
};

#if defined(_MSC_VER)

#define EXPORT extern "C" __declspec(dllexport)

#else

#define EXPORT extern "C" __attribute__((visibility("default")))

#endif
} // end namespace wgt
#endif
