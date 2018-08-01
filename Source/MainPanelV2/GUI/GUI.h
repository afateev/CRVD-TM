#ifndef GUI_H
#define GUI_H

#include "../../RbLib/Rblib.h"
#include "GUI/Desctop.h"
#include "GUI/DesctopStack.h"
#include "../Drivers.h"
#include "../OscGetter.h"

typedef Rblib::Display Display;
Display display;

Rblib::FbgFont generalFont(Rblib::GOSTtypeB8x16);
Rblib::FbgFont menuBigFont(Rblib::GOSTtypeB16x32);

Menu<Display, display> mainMenu(&generalFont, &menuBigFont);
typedef Desctop<Display, display> DesctopType;
DesctopType desctop;

DesctopStack<10> desctopStack;

template<WindowId wnd>
void ShowWindow(MenuItemListBase *sender)
{
	desctopStack.Push(sender);
	desctop.ShowWindow(wnd);
}

template<WindowId wndId>
char *GetEditingValueString()
{
	DesctopType::WindowType *wnd = desctop.FindWindow(wndId);
	if (wnd)
	{
		return wnd->GetEditingValue();
	}
	
	return "---";
}

void ShowOsc(MenuItemListBase *sender)
{
	
	ShowWindow<wndIdGraph>(sender);
}

MenuItemList menuListRoot("������� ����");
MenuItemList menuListSystemSetup("��������� ��������� ����������");
MenuItemList menuListModbusSetup("��������� Modbus");
MenuItemList menuListControllerSetup("��������� ����������");
MenuItemList menuListControllerUstSetup("������� �������������");
MenuItemList menuListIndicationSetup("��������� ���������");
MenuItemList menuListDiagnostic("�����������");

MenuItem menuViewWaveforms("�������������", ShowWindow<wndIdOscList>);
MenuItem menuViewEvents("������ �������", ShowWindow<wndIdEvents>);
MenuItem menuCosinusControlSetup("��������� / ���������� ������ ������������� �� Cos�", ShowWindow<wndIdControllerCosChannel>);
//MenuItem menuCosinusControlSetup("��������� / ���������� ������", 0);
MenuItem menuSystemSetup("��������� ���������", &menuListSystemSetup);
MenuItem menuControllerSetup("��������� ����������", &menuListControllerSetup);
MenuItem menuControllerUstSetup("������� ����������", &menuListControllerUstSetup);
MenuItem menuIndicationSetup("��������� ���������", &menuListIndicationSetup);
MenuItem menuDiagnostic("�����������", &menuListDiagnostic);

MenuItem menuGraph("�������������", ShowOsc);
MenuItem menuDebugRegisters("��������", ShowWindow<wndIdDebugRegisters>);
MenuItem menuDebugOsc("�������������", ShowWindow<wndIdDebugOsc>);
MenuItem menuMainScreen("�����", ShowWindow<wndIdMain>);
MenuItem menuHideAll("������� ��� ����", ShowWindow<wndIdDesctop>);

MenuItem menuSetupDateTime("���� / �����", ShowWindow<wndIdConfigSystemDateTime>);
MenuItem menuSetupModbus("Modbus", &menuListModbusSetup);
MenuItem menuResetOperatingTime("�������� ������� ���������              ", ShowWindow<wndIdResetOperatingTime>);
MenuItem menuSetupIP("IP �����");
MenuItem menuSetupNetwork("������� �����");

MenuItem menuDiagnosticMain("��������", ShowWindow<wndIdDiagnosticMain>);
MenuItem menuDiagnosticReserv("���������", ShowWindow<wndIdDiagnosticReserv>);
//MenuItem menuDiagnosticTemperature("�����������", ShowWindow<wndIdDiagnosticTemperature>);
//MenuItem menuDiagnosticAnalogOutputs("���������� ������", ShowWindow<wndIdDiagnosticAnalogOutputs>);
//MenuItem menuDiagnosticLinkStatus("��������� �����", ShowWindow<wndIdDiagnosticLinkStatus>);

//MenuItem menuControllerNominalStatorCurrent("����������� ��� �������", ShowWindow<wndIdControllerNominalStatorCurrent>, GetEditingValueString<wndIdControllerNominalStatorCurrent>);
//MenuItem menuControllerRotorCurrentMax("I ������ ������������ (�)", ShowWindow<wndIdControllerRotorCurrentMax>, GetEditingValueString<wndIdControllerRotorCurrentMax>);

