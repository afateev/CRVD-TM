////////////////////////////////////////////////////////////////////////////////
//
//
//	Fateev A.I.		26/11/2011
////////////////////////////////////////////////////////////////////////////////

#include <intrinsics.h>
#include "MegaLib\MegaLib.h"
#include "fat_filelib\fat_filelib.h"
#include "fat_filelib\fat_format.h"
#include "DriveController.h"
#include "DriveControllerParams.h"
#include "ControllerDiagnostic.h"
#include "DriveControllerSwitch.h"
#include "PortScanner.h"
#include "KeyboardScanner.h"
#include "Menu.h"
#include "Desctop.h"
#include "DesctopStack.h"
#include "SideIndicators.h"
#include <stdio.h>
#include "Config.h"
#include "DriveEventsGenerator.h"
#include "OscGetter.h"

unsigned short a;

// ��������� ����� �� ������� ����������� �������
template<class DataType>
struct DisplayInterfacePortStruct
{
	DataType Data 			: 8;
	DataType A0				: 1;
	DataType CS				: 1;
	DataType RW				: 1;
	DataType E				: 1;
	DataType Reserved0		: 1;
	DataType Reset			: 1;
	DataType DataReadWrite	: 1;	// 0 - Data �� ����, 1 - Data �� �����
	DataType Reserved1		: 1;
	
	typedef DataType InternalType;
};

/*
// ��������� ����� �� ������� ����������� �������
template<class DataType>
struct DisplayInterfacePortStruct
{
	DataType Data 			: 8;
	DataType A0				: 1;
	DataType CS				: 1;
	DataType RW				: 1;
	DataType E				: 1;
	DataType Reset			: 1;
	DataType Reserved		: 3;
	
	typedef DataType InternalType;
};
*/

typedef Bus6800< Gpio::_E, DisplayInterfacePortStruct<unsigned short> > LcdDataBus;

typedef ScreenRA8835<LcdDataBus> Lcd;

ScreenBuffer<Lcd, 320 / 8, 240 * 2> lcd;

DrawContext context;

FbgFont generalFont(GOSTtypeB8x16);
FbgFont menuBigFont(GOSTtypeB16x32);

Display display;

typedef Usart::_C MainComPort;
typedef Usart::_B ComPort485;
typedef UsbHardware::_A USB;

unsigned char buf[512];

typedef ModBusMasterStateMachine<MainComPort, 256> 		ModBusState;
typedef ModBusSlaveStateMachine<ComPort485, 256> 		ModBusSlaveState;
typedef ModBusSlave<ModBusSlaveState>					ModBus485Slave;
typedef DriveController<ModBusState, 0x01, 0x02> 	PrimaryController;
typedef DriveController<ModBusState, 0x02, 0x01> 	ReserveController;
typedef ControllerDiagnostic<ModBusState, 0x01, 1, 17>	MainControllerDiagnostic;
typedef ControllerDiagnostic<ModBusState, 0x02, 1, 17>	ReservControllerDiagnostic;
typedef ControllerDiagnostic<ModBusState, 0x03, 1, 17>	ControllerDiagnosticTemperature;

DriveControllerInterface<PrimaryController, true>	PrimaryControllerInterface;
DriveControllerInterface<ReserveController, false>	ReserveControllerInterface;

typedef DriveControllerParams<DriveControllerInterfaceBase, Config> ActiveDriveControllerParams;

typedef DriveControllerSwitch<Gpio::_A, 4, 5> ControllerSwitch;

typedef OscGetter<ActiveDriveControllerParams, 13> OscGet;

//����� ��������
class Empty
{
public:
	static bool Run()
	{
		return true;
	}
};

typedef PortScanner<PrimaryController, ReserveController, MainControllerDiagnostic, ReservControllerDiagnostic, ControllerDiagnosticTemperature> portScanner;
/*
typedef Max6951<SoftwareSpi<Gpio::_C, 10, Gpio::_C, 12, Gpio::_C, 11, Gpio::_C, 7, false, true> > StatorDisplay;
typedef Max6951<SoftwareSpi<Gpio::_C, 10, Gpio::_C, 12, Gpio::_C, 11, Gpio::_C, 6, false, true> > RotorDisplay;
typedef MMC<SoftwareSpi<Gpio::_C, 10, Gpio::_C, 12, Gpio::_C, 11, Gpio::_D, 0, false, true> > FlashType;
*/

