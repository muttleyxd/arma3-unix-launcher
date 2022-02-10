#pragma once

#include <functional>
#include <thread>

namespace UpdateChecker
{
    std::thread is_update_available(std::function<void(bool)>&& callback);
}
