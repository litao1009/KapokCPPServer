#pragma once

#include "Predef.h"

#include "websocketpp/common/connection_hdl.hpp"
#include "websocketpp/connection.hpp"
#include "websocketpp/config/asio_no_tls.hpp"
#include "websocketpp/frame.hpp"

using	MessageType = websocketpp::connection<websocketpp::config::asio>::message_ptr;