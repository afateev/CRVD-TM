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

MenuItemList menuListRoot("Главное меню");
MenuItemList menuListSystemSetup("Настройка системных параметров");
MenuItemList menuListModbusSetup("Настройка Modbus");
MenuItemList menuListControllerSetup("Параметры регулятора");
MenuItemList menuListControllerUstSetup("Уставки регулирования");
MenuItemList menuListIndicationSetup("Параметры индикации");
MenuItemList menuListDiagnostic("Диагностика");

MenuItem menuViewWaveforms("Осциллограммы", ShowWindow<wndIdOscList>);
MenuItem menuViewEvents("Журнал событий", ShowWindow<wndIdEvents>);
MenuItem menuCosinusControlSetup("Включение / Отключение канала регулирования по CosФ", ShowWindow<wndIdControllerCosChannel>);
//MenuItem menuCosinusControlSetup("Включение / Отключение канала", 0);
MenuItem menuSystemSetup("Системные параметры", &menuListSystemSetup);
MenuItem menuControllerSetup("Параметры регулятора", &menuListControllerSetup);
MenuItem menuControllerUstSetup("Уставки регулятора", &menuListControllerUstSetup);
MenuItem menuIndicationSetup("Параметры индикации", &menuListIndicationSetup);
MenuItem menuDiagnostic("Диагностика", &menuListDiagnostic);

MenuItem menuGraph("Осциллограмма", ShowOsc);
MenuItem menuDebugRegisters("Регистры", ShowWindow<wndIdDebugRegisters>);
MenuItem menuDebugOsc("Осциллограммы", ShowWindow<wndIdDebugOsc>);
MenuItem menuMainScreen("Выход", ShowWindow<wndIdMain>);
MenuItem menuHideAll("Закрыть все окна", ShowWindow<wndIdDesctop>);

MenuItem menuSetupDateTime("Дата / время", ShowWindow<wndIdConfigSystemDateTime>);
MenuItem menuSetupModbus("Modbus", &menuListModbusSetup);
MenuItem menuResetOperatingTime("Сбросить счётчик наработки              ", ShowWindow<wndIdResetOperatingTime>);
MenuItem menuSetupIP("IP адрес");
MenuItem menuSetupNetwork("Сетевой обмен");

MenuItem menuDiagnosticMain("Основной", ShowWindow<wndIdDiagnosticMain>);
MenuItem menuDiagnosticReserv("Резервный", ShowWindow<wndIdDiagnosticReserv>);
//MenuItem menuDiagnosticTemperature("Температура", ShowWindow<wndIdDiagnosticTemperature>);
//MenuItem menuDiagnosticAnalogOutputs("Аналоговые выходы", ShowWindow<wndIdDiagnosticAnalogOutputs>);
//MenuItem menuDiagnosticLinkStatus("Состояние связи", ShowWindow<wndIdDiagnosticLinkStatus>);

//MenuItem menuControllerNominalStatorCurrent("Номинальный ток статора", ShowWindow<wndIdControllerNominalStatorCurrent>, GetEditingValueString<wndIdControllerNominalStatorCurrent>);
//MenuItem menuControllerRotorCurrentMax("I ротора максимальное (А)", ShowWindow<wndIdControllerRotorCurrentMax>, GetEditingValueString<wndIdControllerRotorCurrentMax>);

MenuItem menuControllerSplitterDrive("----------Параметры двигателя----------");
MenuItem menuControllerSplitterProtections("------------Параметры защит------------");
MenuItem menuControllerSplitterController("----------Параметры реглятора----------");

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
MenuItem menuControllerNominalStatorCurrent (" Iст номинальный, А  ", ShowWindow<wndIdControllerNominalStatorCurrent>, GetEditingValueString<wndIdControllerNominalStatorCurrent>);
WindowEditRegValue<Display, display, float, ControlParamInput<3, 0>,
		ActiveDriveControllerParams::GetRegValue_float<ActiveDriveControllerParams::RegNominalStatorCurrent>,
		ActiveDriveControllerParams::SetRegValue_float<ActiveDriveControllerParams::RegNominalStatorCurrent>,
        1>
			wndControllerNominalStatorCurrent(&menuBigFont, "Параметры регулятора", " Iст номинальный, А  ", "%3.0f");