typedef Max6951<SpiWrapper<Spi::_C, Gpio::_C, 7, false, false> > StatorDisplay;
typedef Max6951<SpiWrapper<Spi::_C, Gpio::_C, 6, false, false> > RotorDisplay;
typedef MMC<SpiWrapper<Spi::_C, Gpio::_D, 0, false, false> > FlashType;


SideIndicators<StatorDisplay, RotorDisplay, ActiveDriveControllerParams> StatorRotorIndicators;

FlashType Flash;

typedef KeyboardScanner<Gpio::_D, 4, 3, 2, 1, 15, 14, 13, 12> keyboardScanner;

typedef EventLog<32, 13> Events;
typedef DriveEventsGenerator<ActiveDriveControllerParams, Events> DriveEvets;

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
MenuItem menuMainScreen("�����", ShowWindow<wndIdMain>);
MenuItem menuHideAll("������� ��� ����", ShowWindow<wndIdDesctop>);

MenuItem menuSetupDateTime("���� / �����", ShowWindow<wndIdConfigSystemDateTime>);
MenuItem menuSetupModbus("Modbus", &menuListModbusSetup);
MenuItem menuResetOperatingTime("�������� ������� ���������              ", ShowWindow<wndIdResetOperatingTime>);
MenuItem menuSetupIP("IP �����");
MenuItem menuSetupNetwork("������� �����");

MenuItem menuDiagnosticMain("��������", ShowWindow<wndIdDiagnosticMain>);
MenuItem menuDiagnosticReserv("���������", ShowWindow<wndIdDiagnosticReserv>);

//MenuItem menuControllerNominalStatorCurrent("����������� ��� �������", ShowWindow<wndIdControllerNominalStatorCurrent>, GetEditingValueString<wndIdControllerNominalStatorCurrent>);
//MenuItem menuControllerRotorCurrentMax("I ������ ������������ (�)", ShowWindow<wndIdControllerRotorCurrentMax>, GetEditingValueString<wndIdControllerRotorCurrentMax>);

MenuItem menuControllerSplitterDrive("----------��������� ���������----------");
MenuItem menuControllerSplitterProtections("------------��������� �����------------");
MenuItem menuControllerSplitterController("----------��������� ���������----------");


int Getter()
{
	return 100;
}

void Setter(float value)
{

}

WindowMain<Display, display, ActiveDriveControllerParams, Events> wndMain;
WindowDebugRegisters<Display, display, PrimaryController, ReserveController, MainControllerDiagnostic, ReservControllerDiagnostic> wndDebugRegisters;
WindowEvents<Display, display, Events, Event> wndEvents;
WindowConfigSystemDateTime<Display, display> wndConfigSystemDateTime(&menuBigFont);
WindowResetOperatingTime<Display, display, ActiveDriveControllerParams> wndResetOperatingTime(&menuBigFont);
WindowGraph<Display, display, DrawContext, &context> wndGraph;
WindowOscList<Display, display, OscGet, OscFileFormat::OscFileDescription, DesctopType, &desctop, WindowGraph<Display, display, DrawContext, &context>, &wndGraph, wndIdGraph> wndOscList;
WindowDiagnosticRegulator<Display, display, MainControllerDiagnostic, true> wndDiagnosticRegulatorMain;
WindowDiagnosticRegulator<Display, display, ReservControllerDiagnostic, false> wndDiagnosticRegulatorReserv;

//WindowControllerNominalStatorCurrent<Display, display, ActiveDriveControllerParams> wndControllerNominalStatorCurrent(&menuBigFont);
/*
WindowEditRegValue<Display, display, float, ControlTextInput<3>,
	ActiveDriveControllerParams::GetNominalStatorCurrent,
	ActiveDriveControllerParams::SetNominalStatorCurrent >
		wndControllerNominalStatorCurrent(&menuBigFont, "��������� ����������", "����������� ��� �������", "%3.0f");
*/
/*
WindowEditRegValue<Display, display, float, ControlTextInput<3>,
	ActiveDriveControllerParams::GetRotorCurrentMax,
	ActiveDriveControllerParams::SetRotorCurrentMax >
		wndControllerRotorCurrentMax(&menuBigFont, "��������� ����������", "I ������ ������������ (�)", "%3.0f");
*/


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
	context.SecondBufferEnable(false);
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
	menuListDiagnostic.Add(&menuDebugRegisters);
		
	mainMenu.SetRoot(&menuListRoot);
}

