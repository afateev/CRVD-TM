///////////////////////////////////////////////////////////////////////////////
//	Парсер пакетов
//	Протокол FirTwig2
//
///////////////////////////////////////////////////////////////////////////////

#ifndef PARSER_FIRTWIG2_H
#define PARSER_FIRTWIG2_H

#include "PacketFormat.h"

namespace Rblib
{
namespace Protocol
{
namespace FirTwig2
{

	///
	/// \brief Класс парсера пакетов в формате протокола FirTwig2
	///
	class Parser
	{
	public:

		/// Максимальный уровень вложенности команд в пакете
		static const unsigned char MaxNestingLevel = 3;

		///
		/// \brief Структура для представления полной информации о пакете,
		/// может являться результатом работы парсера
		///
		struct PacketSummaryInfo
		{
			/// Указатель на буфер, содержащий пакет
			unsigned char* PacketBuffer;
			/// Размер пакета
			unsigned int PacketSize;
			/// Глубина вложенности команд в пакете (1, 2 или 3)
			unsigned char NestingLevel;

			/// Коды команд 0-го, 1-го и 2-го уровня вложенности
			unsigned char Cmd[MaxNestingLevel];
			/// Данные пакета 0-го, 1-го и 2-го уровня вложенности
			unsigned char* Data[MaxNestingLevel];
			/// Размер секции данных 0-го, 1-го и 2-го уровня вложенности
			unsigned int DataSize[MaxNestingLevel];

			/// Источник пакета
			unsigned char Source;
			/// Получатель пакета
			unsigned char Dest;

			/// Код подтверждения
			unsigned char Ack;
			/// флаг запроса
			bool Request;

			/// Сетевой адрес отправителя
			signed char NetAddress;

			PacketSummaryInfo()
			: PacketBuffer(0)
			, PacketSize(0)
			, NestingLevel(0)
			, Source(0)
			, Dest(0)
			, Ack(0)
			, Request(0)
			, NetAddress(-1)
			{
			}

			/// Позволяет определить является ли содержимое пакета командой полученной через эфир (UCMD_DATA)
			bool IsModemData() const
			{
				return (NestingLevel >= 1 && Cmd[0] == ModemCmdData && Source == AddrModem && Request == 0);
			}
		};

	protected:

		/// Указатель на пакет
		unsigned char *_data;
		/// Длина пакета
		unsigned int _length;

		/// Текущая система команд
		DevCommandSet _commandSet;

	public:

		/// Конструктор по-умолчанию
		Parser()
		{
			_commandSet = DevCommandSetDefault;
			_data = 0;
			_length = 0;
		}

		/// Установить текущий пакет для парсинга
		/*! 
			\param data Указатель на буфер, содержащий пакет
			\param length Длина пакета
		*/
		void SelectPacket(unsigned char *data, unsigned int length)
		{
			_data = data;
			_length = length;
		}

		/// Задать текущую систему команд
		/*! 
			\param commandSet Система команд
		*/
		void SelectCommandSet(DevCommandSet commandSet)
		{
			_commandSet = commandSet;
		}

		// Функции обработки "выбранного" пакета

		/*!
			\brief Получить адрес получателя
			\return Адрес получателя
		*/
		unsigned char GetDstAddress(void)
		{
			HeaderField* head = (HeaderField*)_data;
			return head->Dest;
		}

		/*!
			\brief Получить адрес источника
			\return Адрес источника
		*/
		unsigned char GetSrcAddress(void)
		{
			HeaderField* head = (HeaderField*)_data;
			return head->Source;
		}

		/*!
			\brief Получить код команды
			\return Код команды
		*/
		unsigned int GetCommand(void)
		{
			HeaderField* head = (HeaderField*)_data;
			return head->Cmd;
		}

		/*!
			\brief Возвращает длину вложенных данных
			\return Длина вложенных данных
		*/
		unsigned int GetDataSize(void)
		{
			HeaderField* head = (HeaderField*)_data;
			return head->Len;
		}

		/*!
			\brief Возвращает указатель на вложенные данные
			\return Вложенные в пакет данные
		*/
		unsigned char * GetData()
		{
			if (GetDataSize() == 0)
				return 0;
			return &(_data[4]);
		}

		/*!
			\brief Возвращает код подтверждения
			\return Код подтверждения
		*/
		unsigned char GetAckCode(void)
		{
			HeaderField* head = (HeaderField*)_data;
			return head->Ack;
		}

