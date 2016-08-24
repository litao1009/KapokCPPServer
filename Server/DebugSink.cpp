#include "DebugSink.h"

#include <windows.h>

#include <streambuf>
#include <vector>
#include <iostream>


class	DebugSink::Imp
{
public:

	class	DebugStreamBuf : public std::streambuf
	{
	public:

		virtual int_type overflow( int_type c = EOF ) override
		{
			if ( c != EOF )
			{
				char buf[] = { static_cast<char>(c),'\0' };
				OutputDebugStringA( buf );
			}

			return c;
		}
	};

public:

	DebugStreamBuf	StreamBuf_;
	std::streambuf*	DefaultBuf_{};
};


DebugSink::DebugSink():ImpUPtr_(std::make_unique<Imp>())
{
	auto& imp_ = *ImpUPtr_;

	imp_.DefaultBuf_ = std::cout.rdbuf( &( imp_.StreamBuf_ ) );
}

DebugSink::~DebugSink()
{
	auto& imp_ = *ImpUPtr_;

	std::cout.rdbuf( imp_.DefaultBuf_ );
}