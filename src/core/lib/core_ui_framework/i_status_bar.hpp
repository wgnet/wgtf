#ifndef I_STATUS_BAR_HPP
#define I_STATUS_BAR_HPP

namespace wgt
{
class IStatusBar
{
public:
	virtual ~IStatusBar()
	{
	}

	/*! Hides the normal status indications and displays the given message for the specified number of milli-seconds
	   (timeout).
	    If timeout is 0 (default), the message remains displayed until clearMessage() is called or until the
	   showMessage() is called again to change the message.

	    Note that showMessage() is called to show temporary explanations of tool tip texts, so passing a timeout of 0 is
	   not sufficient to display a permanent message.
	*/
	virtual void showMessage(const char* message, int timeout = 0) = 0;

	/*! Removes any temporary message being shown.
	*/
	virtual void clearMessage() = 0;
};
} // end namespace wgt
#endif // I_STATUS_BAR_HPP
