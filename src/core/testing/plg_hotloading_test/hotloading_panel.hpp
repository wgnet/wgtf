#pragma once
#ifndef _HOTLOADINGPANEL_HPP
#define _HOTLOADINGPANEL_HPP

#include "core_reflection/reflection_macros.hpp"
#include "core_dependency_system/depends.hpp"
#include <string>

class QQmlEngine;

namespace wgt
{
class IFileSystem;
class IQtFramework;

class HotloadingPanel : Depends<IFileSystem, IQtFramework>
{
	DECLARE_REFLECTED

public:
	void initialise();

	const HotloadingPanel* getSource() const;

	std::string getHotloadingText();

	std::string getHotloadingBaseText();

	std::string getHotloadingJSText();

	std::string getErrorText();

	void setErrorText(const char* text);

	void setHotloadingText(std::string text);

	void setHotloadingBaseText(std::string text);

	void setHotloadingJSText(std::string text);

private:
	struct HotloadedFile
	{
		std::string path_;
		std::string text_;
	};

	void initialiseFile(const char* name, HotloadedFile& file);

	void saveTextToFile(const std::string& path, const std::string& text);

	std::string getTextFromFile(const std::string& path);

	IFileSystem* fileSystem_;
	HotloadedFile hotLoadedFile_;
	HotloadedFile hotLoadedBaseFile_;
	HotloadedFile hotLoadedJSFile_;
	std::string errorText_;
};

} // end namespace wgt
#endif // _HOTLOADINGPANEL_HPP