		/*!
			\brief Доступ к флагу запроса (Requet)
			\return Флаг запроса
		*/
		unsigned char GetRequestFlag(void)
		{
			HeaderField* head = (HeaderField*)_data;
			return head->Request;
		}

		/*!
			\brief Возвращает указаетль на буфер пакета
			\return Указатель на буфер пакета
		*/
		unsigned char *GetPacketData()
		{
			return _data;
		}
		
		/*!
			\brief Возвращает Размер текущего пакета
			\return Размер текущего пакета
		*/
		unsigned int GetPacketSize()
		{
			return _length;
		}

		/*!
			\brief Получить сетевой адрес отправителя пакета
			\return Сетевой адрес отправителя пакета
		*/
		char GetNetAddr()
		{
			if (IsModemData())
			{
				return _data[_netAddrInd];
			}
			else
				return -1;
		}

		// Нужен ли этот метод вообще???
		bool GetRequestWise()
		{
			if (IsModemData())
			{
				return ((SubCommandField*)&_data[_subDataModemInd])->Request;
			}
			else
			{
				return ((HeaderField*)_data)->Request;
			}
		}

		// Нужен ли этот метод вообще???
		unsigned char GetAckWise()
		{
			if (IsModemData())
			{
				return ((SubCommandField*)&_data[_subDataModemInd])->Ack;
			}
			else
			{
				return ((HeaderField*)_data)->Ack;
			}
		}

		/*!
			\brief Парсинг пакета
			\param[out] summaryInfo результаты работы парсера (полная информация о пакете)
		*/
		void GetSummaryInfo(PacketSummaryInfo& summaryInfo)
		{
			summaryInfo.Source = ((HeaderField*)_data)->Source;
			summaryInfo.Dest = ((HeaderField*)_data)->Dest;
			summaryInfo.PacketBuffer = _data;
			summaryInfo.PacketSize = _length;

			State state;
			while(state.CurrentLevel < MaxNestingLevel)
			{
				bool stop = false;
				if (StateParserNextCommand(state, stop))
				{
					summaryInfo.Cmd[state.CurrentLevel] = state.CurrentCmd;
					summaryInfo.Data[state.CurrentLevel] = state.CurrentCmdSubData;
					summaryInfo.DataSize[state.CurrentLevel] = state.CurrentCmdSubDataLen;
					summaryInfo.NestingLevel++;
					state.CurrentLevel++;
					if (stop)
					{
						break;
					}
				}
				else
				{
					break;
				}
			}

			GetAddlPacketData(summaryInfo.NetAddress, summaryInfo.Request, summaryInfo.Ack);
		}

	protected:

		/// Указатель на вложенные данных для обычной команды
		static const unsigned char _subDataStdInd = sizeof(HeaderField);
		/// Указатель на вложенные данных для команды, полученной через эфир (UCMD_DATA)
		static const unsigned char _subDataModemInd = sizeof(HeaderField) + sizeof(ModemCmdDataHeader); // UCMD_DATA
		/// Поле с адресом узла-отправителя в команде, полученной через эфир (UCMD_DATA)
		static const unsigned char _netAddrInd = sizeof(HeaderField) + 2;

	protected:

		/// Внутреннее состояние парсера
		struct State
		{
			/// Текущая команда в пакете
			unsigned int CurrentCmd;

			/// Вложенне данные текущей команды
			unsigned char* CurrentCmdSubData;
			/// Длина вложенных данных текущей команды
			unsigned int CurrentCmdSubDataLen;

			/// Уровень вложенности текущей команды
			unsigned char CurrentLevel;

			/// Конструктор по умолчанию
			State()
			: CurrentCmd(0)
			, CurrentCmdSubData(0)
			, CurrentCmdSubDataLen(0)
			, CurrentLevel(0)
			{
			}
		};

	protected:

		/// Проверка не является ли пакет командой UCMD_DATA
		inline bool IsModemData()
		{
			return (IsModemPacket() && IsModemDataCmdCode());
		}

		// Проверить не является ли пакет пакетом от модема
		inline bool IsModemPacket()
		{
			return (((HeaderField*)_data)->Source == AddrModem);
		}

