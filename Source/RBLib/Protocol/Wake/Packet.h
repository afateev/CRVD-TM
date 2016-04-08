///////////////////////////////////////////////////////////////////////////////
//	Формат пакета WAKE
//
///////////////////////////////////////////////////////////////////////////////

#ifndef PROTOCOL_WAKE_PACKET_H
#define PROTOCOL_WAKE_PACKET_H

namespace Rblib
{
namespace Protocol
{
namespace Wake
{
#pragma pack(push, 1)
	
	template
		<
			int InternalBufferLen = 255
		>
	class Packet
	{
	public:
		static const unsigned char FrameStart = 0xC0;            // Начало кадра
		static const unsigned char FrameEscape = 0xDB;           // Маркер Escape-последовательности
		static const unsigned char TransposedFrameStart = 0xDC;  // Так пердается FrameStart внутри Escape-последовательности
		static const unsigned char TransposedFrameEscape = 0xDD; // Так пердается FrameEscape внутри Escape-последовательности
		static const unsigned char CrcStartValue = 0xDE;         // начальное значение контрольной суммы
	protected:
		unsigned char _data[InternalBufferLen];
		int _dataCount;
	public:
		// Конструктор из потока байт (байты должны быть со стаффингом)
		Packet(void *data, unsigned char dataLen)
		{
			_dataCount = UnStuff((unsigned char *)data, dataLen, _data, InternalBufferLen);
		}
		
		// Конструктор по полю команды и полю данных
		Packet(unsigned char command, void *data, unsigned char dataLen)
		{
			_dataCount = 0;
			int packetLen = 3 + dataLen + 1;
			
			if (InternalBufferLen < packetLen)
			{
				return;
			}
			
			_dataCount = packetLen;
			_data[0] = FrameStart;
			_data[1] = command & ~0x80;
			_data[2] = dataLen;
			if (dataLen > 0)
			{
				for (int i = 0; i < dataLen; i++)
				{
					_data[3 + i] = ((unsigned char *)data)[i];
				}
			}
			_data[packetLen - 1] = CalcCrc();
		}
		
		// Конструктор по полю адреса, полю команды и полю данных
		Packet(unsigned char address, unsigned char command, void *data, unsigned char dataLen)
		{
			_dataCount = 0;
			int packetLen = 4 + dataLen + 1;
			
			if (InternalBufferLen < packetLen)
			{
				return;
			}
			
			_dataCount = packetLen;
			_data[0] = FrameStart;
			_data[1] =address | 0x80;
			_data[2] = command & ~0x80;
			_data[3] = dataLen;
			if (dataLen > 0)
			{
				for (int i = 0; i < dataLen; i++)
				{
					_data[4 + i] = ((unsigned char *)data)[i];
				}
			}
			_data[packetLen - 1] = CalcCrc();
		}
		
		// Пакет имее начало
		bool HaveFrameStart()
		{
			return _data[0] == FrameStart;
		}
		
		// Пакет имеет поле адреса
		bool HaveAddress()
		{
			if (!HaveFrameStart())
			{
				return false;
			}

			if (_dataCount < 2)
			{
				return false;
			}

			return (_data[1] & 0x80) == 0x80;
        }

		// Адрес
		unsigned char Address()
		{
			if (!HaveAddress())
			{
				return 0;
			}

			return _data[1] & ~0x80;
        }

		// Пакет имеет поле команды
		bool HaveCommand()
		{
			int offset = CommandOffset();
			bool have = _dataCount >= offset + 1;
			if (have)
			{
				if ((_data[offset] & 0x80) != 0)
				{
					// старший байт не пустой, значит не правильный формат пакета
					have = false;
				}
			}
			return have;
        }

		// Код команды
		unsigned char Command()
        {
			if (!HaveCommand())
			{
				return 0;
			}

			int cmdOffset = CommandOffset();
			return _data[cmdOffset] & ~0x80;
        }

		// Пакет имеет поле длины данных
		bool HaveDataLen()
		{
			int offset = DataLenOffset();
			return _dataCount >= offset + 1;
        }

		// Длина данных
		unsigned int DataLen()
		{
			if (!HaveDataLen())
			{
				return 0;
			}

			int offset = DataLenOffset();

            return _data[offset];
        }

		// Пакет имеет поле данных
		bool HaveData()
		{
			int offset = DataLenOffset() + 1;
			unsigned int dataLen = DataLen();

			return _dataCount >= offset + dataLen && dataLen > 0;
        }

		// Содержимое поля данных
		unsigned char * Data()
		{
			if (!HaveData())
			{
				return 0;
			}

			int offset = DataOffset();

			return &_data[offset];
        }

		// Пакет имеет поле контрольной суммы
		bool HaveCrc()
		{
			int offset = CrcOffset();
			return _dataCount >= offset + 1;
        }

		// Содержимое поля контрольной суммы
		unsigned char Crc()
		{
			if (!HaveCrc())
			{
				return 0;
			}

			int offset = CrcOffset();

			return _data[offset];
        }

		// Содержимое поля контрольной суммы совпадает с расчётной контрольной суммой
		bool CrcValid()
		{
			unsigned char expected = CalcCrc();
			return Crc() == expected;
        }
		
