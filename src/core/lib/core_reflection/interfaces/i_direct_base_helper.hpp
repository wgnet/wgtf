#ifndef I_DIRECT_BASE_HELPER_HPP
#define I_DIRECT_BASE_HELPER_HPP

#include "../reflection_dll.hpp"
#include <vector>

namespace wgt
{

class REFLECTION_DLL IDirectBaseHelper
{
public:
	typedef std::vector< std::string > ParentCollection;
	typedef std::function< void *( void * ) > CasterSig;
	typedef std::vector< CasterSig > CasterCollection;

	IDirectBaseHelper() {}
	virtual ~IDirectBaseHelper() {}
	virtual const ParentCollection & getParents() const = 0;
	virtual const CasterCollection & getCasters() const = 0;
};

} // end namespace wgt
#endif // I_DIRECT_BASE_HELPER_HPP
