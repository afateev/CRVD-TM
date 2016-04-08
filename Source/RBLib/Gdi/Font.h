///////////////////////////////////////////////////////////////////////////////
//	������� ����� ��������� ����� �������
//	
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef GDI_FONT_H
#define GDI_FONT_H

namespace Rblib
{
	namespace Gdi
	{
		template<class DeviceContext>
		class Font
		{
		public:
			// ���������� ������
			// dc - �������� ���������
			// chid - ��� �������
			// left - x - ���������� ������ �������� ���� �������
			// top - y - ���������� ������ �������� ���� �������
			// color  - ���� �������
			// ���������� ������ ������������� �������
			virtual int DrawChar(DeviceContext *dc, unsigned int chId, int left, int top, int color)
			{
				// ������� ����� ������ �������������� 8�10 ������ ��������
				dc->FrameRect(left, top, left + 8, top + 10);
				return 8;
			}

			// �������� ������ �������
			// dc - �������� ���������
			// chid - ��� �������
			// width - ������������ ������ �������
			// heigth - ������������ ������ �������
			virtual void GetCharSize(DeviceContext *dc, unsigned int chId, int &width, int &heigth)
			{
				width = 8;
				heigth = 10;
			}
		};
	}
}

#endif