void DesctopInit()
{
	wndGraph.SetOnClose(OnWindowGraphClose);
	wndDebugRegisters.SetOnClose(OnWindowClose);
	wndEvents.SetOnClose(OnWindowClose);
	wndConfigSystemDateTime.SetOnClose(OnWindowClose);
	wndResetOperatingTime.SetOnClose(OnWindowClose);
	wndOscList.SetOnClose(OnWindowClose);
    
	wndDiagnosticRegulatorMain.SetOnClose(OnWindowClose);
	wndDiagnosticRegulatorReserv.SetOnClose(OnWindowClose);
	
	desctop.RegisterWindow(&wndMain, wndIdMain);
	desctop.RegisterWindow(&wndOscList, wndIdOscList);
	desctop.RegisterWindow(&wndGraph, wndIdGraph);
	desctop.RegisterWindow(&wndDebugRegisters, wndIdDebugRegisters);
	desctop.RegisterWindow(&wndEvents, wndIdEvents);
	desctop.RegisterWindow(&wndConfigSystemDateTime, wndIdConfigSystemDateTime);
	desctop.RegisterWindow(&wndResetOperatingTime, wndIdResetOperatingTime);
	desctop.RegisterWindow(&wndDiagnosticRegulatorMain, wndIdDiagnosticMain);
	desctop.RegisterWindow(&wndDiagnosticRegulatorReserv, wndIdDiagnosticReserv);
    
	//wndControllerNominalStatorCurrent.SetOnClose(OnWindowClose);
	//desctop.RegisterWindow(&wndControllerNominalStatorCurrent, wndIdControllerNominalStatorCurrent);
	
	desctop.ShowWindow(wndIdMain);
}

CONTROLLER_PARAM_DECL(NominalStatorCurrent, " I�� �����������, �  ", float, 3, 0, "%3.0f", 1)
CONTROLLER_PARAM_DECL(StartStatorCurrent, " I�� ��������, �     ", float, 4, 0, "%3.0f", 1)
CONTROLLER_PARAM_DECL(NominalRotorCurrent, " I� �����������, �   ", float, 3, 0, "%3.0f", 1)
CONTROLLER_PARAM_DECL(StartRotorCurrent, " I� ��������, �      ", float, 4, 0, "%3.0f", 1)
CONTROLLER_PARAM_DECL(RotorCurrentMax, " I� ����, �          ", float, 3, 1, "%3.1f", 1)
CONTROLLER_PARAM_DECL(RotorCurrentMin, " I� ���, �           ", float, 3, 1, "%3.1f", 1)
CONTROLLER_PARAM_DECL(CosChannel, "����� �� Cos         ", int, 0, 0, "%s", 1)
CONTROLLER_PARAM_COS_DECL(AngleSetupDisplay, "������� �� cos�      ", float, 2, 2, "%2.2f", 1)
CONTROLLER_PARAM_DECL(EnergizingCurrentSetup, "������� �� I������, �", float, 3, 1, "%3.1f", 1)
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
	CONTROLLER_PARAM_INIT(AngleSetupDisplay);
	CONTROLLER_PARAM_INIT(EnergizingCurrentSetup);
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
    menuListControllerSetup.Add(&menuControllerSplitterController);
    CONTROLLER_PARAM_INIT(StartForsingTime);
    CONTROLLER_PARAM_INIT(StatorVoltageMaxDisplay);
    CONTROLLER_PARAM_INIT(StatorCurrentMaxDisplay);
    CONTROLLER_PARAM_INIT(RotorVoltageMaxDisplay);
    CONTROLLER_PARAM_INIT(RotorCurrentMaxDisplay);
	
    CONTROLLER_PARAM_UST_INIT(CosChannel);
	CONTROLLER_PARAM_UST_INIT(AngleSetupDisplay);
	CONTROLLER_PARAM_UST_INIT(EnergizingCurrentSetup);
    
	INDICATION_PARAM_INIT(StatorVoltageMinDisplay);
	INDICATION_PARAM_INIT(StatorCurrentMinDisplay);
	INDICATION_PARAM_INIT(RotorVoltageMinDisplay);
	INDICATION_PARAM_INIT(RotorCurrentMinDisplay);
	
	MODBUS_PARAM_INIT(SlaveAddr);
	MODBUS_PARAM_INIT(Boudrate);
	MODBUS_PARAM_INIT(Parity);
	MODBUS_PARAM_INIT(ParityType);
}

