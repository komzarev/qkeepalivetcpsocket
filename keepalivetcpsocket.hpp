#pragma once
#include <QtCore/qglobal.h>
#include <QTcpServer>
#include <QTcpSocket>
#include "keepalivetcpoption.hpp"

namespace keepalivetcpsocket
{
	class KeepAlive
	{
	public:

		void enableKeepAlive(bool isOn)
		{
			isOn_ = isOn;
		}

		/*
		Gets or sets the number of seconds a TCP connection will remain idle before keepalive probes are sent to the remote.
		Default value is 1 sec
		*/
        void keepidle_sec(unsigned int val) { idleBeforeProbe_ = std::chrono::seconds(val); }
        unsigned keepidle_sec() const {return idleBeforeProbe_.count();}
		/*
		Gets or sets the number of TCP keep alive probes that will be sent before the connection is terminated.
		It is illegal to set TCP_KEEPCNT to a value greater than 255.
		Default value is 3
		*/
        void keepcnt(unsigned int val) {
            if(val > 255)
                val = 255;
            probesCount_ = val;
        }
        unsigned keepcnt() const {return probesCount_;}

		/*
		Gets or sets the number of seconds a TCP connection will wait for a keepalive response before sending another keepalive probe.
		Default value is 1 sec
		*/
        void keepint_sec(unsigned int val) { responseTimeout_ = std::chrono::seconds(val); }
        unsigned keepint_sec() const { return responseTimeout_.count(); }

	protected:
        bool isOn_                              = false;
        std::chrono::seconds idleBeforeProbe_   = keepalivetcpoption::idleBeforeProbeDefault;
        unsigned int probesCount_               = keepalivetcpoption::probesCountDefault;
        std::chrono::seconds responseTimeout_   = keepalivetcpoption::responseTimeoutDefault;
        int applyKeepAliveOptions(qintptr descriptor);
	};

	class AliveTcpServer : public QTcpServer, public KeepAlive
	{
		Q_OBJECT

	public:
		AliveTcpServer(QObject* parent = 0) : QTcpServer(parent) { }

		void incomingConnection(qintptr socketDescriptor) override
		{
			QTcpSocket* socket = new QTcpSocket(this);
			socket->setSocketOption(QAbstractSocket::LowDelayOption, 1);
            applyKeepAliveOptions(socketDescriptor);
			socket->setSocketDescriptor(socketDescriptor);
			addPendingConnection(socket);
		}
	};

	class AliveTcpSocket : public QTcpSocket, public KeepAlive
	{
		Q_OBJECT

	public:

		AliveTcpSocket(QObject* parent = 0) : QTcpSocket(parent) 
		{
			connect(this, &QAbstractSocket::connected,[=] {
				int fd = socketDescriptor();
                applyKeepAliveOptions(fd);
			});
		}
	};

}
