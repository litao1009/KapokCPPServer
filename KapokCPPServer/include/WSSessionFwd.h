#pragma once

#include <memory>

class	WSSession;
using	WSSessionSPtr = std::shared_ptr<WSSession>;
using	WSSessionWPtr = std::weak_ptr<WSSession>;
