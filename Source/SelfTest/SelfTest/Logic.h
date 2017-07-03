#ifndef LOGIC_H
#define LOGIC_H

template
	<
		class SignalPos5VC1,
		class SignalPos3V3,
		class SignalPos5VA1,
		class SignalNeg5VA1,
		class SignalPos5VA2,
		class SignalNeg5VA2,
		class SignalPos5VC2,
		class SignalD_50HzImp,
		class SignalD_Rdy,
		class SignalDO_IPCtrl_R,
		class SignalDO_IPCtrl_G,
		class SignalD_SA,
		class SignalD_SB,
		class SignalD_SC,
		class SignalDO_YPrt
	>
class Logic
{
public:
	float Pos5VC1;
	float Pos3V3;
	float Pos5VA1;
	float Neg5VA1;
	float Pos5VA2;
	float Neg5VA2;
	float Pos5VC2;
	
	bool Pos5VC1Error;
	bool Pos3V3Error;
	bool Pos5VA1Error;
	bool Neg5VA1Error;
	bool Pos5VA2Error;
	bool Neg5VA2Error;
	bool Pos5VC2Error;
	
	bool DRdy;
	
	bool SAError;
	bool SBError;
	bool SCError;
	bool SyncError;
	bool PhError;
	
	bool PmError;
	unsigned int PmErrorCnt;
	
	unsigned int DegPhAB;
	bool DegPhABError;
	unsigned int DegPhBC;
	bool DegPhBCError;
	unsigned int DegPhCA;
	bool DegPhCAError;
	
	float PMFreq;
public:
	Logic()
	{
		Reset();
	}
	
	void Init()
	{
		Reset();
		SignalD_SA::OnFront.Set(OnFrontA, this);
		SignalD_SB::OnFront.Set(OnFrontB, this);
		SignalD_SC::OnFront.Set(OnFrontC, this);
	}
	
	void Reset()
	{
		Pos5VC1 = 0;
		Pos3V3 = 0;
		Pos5VA1 = 0;
		Neg5VA1 = 0;
		Pos5VA2 = 0;
		Neg5VA2 = 0;
		Pos5VC2 = 0;
		
		Pos5VC1Error = false;
		Pos3V3Error = false;
		Pos5VA1Error = false;
		Neg5VA1Error = false;
		Pos5VA2Error = false;
		Neg5VA2Error = false;
		Pos5VC2Error = false;
		
		DRdy = false;
		SAError = false;
		SBError = false;
		SCError = false;
		SyncError = false;
		PhError = false;
		
		PmError = false;
		PmErrorCnt = 0;
		
		PMFreq = 0;
		
		DegPhAB = 0;
		DegPhBC = 0;
		DegPhCA = 0;
		DegPhABError = false;
		DegPhBCError = false;
		DegPhCAError = false;
	}
	
