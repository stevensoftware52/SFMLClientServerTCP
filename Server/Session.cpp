#include "Session.h"
#include "ConnectionMgr.h"

#include <assert.h>

Session::Session(shared_ptr<sf::TcpSocket> socket) :
	SfSocket(socket),
	m_endTime(0)
{
	
}

Session::~Session()
{

}

bool Session::update()
{
	auto tbefore = clock();

	if (!__super::update())
		return false;

	if (getStage() == Stage::StageKicked)
	{
		if (time(nullptr) >= m_endTime)
			return false;

		return true;
	}
	
	vector<StlBuffer> incoming;
	popReceived(incoming);

	// Pretty basic flood protection, could be better since certain opcodes shouldn't be sent very often and we should be checking on a per opcode basis
	//	Time is of the essence so we'll stick with something basic for now
	if (incoming.size() > Defines::MaxPacketsPerTick ||
		getBytesPerSecondDown() > Defines::MaxBytesPerSecond ||
		getPacketsPerSecondDown() > Defines::MaxPacketsPerSecond)
	{
		printf("%s is sending us too much traffic, disconnecting.", getAddress().c_str());
		return false;
	}
	
	for (auto& packet : incoming)
	{
		// I'm an unforgiving person
		if (!processPacket(packet))
			return false;
	}

	return true;
}

void Session::logout()
{
	if (getStage() == Stage::StageKicked)
		return;

	// We'll sit in the connection manager until the grace end timer expires.
	// However, this variable will act to stop us from responding to any more data
	m_stage = Stage::StageKicked;

	// Some time to finish sending data. 
	static int graceEndTime = 5;
	m_endTime = time(nullptr) + graceEndTime;
	eject();
}

void Session::eject()
{
	// Do your own cleanup
}

bool Session::processPacket(StlBuffer& data)
{
	StlBuffer::uint16 opcode;

	if (data.size() < sizeof(opcode))
	{
		printf("%s processed packet with size less than %d\n", getAddress().c_str(), sizeof(opcode));
		return false;
	}

	data >> opcode;
	data.eraseFront(sizeof(opcode));

	try
	{
		// Do something
		// return process_Client_Authenticate(data);
	}
	catch (invalid_argument& e)
	{
		printf("%s bad packet %s %d\n", getAddress().c_str(), e.what(), opcode);
		return false;
	}

	return false;
}

/*bool Session::process_Client_Authenticate(StlBuffer& data)
{
	if (getStage() != Stage::StageLogin)
		return false;

	// Try to authenticate?
	
	return true;
}*/
