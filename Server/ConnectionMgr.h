#pragma once

#include <SFML/Network.hpp>

class Session;

class ConnectionMgr
{
	public:
		void update();
		void enjectAll();

		shared_ptr<Session> getSession(const int accountId) const;

	public:
		static ConnectionMgr* instance()
		{
			static ConnectionMgr selfInstance;
			return &selfInstance;
		}

	private:
		ConnectionMgr();
		~ConnectionMgr();

		void listen();
		void updateSockets();

		int m_listenPort;

		sf::TcpListener m_listener;

		vector<shared_ptr<Session>> m_sessions;
};

#define sConnectionMgr ConnectionMgr::instance()