		// Равен ли код команды коду UCMD_DATA
		inline bool IsModemDataCmdCode()
		{
			return (((HeaderField*)_data)->Cmd == ModemCmdData && ((HeaderField*)_data)->Request == 0);
		}

		/// \brief Извлекает из пакета сетевой адрес отправилетя и биты request и ack
		/// \param[out] netAddr Сетевой адрес отправителя
		/// \param[out] request true - в пакете установлен флаг запроса, false - не установлен
		/// \param[out] ack true - в пакете установлен флаг запроса, false - не установлен
		void GetAddlPacketData(signed char& netAddr, bool& request, unsigned char& ack)
		{
			if (IsModemData())
			{
				netAddr = _data[_netAddrInd];
				request = ((SubCommandField*)&_data[_subDataModemInd])->Request;
				ack = ((SubCommandField*)&_data[_subDataModemInd])->Ack;
			}
			else
			{
				netAddr = -1;
				request = ((HeaderField*)_data)->Request;
				ack = ((HeaderField*)_data)->Ack;
			}
		}

		//
		// Разбор пакета, извлечения команд различного уровня вложенности
		//

		/// \brief Извлечь очередную команду (основная функция)
		/// \param[in,out] state Текущее состояние
		/// \param[in,out] stop - Даже если команда найдена, всё равно остановиться
		/// \return @c true - в пакете найдена очередная команда, @c false - команда не найдена
		bool StateParserNextCommand(State& state, bool &stop)
		{
			HeaderField* head = (HeaderField*)_data;
			
			// возможен ли запрошенный уровень вложенности?
			if (state.CurrentLevel >= MaxNestingLevel)
				return false;
			
			// извлечение команды верхнего уровня
			if (state.CurrentLevel == 0)
			{
				state.CurrentCmd = head->Cmd;
				if (head->Len)
				{
					if (IsModemData())
					{
						state.CurrentCmdSubData = &_data[_subDataModemInd];
						state.CurrentCmdSubDataLen = head->Len - sizeof(ModemCmdDataHeader);
					}
					else
					{
						state.CurrentCmdSubData = &_data[_subDataStdInd];
						state.CurrentCmdSubDataLen = head->Len;
					}
				}
				else
				{
					state.CurrentCmdSubData = 0;
					state.CurrentCmdSubDataLen = 0;
				}
				return true;
			}
			
			// извлечение команды 1-го уровня, вложенной в UCMD_DATA и UCMD_SYS
			if (IsModemPacket())
			{
				if (state.CurrentLevel == 1)
					return StateParserNextCommandByCommandSetModem(state);
				else
					if (!IsModemDataCmdCode())
						return false;
			}
			
			// извлечение команд 1-го и 2-го уровней вложенности, на основе текущего Command Set
			bool commandFound = false;
			switch(_commandSet)
			{
			case DevCommandSetRsu:
				commandFound =  StateParserNextCommandByCommandSetRsu(state, stop);
				break;
			case DevCommandSetRsIr:
				commandFound =  StateParserNextCommandByCommandSetRsIr(state, stop);
				break;
			case DevCommandSetRslTransmitter:
				commandFound =  StateParserNextCommandByCommandSetRslTransmitter(state, stop);
				break;
			case DevCommandSetRsTv:
				commandFound =  StateParserNextCommandByCommandSetRsTv(state, stop);
				break;
			}

			if (!commandFound)
				return StateParserNextCommandByCommandSetDefault(state, stop);
			
			return true;
		}

		/// \brief Обновить поле кода текущей команды и указатель вложенные данные
		/// \param[in,out] state Текущее состояние
		inline bool StateParserUpdateCmdParamsFullCmd(State& state)
		{
			if (state.CurrentCmdSubDataLen == 0)
				return false; // нет вложенных данных/подкоманд
			
			state.CurrentCmd = state.CurrentCmdSubData[0];
			
			StateParserUpdateCmdBuffersDefault(state);
			return true;
		}

		/// \brief Обновить указатель на вложенные данные
		/// \param[in,out] state Текущее состояние
		inline void StateParserUpdateCmdBuffersDefault(State& state)
		{
			if (state.CurrentCmdSubDataLen > 1) // подкоманда имеет вложенные данные
			{
				state.CurrentCmdSubData = &state.CurrentCmdSubData[1];
				state.CurrentCmdSubDataLen = state.CurrentCmdSubDataLen - 1;
			}
			else // у подкоманды нет вложенных данных
			{
				state.CurrentCmdSubData = 0;
				state.CurrentCmdSubDataLen = 0;
			}
		}