	void Update()
	{
		const float errorThreshld = 0.1;
		
		Pos5VC1 = SignalPos5VC1::Get();
		Pos5VC1 *= 1.945;
		
		Pos5VC1Error = (Pos5VC1 < 5.0 - 5.0 * errorThreshld) || (Pos5VC1 > 5.0 + 5.0 * errorThreshld);
		
		Pos3V3 = SignalPos3V3::Get();
		Pos3V3 *= 1.308;
		
		Pos3V3Error = (Pos3V3 < 3.3 - 3.3 * errorThreshld) || (Pos3V3 > 3.3 + 3.3 * errorThreshld);
		
		Pos5VA1 = SignalPos5VA1::Get();
		Pos5VA1 *= 2.0;
		
		Pos5VA1Error = (Pos5VA1 < 5.0 - 5.0 * errorThreshld) || (Pos5VA1 > 5.0 + 5.0 * errorThreshld);
		
		Neg5VA1 = SignalNeg5VA1::Get();
		Neg5VA1 *= 4.84;
		Neg5VA1 += -17.55;
		
		Neg5VA1Error = (-Neg5VA1 < 5.0 - 5.0 * errorThreshld) || (-Neg5VA1 > 5.0 + 5.0 * errorThreshld);
		
		Pos5VA2 = SignalPos5VA2::Get();
		Pos5VA2 *= 2.0;
		
		Pos5VA2Error = (Pos5VA2 < 5.0 - 5.0 * errorThreshld) || (Pos5VA2 > 5.0 + 5.0 * errorThreshld);
		
		Neg5VA2 = SignalNeg5VA2::Get();
		Neg5VA2 *= 4.84;
		Neg5VA2 += -17.55;
		
		Neg5VA2Error = (-Neg5VA2 < 5.0 - 5.0 * errorThreshld) || (-Neg5VA2 > 5.0 + 5.0 * errorThreshld);
		
		Pos5VC2 = SignalPos5VC2::Get();
		Pos5VC2 *= 2.0;
		
		Pos5VC2Error = (Pos5VC2 < 5.0 - 5.0 * errorThreshld) || (Pos5VC2 > 5.0 + 5.0 * errorThreshld);
		
		if (Pos5VC1Error || Pos3V3Error || Pos5VA1Error || Neg5VA1Error || Pos5VA2Error || Neg5VA2Error || Pos5VC2Error)
		{
			SignalDO_IPCtrl_R::Set(true);
			SignalDO_IPCtrl_G::Set(false);
		}
		else
		{
			SignalDO_IPCtrl_R::Set(false);
			SignalDO_IPCtrl_G::Set(true);
		}
		
		DRdy = SignalD_Rdy::Get();
		
		if (DRdy)
		{
			SAError = SignalD_SA::Timeout();
			SBError = SignalD_SB::Timeout();
			SCError = SignalD_SC::Timeout();
			
			SyncError = SAError || SBError || SCError;
			
			PhError = DegPhABError || DegPhBCError || DegPhCAError;
			
			PMFreq = SignalD_SA::GetFrequency();
			
			unsigned int pmErrorDetecFreq = SignalD_50HzImp::GetFrequency();
			if (pmErrorDetecFreq > 45 && pmErrorDetecFreq < 55)
			{
				if (PmErrorCnt < 10)
				{
					PmErrorCnt++;
				}
				else
				{
					PmError = true;
				}
			}
			else
			{
				PmErrorCnt = 0;
				PmError = false;
			}
		}
		else
		{
			SignalD_SA::Reset();
			SignalD_SB::Reset();
			SignalD_SC::Reset();
			SAError = false;
			SBError = false;
			SCError = false;
			DegPhABError = false;
			DegPhBCError = false;
			DegPhCAError = false;
			PhError = false;
			SyncError = false;
			PMFreq = 0;
			PmErrorCnt = 0;
			PmError = false;
		}
	}
	
	
	static void OnFrontA(void *calbackParam)
	{
		Logic *im = (Logic *)calbackParam;
		if (!im)
		{
			return;
		}
		
		if (!im->DRdy)
		{
			im->DegPhCA = 0;
			return;
		}
		
		unsigned int ph = SignalD_SC::Get();
		ph *= 360;
		ph /= SignalD_SC::NormalTickCount;
		im->DegPhCA = ph;
		im->DegPhCAError = (ph < 120 - 120 * 0.1) || (ph > 120 + 120 * 0.1);
	}
	
	static void OnFrontB(void *calbackParam)
	{
		Logic *im = (Logic *)calbackParam;
		if (!im)
		{
			return;
		}
		
		if (!im->DRdy)
		{
			im->DegPhAB = 0;
			return;
		}
		
		unsigned int ph = SignalD_SA::Get();
		ph *= 360;
		ph /= SignalD_SA::NormalTickCount;
		im->DegPhAB = ph;
		im->DegPhABError = (ph < 120 - 120 * 0.1) || (ph > 120 + 120 * 0.1);
	}
	
	static void OnFrontC(void *calbackParam)
	{
		Logic *im = (Logic *)calbackParam;
		if (!im)
		{
			return;
		}
		
		if (!im->DRdy)
		{
			im->DegPhBC = 0;
			return;
		}
		
		unsigned int ph = SignalD_SB::Get();
		ph *= 360;
		ph /= SignalD_SB::NormalTickCount;
		im->DegPhBC = ph;
		im->DegPhBCError = (ph < 120 - 120 * 0.1) || (ph > 120 + 120 * 0.1);
	}
};

#endif