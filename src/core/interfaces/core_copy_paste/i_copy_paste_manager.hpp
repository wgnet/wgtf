#ifndef I_COPY_PASTE_MANAGER_HPP
#define I_COPY_PASTE_MANAGER_HPP

#include "core_serialization/i_datastream.hpp"
#include "core_common/signal.hpp"
#include <map>
#include <string>

namespace wgt
{
typedef std::map<std::string, std::vector<char>> MimeData;
class ICopyPasteManager
{
public:
	virtual ~ICopyPasteManager()
	{
	}

	virtual MimeData getClipboardContents() = 0;
	virtual void setClipboardContents(MimeData& mimeData) = 0;
	virtual std::string getText() = 0;
	virtual void setText(std::string str) = 0;

	typedef void ClipboardSignature();
	typedef std::function<ClipboardSignature> ClipboardCallback;
	virtual Connection connectClipboardDataChanged(ClipboardCallback callback)
	{
		return Connection();
	}
};
} // end namespace wgt
#endif // I_COPY_PASTE_MANAGER_HPP
