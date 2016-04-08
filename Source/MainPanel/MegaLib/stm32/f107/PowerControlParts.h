////////////////////////////////////////////////////////////////////////////////
//
//
//	Fateev A.I.		18/12/2011
////////////////////////////////////////////////////////////////////////////////

#ifndef POWERCONTROLPARTS_STM32_F107_H
#define POWERCONTROLPARTS_STM32_F107_H

#include "..\..\RegisterReference.h"

template<RegisterPointerType RegisterAddr>
class PowerControlRegister
{
protected:
	
	template<class RegisterDataType>
	struct RegisterStruct
	{
		RegisterDataType LowPowerDeepsleep						: 1;
		RegisterDataType PowerDownDeepsleep						: 1;
		RegisterDataType ClearWakeupFlag						: 1;
		RegisterDataType ClearStandbyFlag						: 1;
		RegisterDataType PowerVoltageDetectorEnable				: 1;
		RegisterDataType PvdLevelSelection						: 3;
		RegisterDataType DisableBackupDomainWriteProtection		: 1;
		RegisterDataType Reserved1								: 23;
	};
	
	typedef RegisterReference<RegisterStruct<unsigned int>, RegisterAddr> RegRef;
public:
	inline static void DisableBackupDomainWriteProtection(bool disable = true)
	{
		RegRef::_register->DisableBackupDomainWriteProtection = disable;
	}
};

#endif