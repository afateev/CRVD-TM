////////////////////////////////////////////////////////////////////////////////
//
//
//	Fateev A.I.		26/11/2011
////////////////////////////////////////////////////////////////////////////////

#ifndef RESETANDCLOCKCONTROL_STM32_F107_H
#define RESETANDCLOCKCONTROL_STM32_F107_H

#include "ResetAndClockControlParts.h"
#include "PowerControl.h"

class ResetAndClockControl :
	public ClockControlRegister					<0x40021000 + 0x00>,
	public ClockConfigurationRegister			<0x40021000 + 0x04>,
	public AhbPeripheralClockEnableRegister		<0x40021000 + 0x14>,
	public Apb2PeripheralClockEnableRegister	<0x40021000 + 0x18>,
	public Apb1PeripheralClockEnableRegister	<0x40021000 + 0x1C>,
	public BackupDomainControlRegister			<0x40021000 + 0x20>
{
public:
	static void RtcInit()
	{
		EnableBackupInterface();
		EnablePowerInterface();
		if (!IsRtcEnabled())
		{
			PowerControl::DisableBackupDomainWriteProtection(true);
			ExternalLowSpeedClockEnable();
			unsigned int waitTimeOut = 1000;
			while(! IsExternalLowSpeedClockReady() && waitTimeOut > 0)
			{
				waitTimeOut--;
			}
			RtcClockSourceSelect(1);
			RtcEnable();
			PowerControl::DisableBackupDomainWriteProtection(false);
		}
	}
};

#endif