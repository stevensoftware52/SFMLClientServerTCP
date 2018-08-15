#pragma once

#include "StlBuffer.h"

#include <SFML/Network.hpp>

class SfSocket;

// Client
class Connector
{
	public:
		void cancel();
		void sendPacket(StlBuffer& outgoing);
		void popReceived(vector<StlBuffer>& output);

		bool update();
		bool connect();
		bool connected() const;

	public:
		static Connector* instance()
		{
			static Connector c;
			return &c;
		}

	private:
		Connector();
		~Connector();

		unique_ptr<SfSocket> m_sfSocket;
		shared_ptr<sf::TcpSocket> m_socket;
};

#define sConnector Connector::instance()
