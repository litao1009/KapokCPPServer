#pragma once

#include <boost/asio.hpp>


using	IOService = boost::asio::io_service;
using	Strand = IOService::strand;
using	ErrCode = std::error_code;
using	Socket = boost::asio::ip::tcp::socket;
using	EndPoint = boost::asio::ip::tcp::endpoint;