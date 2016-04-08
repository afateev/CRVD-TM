///////////////////////////////////////////////////////////////////////////////
//	Классы управления разными частями контроллера флэш памяти
//	MSP430X1XXX
//
//	Разные наборы частей разделены по разным пространствам имён по платформам
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef FLASHMEMORYCONTROLLER_PARTS_MSP430X1XXX_H
#define FLASHMEMORYCONTROLLER_PARTS_MSP430X1XXX_H

namespace Rblib
{

namespace Msp430x1xxx
{
	template<class RegisterDataType>
	struct FlashMemoryControl1Register
	{
		RegisterDataType Reserved0						: 1;
		RegisterDataType Erase							: 1;	// Флаг режима очистки
		RegisterDataType MassErase						: 1;	// Флаг режима массовой очистки
		RegisterDataType Reserved1						: 3;
		RegisterDataType Write							: 1;	// Флаг разрешения запись
		RegisterDataType BlockWrite						: 1;	// Флаг разрешения блочной записи
		RegisterDataType Password						: 8;	// Пароль доступа к регистру
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
		RegisterDataType Reserved0		: 2;	// Сбой при выполнении операции
		RegisterDataType Password			: 8;	// Пароль доступа к регистру
	};
	
}
}

#endif
