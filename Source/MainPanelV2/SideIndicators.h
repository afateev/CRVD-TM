////////////////////////////////////////////////////////////////////////////////
//
//
//	Fateev A.I.		21/12/2011
////////////////////////////////////////////////////////////////////////////////

#ifndef SIDEINDICATORS_H
#define SIDEINDICATORS_H

template<class StatorIndicator, class RotorIndicator, class ActiveDriveControllerParams>
class SideIndicators
{
protected:
	bool _inited;
public:
	SideIndicators()
	{
		_inited = false;
	}
	void Init()
	{
		StatorIndicator::Init();
		RotorIndicator::Init();
		StatorIndicator::Enable();
		RotorIndicator::Enable();
		
		_inited = true;
	}
	
	void Update()
	{
		if (!_inited)
		{
			return;
		}
		
		bool problem = ActiveDriveControllerParams::HasProblem();

        // Отображаем в киловольтах
		float uSt = ActiveDriveControllerParams::GetUst() / 1000.0;
		float iSt = ActiveDriveControllerParams::GetIst();
		float uRot = ActiveDriveControllerParams::GetUrot();
		float iRot = ActiveDriveControllerParams::GetIrot();;
		UpdateStator(uSt, iSt, problem);
		UpdateRotor(uRot, iRot, problem);
	}
	
	void UpdateStator(float u, float i, bool problem)
	{
		if (!_inited)
		{
			return;
		}
		
		char uValue[10];
		char iValue[10];
		char str[10];
		unsigned char uLen;
		unsigned char iLen;
		unsigned char len = 0;
		unsigned char skip = 0;
		
		if (problem)
        {
            uLen = sprintf(uValue, "----");
            iLen = sprintf(iValue, "----");
        }
        else
        {
            uLen = sprintf(uValue, "%5.2f", u);
            iLen = sprintf(iValue, "%4.0f", i);
		}
		for(unsigned char c = 0; c < uLen; c++)
		{
			str[len] = uValue[c];
			len++;
			if(uValue[c] != '.' && uValue[c + 1] == '.')
				skip++;
			if (len - skip >= 4)
				break;
		}
		
		for(unsigned char c = 0; c < iLen; c++)
		{
			str[len] = iValue[c];
			len++;
			if(iValue[c] != '.' && iValue[c + 1] == '.')
				skip++;
			if (len - skip >= 8)
				break;
		}
		
		StatorIndicator::WriteString(str, len);
        //StatorIndicator::Enable(true, problem);
	}
	
	void UpdateRotor(float u, float i, bool problem)
	{
		if (!_inited)
		{
			return;
		}
		
		char uValue[10];
		char iValue[10];
		char str[10];
		unsigned char uLen;
		unsigned char iLen;
		unsigned char len = 0;
		unsigned char skip = 0;
		
        if (problem)
        {
            uLen = sprintf(uValue, "----");
            iLen = sprintf(iValue, "----");
        }
        else
        {
            uLen = sprintf(uValue, "%5.1f", u);
		    iLen = sprintf(iValue, "%5.1f", i);
		}
		
		
		for(unsigned char c = 0; c < uLen; c++)
		{
			str[len] = uValue[c];
			len++;
			if(uValue[c] != '.' && uValue[c + 1] == '.')
				skip++;
			if (len - skip >= 4)
				break;
		}
		
		for(unsigned char c = 0; c < iLen; c++)
		{
			str[len] = iValue[c];
			len++;
			if(iValue[c] != '.' && iValue[c + 1] == '.')
				skip++;
			if (len - skip >= 8)
				break;
		}
		
		RotorIndicator::WriteString(str, len);
        //RotorIndicator::Enable(true, problem);
	}
};

#endif