*/

CONTROLLER_PARAM_DECL(NominalStatorCurrent, " Iст номинальный, А  ", float, 3, 0, "%3.0f", 1)
CONTROLLER_PARAM_DECL(StartStatorCurrent, " Iст пусковой, А     ", float, 4, 0, "%3.0f", 1)
CONTROLLER_PARAM_DECL(NominalRotorCurrent, " Iр номинальный, А   ", float, 3, 0, "%3.0f", 1)
CONTROLLER_PARAM_DECL(StartRotorCurrent, " Iр пусковой, А      ", float, 4, 0, "%3.0f", 1)
CONTROLLER_PARAM_DECL(RotorCurrentMax, " Iр макс, А          ", float, 3, 1, "%3.1f", 1)
CONTROLLER_PARAM_DECL(RotorCurrentMin, " Iр мин, А           ", float, 3, 1, "%3.1f", 1)
CONTROLLER_PARAM_DECL(CosChannel, "Канал регулирования                ", int, 1, 0, "%1d", 1)
CONTROLLER_PARAM_DECL(EnergizingCurrentSetup, " 1 - Уставка по Iротора, А", float, 3, 1, "%3.1f", 1)
CONTROLLER_PARAM_COS_DECL(AngleSetupDisplay, " 2 - Уставка по cosФ      ", float, 2, 2, "%2.2f", 1)
CONTROLLER_PARAM_DECL(EnergizingReactCurrentSetup, " 3 - Уставка по Iреакт, А", float, 4, 0, "%4.0f", 1)
CONTROLLER_PARAM_DECL(EnergizingReactPowerSetup, " 4 - Уставка по Qреакт, МВАр", float, 3, 1, "%3.1f", 1)
CONTROLLER_PARAM_DECL(GlideSetup, "Уставка по скольжению, Гц", float, 1, 1, "%1.1f", 1)
CONTROLLER_PARAM_DECL(GlideStart, "Пуск по скольжению   ", int, 0, 0, "%s", 1)
//CONTROLLER_PARAM_DECL(H_ka_f_I0, "H_ka.f_I0            ", float, 3, 0, "%3.0f")
//CONTROLLER_PARAM_DECL(H_ka_f_tg, "H_ka.f_tg            ", float, 3, 0, "%3.0f")
//CONTROLLER_PARAM_DECL(H_ka_f_v, "H_ka.f_v            ", float, 3, 0, "%3.0f")
CONTROLLER_PARAM_DECL(LowStatorVoltageProtection, "Защита от низкого Uст", int, 0, 0, "%s", 1)
CONTROLLER_PARAM_DECL(LowStatorVoltageProtectionThreshold, "Порог низкого U статора, В", float, 5, 1, "%5.1f", 1)
CONTROLLER_PARAM_DECL(LowStatorVoltageProtectionTimer, "Таймер защиты от низкого Uст, с", int, 3, 0, "%3d", 1000)
CONTROLLER_PARAM_DECL(LowStatorCurrentProtection, "Защита от низкого Iст", int, 0, 0, "%s", 1)
CONTROLLER_PARAM_DECL(LowStatorCurrentProtectionThreshold, "Порог низкого I статора, А", float, 3, 1, "%3.1f", 1)
CONTROLLER_PARAM_DECL(LowStatorCurrentProtectionTimer, "Таймер защиты от низкого Iст, с", int, 3, 0, "%3d", 1000)
CONTROLLER_PARAM_DECL(AsyncRotorRotationCount, "Поворотов ротора при щет.асин.ходе", int, 2, 0, "%3d", 1)
CONTROLLER_PARAM_DECL(AsyncCountdownTimer, "Время обр.отсчета при щет.асин.ходе", int, 3, 0, "%3d", 1000)
CONTROLLER_PARAM_DECL(AsyncProtectionTimer, "Таймер защиты при бесщет.асин.ходе", int, 3, 0, "%3d", 1000)
CONTROLLER_PARAM_DECL(EnergizingLostTimer, "Таймер защиты от потери возбуждения", int, 3, 0, "%3d", 1000)
CONTROLLER_PARAM_DECL(LongStartTimer, "Таймер защиты от затянувшегося пуска", int, 3, 0, "%3d", 1000)
CONTROLLER_PARAM_DECL(ShortCircuitProtectionThreshold, "Порог защиты от КЗ, А", float, 3, 0, "%3.0f", 1)
CONTROLLER_PARAM_DECL(ShortCircuitProtectionTimer, "Таймер защиты от КЗ, мс", int, 3, 0, "%3d", 1)
CONTROLLER_PARAM_DECL(OverloadProtectionThreshold, "Порог для перегрузки, А", float, 3, 0, "%3.0f", 1)
CONTROLLER_PARAM_DECL(RotorCoolingCurrent, "Ток остывания ротора, А", float, 3, 0, "%3.0f", 1)
CONTROLLER_PARAM_DECL(RotorCoolingTime, "Время остывания ротора, с", float, 3, 0, "%3.0f", 1000)
CONTROLLER_PARAM_DECL(HeatingTimeOnOverload0_10, "Время нагрева при перегр. < 10%, с", float, 3, 0, "%3.0f", 1000)
CONTROLLER_PARAM_DECL(HeatingTimeOnOverload10_20, "Время нагрева при перегр. 10-20%, с", float, 3, 0, "%3.0f", 1000)
CONTROLLER_PARAM_DECL(HeatingTimeOnOverload20, "Время нагрева при перегр. > 20%, с", float, 3, 0, "%3.0f", 1000)
CONTROLLER_PARAM_DECL(WaitAfterDriveOn, "Задержка после вкл. двигателя, с", int, 3, 0, "%3d", 1000)
CONTROLLER_PARAM_DECL(WaitAfterStatorCurrenLow, "Задержка после снижения Iст, мс", int, 6, 0, "%6d", 1)
CONTROLLER_PARAM_DECL(WaitAfterenergizingOn, "Задержка после вкл.возбуждения, с", int, 3, 0, "%3d", 1000)
CONTROLLER_PARAM_DECL(StartForsingTime, "Время стартовой форсировки, с", int, 3, 0, "%3.0d", 1000)
CONTROLLER_PARAM_DECL(StatorVoltageMaxDisplay, " Uст (максимум на приборах), В", int, 5, 0, "%5d", 1)
CONTROLLER_PARAM_DECL(StatorCurrentMaxDisplay, " Iст (максимум на приборах), А", int, 5, 0, "%5d", 1)
CONTROLLER_PARAM_DECL(RotorVoltageMaxDisplay, " Uр (максимум на приборах), В", int, 5, 0, "%5d", 1)
CONTROLLER_PARAM_DECL(RotorCurrentMaxDisplay, " Iр (максимум на приборах), А", int, 5, 0, "%5d", 1)

