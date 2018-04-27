//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//  interpolator_factory.hpp
//
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  Copyright (c) Wargaming.net. All rights reserved.
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#ifndef INTERPOLATOR_FACTORY_
#define INTERPOLATOR_FACTORY_

#pragma once

#include "curve_editor/curve_types.hpp"
#include "curve_editor/i_curve_interpolator.hpp"

namespace wgt
{
ICurveInterpolatorPtr createInterpolator(CurveTypes::CurveType);
} // end namespace wgt
#endif // INTERPOLATOR_FACTORY_
