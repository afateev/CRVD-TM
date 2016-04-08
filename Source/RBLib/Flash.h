///////////////////////////////////////////////////////////////////////////////
//	����� ���������� �������� ���� ������ ��� �������� ���������������� ������
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef FLASH_H
#define FLASH_H

#ifdef MSP430
#include "FlashMemoryTmpSpace.h"
#include "InterruptMap.h"
#endif

namespace Rblib
{
#ifdef MSP430
	///////////////////////////////////////////////////////////////
	///	\brief ����� ������� � ���������������� ������� ���� ������
	///	
	///	\tparam UserDataType	��� ���������������� ������ (����� ���� ��� ������� �����, ��� � ����������)
	///	\tparam Offset			�������� ���������������� ������ ������������ ������ ������� ���������������� ������
	///
	///////////////////////////////////////////////////////////////
	template<class UserDataType, unsigned long Offset>
	class Flash
	{
	protected:
		// ����� ������ ������
		typedef FlashMemoryController MemoryReader;
		// ����� ������ ������
		typedef FlashMemorySegmented<unsigned long, FlashMemoryController,
			// ��������� ������� ����� � ��������� �������� ����������� ������
			FlashMemoryTmpSpace<unsigned long,
				FlashMemoryController::Main::Start + FlashMemoryController::Main::Size - FlashMemoryController::Main::SegmentSize,
				FlashMemoryController::Main::SegmentSize,
				FlashMemoryController
			>
		> MemoryWriter;
	public:
		///	������ ������
		/*!
			\return ��������� ���������������� ������, ����������� �� ����.
		*/
		static inline UserDataType Read()
		{
			UserDataType userData;

			unsigned long userDataSize = sizeof(UserDataType);
			if (Offset >= FlashMemoryController::Info::Size)
				return UserDataType();
			if (Offset + userDataSize >= FlashMemoryController::Info::Size)
				return UserDataType();
			
			MemoryReader::Read(FlashMemoryController::Info::Start + Offset, (unsigned char *)&userData, userDataSize);
			return userData;
		}
		///	������ ������
		/*!
			\param userData ��������� ���������������� ������, ������� ���������� ��������.
		*/
		static inline void Write(UserDataType userData)
		{
            InterruptMap::SaveInterruptsSateAndDisable();
			// ��������� �������
			unsigned long userDataSize = sizeof(UserDataType);
			if (Offset >= FlashMemoryController::Info::Size)
				return;
			if (Offset + userDataSize >= FlashMemoryController::Info::Size)
				return;

			MemoryWriter::Write(FlashMemoryController::Info::Start + Offset, (unsigned char *)&userData, userDataSize);
            InterruptMap::RestoreInterruptsSate();
		}
	};
#endif // MSP430
	
/*!
\class Flash
����� ��������� �������� ������ � ������� �����������������
������ ����������� ��������������� ��� �������� ���������������� ������.

��� ������ �������� ������ � ������ �������������� �����
�� ������� ���������� �������, ��� ������������� ����� ������
�� ��������� ���� �������.

�� ��������� ���������� �������� ��������� ������� ���������������� ������:
<table>
<tr> <td>���������</td>		<td>�����</td> <td>������, ����</td>	</tr>
<tr> <td>Msp430f2618</td>	<td>0x1000</td>	<td>256</td>			</tr>
<tr> <td>Msp430f5435</td>	<td>0x1800</td>	<td>512</td>			</tr>
<tr> <td>Msp430f5438</td>	<td>0x1800</td>	<td>512</td>			</tr>
</table>

��� ��������� ���������������� ������ � ���������������� ������ � ������ � ���� ����������:
-# ���������� ��� ���������������� ������ (��������� ������) � ������� ��� � �������� ��������� ������� \a UserDataType.
-# ������ � ������� ��������� ������� \a Offset ���������� ����� ������� ����, ��� ����� ��������� ������ ������������� �� ���� 1 ����.
-# ���������� ��������� ����������� �� ���� ������ ������ � ������� ������ Read().
-# ��������� ��������� ����� �� ���� � ������� ������ Write().

������ �������������:
\code
// ��������� ������ �������� � ����������������� ������
struct DeviceParamsStruct
{
	unsigned char	One;
	unsigned int	Two;
	unsigned long	Four;
};
// ����� ������� � ������ � ����������������� ������
typedef Flash<
	DeviceParamsStruct,	// ��������� ������
	0			// ������ ����������� � ������ ���������������� ������� Flash
> DeviceParams;

int main( void )
{
	...
	// ������ ������� �������� �� ����
	DeviceParamsStruct params = DeviceParams::Read();
	//��������� ��������
	params.One++;
	params.Two++;
	params.Four++;
	// ���������� �������� �� ����
	DeviceParams::Write(params);
	...
}
\endcode

����������� �������� � ����������� ���������� ���������������� ������ ���������� �� ������ ��������� ��������� ��������� ��������� �������:
- Flash<DataType1, 0> - ������ ���� 1 �������� �� �������� 0.
- Flash<DataType2, 5> - ������ ���� 2 �������� �� �������� 5.

*/

/*!
\page PageFlash Flash - ���������� ����������������� ������ �����������
	���� ������ ����������� ������ ��������� �� ������� ���� ���������, ������� ���������������� ������ � �������� ������ ����������� ������� (���������, ������������� ������ � �.�.).
	<br>����� Flash ��������� ������� �� ������ � \b ���������������� \b �������� ���������������� ������ �����������.

*/

}

#endif