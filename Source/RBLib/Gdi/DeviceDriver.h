///////////////////////////////////////////////////////////////////////////////
//	��������� �������� ������������ ����������
//	������ ������� ������ ������������� ���� ��������� ���� �� ���������
//
//  ����������� ���������� ��������� DeviceCaps (��������������, ���� ������� ���������� ����������� �� ����� ������)
//	��� ���������� ����� ������������ ��������
//	���� ����� �� ����������� �� ����������� ���������, �� �������� ���������� ��������� ��� �����������
//
//	������ ������� ������ ������������� ���� �� SetPixel, ����� ���-�� ����������
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef GDI_DEVICEDRIVER_H
#define GDI_DEVICEDRIVER_H

namespace Rblib
{
namespace Gdi
{
	class DeviceDriver
	{
	public:
		// ����� ������������ ��������
		// ��������,
		//    ���� LineTo == true,
		//    �� ������� ��������� ����� LineTo,
		//    ����� ����� LineTo �� ����������� � ����� �������������
		struct DeviceCaps
		{
			static const bool SetPixel = false;
			static const bool MoveTo = false;
			static const bool LineTo = false;
			static const bool FillRect = false;
			static const bool FrameRect = false;
			static const bool Rectangle = false;
		};
		
		// ��������� �������� ����� �� ������������� �������� RGB
		static int GetColorFromIntRgb(int r, int g, int b);
		// ��������� �������� ����� �� ������������ �������� RGB
		static int GetColorFromFloatRgb(float r, float g, float b);
		
		static void SetPixel(int x, int y, int color);
		static void MoveTo(int x, int y);
		static void LineTo(int x, int y);
	};
}
}

#endif