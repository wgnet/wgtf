#ifndef I_COPY_PASTE_MANAGER_HPP
#define I_COPY_PASTE_MANAGER_HPP

#include "core_serialization/i_datastream.hpp"

namespace wgt
{
class ICopyableObject;
class ICopyPasteManager
{

public:
	virtual ~ICopyPasteManager(){}

	virtual void onSelect( ICopyableObject* pObject, bool append = false ) = 0;
	virtual void onDeselect( ICopyableObject* pObject, bool reset = true ) = 0;
	virtual bool copy() = 0;
	virtual bool paste() = 0;
	virtual bool canCopy() const = 0;
	virtual bool canPaste() const = 0;

};
} // end namespace wgt
#endif // I_COPY_PASTE_MANAGER_HPP
