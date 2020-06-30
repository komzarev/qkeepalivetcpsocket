#include "keepalivetcpsocket.hpp"
#include "keepalivetcpoption.hpp"

int keepalivetcpsocket::KeepAlive::setOptions(qintptr descriptor, bool isOn, unsigned int keepidle_sec , unsigned int keepcnt , unsigned int keepint_sec )
{
    return keepalivetcpoption::setOptions(descriptor, isOn, std::chrono::seconds(keepidle_sec), keepcnt, std::chrono::seconds(keepint_sec));
}