		/// \brief Извлечь очередную команду на основе системы команд DevCommandSetDefault
		/// \param[in,out] state Текущее состояние
		/// \return @c true - найдена команда, @c false - команда не найдена
		/// \param[in,out] stop - Даже если команда найдена, всё равно остановиться
		bool StateParserNextCommandByCommandSetDefault(State& state, bool &stop)
		{
			// команды, имеющие вложенность
			switch(state.CurrentCmd)
			{
			case DevCmdSystemControl:
			case DevCmdExtended:
				{
					stop = true; // подкоманды будет извлечена в следующей строке, дальше парсить не надо
					return StateParserUpdateCmdParamsFullCmd(state);
				}
			}
			return false;
		}

		/// \brief Извлечь очередную команду на основе системы команд DevCommandSetRsu
		/// \param[in,out] state Текущее состояние
		/// \param[in,out] stop - Даже если команда найдена, всё равно остановиться
		/// \return @c true - найдена команда, @c false - команда не найдена
		bool StateParserNextCommandByCommandSetRsu(State& state, bool &stop)
		{
			switch(state.CurrentCmd)
			{
			case DevCmdRsuSeismicInveroment:
			case DevCmdRsuSeismicParamEx:
			case DevCmdRsuExtDevParam:
			case DevCmdRsuExtDevControl:
			case DevCmdRsuSelfTest:
				{
					stop = true; // подкоманда будет извлечена в следующей строке, дальше парсить не надо
					return StateParserUpdateCmdParamsFullCmd(state);
				}
			}
			return false;
		}

		/// \brief Извлечь очередную команду на основе системы команд DevCommandSetRsIr
		/// \param[in,out] state Текущее состояние
		/// \param[in,out] stop - Даже если команда найдена, всё равно остановиться
		/// \return @c true - найдена команда, @c false - команда не найдена
		bool StateParserNextCommandByCommandSetRsIr(State& state, bool &stop)
		{
			switch(state.CurrentCmd)
			{
//			case ...:
//				{
//					stop = true; // подкоманда будет извлечена в следующей строке, дальше парсить не надо
//					return StateParserUpdateCmdParamsFullCmd(state);
//				}
			}
			return false;
		}

		/// \brief Извлечь очередную команду на основе системы команд DevCommandSetRslTransmitter
		/// \param[in,out] state Текущее состояние
		/// \param[in,out] stop - Даже если команда найдена, всё равно остановиться
		/// \return @c true - найдена команда, @c false - команда не найдена
		bool StateParserNextCommandByCommandSetRslTransmitter(State& state, bool &stop)
		{
			switch(state.CurrentCmd)
			{
			case RslTrasmitterCmdTransmitter:
				{
					stop = true; // подкоманда будет извлечена в следующей строке, дальше парсить не надо
					return StateParserUpdateCmdParamsFullCmd(state);
				}
			}
			return false;
		}

		/// \brief Извлечь очередную команду на основе системы команд DevCommandSetRsTv
		/// \param[in,out] state Текущее состояние
		/// \param[in,out] stop - Даже если команда найдена, всё равно остановиться
		/// \return @c true - найдена команда, @c false - команда не найдена
		bool StateParserNextCommandByCommandSetRsTv(State& state, bool &stop)
		{
			switch(state.CurrentCmd)
			{
			case DevCmdRsTvWorkControl:
			case DevCmdRsTvCameraControl:
			case DevCmdRsTvVrControl:
				{
					stop = true; // подкоманда будет извлечена в следующей строке, дальше парсить не надо
					return StateParserUpdateCmdParamsFullCmd(state);
				}
			}
			return false;
		}

		/// \brief Извлечь очередную команду на основе системы команд DevCommandSetModem
		/// \param[in,out] state Текущее состояние
		/// \return @c true - найдена команда, @c false - команда не найдена
		bool StateParserNextCommandByCommandSetModem(State& state)
		{
			switch(state.CurrentCmd)
			{
			case ModemCmdData:
			case ModemCmdSys:
            case DevCmdRsuSeismicInveroment:
				return StateParserUpdateCmdParamsFullCmd(state);
			}
			return false;
		}

	};

}
}
}

#endif
