#pragma once

#include <memory>

class	TcpSession;
using	TcpSessionSPtr = std::shared_ptr<TcpSession>;
using	TcpSessionWPtr = std::weak_ptr<TcpSession>;
