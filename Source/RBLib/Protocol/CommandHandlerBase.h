///////////////////////////////////////////////////////////////////////////////
//	Класс выбора базового обработчика команд в зависимости от протокола
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef COMMANDHANDLERBASE_H
#define COMMANDHANDLERBASE_H

#include "..\Communication\InterfaceBase.h"
#include "..\Utils\Functor.h"
#include "..\NullType.h"

namespace Rblib
{
namespace Protocol
{
	/// Тип внешнего обработчика команд.
	typedef BasicFunctor<MakeTypeList<CmdInfo *, Communication::InterfaceBase *>::Result> ExternalCommandHandlerFunctor;

	/// \brief Базовый класс внешнего обработчика команд. 
	/// \details От этого класса него необходимо унаследоваться и пердать себя в конструктор, 
	/// чтобы можно было обрабатывать команды.
	/// \tparam T Тип Внешнего обработчика-наследника.
	template<class T>
	class ExternalCommandHandler
	{
	public:
		ExternalCommandHandler(T *object) : handler(object)
		{
		}
		T *handler;
		void operator ()(CmdInfo *cmd, Communication::InterfaceBase *outputInterface)
		{
			// Этот метод вызывается у внешнего обработчика
			// Должен быть объявлен
			handler->CommandHandler(cmd, outputInterface);
		}
	};

	/// \brief Реализация базового внутреннего библиотечного обработчика команд.
	/// \details Обращение к этому обработчику происходит из библиотеки при получении пакета.
	/// Здесь реализуется обработка команд по умолчанию, и происходит обращение к внешнему пользовательскоум обработчику.
	/// Шаблонный параметр данного класса не имеет смысла, 
	/// в клиентском коде следует использовать специализацию CommandHandlerBase.
	template<class NotUsedType>
	class _CommandHandlerBase
	{
	protected:

		/// Подключенный внешний обработчик
		static ExternalCommandHandlerFunctor _externalCommandHandler;

	public:

		/// Назначить внешний обработчик команд
		/// \param commandHandler Ссылка на внешний обработчик команд.
		/// \memberof Rblib::Protocol::CommandHandlerBase
		static void SetExternalCommandHandler(const ExternalCommandHandlerFunctor &commandHandler)
		{
			_externalCommandHandler = commandHandler;
		}

		/// Обработка команды, основной метод.
		/// \details Выполняет обработку по умлочанию и вызывает внешний пользовательский обработчик.
		/// \param cmd Указатель на структуру параметров команды.
		/// \param outputInterface Интерфейс для оправки ответа.
		static inline void ProcessCommand(CmdInfo *cmd, Communication::InterfaceBase *outputInterface)
		{
			if (0 == cmd)
				return;
			switch(cmd->Cmd)
			{
			case CmdDevicePing:
				if (outputInterface)
				{
					CmdInfo answer(*cmd);
					answer.Cmd = CmdDevicePong;
					outputInterface->SendCommand(&answer);
				}
			break;
			// Команда не распознана парсером протокола
			case CmdUnknown:
			{
				// отправить "команда не поддерживатеся"
				CmdInfo answer(*cmd);
				outputInterface->SendCommand(&answer);
			}
			break;
			// команда распознана парсером, библиотека её не обрабатывает
			default:
				// передаем внешнему обработчику
				_externalCommandHandler(cmd, outputInterface);
				// если команда не была обработана внешним обработчиком
				if (!cmd->Handled)
				{
					// TODO вернуть "не поддерживается"
				}
			break;
			}
		}

		// Переслать команду
		static inline void ResendCommand(unsigned int cmd, unsigned char *data)
		{
		}
	};

	template <class NotUsedType>
	ExternalCommandHandlerFunctor _CommandHandlerBase<NotUsedType>::_externalCommandHandler;

	/// \class Rblib::Protocol::CommandHandlerBase
	/// \brief Внутренний обработчик, базовый класс.
	/// \details Класс является единственной специализацией класса _CommandHandlerBase, в котором определена вся функциональность.
	/// \n Определён как <tt>typedef _CommandHandlerBase<NullType> %CommandHandlerBase;</tt>
	typedef _CommandHandlerBase<NullType> CommandHandlerBase;

}
}

#endif
