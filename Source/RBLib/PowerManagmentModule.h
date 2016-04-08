
#ifndef POWER_MANAGMENT_MODULE_H
#define POWER_MANAGMENT_MODULE_H

#include "Platform.h"

#ifdef __ICC430__

#include "msp430f5435a\PowerManagmentModule.h"
#include "msp430f5438a\PowerManagmentModule.h"
#include "msp430f5659\PowerManagmentModule.h"

namespace Rblib
{

	// �������� ��������� / ������������ ���
	typedef MakePlatformTypelist<
		PlatformMsp430f5435a, Msp430f5435a::PowerManagmentModule,
		PlatformMsp430f5438a, Msp430f5438a::PowerManagmentModule,
		PlatformMsp430f5659, Msp430f5659::PowerManagmentModule
	>::Result PowerManagmentModuleTypes;

	template<Platform p>
	class PowerManagmentModuleSelector : public PlatformSelector<PowerManagmentModuleTypes, p>::Result
	{
	private:

		PowerManagmentModuleSelector(){}
		~PowerManagmentModuleSelector(){}
		PowerManagmentModuleSelector(const PowerManagmentModuleSelector&){}
		void operator = (const PowerManagmentModuleSelector&) {}

	};

	typedef PowerManagmentModuleSelector<RBLIB_PALTFORM_AUTODETECT> PowerManagmentModule;

	/// \page PowerManagmentModule PowerManagmentModule - ������ ���������� ���������������.
	/// ���������������� ������ ����������� � ����c� PowerManagmentModule.
	/// \class Rblib::PowerManagmentModule
	/// \brief ������ ���������� ���������������.
	/// \details � ����������� �� ��������� ��������� ��������� ���������� � ��������� ������ ���������� �������������� ��� ������ �� �����������:\n
	/// \ref Msp430f5xxx::PowerManagmentModule "Msp430f5435a" \n
	/// \ref Msp430f5xxx::PowerManagmentModule "Msp430f5438a" \n
	/// \ref Msp430f5xxx::PowerManagmentModule "Msp430f5659" \n

}

#endif // __ICC430__

#endif // POWER_MANAGMENT_MODULE_H
