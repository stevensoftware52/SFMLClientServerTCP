#include "Connector.h"
#include "SfSocket.h"

Connector::Connector()
{
	m_socket = make_shared<sf::TcpSocket>();
}

Connector::~Connector()
{

}

bool Connector::update()
{
	if (m_sfSocket == nullptr)
		return false;

	if (!m_sfSocket->update())
	{
		m_sfSocket = nullptr;
		return false;
	}

	return true;
}

void Connector::cancel()
{
	if (m_sfSocket != nullptr)
		m_sfSocket->cancel();

	m_sfSocket = nullptr;
}

void Connector::sendPacket(StlBuffer& outgoing)
{
	if (m_sfSocket != nullptr)
		m_sfSocket->sendPacket(outgoing);
}

void Connector::popReceived(vector<StlBuffer>& output)
{
	if (m_sfSocket != nullptr)
		m_sfSocket->popReceived(output);
}

bool Connector::connect()
{
	cancel();
	m_socket->setBlocking(true);
	
	// Fill this in as you see fit
	const string tcpAddr = "127.0.0.1";
	const int port = 6666;
	const float timeoutSeconds = 5.0f;
	
	if (m_socket->connect(tcpAddr, port, sf::seconds(timeoutSeconds)) != sf::Socket::Done)
		return false;

	m_socket->setBlocking(false);
	m_sfSocket = make_unique<SfSocket>(m_socket);
	return true;
}

bool Connector::connected() const
{
	return m_sfSocket != nullptr && m_sfSocket->connected();
}
