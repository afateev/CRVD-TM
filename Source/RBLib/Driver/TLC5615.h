////////////////////////////////////////////////////////////////////////////////
//
//
//	Fateev A.I.		04/08/2016
////////////////////////////////////////////////////////////////////////////////

#ifndef RBLIB_DRIVER_TLC5615_H
#define RBLIB_DRIVER_TLC5615_H

namespace Rblib
{
namespace Driver
{
	template
		<
			class Connection
		>
	class TLC5615
	{
	public:
		typedef typename Connection::ChipSelect ChipSelect;
		typedef typename Connection::SerialClock SerialClock;
		typedef typename Connection::DataInput DataInput;
	public:
		static void Init()
		{
			Select(false);
		}
		
		static void Write(unsigned short value)
		{
			SerialClock::Clear();
			Select(true);

			unsigned short output = value << 2;
			for (int i = 11; i >= 0; i--)
			{
				bool bit = (output >> i) & 1 == 1;
				WriteBit(bit);
			}
			
			SerialClock::Clear();
			Select(false);
		}
	protected:
		static void Select(bool select)
		{
			if (select)
			{
				ChipSelect::Clear();
			}
			else
			{
				ChipSelect::Set();
			}
		}
		
		static void WriteBit(bool bit)
		{
			SerialClock::Clear();
			
			if (bit)
			{
				DataInput::Set();
			}
			else
			{
				DataInput::Clear();
			}
			
			SerialClock::Set();
		}
	};
}
}

#endif