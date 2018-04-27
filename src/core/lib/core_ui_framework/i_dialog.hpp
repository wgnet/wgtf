#pragma once

#include "core_common/signal.hpp"
#include "core_reflection/object_handle.hpp"

#include <memory>

namespace wgt
{
class DialogModel;
typedef std::unique_ptr<class IManagedObject> ManagedObjectPtr;

class IDialog
{
public:
	enum class Mode
	{
		MODAL,
		MODELESS,
		MODAL_NONBLOCKING,
	};

	typedef int Result;
	static const Result INVALID_RESULT = -1;

	typedef void ClosedSignature(IDialog& dialog);
	typedef std::function<ClosedSignature> ClosedCallback;

	virtual ~IDialog()
	{
	}

	virtual const char* title() const = 0;
	virtual Result result() const = 0;
	virtual ObjectHandleT<DialogModel> model() const = 0;
	virtual void setModel(ObjectHandleT<DialogModel> model) = 0;
	virtual void setModel(ManagedObjectPtr model) = 0;
	virtual void setModel(const std::nullptr_t&) = 0;
	virtual void load(const char* resource) = 0;
	virtual void show(Mode mode) = 0;
	virtual void close(Result result) = 0;
	virtual bool isOpen() const = 0;
	virtual void waitForClose() = 0;
	virtual void raise() = 0;

	virtual Connection connectClosedCallback(ClosedCallback callback) = 0;
};

} // end namespace wgt