MenuItem menuControllerSplitterDrive("----------��������� ���������----------");
MenuItem menuControllerSplitterProtections("------------��������� �����------------");
MenuItem menuControllerSplitterController("----------��������� ���������----------");

WindowMain<Display, display, MainWindowDisplayData> wndMain;
WindowDebugRegisters<Display, display> wndDebugRegisters;
WindowDebugOsc<Display, display> wndDebugOsc;
WindowEvents<Display, display> wndEvents;
WindowConfigSystemDateTime<Display, display> wndConfigSystemDateTime(&menuBigFont);
WindowResetOperatingTime<Display, display> wndResetOperatingTime(&menuBigFont);
typedef WindowGraph<Display, display, Drivers::DrawContextType, &Drivers::DrawContext> WindowGraphType;
WindowGraphType wndGraph;
WindowOscList<Display, display, OscGet, OscFileFormat::OscFileDescription, DesctopType, &desctop, WindowGraphType, &wndGraph, wndIdGraph> wndOscList;
WindowDiagnosticRegulator<Display, display, true> wndDiagnosticRegulatorMain;
WindowDiagnosticRegulator<Display, display, false> wndDiagnosticRegulatorReserv;

void OnWindowClose()
{
	MenuItemListBase *menu = 0;
	if (desctopStack.Pop(&menu) && menu)
		mainMenu.Show(menu);
	else
		mainMenu.Show();
}

void OnWindowGraphClose()
{
	Drivers::DrawContext.SecondBufferEnable(false);
	wndGraph.OnClose();
	desctop.ShowWindow(wndIdOscList);
}

void MenuInit()
{
	menuListRoot.Add(&menuViewWaveforms);
	menuListRoot.Add(&menuViewEvents);
    menuListRoot.Add(&menuControllerUstSetup);
	//menuListRoot.Add(&menuCosinusControlSetup);
	menuListRoot.Add(&menuSystemSetup);
	menuListRoot.Add(&menuControllerSetup);
	menuListRoot.Add(&menuIndicationSetup);
	menuListRoot.Add(&menuDiagnostic);
	//menuListRoot.Add(&menuDebugRegisters);
	//menuListRoot.Add(&menuMainScreen);
	//menuListRoot.Add(&menuHideAll);
	
	menuListSystemSetup.Add(&menuSetupDateTime);
	menuListSystemSetup.Add(&menuSetupModbus);
	//menuListSystemSetup.Add(&menuResetOperatingTime);
	//menuListSystemSetup.Add(&menuSetupIP);
	//menuListSystemSetup.Add(&menuSetupNetwork);
	
    
    //menuListControllerSetup.Add(&menuControllerNominalStatorCurrent);
	menuListIndicationSetup.Add(&menuResetOperatingTime);
	
	menuListDiagnostic.Add(&menuDiagnosticMain);
	menuListDiagnostic.Add(&menuDiagnosticReserv);
	//menuListDiagnostic.Add(&menuDiagnosticTemperature);
	//menuListDiagnostic.Add(&menuDiagnosticAnalogOutputs);
	//menuListDiagnostic.Add(&menuDiagnosticLinkStatus);
	menuListDiagnostic.Add(&menuDebugRegisters);
	menuListDiagnostic.Add(&menuDebugOsc);
		
	mainMenu.SetRoot(&menuListRoot);
}

void GetMainWindowDisplayData(MainWindowDisplayData &displayData);
void GetDebugOscDisplayData(WindowDebugOsc<Display, display>::DisplayData &data);