char lastKey = 0;

void OnKeyDownCallback(char key)
{
	lastKey = key;
}

// ������������ ������
void TenKiloHertz()
{
	ModBusState::PacketTimeoutDetectorTick();
	ModBusSlaveState::PacketTimeoutDetectorTick();
}

// ������������ ������ 2
void OneKiloHertz2()
{
	static unsigned int portScannerCnt = 0;
	static unsigned int oscGetterCnt = 0;
	static unsigned int diagnosticCnt = 0;
	
	if (portScannerCnt >= 10)
	{
		ModBusState::Run();
		
		
		if (ControllerSwitch::IsPrimaryActive() || true)
		{
			portScanner::SkipPrimary = false;
			if (!portScanner::SkipReserve)
			{
				portScanner::SkipReserve = portScanner::RunCountReserve > 7;
			}
			else
			{
				portScanner::RunCountReserve = 0;
			}
		}
		
		if (ControllerSwitch::IsReserveActive())
		{
			portScanner::SkipPrimary = true;
			portScanner::SkipReserve = false;
			
			if (!portScanner::SkipPrimary)
			{
				portScanner::SkipPrimary = portScanner::RunCountPrimary > 7;
			}
			else
			{
				portScanner::RunCountPrimary = 0;
			}
		}
		
		portScanner::SkipDiagnostic = diagnosticCnt > 0;
		if (diagnosticCnt < 100)
		{
			if (portScanner::RunCountDiagnostic > 0)
			{
				diagnosticCnt++;
			}
		}
		else
		{
			portScanner::RunCountDiagnostic = 0;
			diagnosticCnt = 0;
		}
		
		portScanner::Run();
		portScannerCnt = 0;
	}
	portScannerCnt++;
	/* // ����� ������ ��� ������, �.�. ��������� �������� �������������� ��������� � ������
	if (oscGetterCnt >= 100)
	{
		OscGet::Run();
	}
	oscGetterCnt++;
	*/
	
	keyboardScanner::Tick();
	display.Tick();
	ActiveDriveControllerParams::Tick();
	ModBusSlaveState::Run();
	/*
	Gpio::_D::SetOutputPin(1);
	Gpio::_D::SetBit(1);
	Gpio::_D::SetBit(1);
	Gpio::_D::SetBit(1);
	Gpio::_D::ClearBit(1);
	*/
}

char testKeys[] = {13, '-', '-', 13, 13, '1', '6', '0', '1', '2', '0', '1', '2', '0', '1', '0', '2', '0', '3', 13, 13, '+', '+', 'E'};
unsigned char nextTestKey = 0;

int media_read(unsigned long sector, unsigned char *buffer, unsigned long sectorsCount)
{
	if (sectorsCount > 1)
		return 0;
	unsigned long long addr = sector * FAT_SECTOR_SIZE;
	if (!Flash.ReadBlock(addr, buffer, FAT_SECTOR_SIZE))
		return 0;
	return 1;
}

int media_write(unsigned long sector, unsigned char *buffer, unsigned long sectorsCount)
{
	if (sectorsCount > 1)
		return 0;
	unsigned long long addr = sector * FAT_SECTOR_SIZE;
	if (!Flash.WriteBlock(addr, buffer, FAT_SECTOR_SIZE))
		return 0;
	return 1;
}

int putchar(int c)
{
  return c;
}

enum FatState
{
	FatStateInit = 0,
	FatStateError,
    FatStateWaitGuard,
	FatStateReady
};

FatState fatState = FatStateInit;

FL_FILE *eventsFile = 0;
FL_FILE *controllerFile = 0;
FL_FILE *oscFile = 0;

struct fatfs		fs_formatting;

bool EventsRead(long int offset, int origin, unsigned char *data, unsigned int count)
{
	if (fatState != FatStateReady)
		return false;
	
	eventsFile = (FL_FILE*)fl_fopen("/events.bin", "rw");
	
	if (!eventsFile)
		return false;
	
	bool res = fl_fseek(eventsFile, offset, origin) == 0;
	if (res)
	{
		res = fl_fread(data, count, 1, eventsFile) == count;
		if (!res && count > 0)
		{
			fatState = FatStateInit;
		}
	}

	fl_fclose(eventsFile);
	eventsFile = 0;
	return res;
}