		// Получить представление пакета в виде массива байт (со стаффингом)
		int ToByteArray(void *dst, unsigned char dstSize)
		{
			if (!dst)
			{
				return 0;
			}

			if (dstSize < 1)
			{
				return 0;
			}
			
			unsigned char *dstPtr = (unsigned char *)dst;
			
			// первый байт без стаффинга
			dstPtr[0] = _data[0];
			
			unsigned char dstIndex = 1;
			
			// на стафиинг отправляем всё кроме первого байта
			for (int i = 1; i < _dataCount; i++)
			{
				unsigned char tmp[2];
				// стаффинг во временном буфере
				unsigned char stuffed = Stuff(_data[i], tmp);
				
				// Проверка на выход за диапазон массива dst
				if (dstIndex + stuffed >= dstSize)
				{
					return 0;
				}
				
				// копируем в выходной буфер результат стаффинга
				for (int j = 0; j < stuffed; j++)
				{
					dstPtr[dstIndex] = tmp[j];
					dstIndex++;
				}
			}
			
			return dstIndex;
        }
	protected:
		// Смещение поля команды в исходном массиве данных
		int CommandOffset()
		{
			int res = 1;

			if (HaveAddress())
			{
				res++;
			}

			return res;
        }

		// Смещение поля длины данных в исходном массиве данных
		int DataLenOffset()
		{
			return CommandOffset() + 1;
		}

		// Смещение начала поля данных в исходном массиве данных
		int DataOffset()
		{
			if (!HaveData())
			{
				return 0;
			}

			return DataLenOffset() + 1;
        }

		// Смещение поля контрольной суммы в исходном массиве данных
		int CrcOffset()
		{
			return DataLenOffset() + DataLen() + 1;
		}
		
		// Вычислить контрольную сумму по текущему содержимому пакета
		unsigned char CalcCrc()
		{
			unsigned char crc = CrcStartValue;

			for (int i = 0; i < _dataCount - 1; i++)
			{
				crc = GetNextCrcValue(crc, _data[i]);
			}

			return crc;
        }
		
		// Вычислить значение контрольной суммы, на основе предыдущего значения и значения нового байта
        unsigned char GetNextCrcValue(unsigned char currentCrcValue, unsigned char nextByte)
		{
			unsigned char crc = currentCrcValue;
			unsigned char b = nextByte;

			for (unsigned char i = 0; i < 8; i++)
			{
				if (((b ^ crc) & 1) != 0)
				{
					crc = ((crc ^ 0x18) >> 1) | 0x80;
				}
				else
				{
					crc = (crc >> 1) & 0x7F;
				}
				b >>= 1;
			}

			return crc;
        }
		
		// Стаффинг одного байта
        unsigned char Stuff(unsigned char data, void *dst)
        {
			if (!dst)
			{
				return 0;
			}
			
			unsigned char *dstPtr = (unsigned char *)dst;
			
			switch (data)
			{
				case FrameStart:
					{
						dstPtr[0] = FrameEscape;
						dstPtr[0] = TransposedFrameStart;
						return 2;
					}
				case FrameEscape:
					{
						dstPtr[0] = FrameEscape;
						dstPtr[0] = TransposedFrameEscape;
						return 2;
					}
				default:
					{
						dstPtr[0] = data;
						return 1;
					}
            }
        }
		
		// Дестаффинг последовательности байт
		unsigned int UnStuff(unsigned char *data, unsigned int dataCount, unsigned char *res, unsigned char resSize)
        {
            if (data == 0 || dataCount < 1 || res == 0 || resSize < 1)
			{
				return 0;
			}
			
			// сколько распаковано байт
			unsigned char resCount = 0;
			
			// признак того, что следующий обрабатываемый байт является элементом Escape-последовательности
			bool nextEscaped = false;
			
			for (int i = 0; i < dataCount; i++)
			{
				unsigned char b = data[i];

				if (!nextEscaped)
				{
					switch (b)
					{
					// начало кадра
					case FrameStart:
						{
							// если данные пакета уже начали заполнять
							if (resCount > 0)
							{
								// то на этом пакет обрывается
								return resCount;
							}
							if (resCount < resSize)
							{
								res[resCount] = b;
								resCount++;
							}
						}
						break;
					// начало Escape-последовательности
					case FrameEscape:
						{
							nextEscaped = true;
						}
						break;
					// обычный байт
					default:
						{
							if (resCount < resSize)
							{
								res[resCount] = b;
								resCount++;
							}
						}
						break;
					}
				}
				else
				{
					switch (b)
					{
					// зашифрован байт FrameStart
					case TransposedFrameStart:
						{
							if (resCount < resSize)
							{
								res[resCount] = FrameStart;
								resCount++;
							}
							nextEscaped = false;
						}
						break;
					// зашифрован байт FrameEscape
					case TransposedFrameEscape:
						{
							if (resCount < resSize)
							{
								res[resCount] = FrameEscape;
								resCount++;
							}
							nextEscaped = false;
						}
						break;
					// не бывает других Escape-последовательностей
					default:
						{
							//TODO выкинуть ошибку
						}
					}
				}
			}

            // возвращаем кол-во распакованных байт
			return resCount;
		}
	};
	
#pragma pack (pop)
}
}
}

#endif