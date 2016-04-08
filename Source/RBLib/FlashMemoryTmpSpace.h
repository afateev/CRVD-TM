///////////////////////////////////////////////////////////////////////////////
//	����� ���������� �������� ���� ������ ��� ���������� �������� ������
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef FLASHMEMORYTMPSPACE_H
#define FLASHMEMORYTMPSPACE_H

namespace Rblib
{
	template<class PointerType,					// ��� (������) �������� �������
			PointerType StartAddress,			// ������ �������
			PointerType Length,					// ������ �������
			class FlashMemoryController			// ���������� ����
		>
	class FlashMemoryTmpSpace
	{
	public:
		// ������ ������ �� ��������� �������
		static inline void Write(PointerType address, unsigned char *data, PointerType len)
		{
			// ������� � ��������� ������
			PointerType srcPos = 0;
			// ������� ������ � ����� ������� ������ ������
			PointerType writePos = StartAddress + address;
			PointerType writeEnd = writePos + len - 1;
			// ������ �������� ��������
			PointerType currentSegmentStart = 0;
			// ������ � ������ �������� ���� ������ �����
			PointerType segmentStart = 0;
			PointerType segmentSize = 0;
			do
			{
				FlashMemoryController::GetSegmentBounds(writePos, segmentStart, segmentSize);
				// ���� ������� ����� �������
				if (currentSegmentStart != segmentStart)
				{
					currentSegmentStart = segmentStart;
					writePos = currentSegmentStart;
					// ������� �������
					FlashMemoryController::Erase(currentSegmentStart);
				}
				FlashMemoryController::WriteByte(writePos, data[srcPos]);
				srcPos++;
				writePos++;
			}
			while(writePos <= writeEnd);
		}
		// ��������� ������ �� ��������� �������
		static inline unsigned char ReadByte(PointerType address)
		{
			return FlashMemoryController::Read(StartAddress + address);
		}
	};
}

#endif