bool EventsWrite(long int offset, int origin, unsigned char *data, unsigned int count)
{
	if (fatState != FatStateReady)
		return false;
	
	eventsFile = (FL_FILE*)fl_fopen("/events.bin", "rw");
	
	if (!eventsFile)
		return false;
	
	bool res = fl_fseek(eventsFile, offset, origin) == 0;
	if (res)
	{
		res = fl_fwrite(data, count, 1, eventsFile) == count;
	}

	fl_fclose(eventsFile);
	eventsFile = 0;
	return res;
}

bool EventsSeek(long int offset, int origin, unsigned long &pos)
{
	if (fatState != FatStateReady)
		return false;
    
    eventsFile = (FL_FILE*)fl_fopen("/events.bin", "rw");
	
	if (!eventsFile)
		return false;
	bool res = fl_fseek(eventsFile, offset, origin) == 0;
	unsigned long p = 0;
	fl_fgetpos(eventsFile, &p);
	pos = p;
	fl_fclose(eventsFile);
	eventsFile = 0;
	return res;
}

bool ControllerFilesRead(const char *fileName, long int offset, int origin, unsigned char *data, unsigned int count)
{
	if (fatState != FatStateReady)
		return false;
	
	controllerFile = (FL_FILE*)fl_fopen(fileName, "rw");
	
	if (!controllerFile)
		return false;
	
	bool res = fl_fseek(controllerFile, offset, origin) == 0;
	if (res)
		res = fl_fread(data, count, 1, controllerFile) == count;

	fl_fclose(controllerFile);
	controllerFile = 0;
	return res;
}

bool ControllerFilesWrite(const char *fileName, long int offset, int origin, unsigned char *data, unsigned int count)
{
	if (fatState != FatStateReady)
		return false;
	
	controllerFile = (FL_FILE*)fl_fopen(fileName, "rw");
	
	if (!controllerFile)
		return false;
	
	bool res = fl_fseek(controllerFile, offset, origin) == 0;
	if (res)
		res = fl_fwrite(data, count, 1, controllerFile) == count;

	fl_fclose(controllerFile);
	controllerFile = 0;
	return res;
}

bool OscFilesRead(const char *fileName, long int offset, int origin, unsigned char *data, unsigned int count)
{
	if (fatState != FatStateReady)
		return false;
	
	oscFile = (FL_FILE*)fl_fopen(fileName, "rw");
	
	if (!oscFile)
		return false;
	
	bool res = fl_fseek(oscFile, offset, origin) == 0;
	if (res)
		res = fl_fread(data, count, 1, oscFile) == count;

	fl_fclose(oscFile);
	oscFile = 0;
	return res;
}

bool OscFilesWrite(const char *fileName, long int offset, int origin, unsigned char *data, unsigned int count)
{
	if (fatState != FatStateReady)
		return false;
	
	oscFile = (FL_FILE*)fl_fopen(fileName, "rw");
	
	if (!oscFile)
	{
		fl_createdirectory("/osc");
		
		oscFile = (FL_FILE*)fl_fopen(fileName, "rw");
	}
	
	if (!oscFile)
		return false;
	
	bool res = fl_fseek(oscFile, offset, origin) == 0;
	if (res)
		res = fl_fwrite(data, count, 1, oscFile) == count;

	fl_fclose(oscFile);
	oscFile = 0;
	return res;
}

void ExecuteOscRun()
{
	for (unsigned short i = 0; i < 100; i++)
	{
		if (OscGet::Run())
		{
			break;
		}
	}
}

void TxEnable485(bool txEnable)
{
	if (txEnable)
	{
		Gpio::_D::SetBit(7);
	}
	else
	{
		Gpio::_D::ClearBit(7);
	}
}

unsigned long FatStateWaitGuardTimeout = 0;

bool GetRegValues(unsigned char *buffer, unsigned int bufferLen, unsigned short firstAddr, unsigned short quantity)
{
	bool res = false;
	/*
	if (firstAddr >= 200 && (firstAddr + quantity - 1) < 201 )
	{
		// �������� ��������
		// 10 - � ������� �� 200-�
		res = InsulationController::GetRegValues(buffer, bufferLen, firstAddr + 10, quantity);
	}
	else*/
	{
		// �������� ���������
		res = ActiveDriveControllerParams::GetRegValues(buffer, bufferLen, firstAddr, quantity);
	}
	
	return res;
}

