#ifndef DATASTREAMBUF_HPP_INCLUDED
#define DATASTREAMBUF_HPP_INCLUDED

#include <cstddef>
#include <cassert>
#include <streambuf>
#include <algorithm>

#include "basic_stream.hpp"

// #include <iostream>
// #define DATA_STREAM_BUF_LOG(x) std::cout << x << std::endl;
#define DATA_STREAM_BUF_LOG(x)

namespace wgt
{
/**
std::streambuf adapter for BasicStream.

Use this adapter to perform stream IO on BasicStream using std::iostream.
*/
template <typename _CharT, typename _Traits = std::char_traits<_CharT>>
class BasicDataStreamBuf :
public std::basic_streambuf<_CharT, _Traits>
{
	typedef std::basic_streambuf<_CharT, _Traits> base;

public:
	typedef _CharT char_type;
	typedef _Traits traits_type;
	typedef typename traits_type::int_type int_type;
	typedef typename traits_type::pos_type pos_type;
	typedef typename traits_type::off_type off_type;

	explicit BasicDataStreamBuf(BasicStream& stream)
	    :
	    base()
	    ,
	    stream_(stream)
	{
		// allow ungetting at least one char
		if (stream_.ungetBufferSize() < sizeof(char_type))
		{
			stream_.setUngetBufferSize(sizeof(char_type));
		}
	}

	void imbue(const std::locale& loc) override
	{
		DATA_STREAM_BUF_LOG("imbue()")
		// ignore locale
	}

	std::basic_streambuf<char_type, traits_type>* setbuf(
	char_type* b,
	std::streamsize s) override
	{
		DATA_STREAM_BUF_LOG("setbuf(s=" << s << ")")
		// stream_ has its own buffer, ignore buffer
		return this;
	}

	pos_type seekoff(
	off_type off,
	std::ios_base::seekdir dir,
	std::ios_base::openmode mode = std::ios_base::in | std::ios_base::out) override
	{
		// ignore mode
		auto r = stream_.seek(off, dir);
		DATA_STREAM_BUF_LOG("seekoff(off=" << off << ", dir=" << dir << ", mode=" << mode << ")=" << r)
		return r;
	}

	pos_type seekpos(
	pos_type pos,
	std::ios_base::openmode mode = std::ios_base::in | std::ios_base::out) override
	{
		auto r = seekoff(pos, std::ios_base::beg, mode);
		DATA_STREAM_BUF_LOG("seekpos(pos=" << pos << ", mode=" << mode << ")=" << r)
		return r;
	}

	int sync() override
	{
		auto r = stream_.sync();
		int result = r ? 0 : -1;
		DATA_STREAM_BUF_LOG("sync()=" << result)
		return result;
	}

	std::streamsize showmanyc() override
	{
		std::streamsize chars = 0; // no information

		const auto cur = stream_.seek(0, std::ios_base::cur);
		if (cur >= 0)
		{
			const auto end = stream_.seek(0, std::ios_base::end);
			stream_.seek(cur, std::ios_base::beg);
			if (end >= 0)
			{
				chars = (end - cur) / sizeof(char_type);
			}
		}

		DATA_STREAM_BUF_LOG("showmanyc()=" << chars)
		return chars;
	}

	std::streamsize xsgetn(char_type* s, std::streamsize n) override
	{
		auto result = read(s, n);
		DATA_STREAM_BUF_LOG("xsgetn(s=" << s << ", n=" << n << ")=" << result)
		return result;
	}

	int_type underflow() override
	{
		int_type result = uflow();
		if (!traits_type::eq_int_type(result, traits_type::eof()))
		{
			if (!stream_.unget(sizeof(char_type)))
			{
				result = traits_type::eof();
			}
		}

		DATA_STREAM_BUF_LOG("underflow()=" << result << " '" << traits_type::to_char_type(result) << "'")
		return result;
	}

	int_type uflow() override
	{
		int_type result;
		char_type c;
		auto r = read(&c, 1);
		if (r > 0)
		{
			result = traits_type::to_int_type(c);
		}
		else
		{
			result = traits_type::eof();
		}

		DATA_STREAM_BUF_LOG("uflow()=" << result << " '" << traits_type::to_char_type(result) << "'")
		return result;
	}

	int_type pbackfail(int_type c = traits_type::eof()) override
	{
		int_type result = traits_type::eof();
		if (!traits_type::eq_int_type(c, traits_type::eof()))
		{
			char_type t = traits_type::to_char_type(c);
			if (stream_.unget(sizeof(char_type), &t))
			{
				result = c;
			}
		}
		else
		{
			if (stream_.unget(sizeof(char_type)))
			{
				result = traits_type::not_eof(c);
			}
		}

		DATA_STREAM_BUF_LOG("pbackfail(c=" << c << " '" << traits_type::to_char_type(c) << "')=" << result)
		return result;
	}

	std::streamsize xsputn(const char_type* s, std::streamsize n) override
	{
		std::streamsize result = write(s, n);

		DATA_STREAM_BUF_LOG("xsputn(s=" << s << ", n=" << n << ")=" << result)
		return result;
	}

	int_type overflow(int_type c = traits_type::eof()) override
	{
		int_type result;
		if (traits_type::eq_int_type(c, traits_type::eof()))
		{
			result = traits_type::not_eof(c);
		}
		else
		{
			auto t = traits_type::to_char_type(c);
			auto r = write(&t, 1);
			result = (r > 0) ? c : traits_type::eof();
		}

		DATA_STREAM_BUF_LOG("overflow(c=" << c << " '" << traits_type::to_char_type(c) << "')=" <<
		                    result << " '" << traits_type::to_char_type(result) << "'")
		return result;
	}

private:
	BasicStream& stream_;

	std::streamsize read(char_type* destination, std::streamsize count)
	{
		auto result = stream_.readHard(destination, count * sizeof(char_type));
		if (result <= 0)
		{
			return 0;
		}

		// ignore partially read char
		return result / sizeof(char_type);
	}

	std::streamsize write(const char_type* source, std::streamsize count)
	{
		auto result = stream_.writeHard(source, count * sizeof(char_type));
		if (result <= 0)
		{
			return 0;
		}

		// ignore partially written char
		return result / sizeof(char_type);
	}
};

typedef BasicDataStreamBuf<char> DataStreamBuf;
typedef BasicDataStreamBuf<wchar_t> WDataStreamBuf;


#undef DATA_STREAM_BUF_LOG
} // end namespace wgt
#endif // DATASTREAMBUF_HPP_INCLUDED
