///////////////////////////////////////////////////////////////////////////////
//	����� ���������� ���������������� ���� �������
//	��� ����� ������, ������� ��� ������ � �������
//	������� ������� �������� ��������
//	
//	����� ������� ������ ������ ���������:
//	1. ���������� � ����� ������� ����� ������������ ������ � ��� ������
//	2. �������� ������� ���������� �������� �� ��������� ������
//	3. ������� ������� �������
//	4. �� ����� ������ ���������������� ������ ������������ ���������������
//		������ ������ ��������, ������� �� ���������� � ����������
//		������ ���������������� ������
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef FLASHMEMORYSEGMENTED_H
#define FLASHMEMORYSEGMENTED_H

namespace Rblib
{
	template<class PointerType,				// ��� ����������
			class FlashMemoryController,	// ���������� ����
			class TmpSpace					// ����� ��� ��������� ����� ��������
		>
	class FlashMemorySegmented
	{
	public:
		// �������� ������ �� ������
		// address - ��������� �����
		// data - ���� ������
		// len - ����� ����� ������
		static inline void Write(PointerType address, unsigned char *data, PointerType len)
		{
			// ������� � ��������� ������
			PointerType srcPos = 0;
			// ������� ������ � ����� ������� ������ ������
			PointerType writePos = address;
			PointerType writeEnd = address + len - 1;
			// ������ � ������ �������� ��� ��������� ������ �����
			PointerType firstSegmentStart;
			PointerType firstSegmentSize;
			// ������ � ������ �������� ��� ��������� ����� �����
			PointerType lastSegmentStart;
			PointerType lastSegmentSize;
			// ������ �������� ��������
			PointerType currentSegmentStart = 0;
			// � ����� �������� ����� �������� ������
			FlashMemoryController::GetSegmentBounds(writePos, firstSegmentStart, firstSegmentSize);
			// � ����� �������� �������� ������
			FlashMemoryController::GetSegmentBounds(writeEnd, lastSegmentStart, lastSegmentSize);
			// ���� ���� ������ ����������, �������
			if (firstSegmentSize == 0 || lastSegmentSize == 0)
				return;
			do
			{
				// ������ � ������ �������� ���� ������ �����
				PointerType segmentStart;
				PointerType segmentSize;
				FlashMemoryController::GetSegmentBounds(writePos, segmentStart, segmentSize);
				// ���� ������� ����� �������
				if (currentSegmentStart != segmentStart)
				{
					currentSegmentStart = segmentStart;
					writePos = currentSegmentStart;
					// �������� �������
					TmpSpace::Write(0, (unsigned char *)segmentStart, segmentSize);
					// ������� �������
					FlashMemoryController::Erase(currentSegmentStart);
				}
				if (writePos < address)
				{
					FlashMemoryController::WriteByte(writePos, TmpSpace::ReadByte(writePos - currentSegmentStart));
				}
				else
				{
					FlashMemoryController::WriteByte(writePos, data[srcPos]);
					srcPos++;
				}

				writePos++;
			}
			while(writePos <= writeEnd);

			// ���������� ���������� ����� ���������� ��������
			while(writePos < lastSegmentStart + lastSegmentSize)
			{
				FlashMemoryController::WriteByte(writePos, TmpSpace::ReadByte(writePos - lastSegmentStart));
				writePos++;
			}
		}
	};
}
#endif