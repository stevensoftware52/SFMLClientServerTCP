#include "ConnectionMgr.h"

ConnectionMgr::ConnectionMgr()
{
	bool printOnce = false;

	while (true)
	{
		// Put whatever you want here.
		m_listenPort = 5555;

		if (m_listener.listen(m_listenPort) != sf::Socket::Done)
		{
			if (!printOnce)
			{
				printf("Unable to listen on port %d.\n", m_listenPort);
				printOnce = true;
			}

			Sleep(1);
		}
		else
		{
			printf("Listening on port %d\n", m_listenPort);
			break;
		}
	} 

	m_listener.setBlocking(false);
}

ConnectionMgr::~ConnectionMgr()
{

}

void ConnectionMgr::update()
{
	listen();
	updateSockets();
}

void ConnectionMgr::enjectAll()
{
	for (auto itr : m_sessions)
		itr->eject();
}

void ConnectionMgr::listen()
{
	// Accept new connections
	shared_ptr<sf::TcpSocket> socket = make_shared<sf::TcpSocket>();

	if (m_listener.accept(*socket) == sf::Socket::Done)
	{
		socket->setBlocking(false);
		printf("Accepted connection from %s:%d\n", socket->getRemoteAddress().toString().c_str(), socket->getRemotePort());
		m_sessions.push_back(make_shared<Session>(socket));
	}
}

void ConnectionMgr::updateSockets()
{
	// Update the ones we have
	auto itr = m_sessions.begin();

	while (itr != m_sessions.end())
	{
		auto& s = *(*itr);

		if (!s.connected() || !s.update())
		{
			if (s.connected())
				s.cancel();

			s.eject();
			itr = m_sessions.erase(itr);
		}
		else
		{
			++itr;
		}
	}
}