void DesctopInit()
{
	wndMain.GetDisplayDataCallback = GetMainWindowDisplayData;
	wndGraph.SetOnClose(OnWindowGraphClose);
	wndDebugRegisters.SetOnClose(OnWindowClose);
	wndDebugRegisters.PrimaryControllerGetAddressCallback = PrimaryController::GetAddress;
	wndDebugRegisters.PrimaryControllerGetRegValueCallback = PrimaryController::GetRegValue;
	wndDebugRegisters.ReserveControllerGetAddressCallback = ReserveController::GetAddress;
	wndDebugRegisters.ReserveControllerGetRegValueCallback = ReserveController::GetRegValue;
	wndDebugRegisters.PrimaryDiagnosticGetAddressCallback = MainControllerDiagnostic::GetAddress;
	wndDebugRegisters.PrimaryDiagnosticGetRegValueCallback = MainControllerDiagnostic::GetRegValue;
	wndDebugRegisters.ReserveDiagnosticGetAddressCallback = ReservControllerDiagnostic::GetAddress;
	wndDebugRegisters.ReserveDiagnosticGetRegValueCallback = ReservControllerDiagnostic::GetRegValue;
	//wndDebugRegisters.InsulationControllerGetAddressCallback = InsulationController::GetAddress;
	//wndDebugRegisters.InsulationControllerGetRegValueCallback = InsulationController::GetRegValue;
	
	wndDebugOsc.SetOnClose(OnWindowClose);
	wndDebugOsc.GetDisplayDataCallback = GetDebugOscDisplayData;
	
	wndEvents.SetOnClose(OnWindowClose);
	wndConfigSystemDateTime.SetOnClose(OnWindowClose);
	wndResetOperatingTime.SetOnClose(OnWindowClose);
	wndResetOperatingTime.GetOperatingTimeCallback = ActiveDriveControllerParams::GetOperatingTime;
	wndResetOperatingTime.ResetOperatingTimeCallback = ActiveDriveControllerParams::ResetOperatingTime;
	wndOscList.SetOnClose(OnWindowClose);
	
	wndDiagnosticRegulatorMain.SetOnClose(OnWindowClose);
	wndDiagnosticRegulatorMain.GetRegValueCallback = MainControllerDiagnostic::GetRegValue;
	wndDiagnosticRegulatorMain.GetTimeoutFlagCallback = MainControllerDiagnostic::NoResponse;
	
	wndDiagnosticRegulatorReserv.SetOnClose(OnWindowClose);
	wndDiagnosticRegulatorReserv.GetRegValueCallback = ReservControllerDiagnostic::GetRegValue;
	wndDiagnosticRegulatorReserv.GetTimeoutFlagCallback = ReservControllerDiagnostic::NoResponse;
    
	
	desctop.RegisterWindow(&wndMain, wndIdMain);
	desctop.RegisterWindow(&wndOscList, wndIdOscList);
	desctop.RegisterWindow(&wndGraph, wndIdGraph);
	desctop.RegisterWindow(&wndDebugRegisters, wndIdDebugRegisters);
	desctop.RegisterWindow(&wndDebugOsc, wndIdDebugOsc);
	desctop.RegisterWindow(&wndEvents, wndIdEvents);
	desctop.RegisterWindow(&wndConfigSystemDateTime, wndIdConfigSystemDateTime);
	desctop.RegisterWindow(&wndResetOperatingTime, wndIdResetOperatingTime);
	desctop.RegisterWindow(&wndDiagnosticRegulatorMain, wndIdDiagnosticMain);
	desctop.RegisterWindow(&wndDiagnosticRegulatorReserv, wndIdDiagnosticReserv);
    
	//wndControllerNominalStatorCurrent.SetOnClose(OnWindowClose);
	//desctop.RegisterWindow(&wndControllerNominalStatorCurrent, wndIdControllerNominalStatorCurrent);
	
	desctop.ShowWindow(wndIdMain);
}
/*
//#define CONTROLLER_PARAM_DECL(VarName, VarCaption, VarType, IntPart, FracPart, Format, Divider) MenuItem menuController##VarName (VarCaption, ShowWindow<wndIdController##VarName>, GetEditingValueString<wndIdController##VarName>);
MenuItem menuControllerNominalStatorCurrent (" I�� �����������, �  ", ShowWindow<wndIdControllerNominalStatorCurrent>, GetEditingValueString<wndIdControllerNominalStatorCurrent>);
WindowEditRegValue<Display, display, float, ControlParamInput<3, 0>,
		ActiveDriveControllerParams::GetRegValue_float<ActiveDriveControllerParams::RegNominalStatorCurrent>,
		ActiveDriveControllerParams::SetRegValue_float<ActiveDriveControllerParams::RegNominalStatorCurrent>,
        1>
			wndControllerNominalStatorCurrent(&menuBigFont, "��������� ����������", " I�� �����������, �  ", "%3.0f");
*/