void DiagnosticRs485TxEnable(bool enable)
{
	if (enable)
	{
		Gpio::_B::SetBit(7);
	}
	else
	{
		Gpio::_B::ClearBit(7);
	}
}

int main()
{
	ResetAndClockControl::ExternalHightSpeedClockEnable();
	unsigned int waitTimeOut = 1000;
	while(! ResetAndClockControl::IsExternalHightSpeedClockReady() && waitTimeOut > 0)
	{
		waitTimeOut--;
	}
	
	ResetAndClockControl::SetPllSource(1);
	ResetAndClockControl::SetPllMultiplicationFactor(2); // mul6
	ResetAndClockControl::PllEnable();
	
	if (ResetAndClockControl::IsPllReady())
	{
		ResetAndClockControl::SetPllSystemClock();
	}
	/* ���� �� ������� USB
	// PLL 48��� ��� USB
	ResetAndClockControl::SetPllMultiplicationFactor(0x0A);
	ResetAndClockControl::PllEnable();
	// �.�. PLL 48��� �� ��� USB PLLVCO / 2
	ResetAndClockControl::SetUsbOtgFsPrescaler(1);
	*/
	ResetAndClockControl::EnableAlternateFunction();
	ResetAndClockControl::EnableGpioPortA();
	ResetAndClockControl::EnableGpioPortB();
	ResetAndClockControl::EnableGpioPortC();
	ResetAndClockControl::EnableGpioPortD();
	ResetAndClockControl::EnableGpioPortE();
	ResetAndClockControl::EnableUsart2();
	ResetAndClockControl::EnableUsart3();
	ResetAndClockControl::EnableSpi3();
	/* ���� �� ������� USB
	ResetAndClockControl::EnableUsb();
	*/
	InterruptController::InterruptEnable(InterruptIdUsart2);
	InterruptController::InterruptEnable(InterruptIdUsart3);
	
	ResetAndClockControl::RtcInit();
	
	// ��� �����
	Gpio::_A::SetAlternateOutputPin(8);
	ResetAndClockControl::SetClockOutput(4);
	
	//���������� 4
	Gpio::_D::SetOutputPin(1);
	
	// DEC0, DEC1
	Gpio::_D::SetOutputPin(10);
	Gpio::_D::SetOutputPin(11);
	
	//Gpio::_D::SetBit(10);
	
	// USART2
	Gpio::_D::SetAlternateOutputPin(5);
	Afio::RemapUsart2(1);
	// 485TXEnable
	Gpio::_D::SetOutputPin(7);
	
	// USART3
	Gpio::_D::SetAlternateOutputPin(8);
	Afio::RemapUsart3(3);
	// div = f / 16 / br;
	// 25��� / 16 / 115200 = 13,56
	// Mantissa = 13
	// Fraction = 0.56 * 16 = 9.01
	
	// 8��� / 16 / 115200 = 4,34
	// Mantissa = 4
	// Fraction = 0.34 * 16 = 5
	
	//UsartHardware::_C::SetBaudRate(13, 9);
	//UsartHardware::_C::Enable();
	//UsartHardware::_C::RxInterruptEnable();
	
	MainComPort::Init(Config::CoreFrequency, 9600);
	
	// afio clock enable
	// usart3 full remap 
	
	//PD8 afio out
	//PD9 input
	
	// usart
	// div mantissa = D (13)
	// div fraction = 9 (0.563 * 16)
	// UE = 1
	// RE = 1
	// TE = 1__enable_irq ();
	
	// SPI3
	// ��� as alternate output
	Gpio::_C::SetAlternateOutputPin(10);
	// MOSI as alternate output
	Gpio::_C::SetAlternateOutputPin(12);
	Afio::RemapSpi3(1);
	
	// USB Setup
	/* ���� �� ������� USB
	Gpio::_A::SetInputPin(9); 	// VBUS
	// Gpio::_A::SetInputPin(10);	// DM (����� �� ����������, USB ������ ��������� ������������ ���)
	// Gpio::_A::SetInputPin(12);	// DP (����� �� ����������, USB ������ ��������� ������������ ���)
	Gpio::_C::SetInputPin(8); 	// USB_OverCurrent ���� ������ �������, �� ����������
	Gpio::_C::SetOutputPin(9); 	// USB_Power_ON ���������� ����
	
	USB::Init();
	
	InterruptController::InterruptEnable(InterruptIdUsb);
	*/
	
	// Diagnostic Rs485 TxEnable
	Gpio::_B::SetOutputPin(7);
	
	context.Init(&lcd);
	context.SelectFont(&generalFont);
	
	display.SelectContext(&context);
	
	MenuInit();
	DesctopInit();
	ControllerParamsMenuEndEditorsInit();
	
	//display.MoveCursorTo((40 - 18) / 2, 104);
	//display.WriteLine("������, ���������!", 18);
	//display.MoveCursorTo((40 - 9) / 2, 120);
	//display.WriteLine("��� ����?", 9);
	
	__enable_interrupt();
	
	ModBusState::Init();
	ModBusSlaveState::Init();
	ModBusSlaveState::SetTxEnableCallback(TxEnable485);
	ModBusSlaveState::SetBuildResponseCallback(ModBus485Slave::OnRequest);
	// �� ��������� 4800
	ModBusSlaveState::ChangeSpeed(Config::CoreFrequency, 4800);
	
	ModBus485Slave::GetRegValuesCallback = GetRegValues;
	
	ActiveDriveControllerParams::SetActiveController(&PrimaryControllerInterface);
	ControllerSwitch::Init();
	
	MainControllerDiagnostic::TxEnableCallback = DiagnosticRs485TxEnable;
	ReservControllerDiagnostic::TxEnableCallback = DiagnosticRs485TxEnable;
	ControllerDiagnosticTemperature::TxEnableCallback = DiagnosticRs485TxEnable;
	
	keyboardScanner::Init();
	keyboardScanner::SetOnKeyDown(OnKeyDownCallback);
	
	// ������ ������������ ������
	ResetAndClockControl::EnableTimer6();
	InterruptController::InterruptEnable(InterruptIdTimer6);
	//Timer::_G::SetPrescaler(780);
	//Timer::_G::SetMaxCount(31);
	Timer::_G::SetPrescaler(Config::CoreFrequency / 1000000 - 1);
	Timer::_G::SetMaxCount(100 - 1);
	Timer::_G::SetInterruptHandler(TenKiloHertz);
	Timer::_G::InterruptEnable();
	Timer::_G::Start();
	
	// ������������ ������
	ResetAndClockControl::EnableTimer7();
	InterruptController::InterruptEnable(InterruptIdTimer7);
	Timer::_H::SetPrescaler(Config::CoreFrequency / 1000000 - 1);
	Timer::_H::SetMaxCount(1000 - 1);
	Timer::_H::SetInterruptHandler(OneKiloHertz2);
	Timer::_H::InterruptEnable();
	Timer::_H::Start();
	
	StatorRotorIndicators.Init();
	
	//RotorDisplay::Init();
	//RotorDisplay::Test();
	//RotorDisplay::Enable();
	
	Flash.Init();
	fl_init();
	
	Events::Init(EventsRead, EventsWrite, EventsSeek);
	PrimaryController::Init();
	ReserveController::Init();
	ActiveDriveControllerParams::Init(ControllerFilesRead, ControllerFilesWrite);
	
	OscGet::Init(OscFilesRead, OscFilesWrite);
	/*
	if(Flash.Reset())
	{
		Flash.ReadCID();
		Flash.ReadCSD();
		Flash.GetBlockSize();
		Flash.GetBlockCount();
		Flash.GetSize();
	}*/
	
	// ������ ������
	Event e(Rtc::GetTime(), EventProgramStart);
	Events::Push(e);
	
	while(1)
	{
		/* ���� �� ������� USB
		USB::Run();
		*/

		if (ControllerSwitch::IsPrimaryActive())
		{
			ActiveDriveControllerParams::SetActiveController(&PrimaryControllerInterface);
		}
		else
		{
			PrimaryControllerInterface.SetDoOnlyLowerRegsRequest(false);
		}
		
		if (ControllerSwitch::IsReserveActive())
		{
			ActiveDriveControllerParams::SetActiveController(&ReserveControllerInterface);
		}
		else
		{
			ReserveControllerInterface.SetDoOnlyLowerRegsRequest(false);
		}
		
		DriveEvets::Run();
		
		ExecuteOscRun();
		
		if (lastKey)
		{
			if (mainMenu.Visible())
			{
				mainMenu.OnKeyDown(lastKey);
				if ('E' == lastKey)
				{
					if (!mainMenu.Visible())
						desctop.ShowWindow(wndIdMain);
				}
			}
			else
			{
				if (!desctop.OnKeyDown(lastKey))
				{
					if (desctop.IsWindowVisible(wndIdMain))
					{
						if (13 == lastKey)
							mainMenu.Show();
					}
					else
					{
						mainMenu.OnKeyDown(lastKey);
					}
				}
			}
			lastKey = 0;
		}
		if (mainMenu.Visible())
		{
			mainMenu.Draw();
		}
		else
		{
			//DrawDebugRegistersScreen();
			//DrawMainScreen();
			desctop.Draw();
		}
		
		ExecuteOscRun();
		
		StatorRotorIndicators.Update();
		Flash.Run();
		switch(fatState)
		{
		case FatStateInit:
			{
				if (Flash.Ready())
				{
					if (fl_attach_media(&media_read, media_write) != FAT_INIT_OK)
					{
						/// ���� ���������� ����� ���������������
						/*
						fatfs_init(&fs_formatting);
						fs_formatting.disk_io.read_media = media_read;
						fs_formatting.disk_io.write_media = media_write;
						fatfs_format_fat16(&fs_formatting, 0x10000, "123");
						fatState = FatStateError;
						*/
					}
					else
					{
						//fl_listdirectory("/");
                        //FatStateWaitGuardTimeout = 10;
                        //fatState = FatStateWaitGuard;
                        fatState = FatStateReady;
					}
				}
			}
			break;
        // �� ��������
        case FatStateWaitGuard:
            {
                if (FatStateWaitGuardTimeout > 0)
                {
                    FatStateWaitGuardTimeout--;
                }
                else
                {
                    fatState = FatStateReady;
                }
                if (!Flash.Ready())
				{
					fatState = FatStateInit;
				}
            }
            break;
		case FatStateError:
		case FatStateReady:
			{
				if (!Flash.Ready())
				{
					fatState = FatStateInit;
				}
			}
			break;
		}
		
		ExecuteOscRun();
		
		Events::Run();
		ActiveDriveControllerParams::Run();
		
		if (ModBus485Slave::MyAddress != ActiveDriveControllerParams::GetModbusParamValue_int<ActiveDriveControllerParams::ModbusParamSlaveAddr>())
		{
			ModBus485Slave::MyAddress = ActiveDriveControllerParams::GetModbusParamValue_int<ActiveDriveControllerParams::ModbusParamSlaveAddr>();
		}
		
		if (ModBusSlaveState::CurrentBoudrate != ActiveDriveControllerParams::GetModbusParamValue_int<ActiveDriveControllerParams::ModbusParamBoudrate>())
		{
			ModBusSlaveState::ChangeSpeed(Config::CoreFrequency, ActiveDriveControllerParams::GetModbusParamValue_int<ActiveDriveControllerParams::ModbusParamBoudrate>());
		}
		
		if ((ModBusSlaveState::CurrentParityEnabled != ActiveDriveControllerParams::GetModbusParamValue_int<ActiveDriveControllerParams::ModbusParamParity>()) ||
			(ModBusSlaveState::CurrentParityEven != ActiveDriveControllerParams::GetModbusParamValue_int<ActiveDriveControllerParams::ModbusParamParityType>()))
		{
			ModBusSlaveState::ChangeParity(ActiveDriveControllerParams::GetModbusParamValue_int<ActiveDriveControllerParams::ModbusParamParity>(),
										   ActiveDriveControllerParams::GetModbusParamValue_int<ActiveDriveControllerParams::ModbusParamParityType>());
		}
		
		ExecuteOscRun();
		/*
		if (ModBusState::IsReady())
		{
			unsigned int readed = 0;
			unsigned char *tmp = ModBusState::GetResponse(readed);
			
			for (int i = 0; i < readed; i++)
				buf[i] = tmp[i];
			
			if (readed > 0)
			{
				readed++;
			}
			
			unsigned int requestSize = ModBusMaster::BuildReadHoldingRegisters(buf, 2, 1, 9);
			ModBusState::SendRequest(buf, requestSize);
		}
		*/
		
	}
}
