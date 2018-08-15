#pragma once

#include "StlBuffer.h"

#include <SFML/Network.hpp>
#include <memory>

class SfSocket
{
	public:
		SfSocket(shared_ptr<sf::TcpSocket> socket);
		virtual ~SfSocket();

		void cancel();
		void sendPacket(StlBuffer& outgoing);
		void popReceived(vector<StlBuffer>& output);

		auto getBytesPerSecondUp() const { return m_transferRateUp; }
		auto getBytesPerSecondDown() const { return m_transferRateDown; }
		auto getPacketsPerSecondUp() const { return m_transferRateUpCount; }
		auto getPacketsPerSecondDown() const { return m_transferRateDownCount; }

		bool connected() const { return m_connected; }

		virtual bool update();

		const string getAddress() const;

	private:
		bool m_connected;

		int m_nextPacketSize;

		// Needed for flood protection, I guess?
		// If someone is sending us a ton of data then I'd like to know about it.
		size_t m_transferRateUp;
		size_t m_transferRateDown;
		size_t m_transferRateUpCount;
		size_t m_transferRateDownCount;
		size_t m_transferRateTallyUp;
		size_t m_transferRateTallyDown;
		size_t m_transferRateTallyUpCount;
		size_t m_transferRateTallyDownCount;

		time_t m_transferRateTimer;

		StlBuffer m_outgoing;
		StlBuffer m_incoming;

		vector<StlBuffer> m_receivedPackets;

		shared_ptr<sf::TcpSocket> m_socket;
};

