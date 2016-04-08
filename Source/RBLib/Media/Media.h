///////////////////////////////////////////////////////////////////////////////
//	������� ����� �������� �������� ����������
//	
//
///////////////////////////////////////////////////////////////////////////////

#ifndef MEDIA_MEDIA_MEDIA_H
#define MEDIA_MEDIA_MEDIA_H

namespace Rblib
{
namespace Media
{
	class Media
	{
	public:
		Media()
		{
		}
		
		virtual ~Media()
		{
		}

		// �������������� �������������
		virtual bool Init()
		{
			return false;
		}

		// �������� ���� ��������
		virtual void Run()
		{
			// �������������� ��� ����� ��� �������� ��������� �������� ���������� � ���������� ���������� ������ ���������
		}

		// ���������� �������� � ������
		virtual bool Ready()
		{
			return false;
		}

		// ������� �������� � ��������
		virtual unsigned long long SectorsCount()
		{
			return 0;
		}

		// ������ �������
		virtual unsigned int SectorSize()
		{
			return 0;
		}

		// ��������� ������ ��� ������� sector � ����� buffer
		virtual bool ReadSector(unsigned long sector, unsigned char *buffer)
		{
			return false;
		}

		// �������� ������ data � ������ ��� ������� sector
		virtual bool WriteSector(unsigned long sector, unsigned char *data)
		{
			return false;
		}
	};
}
}

#endif
