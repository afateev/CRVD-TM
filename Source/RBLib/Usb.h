#ifndef RBLIB_USB_H
#define RBLIB_USB_H

#include "Platform.h"

#ifdef STM32
#include "stm32\Usb.h"
#endif

namespace Rblib
{
	// �������� ��������� / ������������ ���
	typedef MakePlatformTypelist<
#ifdef STM32
			PlatformStm32F4, Stm32::F4::Usb
#endif
		>::Result UsbTypes;
	
	// �������� ������
	template <Platform p>
	class UsbSelector: public PlatformSelector<UsbTypes, p>::Result
	{
	private:
		// ����������� �� ��������� (������, �.�. ����� ��������� �����������)
		UsbSelector()
		{
		
		}
	};

	// �����
	typedef UsbSelector<RBLIB_PALTFORM_AUTODETECT> Usb;
}

#endif