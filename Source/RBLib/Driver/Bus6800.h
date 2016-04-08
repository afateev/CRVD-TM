////////////////////////////////////////////////////////////////////////////////
//
//
//	Fateev A.I.		30/11/2011
////////////////////////////////////////////////////////////////////////////////

#ifndef RBLIB_DRIVER_BUS6800_H
#define RBLIB_DRIVER_BUS6800_H

namespace Rblib
{
namespace Driver
{
	template<class Port, class PortStruct >
	class Bus6800
	{
	protected:
		union PortValue
		{
			typename PortStruct::InternalType Value;
			PortStruct Struct;
		};
		static PortValue _portValue;
	public:
		static inline void Init()
		{
			_portValue.Struct.DataReadWrite = 1;
			_portValue.Struct.CS = 0;
			_portValue.Struct.E = 0;
			_portValue.Struct.Reset = 0;
			UpdatePortValue();
		}
		
		static inline void UpdatePortValue()
		{
			Port::Write(_portValue.Value);
		}
		
		static inline void WriteCommand(unsigned char cmd)
		{
			_portValue.Struct.Reset = !0;
			_portValue.Struct.CS = !1;
			_portValue.Struct.RW = 0;
			_portValue.Struct.A0 = 1;
			_portValue.Struct.Data = cmd;
			
			_portValue.Struct.E = 1;
			UpdatePortValue();
			UpdatePortValue();
			UpdatePortValue();
			_portValue.Struct.E = 0;
			UpdatePortValue();
			UpdatePortValue();
			UpdatePortValue();
		}
		
		static inline void WriteData(unsigned char data)
		{
			_portValue.Struct.Reset = !0;
			_portValue.Struct.CS = !1;
			_portValue.Struct.RW = 0;
			_portValue.Struct.A0 = 0;
			_portValue.Struct.Data = data;
			
			_portValue.Struct.E = 1;
			UpdatePortValue();
			//UpdatePortValue();
			//UpdatePortValue();
			_portValue.Struct.E = 0;
			UpdatePortValue();
			//UpdatePortValue();
			//UpdatePortValue();
		}
		
		static inline void WriteStop()
		{
			_portValue.Struct.CS = !0;
			_portValue.Struct.E = 1;
			UpdatePortValue();
			_portValue.Struct.E = 0;
			UpdatePortValue();
		}
	};
	
	template<class Port, class PortStruct >
	Bus6800<Port, PortStruct>::PortValue
	Bus6800<Port, PortStruct>::_portValue;
}
}

#endif