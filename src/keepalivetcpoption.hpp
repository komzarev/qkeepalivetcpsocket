#ifndef KEEPALIVETCPOPTION_HPP
#define KEEPALIVETCPOPTION_HPP

#include <cstdint>
#include <chrono>

namespace keepalivetcpoption
{
constexpr auto idleBeforeProbeDefault   = std::chrono::seconds{1};
constexpr auto probesCountDefault       = (unsigned int) 3;
constexpr auto responseTimeoutDefault   = std::chrono::seconds{1};

int setKeepAliveOptions(std::intptr_t descriptor, bool isOn, std::chrono::seconds idleBeforeProbe = idleBeforeProbeDefault, unsigned int probesCount = probesCountDefault, std::chrono::seconds responseTimeout = responseTimeoutDefault);
}

#endif // KEEPALIVETCPOPTION_HPP
