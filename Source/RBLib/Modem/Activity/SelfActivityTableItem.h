///////////////////////////////////////////////////////////////////////////////
//	������� ������ ������� ���������� ����� ���� �������� ��������
//
///////////////////////////////////////////////////////////////////////////////

#ifndef MODEM_ACTIVITY_SELFACTIVITYTABLEITEM_H
#define MODEM_ACTIVITY_SELFACTIVITYTABLEITEM_H

namespace Rblib
{
namespace Modem
{
namespace Activity
{
#pragma pack(push, 1)

	class SelfActivityTableItem
	{
	protected:	
		unsigned char _activityInCurrentIntervalCount;		// ������� ������ ������� � ������� ��������� ��������
		unsigned short _checkIntervalCount;					// ������� ������� ���� � ������� ��������� ��������
		unsigned char _currentRate;							// ������� ������� ����� (_activityInCurrentIntervalCount / _chectPeriod) * 100
	public:
		// ����������
		SelfActivityTableItem()
		{
			Reset();
		}
		
		// �����
		void Reset()
		{
			_checkIntervalCount = 0;
			_currentRate = 0;
			ResetActivityInCurrentIntervalCount();
			ResetCurrentRate();
		}
		
		// ���������� ����������
		void OnActivityDetected(unsigned int normalActivityInterval)
		{
			if (normalActivityInterval < 1)
			{
				return;
			}
			
			// ���������� ����� ������� �������� ����� ������ ����������� ��������� ����������,
			// ����� �� ���� ����������� ��������� +- ���� �������� ����������
			unsigned char offset = _checkIntervalCount % normalActivityInterval;
			if (offset > 0)
			{
				_checkIntervalCount += offset;
				
				// ���� ������ ��� ���, �� �� ���� ������� �� ������ ������������, �� ������ ����� � ����� �������
				// ��� ���� ����� ������ ������������ � �������� �����
				if (_activityInCurrentIntervalCount < 1)
				{
					_checkIntervalCount = 0;
				}
			}
			
			IncrementActivityInCurrentIntervalCount();
		}
		
		// �������� ������� ���������� � ������� ���������
		void ResetActivityInCurrentIntervalCount()
		{
			_activityInCurrentIntervalCount = 0;
		}
		
		// �������� ������� ��������� ��������
		void ResetIntervalCount()
		{
			_checkIntervalCount = 0;
		}
		
		// �������� ������� �������
		void ResetCurrentRate()
		{
			_currentRate = 0;
		}
		
		// ��������� ������� ��������� ��������
		void IncrementCheckIntervalCounter()
		{
			if (_checkIntervalCount < (256 * sizeof(_checkIntervalCount) - 1))
			{
				_checkIntervalCount++;
			}
		}
		
		// ��������� ������� ���������� � ������� ���������
		void IncrementActivityInCurrentIntervalCount()
		{
			if (_activityInCurrentIntervalCount < 255)
			{
				_activityInCurrentIntervalCount++;
			}
		}
		
		// ��������� ���������
		// ����� true ���� ��������� ��������� � ������� ��������
		bool CheckActivity(unsigned int normalActivityInterval, unsigned int checkPeriod)
		{
			if (normalActivityInterval < 1 || checkPeriod < 1)
			{
				return false;
			}
			
			bool changed = false;
			if (_checkIntervalCount >= (normalActivityInterval * checkPeriod))
			{
				unsigned char normalActivityCount = checkPeriod;
				float res = _activityInCurrentIntervalCount;
				res /= (float)normalActivityCount;
				res *= 100.0;
				
				unsigned char result = (unsigned char)res;
				changed = _currentRate != result;
				_currentRate = result;
					
				ResetActivityInCurrentIntervalCount();
				ResetIntervalCount();
			}
			
			return changed;
		}
		
		// �������� ������� �������
		unsigned char GetRate()
		{
			return _currentRate;
		}
	};
	
#pragma pack (pop)
}
}
}

#endif