//CONTROLLER_PARAM_UST_DECL(CosChannel, "Канал по Cos         ", int, 0, 0, "%s", 1)
//CONTROLLER_PARAM_UST_COS_DECL(AngleSetupDisplay, "Уставка по cosФ      ", float, 2, 2, "%2.2f", 1)
//CONTROLLER_PARAM_UST_DECL(EnergizingCurrentSetup, "Уставка по Iротора, А", float, 3, 1, "%3.1f", 1)

INDICATION_PARAM_DECL(StatorVoltageMinDisplay, "Порог зануления Uст, kВ", float, 2, 2, "%2.2f", 1000)
INDICATION_PARAM_DECL(StatorCurrentMinDisplay, "Порог зануления Iст, A", int, 4, 0, "%4d", 1)
INDICATION_PARAM_DECL(RotorVoltageMinDisplay, "Порог зануления Uр, В", float, 3, 1, "%3.1f", 1)
INDICATION_PARAM_DECL(RotorCurrentMinDisplay, "Порог зануления Iр, A", float, 3, 1, "%3.1f", 1)

MODBUS_PARAM_DECL(SlaveAddr, "Адрес", int, 3, 0, "%3d", 1)
MODBUS_PARAM_DECL(Boudrate, "Скорость", int, 6, 0, "%6d", 1)
MODBUS_PARAM_DECL(Parity, "Четность", int, 0, 0, "%s", 1)
MODBUS_PARAM_DECL(ParityType, "Четность (тип)", int, 0, 1, "%s", 1)


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