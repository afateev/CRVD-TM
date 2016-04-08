///////////////////////////////////////////////////////////////////////////////
//	Класс управления областью флэш памяти для хранения пользовательских данных
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef FLASH_H
#define FLASH_H

#ifdef MSP430
#include "FlashMemoryTmpSpace.h"
#include "InterruptMap.h"
#endif

namespace Rblib
{
#ifdef MSP430
	///////////////////////////////////////////////////////////////
	///	\brief Класс доступа к пользовательской области флэш памяти
	///	
	///	\tparam UserDataType	Тип пользовательских данных (может быть как простым типом, так и структурой)
	///	\tparam Offset			Смещение пользовательских данных относительно начала области пользовательских данных
	///
	///////////////////////////////////////////////////////////////
	template<class UserDataType, unsigned long Offset>
	class Flash
	{
	protected:
		// Класс чтения данных
		typedef FlashMemoryController MemoryReader;
		// Класс записи данных
		typedef FlashMemorySegmented<unsigned long, FlashMemoryController,
			// Временная область будет в последнем сегменте программной памяти
			FlashMemoryTmpSpace<unsigned long,
				FlashMemoryController::Main::Start + FlashMemoryController::Main::Size - FlashMemoryController::Main::SegmentSize,
				FlashMemoryController::Main::SegmentSize,
				FlashMemoryController
			>
		> MemoryWriter;
	public:
		///	Чтение данных
		/*!
			\return Экземпляр пользовательских данных, прочитанный из флэш.
		*/
		static inline UserDataType Read()
		{
			UserDataType userData;

			unsigned long userDataSize = sizeof(UserDataType);
			if (Offset >= FlashMemoryController::Info::Size)
				return UserDataType();
			if (Offset + userDataSize >= FlashMemoryController::Info::Size)
				return UserDataType();
			
			MemoryReader::Read(FlashMemoryController::Info::Start + Offset, (unsigned char *)&userData, userDataSize);
			return userData;
		}
		///	Запись данных
		/*!
			\param userData Экземпляр пользовательских данных, которые необходимо записать.
		*/
		static inline void Write(UserDataType userData)
		{
            InterruptMap::SaveInterruptsSateAndDisable();
			// проверяем границы
			unsigned long userDataSize = sizeof(UserDataType);
			if (Offset >= FlashMemoryController::Info::Size)
				return;
			if (Offset + userDataSize >= FlashMemoryController::Info::Size)
				return;

			MemoryWriter::Write(FlashMemoryController::Info::Start + Offset, (unsigned char *)&userData, userDataSize);
            InterruptMap::RestoreInterruptsSate();
		}
	};
#endif // MSP430
	
/*!
\class Flash
Класс позволяет получить доступ к области энергонезависимой
памяти контроллера предназанченной для хранения пользовательских данных.

При вызове операций чтения и записи контролируется выход
за границы отведенной области, что предотвращает порчу данных
за пределами этой области.

На различных платформах доступны различные области пользовательских данных:
<table>
<tr> <td>Платформа</td>		<td>Адрес</td> <td>Размер, байт</td>	</tr>
<tr> <td>Msp430f2618</td>	<td>0x1000</td>	<td>256</td>			</tr>
<tr> <td>Msp430f5435</td>	<td>0x1800</td>	<td>512</td>			</tr>
<tr> <td>Msp430f5438</td>	<td>0x1800</td>	<td>512</td>			</tr>
</table>

Для помещения пользовательских данных в энергонезависмую память и работы с ними достаточно:
-# Определить тип пользовательских данных (структуру данных) и указать его в качестве параметра шаблона \a UserDataType.
-# Задать с помощью параметра шаблона \a Offset конкретное место области флэш, где будут храниться данные определенного на шаге 1 типа.
-# Получитать экземпляр сохраненных во флэш памяти данных с помощью метода Read().
-# Сохранять экземпляр даных во флеш с помощью метода Write().

Пример использования:
\code
// структура данных хранимых в энергонезависимой памяти
struct DeviceParamsStruct
{
	unsigned char	One;
	unsigned int	Two;
	unsigned long	Four;
};
// класс доступа к данным в энергонезависимой памяти
typedef Flash<
	DeviceParamsStruct,	// структура данных
	0			// данные расположены в начале пользовательской области Flash
> DeviceParams;

int main( void )
{
	...
	// чтение текущих значений из флэш
	DeviceParamsStruct params = DeviceParams::Read();
	//изменение значений
	params.One++;
	params.Two++;
	params.Four++;
	// сохранение значений во флэш
	DeviceParams::Write(params);
	...
}
\endcode

Допускается работать с несколькими вариантами пользовательских данных хранящихся по разным смещениям используя различную типизацию шаблона:
- Flash<DataType1, 0> - данные типа 1 хранятся по смещению 0.
- Flash<DataType2, 5> - данные типа 2 хранятся по смещению 5.

*/

/*!
\page PageFlash Flash - Внутренняя энергонезависимая памать контроллера
	Флэш память контроллера обычно разделяют на область кода программы, область пользовательских данных и возможно другие специальные области (загрузчик, калибровочные данные и т.п.).
	<br>Класс Flash реализует функции по работе с \b пользовательской \b областью энергонезависмой памяти контроллера.

*/

}

#endif