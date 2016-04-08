///////////////////////////////////////////////////////////////////////////////
//	������ ���������� ������� ������� ����������� ���� ������
//
//	������ ������ ������ ��������� �� ������ ������������� ��� �� ����������
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef FLASHMEMORYCONTROLLER_PARTS_MSP430_H
#define FLASHMEMORYCONTROLLER_PARTS_MSP430_H

#include "Common.h"

namespace Rblib
{
// ����� ����������� ��� ��������� MSP430 ���� ��������
namespace Msp430
{
	// ��� �������� ��������
	typedef unsigned short FlashRegisterDataType;
	// ��� ������ ��������
	typedef unsigned int FlashRegisterAddresType;
	// ��� ��������� �� ����
	typedef unsigned long FlashPointerType;

	// ����� ���������� ���������� ���� �����������
	template <class RegisterDataType,				// ��� (������) �������� ��������
			class PointerType,				// ��� (������) �������� ������� ���������
			class RegisterStructure,		// ��������� ��������
			PointerType ControlRegisterAddr	// ����� �������� ����������
		>
	class FlashMemoryControllerRegister
	{
	protected:
		// ����� �������� ����������
		static volatile RegisterDataType * const _controlRegisterAddr;
	public:
		// ������ �������� ��������
		static inline void GetValue(RegisterStructure *value)
		{
			 memcpy(value, (const void*)_controlRegisterAddr, sizeof(RegisterDataType));
		}
		// ������ �������� ��������
		static inline void SetValue(RegisterStructure *value)
		{
			// ���������� ������������ ������ �� ������
			value->Password = 0xA5;
			// �.�. � ������� ����� ������ ������ ����� ������
			// ������� ��� �����
			RegisterDataType tmp;
			// �������� ���� �������� ���������
			memcpy(&tmp, value, sizeof(RegisterDataType));
			// ���������� � �������
			*_controlRegisterAddr = tmp;
		}
	};

	// ��������������� ����������� ����������
	template<class RegisterDataType, class PointerType, class RegisterStructure, PointerType ControlRegisterAddr>
	volatile  RegisterDataType* const FlashMemoryControllerRegister<RegisterDataType, PointerType, RegisterStructure, ControlRegisterAddr>
		::_controlRegisterAddr = reinterpret_cast<volatile  RegisterDataType*>(ControlRegisterAddr);

	// ����� ������ ������� ���� ������
	template<class PointerType, PointerType start, PointerType size, PointerType segmentSize>
	class FlashMemorySegmentBounds
	{
	public:
		static const PointerType Start = start;
		static const PointerType Size = size;
		static const PointerType SegmentSize = segmentSize;
	};

	// ���������� ���� ������
	template<class PointerType,			// ��� ����������
			class RegisterDataType,		// ��� (������) �������� ��������
			class RegisterAddressType,	// ��� (������) �������� ������� ���������
			RegisterAddressType Control1Address,
			class Control1Structure,
			RegisterAddressType Control3Address,
			class Control3Structure,
			class InfoMemory,
			class MainMemory
		>
	class FlashMemoryController
	{
	protected:
		typedef FlashMemoryControllerRegister<RegisterDataType, RegisterAddressType, Control1Structure, Control1Address> Control1;
		typedef FlashMemoryControllerRegister<RegisterDataType, RegisterAddressType, Control3Structure, Control3Address> Control3;
	public:
		typedef InfoMemory Info;
		typedef MainMemory Main;
	public:
		// ��������� ���� �� ������
		static inline unsigned char Read(PointerType address)
		{
			return RBLIB_READ_FULL_ADDR_CHAR(address);
		}
		// ��������� ����� �� ������
		static inline void Read(PointerType address, unsigned char *data, PointerType len)
		{
			if (0 == data)
				return;
			for (PointerType i = 0; i < len; i++)
			{
				data[i] = Read(address + i);
			}
		}
		// �������� ��������� ����� �������� � ��� ������, �� ����������� ������
		static inline void GetSegmentBounds(PointerType address, PointerType &segmentStart, PointerType &segmentSize)
		{
			PointerType start = 0;
			PointerType size = 0;
			// Info
			if (address >= InfoMemory::Start && address <= InfoMemory::Start + InfoMemory::Size)
			{
				start = InfoMemory::Start;		// ������ ������� ������
				size = InfoMemory::SegmentSize;	// ������ ���������
			}
			// Main memory
			if (address >= MainMemory::Start && address <= MainMemory::Start + MainMemory::Size)
			{
				start = MainMemory::Start;		// ������ ������� ������
				size = MainMemory::SegmentSize;	// ������ ���������
			}

			// ���� �� ������ � ���������� �������
			if (size == 0)
			{
				// ���������� ������� ������
				segmentSize = size;
				return;
			}

			// ��������� ������ ��������
			
			// ��������� �������� �� ������ �����
			segmentStart = address - start;
			// ������� ��������� ��������� ��� ��������
			segmentStart /= size;
			// ������� ����� ���������
			segmentStart *= size;
			// �������� ������ � ������� ����� ��������� �������� ����� ������ ��������
			segmentStart += start;
			// ������ ��������
			segmentSize = size;
		}
		// ������� �������
		static inline void Erase(PointerType address)
		{       
			Control1Structure tmp1;
			Control3Structure tmp3;
			// ��������� �� ���������
			do
			{
				Control3::GetValue(&tmp3);
			}
			while(tmp3.Busy);
			// ������������
			tmp3.Lock = 0;
			Control3::SetValue(&tmp3);
			// ���������� ���� ��������
			Control1::GetValue(&tmp1);
			tmp1.Erase = 1;
			Control1::SetValue(&tmp1);
			
			// ������ "���� ������", ��� ������� ��������� ������ ��������
			RBLIB_WRITE_FULL_ADDR_CHAR(address, 0);

			// ��� ���� ������
			do
			{
				Control3::GetValue(&tmp3);
			}
			while(tmp3.Busy);
			// ��������� �������
			tmp3.Lock = 1;
			Control3::SetValue(&tmp3);
		}
		// �������� ���� �� ������
		static inline void WriteByte(PointerType address, unsigned char value)
		{
			Control1Structure tmp1;
			Control3Structure tmp3;
			// ��������� �� ���������
			do
			{
				Control3::GetValue(&tmp3);
			}
			while(tmp3.Busy);
			// ������������
			Control3::GetValue(&tmp3);
			tmp3.Lock = 0;
			Control3::SetValue(&tmp3);
			// ��������� ������
			Control1::GetValue(&tmp1);
			tmp1.Write = 1;
			Control1::SetValue(&tmp1);

			// ����������
			RBLIB_WRITE_FULL_ADDR_CHAR(address, value);
            
			// ��� ���� ������
			do
			{
				Control3::GetValue(&tmp3);
			}
			while(tmp3.Busy);
			// ��������� ������
			Control1::GetValue(&tmp1);
			tmp1.Write = 0;
			Control1::SetValue(&tmp1);
			// ��������� �������
			Control3::GetValue(&tmp3);
			tmp3.Lock = 1;
			Control3::SetValue(&tmp3);
		}
	};
}
}

#endif