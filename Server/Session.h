#pragma once

#include "SfSocket.h"

class Session : public SfSocket
{
	public:
		enum Stage
		{
			StageLogin,
			StageKicked,
		};

		enum Defines
		{
			MaxBytesPerSecond = SHRT_MAX,
			MaxPacketsPerSecond = SCHAR_MAX,
			MaxPacketsPerTick = MaxPacketsPerSecond / 2
		};

	public:
		Session(shared_ptr<sf::TcpSocket> socket);
		virtual ~Session();

		void logout();
		void eject();

		bool update() final;

	private:
		bool processPacket(StlBuffer& data);
		
		time_t m_endTime;
};