CONTROLLER_PARAM_DECL(NominalStatorCurrent, " I�� �����������, �  ", float, 3, 0, "%3.0f", 1)
CONTROLLER_PARAM_DECL(StartStatorCurrent, " I�� ��������, �     ", float, 4, 0, "%3.0f", 1)
CONTROLLER_PARAM_DECL(NominalRotorCurrent, " I� �����������, �   ", float, 3, 0, "%3.0f", 1)
CONTROLLER_PARAM_DECL(StartRotorCurrent, " I� ��������, �      ", float, 4, 0, "%3.0f", 1)
CONTROLLER_PARAM_DECL(RotorCurrentMax, " I� ����, �          ", float, 3, 1, "%3.1f", 1)
CONTROLLER_PARAM_DECL(RotorCurrentMin, " I� ���, �           ", float, 3, 1, "%3.1f", 1)
CONTROLLER_PARAM_DECL(CosChannel, "����� �������������                ", int, 1, 0, "%1d", 1)
CONTROLLER_PARAM_DECL(EnergizingCurrentSetup, " 1 - ������� �� I������, �", float, 3, 1, "%3.1f", 1)
CONTROLLER_PARAM_COS_DECL(AngleSetupDisplay, " 2 - ������� �� cos�      ", float, 2, 2, "%2.2f", 1)
CONTROLLER_PARAM_DECL(EnergizingReactCurrentSetup, " 3 - ������� �� I�����, �", float, 4, 0, "%4.0f", 1)
CONTROLLER_PARAM_DECL(EnergizingReactPowerSetup, " 4 - ������� �� Q�����, ����", float, 3, 1, "%3.1f", 1)
CONTROLLER_PARAM_DECL(GlideSetup, "������� �� ����������, ��", float, 1, 1, "%1.1f", 1)
CONTROLLER_PARAM_DECL(GlideStart, "���� �� ����������   ", int, 0, 0, "%s", 1)
//CONTROLLER_PARAM_DECL(H_ka_f_I0, "H_ka.f_I0            ", float, 3, 0, "%3.0f")
//CONTROLLER_PARAM_DECL(H_ka_f_tg, "H_ka.f_tg            ", float, 3, 0, "%3.0f")
//CONTROLLER_PARAM_DECL(H_ka_f_v, "H_ka.f_v            ", float, 3, 0, "%3.0f")
CONTROLLER_PARAM_DECL(LowStatorVoltageProtection, "������ �� ������� U��", int, 0, 0, "%s", 1)
CONTROLLER_PARAM_DECL(LowStatorVoltageProtectionThreshold, "����� ������� U �������, �", float, 5, 1, "%5.1f", 1)
CONTROLLER_PARAM_DECL(LowStatorVoltageProtectionTimer, "������ ������ �� ������� U��, �", int, 3, 0, "%3d", 1000)
CONTROLLER_PARAM_DECL(LowStatorCurrentProtection, "������ �� ������� I��", int, 0, 0, "%s", 1)
CONTROLLER_PARAM_DECL(LowStatorCurrentProtectionThreshold, "����� ������� I �������, �", float, 3, 1, "%3.1f", 1)
CONTROLLER_PARAM_DECL(LowStatorCurrentProtectionTimer, "������ ������ �� ������� I��, �", int, 3, 0, "%3d", 1000)
CONTROLLER_PARAM_DECL(AsyncRotorRotationCount, "��������� ������ ��� ���.����.����", int, 2, 0, "%3d", 1)
CONTROLLER_PARAM_DECL(AsyncCountdownTimer, "����� ���.������� ��� ���.����.����", int, 3, 0, "%3d", 1000)
CONTROLLER_PARAM_DECL(AsyncProtectionTimer, "������ ������ ��� ������.����.����", int, 3, 0, "%3d", 1000)
CONTROLLER_PARAM_DECL(EnergizingLostTimer, "������ ������ �� ������ �����������", int, 3, 0, "%3d", 1000)
CONTROLLER_PARAM_DECL(LongStartTimer, "������ ������ �� ������������� �����", int, 3, 0, "%3d", 1000)
CONTROLLER_PARAM_DECL(ShortCircuitProtectionThreshold, "����� ������ �� ��, �", float, 3, 0, "%3.0f", 1)
CONTROLLER_PARAM_DECL(ShortCircuitProtectionTimer, "������ ������ �� ��, ��", int, 3, 0, "%3d", 1)
CONTROLLER_PARAM_DECL(OverloadProtectionThreshold, "����� ��� ����������, �", float, 3, 0, "%3.0f", 1)
CONTROLLER_PARAM_DECL(RotorCoolingCurrent, "��� ��������� ������, �", float, 3, 0, "%3.0f", 1)
CONTROLLER_PARAM_DECL(RotorCoolingTime, "����� ��������� ������, �", float, 3, 0, "%3.0f", 1000)
CONTROLLER_PARAM_DECL(HeatingTimeOnOverload0_10, "����� ������� ��� ������. < 10%, �", float, 3, 0, "%3.0f", 1000)
CONTROLLER_PARAM_DECL(HeatingTimeOnOverload10_20, "����� ������� ��� ������. 10-20%, �", float, 3, 0, "%3.0f", 1000)
CONTROLLER_PARAM_DECL(HeatingTimeOnOverload20, "����� ������� ��� ������. > 20%, �", float, 3, 0, "%3.0f", 1000)
CONTROLLER_PARAM_DECL(WaitAfterDriveOn, "�������� ����� ���. ���������, �", int, 3, 0, "%3d", 1000)
CONTROLLER_PARAM_DECL(WaitAfterStatorCurrenLow, "�������� ����� �������� I��, ��", int, 6, 0, "%6d", 1)
CONTROLLER_PARAM_DECL(WaitAfterenergizingOn, "�������� ����� ���.�����������, �", int, 3, 0, "%3d", 1000)
CONTROLLER_PARAM_DECL(StartForsingTime, "����� ��������� ����������, �", int, 3, 0, "%3.0d", 1000)
CONTROLLER_PARAM_DECL(StatorVoltageMaxDisplay, " U�� (�������� �� ��������), �", int, 5, 0, "%5d", 1)
CONTROLLER_PARAM_DECL(StatorCurrentMaxDisplay, " I�� (�������� �� ��������), �", int, 5, 0, "%5d", 1)
CONTROLLER_PARAM_DECL(RotorVoltageMaxDisplay, " U� (�������� �� ��������), �", int, 5, 0, "%5d", 1)
CONTROLLER_PARAM_DECL(RotorCurrentMaxDisplay, " I� (�������� �� ��������), �", int, 5, 0, "%5d", 1)

