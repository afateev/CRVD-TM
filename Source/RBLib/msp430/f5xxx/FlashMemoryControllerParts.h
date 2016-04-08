///////////////////////////////////////////////////////////////////////////////
//	Классы управления разными частями контроллера флэш памяти
//	MSP430F5XXX
//
//	Разные наборы частей разделены по разным пространствам имён по платформам
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef FLASHMEMORYCONTROLLER_PARTS_MSP430F5XXX_H
#define FLASHMEMORYCONTROLLER_PARTS_MSP430F5XXX_H

namespace Rblib
{
// Части специфичные для платформы MSP430 всех семейств
namespace Msp430f5xxx
{
	template<class RegisterDataType>
	struct FlashMemoryControl1Register
	{
		RegisterDataType Reserved0			: 1;
		RegisterDataType Erase				: 1;	// Флаг режима очистки
		RegisterDataType MassErase			: 1;	// Флаг режима массовой очистки
		RegisterDataType Reserved1			: 2;
		RegisterDataType SmartWrite			: 1;	// Быстрая запись
		RegisterDataType Write				: 1;	// Флаг разрешения запись
		RegisterDataType BlockWrite			: 1;	// Флаг разрешения блочной записи
		RegisterDataType Password			: 8;	// Пароль доступа к регистру
	};
	
	template<class RegisterDataType>
	struct FlashMemoryControl3Register
	{
		RegisterDataType Busy				: 1;	// Занято
		RegisterDataType PasswordViolation	: 1;	// Неверный пароль
		RegisterDataType AccessViolationInt	: 1;	// Разрешить прерывание при ошибке доступа
		RegisterDataType Wait				: 1;	// Флэшка готова для записи
		RegisterDataType Lock				: 1;	// Запрет/разрешение записи
		RegisterDataType EmergencyExit		: 1;	// Аварийно завершить операцию записи
		RegisterDataType LockA				: 1;	// Блокировка сегмента A
		RegisterDataType Reserved1			: 1;
		RegisterDataType Password			: 8;	// Пароль доступа к регистру
	};
	
	template<class RegisterDataType>
	struct FlashMemoryControl4Register
	{
		RegisterDataType VoltageChangedError	: 1;	// Во время записи был скачек напряжения
		RegisterDataType Reserved0				: 3;
		RegisterDataType MarginalRead0			: 1;	// Режим чтения нулей при граничных условиях
		RegisterDataType MarginalRead1			: 1;	// Режим чтения единиц при граничных условиях
		RegisterDataType Reserved1				: 1;
		RegisterDataType LockInformationMemory	: 1;	// 
		RegisterDataType Password				: 8;	// Пароль доступа к регистру
	};
}
}

#endif