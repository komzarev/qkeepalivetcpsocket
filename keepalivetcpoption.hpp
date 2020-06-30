#ifndef KEEPALIVETCPOPTION_HPP
#define KEEPALIVETCPOPTION_HPP

#include <cstdint>
#include <chrono>

namespace keepalivetcpoption
{
int setOptions(std::intptr_t descriptor, bool isOn, std::chrono::seconds idleBeforeProbe = std::chrono::seconds(1), unsigned int probesCount = 3, std::chrono::seconds responseTimeout = std::chrono::seconds(1));
}

#endif // KEEPALIVETCPOPTION_HPP
