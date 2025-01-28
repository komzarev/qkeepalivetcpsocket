#include "keepalivetcpsocket.hpp"

int keepalivetcpsocket::KeepAlive::applyKeepAliveOptions(qintptr descriptor)
{
    return keepalivetcpoption::setKeepAliveOptions(descriptor, isOn_, idleBeforeProbe_, probesCount_, responseTimeout_);
}
