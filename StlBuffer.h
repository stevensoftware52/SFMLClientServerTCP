#pragma once

#include <string>
#include <vector>

class StlBuffer
{
	public:
		typedef signed __int64 int64;
		typedef int int32;
		typedef short int16;
		typedef char int8;

		typedef unsigned __int64 uint64;
		typedef unsigned int uint32;
		typedef unsigned short uint16;
		typedef unsigned char uint8;

	public:
		enum BufferSizes
		{
			OneKilobyte = 1000,
			OneMegabyte = 1000 * OneKilobyte,
			OneGigabyte = 1000 * OneMegabyte,
		};

	public:
		StlBuffer();
		StlBuffer(const StlBuffer& buf);
		~StlBuffer();

		void clear();
		void append(const StlBuffer& buffer);
		void resize(const size_t newsize);
		void eraseFront(const size_t amount);

		bool readFile(const std::string& addr);
		bool writeFile(const std::string& addr);

		bool empty() const { return m_data.empty(); }

		size_t size() const { return m_data.size(); }
		size_t getReadPosition() const { return m_readPosition; }

		StlBuffer& operator<<(const uint8 value)
		{
			append<uint8>(value);
			return *this;
		}

		StlBuffer& operator<<(const uint16 value)
		{
			append<uint16>(value);
			return *this;
		}

		StlBuffer& operator<<(const uint32 value)
		{
			append<uint32>(value);
			return *this;
		}

		StlBuffer& operator<<(const uint64 value)
		{
			append<uint64>(value);
			return *this;
		}

		StlBuffer& operator<<(const int8 value)
		{
			append<int8>(value);
			return *this;
		}

		StlBuffer& operator<<(const int16 value)
		{
			append<int16>(value);
			return *this;
		}

		StlBuffer& operator<<(const int32 value)
		{
			append<int32>(value);
			return *this;
		}

		StlBuffer& operator<<(const bool value)
		{
			append<bool>(value);
			return *this;
		}

		StlBuffer& operator<<(const int64 value)
		{
			append<int64>(value);
			return *this;
		}

		StlBuffer& operator<<(const float value)
		{
			append<float>(value);
			return *this;
		}

		StlBuffer& operator<<(const double value)
		{
			append<double>(value);
			return *this;
		}

		StlBuffer& operator<<(const std::string& value)
		{
			append((uint8 const *)value.c_str(), value.length());
			append((uint8)0);
			return *this;
		}

		StlBuffer& operator<<(const char* str)
		{
			append((uint8 const *)str, str ? strlen(str) : 0);
			append((uint8)0);
			return *this;
		}

		StlBuffer& operator>>(bool& value)
		{
			value = read<char>() > 0 ? true : false;
			return *this;
		}

		StlBuffer& operator>>(uint8& value)
		{
			value = read<uint8>();
			return *this;
		}

		StlBuffer& operator>>(uint16& value)
		{
			value = read<uint16>();
			return *this;
		}

		StlBuffer& operator>>(uint32& value)
		{
			value = read<uint32>();
			return *this;
		}

		StlBuffer& operator>>(uint64& value)
		{
			value = read<uint64>();
			return *this;
		}

		StlBuffer& operator>>(int8& value)
		{
			value = read<int8>();
			return *this;
		}

		StlBuffer& operator>>(int16& value)
		{
			value = read<int16>();
			return *this;
		}

		StlBuffer& operator>>(int32& value)
		{
			value = read<int32>();
			return *this;
		}

		StlBuffer& operator>>(int64& value)
		{
			value = read<int64>();
			return *this;
		}

		StlBuffer& operator>>(float& value)
		{
			value = read<float>();
			return *this;
		}

		StlBuffer& operator>>(double& value)
		{
			value = read<double>();
			return *this;
		}

		StlBuffer& operator>>(std::string& value)
		{
			value.clear();

			while (getReadPosition() < size())
			{
				char c = read<char>();

				if (c == 0)
					break;

				value += c;
			}

			return *this;
		}

		uint8 operator[](const size_t pos) const
		{
			return read<uint8>(pos);
		}

		const uint8* contents() const
		{
			return &m_data[0];
		}

		void append(const uint8* src, const size_t cnt)
		{
			if (!cnt)
				return;

			if (cnt > OneGigabyte)
			{
				printf("StlBuffer tried to append OneGigabyte.\n");
				return;
			}

			if (m_data.size() < m_writePosition + cnt)
				m_data.resize(m_writePosition + cnt);

			memcpy(&m_data[m_writePosition], src, cnt);
			m_writePosition += cnt;
		}

	protected:
		size_t m_readPosition;
		size_t m_writePosition;

		std::vector<uint8> m_data;

	private:

		template <typename T>
		void append(T value)
		{
			append((uint8 *)&value, sizeof(value));
		}

		void read(uint8* dest, size_t len)
		{
			memcpy(dest, &m_data[m_readPosition], len);
			m_readPosition += len;
		}

		template <typename T>
		T read()
		{
			T r = read<T>(m_readPosition);
			m_readPosition += sizeof(T);
			return r;
		}

		template <typename T>
		T read(const size_t pos) const
		{
			if (pos + sizeof(T) > size())
				throw invalid_argument("not big enough");

			T val = *((T const*)&m_data[pos]);
			return val;
		}
};