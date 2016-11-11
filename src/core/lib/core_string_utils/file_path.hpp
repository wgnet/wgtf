//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//  file_path.hpp
//
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  Copyright (c) Wargaming.net. All rights reserved.
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#ifndef FILE_PATH_H_
#define FILE_PATH_H_

#pragma once

#include <string>
#include <algorithm>

namespace wgt
{
/*!
 * \class FilePath
 *
 * \brief Normalized file paths using a consistent native directory separator
 *
 * Provides convenience methods to retrieve file extension, file name, and folder
 *
 * \author m_martin
 * \date August 2015
 */
class FilePath
{
public:
// Defined in this header to allow using the FilePath class without linking
#if defined(_WIN32)
	static const char kNativeDirectorySeparator = '\\';
	static const char kNativeAltDirectorySeparator = '/';
#else
	static const char kNativeDirectorySeparator = '/';
	static const char kNativeAltDirectorySeparator = '\\';
#endif

	static const char kAltDirectorySeparator = '\\';
	static const char kDirectorySeparator = '/';
	static const char kExtensionSeparator = '.';
	static const char kVolumeSeparator = ':';

	template <class Type>
	FilePath(Type&& path, const char& directorySeparator = kNativeDirectorySeparator)
	    : fullPath_(combine(std::forward<Type>(path), "", directorySeparator)), directorySeparator_(directorySeparator)
	{
	}

	template <class Type1, class Type2>
	FilePath(Type1&& part1, Type2&& part2, const char& directorySeparator = kNativeDirectorySeparator)
	    : fullPath_(combine(std::forward<Type1>(part1), std::forward<Type2>(part2), directorySeparator)),
	      directorySeparator_(directorySeparator)
	{
	}

	FilePath(const FilePath& other) : fullPath_(other.fullPath_), directorySeparator_(other.directorySeparator_)
	{
	}

	FilePath(FilePath&& other) : fullPath_(std::move(other.fullPath_)), directorySeparator_(other.directorySeparator_)
	{
	}

	FilePath& operator=(const FilePath& rhs)
	{
		fullPath_ = rhs.fullPath_;
		directorySeparator_ = rhs.directorySeparator_;
		return *this;
	}

	FilePath& operator=(FilePath&& rhs)
	{
		fullPath_ = std::move(rhs.fullPath_);
		directorySeparator_ = rhs.directorySeparator_;
		return *this;
	}

	/*! Gets the full path as a string
	*/
	const std::string& str() const
	{
		return fullPath_;
	}

	/*! Gets the extension of this path without the separator
	*/
	std::string getExtension() const
	{
		auto pos = fullPath_.rfind(kExtensionSeparator);
		return std::string::npos == pos ? "" : fullPath_.substr(pos + 1);
	}

	/*! Gets the folder of the path including the final directory separator
	*/
	std::string getFolder() const
	{
		auto pos = fullPath_.rfind(directorySeparator_);
		return fullPath_.substr(0, pos + 1);
	}

	/*! Gets the file of the path including the extension
	*/
	std::string getFileWithExtension() const
	{
		auto pos = fullPath_.rfind(directorySeparator_);
		return fullPath_.substr(pos + 1);
	}

	/*! Gets the file of the path without the extension
	*/
	std::string getFileNoExtension() const
	{
		auto startPos = fullPath_.rfind(directorySeparator_) + 1;
		auto endPos = fullPath_.rfind(kExtensionSeparator);
		return fullPath_.substr(startPos, endPos != std::string::npos ? endPos - startPos : std::string::npos);
	}

	/*! Gets the folder of the path including the final directory separator
	*/
	static std::string getFolder(const std::string& file)
	{
		auto pos = file.rfind(kNativeDirectorySeparator);
		if (pos == std::string::npos)
		{
			pos = file.rfind(kNativeAltDirectorySeparator);
		}
		return file.substr(0, pos + 1);
	}

	/*! Gets the file of the path including the extension
	*/
	static std::string getFileWithExtension(const std::string& file)
	{
		auto pos = file.rfind(kNativeDirectorySeparator);
		if (pos == std::string::npos)
		{
			pos = file.rfind(kNativeAltDirectorySeparator);
		}
		return file.substr(pos + 1);
	}

	/*! Gets the file of the path without the extension
	*/
	static std::string getFileNoExtension(const std::string& file)
	{
		auto startPos = file.rfind(kNativeDirectorySeparator);
		if (startPos == std::string::npos)
		{
			startPos = file.rfind(kNativeAltDirectorySeparator);
		}
		startPos += 1;
		auto endPos = file.rfind(kExtensionSeparator);
		return file.substr(startPos, endPos != std::string::npos ? endPos - startPos : std::string::npos);
	}

	/*! Appends the path to the end of this file path
	*/
	template <class Type>
	FilePath& operator+=(const Type&& path)
	{
		fullPath_ = combine(fullPath_, std::forward(path), directorySeparator_);
	}

	template <class Type1, class Type2>
	static std::string combine(const Type1& part1, const Type2& part2,
	                           const char& directorySeparator = kNativeDirectorySeparator)
	{
		std::string fullPath(part1);
		std::string nextPart(part2);
		// Normalize the strings with the preferred separator
		auto altSeparator = (directorySeparator == kDirectorySeparator) ? kAltDirectorySeparator : kDirectorySeparator;
		std::replace(fullPath.begin(), fullPath.end(), altSeparator, directorySeparator);
		std::replace(nextPart.begin(), nextPart.end(), altSeparator, directorySeparator);

		if (fullPath.empty())
			return nextPart;
		// Check for a fully qualified path with file extension
		if (nextPart.empty() && fullPath.rfind(".") > fullPath.rfind(directorySeparator))
			return fullPath;
		if (nextPart.empty())
			nextPart.push_back(directorySeparator);
		if (fullPath.back() == directorySeparator && nextPart.front() != directorySeparator)
			return fullPath + nextPart;
		if (fullPath.back() == directorySeparator)
			return fullPath + nextPart.substr(1);
		if (nextPart.front() == directorySeparator)
			return fullPath + nextPart;
		return fullPath + directorySeparator + nextPart;
	}

private:
	std::string fullPath_;
	char directorySeparator_;
};

FilePath operator+(const FilePath& lhs, const FilePath& rhs);
} // end namespace wgt
#endif // FILE_PATH_H_
