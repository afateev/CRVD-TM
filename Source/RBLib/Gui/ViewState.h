///////////////////////////////////////////////////////////////////////////////
//	Параметры внешнего вида элемента управления графического интерфейса пользователя
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef GUI_VIEWSTATE_H
#define GUI_VIEWSTATE_H

namespace Rblib
{
namespace Gui
{
	class ViewState
	{
	public:
		enum TextAling
		{
			TextAlingLeft,
			TextAlingCenter,
			TextAlingRight
		};

		enum ImageTransparencyCode
		{
			ImageTransparencyNone,
			ImageTransparencyAuto
		};
	public:
		void *Skin;
		int Left;
		int Right;
		int Top;
		int Bottom;
		bool Focused;
		bool Enabled;
		bool Visible;
		bool Checked;
		char *Text;
		int TextLen;
		TextAling TextHorizontalAlign;
		void *ImageDump;
		ImageTransparencyCode ImageTransparency;
		float FloatingValue;
		bool WaitFlag;
		void *Data;
		int CustomColor;
	public:

		inline int Width()
		{
			return Right - Left;
		}

		inline int Height()
		{
			return Bottom - Top;
		}

		ViewState()
		{
			Left = 0;
			Right = 0;
			Top = 0;
			Bottom = 0;
			Focused = false;
			Enabled = false;
			Visible = false;
			Text = 0;
			TextLen = 0;
			TextHorizontalAlign = TextAlingLeft;
			ImageDump = 0;
			ImageTransparency = ImageTransparencyNone;
			FloatingValue = 0.0;
			WaitFlag = false;
			Data = 0;
			CustomColor = 0;
		}
	};
}
}

#endif