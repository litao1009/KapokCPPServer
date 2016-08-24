#pragma once

#include <system_error>

#include <boost/system/error_code.hpp>

std::error_code	ConvertBoostECToStdEC( const boost::system::error_code& ec );