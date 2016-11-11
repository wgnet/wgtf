#ifndef I_COPYABLE_OBJECT_HPP
#define I_COPYABLE_OBJECT_HPP

namespace wgt
{
class ICopyPasteManager;
class Variant;
class ICopyableObject
{
public:
	virtual ~ICopyableObject()
	{
	}

	virtual const Variant& getData() = 0;
	virtual const char* getDataHint() const = 0;
	virtual bool setData(const Variant& value) = 0;
};
} // end namespace wgt
#endif // I_COPYABLE_OBJECT_HPP
