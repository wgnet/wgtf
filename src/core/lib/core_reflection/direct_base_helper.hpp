#ifndef DIRECT_BASE_HELPER_HPP
#define DIRECT_BASE_HELPER_HPP

#include "interfaces/i_direct_base_helper.hpp"
#include "core_variant/collection.hpp"
#include "reflected_object.hpp"
#include <vector>

namespace wgt
{

template< typename BaseType, typename Arg1, typename ...Args >
struct BaseHelperT
{
	typedef std::function< void *(BaseType *) > CastSig;
	typedef std::vector< CastSig > CastingCollection;

	static void bases(
		IDirectBaseHelper::ParentCollection & o_Bases,
		IDirectBaseHelper::CasterCollection & o_CastMap )
	{
		BaseHelperT< BaseType, Arg1 >::bases( o_Bases, o_CastMap );
		BaseHelperT< BaseType, Args... >::bases( o_Bases, o_CastMap );
	}
};


template< typename BaseType, typename Arg1 >
struct BaseHelperT< BaseType, Arg1 >
{
	static void bases(
		IDirectBaseHelper::ParentCollection & o_Bases,
		IDirectBaseHelper::CasterCollection & o_CastMap)
	{
		static_assert(
			std::is_base_of< Arg1, BaseType >::value,
			"Invalid base type defined in reflection meta tag." );
		o_Bases.push_back(getClassIdentifier< Arg1 >());
		o_CastMap.push_back( [](void * pObject)
		{
			return reinterpret_cast< void * >(
				static_cast<Arg1 *>(
					reinterpret_cast<BaseType *>( pObject) ) );
		});
	}
};


template< typename BaseType, typename ...Args >
class DirectBaseHelperT : public IDirectBaseHelper
{
public:
	DirectBaseHelperT()
	{
		BaseHelperT< BaseType, Args... >::bases( parents_ , casters_ );
	}

	const ParentCollection & getParents() const override
	{
		return parents_;
	}

	const CasterCollection & getCasters() const override
	{
		return casters_;
	}
private:
	ParentCollection parents_;
	CasterCollection casters_;
};

} // end namespace wgt
#endif // #define DIRECT_BASE_HELPER_HPP