//CONTROLLER_PARAM_UST_DECL(CosChannel, "����� �� Cos         ", int, 0, 0, "%s", 1)
//CONTROLLER_PARAM_UST_COS_DECL(AngleSetupDisplay, "������� �� cos�      ", float, 2, 2, "%2.2f", 1)
//CONTROLLER_PARAM_UST_DECL(EnergizingCurrentSetup, "������� �� I������, �", float, 3, 1, "%3.1f", 1)

INDICATION_PARAM_DECL(StatorVoltageMinDisplay, "����� ��������� U��, k�", float, 2, 2, "%2.2f", 1000)
INDICATION_PARAM_DECL(StatorCurrentMinDisplay, "����� ��������� I��, A", int, 4, 0, "%4d", 1)
INDICATION_PARAM_DECL(RotorVoltageMinDisplay, "����� ��������� U�, �", float, 3, 1, "%3.1f", 1)
INDICATION_PARAM_DECL(RotorCurrentMinDisplay, "����� ��������� I�, A", float, 3, 1, "%3.1f", 1)

MODBUS_PARAM_DECL(SlaveAddr, "�����", int, 3, 0, "%3d", 1)
MODBUS_PARAM_DECL(Boudrate, "��������", int, 6, 0, "%6d", 1)
MODBUS_PARAM_DECL(Parity, "��������", int, 0, 0, "%s", 1)
MODBUS_PARAM_DECL(ParityType, "�������� (���)", int, 0, 1, "%s", 1)


