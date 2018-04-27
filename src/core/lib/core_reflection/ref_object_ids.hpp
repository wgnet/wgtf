#ifndef REF_OBJECT_IDS_HPP
#define REF_OBJECT_IDS_HPP

#include "ref_object_id.hpp"
//#include "reflection_dll.hpp"
#include <vector>

namespace wgt
{
class REFLECTION_DLL RefObjectIds
{
public:
	/*! Retrieves the current id
	*/
	const RefObjectId& current() const;
	/*! Will retrieve the next id, generating a new one if necessary
	*/
	const RefObjectId& next();
	/*! Resets back to the beginning
	*/
	void begin();

private:
	std::vector<RefObjectId>	ids_;
	size_t						current_ = -1;
};

} // end namespace wgt
#endif // REF_OBJECT_IDS_HPP
