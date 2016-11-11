#pragma once
#ifndef _I_SPASH_V0_HPP
#define _I_SPASH_V0_HPP

#include "core_dependency_system/i_interface.hpp"

namespace wgt
{
class BinaryBlock;

DECLARE_INTERFACE_BEGIN(ISplash, 0, 0)
/**
 *	Get data for splash screen.
 *	@param o_data output image data.
 *	@param o_format output image format, e.g. "PNG".
 *		@see http://doc.qt.io/qt-5/qpixmap.html#reading-and-writing-image-files
 *	@note ownership is transferred from implementation to caller.
 *	@return true on success.
 */
virtual bool loadData(std::unique_ptr<BinaryBlock>& o_data, std::string& o_format) const = 0;
DECLARE_INTERFACE_END()

} // end namespace wgt

#endif // _I_SPASH_V0_HPP