void ControllerParamsMenuEndEditorsInit()
{
	menuListControllerSetup.Add(&menuControllerSplitterDrive);
    CONTROLLER_PARAM_INIT(NominalStatorCurrent);
    CONTROLLER_PARAM_INIT(StartStatorCurrent);
    CONTROLLER_PARAM_INIT(NominalRotorCurrent);
    CONTROLLER_PARAM_INIT(StartRotorCurrent);
    CONTROLLER_PARAM_INIT(RotorCurrentMax);
	CONTROLLER_PARAM_INIT(RotorCurrentMin);
    CONTROLLER_PARAM_INIT(CosChannel);
	CONTROLLER_PARAM_INIT(EnergizingCurrentSetup);
	CONTROLLER_PARAM_INIT(AngleSetupDisplay);
	CONTROLLER_PARAM_INIT(EnergizingReactCurrentSetup);
	CONTROLLER_PARAM_INIT(EnergizingReactPowerSetup);
	CONTROLLER_PARAM_INIT(GlideSetup);
    CONTROLLER_PARAM_INIT(GlideStart);
	//CONTROLLER_PARAM_INIT(H_ka_f_I0);
	//CONTROLLER_PARAM_INIT(H_ka_f_tg);
	//CONTROLLER_PARAM_INIT(H_ka_f_v);
    menuListControllerSetup.Add(&menuControllerSplitterProtections);
    CONTROLLER_PARAM_INIT(LowStatorVoltageProtection);
	CONTROLLER_PARAM_INIT(LowStatorVoltageProtectionThreshold);
    CONTROLLER_PARAM_INIT(LowStatorVoltageProtectionTimer);
    CONTROLLER_PARAM_INIT(LowStatorCurrentProtection);
	CONTROLLER_PARAM_INIT(LowStatorCurrentProtectionThreshold);
    CONTROLLER_PARAM_INIT(LowStatorCurrentProtectionTimer);
    CONTROLLER_PARAM_INIT(AsyncRotorRotationCount);
    CONTROLLER_PARAM_INIT(AsyncCountdownTimer);
    CONTROLLER_PARAM_INIT(AsyncProtectionTimer);
    CONTROLLER_PARAM_INIT(EnergizingLostTimer);
    CONTROLLER_PARAM_INIT(LongStartTimer);
	CONTROLLER_PARAM_INIT(ShortCircuitProtectionThreshold);
    CONTROLLER_PARAM_INIT(ShortCircuitProtectionTimer);
	CONTROLLER_PARAM_INIT(OverloadProtectionThreshold);
	CONTROLLER_PARAM_INIT(RotorCoolingCurrent);
	CONTROLLER_PARAM_INIT(RotorCoolingTime);
	CONTROLLER_PARAM_INIT(HeatingTimeOnOverload0_10);
	CONTROLLER_PARAM_INIT(HeatingTimeOnOverload10_20);
	CONTROLLER_PARAM_INIT(HeatingTimeOnOverload20);
    CONTROLLER_PARAM_INIT(WaitAfterDriveOn);
    CONTROLLER_PARAM_INIT(WaitAfterStatorCurrenLow);
    CONTROLLER_PARAM_INIT(WaitAfterenergizingOn);
    CONTROLLER_PARAM_INIT(StartForsingTime);
	menuListControllerSetup.Add(&menuControllerSplitterController);
    CONTROLLER_PARAM_INIT(StatorVoltageMaxDisplay);
    CONTROLLER_PARAM_INIT(StatorCurrentMaxDisplay);
    CONTROLLER_PARAM_INIT(RotorVoltageMaxDisplay);
    CONTROLLER_PARAM_INIT(RotorCurrentMaxDisplay);
	
    CONTROLLER_PARAM_UST_INIT(CosChannel);
	CONTROLLER_PARAM_UST_INIT(EnergizingCurrentSetup);
	CONTROLLER_PARAM_UST_INIT(AngleSetupDisplay);
	CONTROLLER_PARAM_UST_INIT(EnergizingReactCurrentSetup);
	CONTROLLER_PARAM_UST_INIT(EnergizingReactPowerSetup);
    
	INDICATION_PARAM_INIT(StatorVoltageMinDisplay);
	INDICATION_PARAM_INIT(StatorCurrentMinDisplay);
	INDICATION_PARAM_INIT(RotorVoltageMinDisplay);
	INDICATION_PARAM_INIT(RotorCurrentMinDisplay);
	
	MODBUS_PARAM_INIT(SlaveAddr);
	MODBUS_PARAM_INIT(Boudrate);
	MODBUS_PARAM_INIT(Parity);
	MODBUS_PARAM_INIT(ParityType);
}

#endif