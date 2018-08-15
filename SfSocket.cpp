#include "SfSocket.h"
#include "TcpDefines.h"

SfSocket::SfSocket(shared_ptr<sf::TcpSocket> socket) :
	m_socket(socket),
	m_connected(true),
	m_nextPacketSize(0),
	m_transferRateTallyUp(0),
	m_transferRateTallyDown(0),
	m_transferRateTimer(0),
	m_transferRateUp(0),
	m_transferRateDown(0),
	m_transferRateTallyDownCount(0),
	m_transferRateTallyUpCount(0)
{
	// We assume that this socket is connected when we receive it
}

SfSocket::~SfSocket()
{
	
}

bool SfSocket::update()
{
	if (!m_connected)
		return false;

	size_t amountReceived;
	unsigned char buffer[TcpDefines::BufferSize];
	auto result = m_socket->receive(buffer, TcpDefines::BufferSize, amountReceived);

	if (result == sf::Socket::Disconnected || result == sf::Socket::Error)
	{
		cancel();
		return false;
	}

	m_incoming.append(buffer, amountReceived);
	m_transferRateTallyDown += amountReceived;

	// We might have received many packets in one go
	while (!m_incoming.empty())
	{
		// Packets are sent in this order: size -> data -> size -> etc.
		//	If we've gotten at least 4 bytes, then this could be the size portion, but only if we aren't presently grabbing the data.
		if (m_incoming.size() >= 4 && m_nextPacketSize == 0)
		{
			m_incoming >> m_nextPacketSize;

			if (m_nextPacketSize < TcpDefines::MinPacketSize || m_nextPacketSize > TcpDefines::MaxPacketSize)
			{
				printf("Bad next packet size %d from %s\n", m_nextPacketSize, m_socket->getRemoteAddress().toString().c_str());
				cancel();
				return false;
			}

			m_incoming.eraseFront(4);
		}

		// Once we know how big the next packet is, wait for the incoming to be at least that big.
		// At that point, process said data as a completed packet.
		if (m_nextPacketSize > 0 && m_incoming.size() >= static_cast<size_t>(m_nextPacketSize))
		{
			StlBuffer packet;
			packet.append(m_incoming.contents(), m_nextPacketSize);
			m_receivedPackets.push_back(packet);
			m_incoming.eraseFront(m_nextPacketSize);
			m_nextPacketSize = 0;
		}
		else
		{
			// Either we're done or we don't have enough data to complete a formal packet
			break;
		}
	}

	// Sending is pretty straightforward: just send!
	if (!m_outgoing.empty())
	{
		size_t sentSize;
		result = m_socket->send(m_outgoing.contents(), m_outgoing.size(), sentSize);

		if (result == sf::Socket::Disconnected || result == sf::Socket::Error)
		{
			cancel();
			return false;
		}

		if (sentSize > 0)
			m_outgoing.eraseFront(sentSize);
	}

	if (time(nullptr) > m_transferRateTimer + 2)
	{
		m_transferRateUp = m_transferRateTallyUp / 3;
		m_transferRateDown = m_transferRateTallyDown / 3;
		m_transferRateDownCount = m_transferRateTallyDownCount / 3;
		m_transferRateUpCount = m_transferRateTallyUpCount / 3;
		m_transferRateTallyUp = 0;
		m_transferRateTallyDown = 0;
		m_transferRateTallyDownCount = 0;
		m_transferRateTallyUpCount = 0;
		m_transferRateTimer = time(nullptr);
	}

	return true;
}

void SfSocket::cancel()
{
	printf("Terminated connection for %s:%d\n", m_socket->getRemoteAddress().toString().c_str(), m_socket->getRemotePort());

	m_connected = false;
	m_socket->disconnect();
	m_nextPacketSize = 0;

	m_transferRateTallyUp = 0;
	m_transferRateTallyDown = 0;
	m_transferRateTallyDownCount = 0;
	m_transferRateTallyUpCount = 0;
	m_transferRateTimer = 0;
	m_transferRateUp = 0;
	m_transferRateDown = 0;
}

void SfSocket::sendPacket(StlBuffer& outgoing)
{
	m_outgoing << int(outgoing.size());
	m_outgoing.append(outgoing);
	m_transferRateTallyUp += outgoing.size();
	++m_transferRateTallyUpCount;
}

void SfSocket::popReceived(vector<StlBuffer>& output)
{
	if (m_receivedPackets.empty())
		return;

	output = m_receivedPackets;
	m_receivedPackets.clear();
	++m_transferRateTallyDownCount;
}

const string SfSocket::getAddress() const
{
	if (m_socket == nullptr)
		return "null:0";

	return m_socket->getRemoteAddress().toString() + ":" + to_string(m_socket->getLocalPort());
}