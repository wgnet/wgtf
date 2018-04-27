#ifndef I_CURSOR_HPP
#define I_CURSOR_HPP

#include "core_common/assert.hpp"

#include <stdint.h>
#include <memory>

namespace wgt
{

constexpr unsigned int kIdBits = 5;
constexpr uint64_t kIdMask = ~(-1 << kIdBits);

/*! 
	Cursor Identifier which supports a set of standard cursors OR a custom cursor
	The identifier can be implicitly converted to a 64bit integer so it can be opaquely passed to code external to TF.

	A minimal set of bits are used for standard cursor identifiers, the remaining bits represent a pointer to a native
	cursor.
*/
struct CursorId
{
	static CursorId make(uint8_t id, void* nativeCursor = nullptr)
	{
		CursorId cursorId;
		// First set of bits for standard cursor ids
		auto data = reinterpret_cast<uint64_t>(nativeCursor) << kIdBits;
		cursorId.cursorData_ = data;
		// Make sure the pointer fits inside the remaining bits
		TF_ASSERT(data >> kIdBits == reinterpret_cast<uint64_t>(nativeCursor));
		cursorId.cursorData_ |= id;
		return cursorId;
	}

	uint8_t id() const { return cursorData_ & kIdMask; }
	void* nativeCursor() const { return reinterpret_cast<void*>(cursorData_ >> kIdBits); }

	operator uint64_t() { return cursorData_; }

	bool operator==(const CursorId& other){ return cursorData_ == other.cursorData_; }
	bool operator!=(const CursorId& other){ return cursorData_ != other.cursorData_; }

	uint64_t	cursorData_;
};
static_assert(sizeof(CursorId) == sizeof(uint64_t), "Incorrect size for CursorId, should be a 64bit value");

#pragma region Standard Cursors - Ids must match the enum CursorShape in qnamespace.h and GameCentralUtils CursorTypes.h
constexpr CursorId ArrowCursor			= {0};
constexpr CursorId UpArrowCursor		= {1};
constexpr CursorId CrossCursor			= {2};
constexpr CursorId WaitCursor			= {3};
constexpr CursorId IBeamCursor			= {4};
constexpr CursorId SizeVerCursor		= {5};
constexpr CursorId SizeHorCursor		= {6};
constexpr CursorId SizeBDiagCursor		= {7};
constexpr CursorId SizeFDiagCursor		= {8};
constexpr CursorId SizeAllCursor		= {9};
constexpr CursorId BlankCursor			= {10};
constexpr CursorId SplitVCursor			= {11};
constexpr CursorId SplitHCursor			= {12};
constexpr CursorId PointingHandCursor	= {13};
constexpr CursorId ForbiddenCursor		= {14};
constexpr CursorId WhatsThisCursor		= {15};
constexpr CursorId BusyCursor			= {16};
constexpr CursorId OpenHandCursor		= {17};
constexpr CursorId ClosedHandCursor		= {18};
constexpr CursorId DragCopyCursor		= {19};
constexpr CursorId DragMoveCursor		= {20};
constexpr CursorId DragLinkCursor		= {21};
constexpr CursorId LastCursor = DragLinkCursor;
constexpr CursorId BitmapCursor			= {24};
constexpr CursorId CustomCursor			= {25};
constexpr CursorId InvalidCursor		= {-1};
static_assert(LastCursor.cursorData_ < 32u, "Can't fit Cursor id into bits available for Cursor's type");
#pragma endregion

class ICursor
{
public:
	virtual ~ICursor()
	{
	}

	virtual CursorId id() const = 0;
	virtual void* nativeCursor() const = 0;
};

using ICursorPtr = std::unique_ptr<ICursor>;
} // end namespace wgt
#endif // I_CURSOR_HPP
