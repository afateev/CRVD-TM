///////////////////////////////////////////////////////////////////////////////
//	Формат пакета NMEA
//
///////////////////////////////////////////////////////////////////////////////

#ifndef PROTOCOL_NMEA_PACKET_H
#define PROTOCOL_NMEA_PACKET_H

#include <stdio.h>

namespace Rblib
{
namespace Protocol
{
namespace Nmea
{
	class Packet
	{
	public:
		static const char PacketStart = '$';            // Начало пакета
		static const char PacketCrcStart = '*';			// Начало поля CRC
		static const char PacketFieldsDelimiter = ',';	// Разделитель полей
		static const char PacketEnd1 = '\r';            // Конец пакета
		static const char PacketEnd2 = '\n';            // Конец пакета
		
		enum SourceId
		{
			SourceUnknown,	// не определено
			SourceGps,		// GP
			SourceGlonass,	// GL
			SourceGalileo,	// GA
			SourceGnss,		// GN
			SourceOther,	// все остальные
		};
		
		enum MessageId
		{
			MessageUnknown,
			MessageGGA,
			MessageGSA,
			MessageGSV,
			MessageRMC,
			MessageZDA
		};
	protected:
		static const int maxPacketSize = 80;
		static const int BufferSize = 100;
		char _data[BufferSize];
		int _dataCount;
		int _fieldsCount;
		unsigned char _fieldStartList[maxPacketSize];
		unsigned char _fieldSizeList[maxPacketSize];
	public:
		// Конструктор по умолчанию
		Packet()
		{
			_dataCount = 0;
			_fieldsCount = 0;
		}
		
		// Конструктор из набора символов
		Packet(char *data, unsigned char dataLen)
		{
			FromByteArray(data, dataLen);
		}
		
		// Конструктор из потока символов
		void FromByteArray(char *data, unsigned int dataLen)
		{
			_dataCount = dataLen;
			if (_dataCount > BufferSize)
			{
				_dataCount = BufferSize;
			}
			for (int i = 0; i < _dataCount; i++)
			{
				_data[i] = data[i];
			}
			
			_fieldsCount = 0;
			if (CrcValid())
			{
				int fieldNumber = 0;
				_fieldStartList[fieldNumber] = 0;
				_fieldSizeList[fieldNumber] = 0;
				for (int i = 1; i < _dataCount; i++)
				{
					char nextChar = _data[i];
					if (nextChar != PacketFieldsDelimiter)
					{
						if (nextChar == PacketCrcStart)
						{
							break;
						}
						else
						{
							_fieldSizeList[fieldNumber]++;
						}
					}
					else
					{
						fieldNumber++;
						_fieldStartList[fieldNumber] = i;
						_fieldSizeList[fieldNumber] = 0;
					}
				}
				
				_fieldsCount = fieldNumber + 1;
			}
		}
		
		bool Complete()
		{
			bool haveCrcStart = false;
			int crcStartPos = -1;
			
			// длиннее не бывает
			if (_dataCount > maxPacketSize)
			{
				return true;
			}
			
			for (int i = 0; i < _dataCount; i++)
			{
				// ищем поле CRC
				if (!haveCrcStart)
				{
					if (_data[i] == PacketCrcStart)
					{
						haveCrcStart = true;
						crcStartPos = i;
					}
				}
				else
				{
					// после CRC есть 2 символа, конец пакета можно не ждать
					if (_dataCount >= crcStartPos + 3)
					{
						return true;
					}
				}
				
				// если конец пакета, то безусловно, принят полностью, независимо от наличия CRC
				if (_data[i] == PacketEnd1 || _data[i] == PacketEnd2)
				{
					return true;
				}
			}
			
			return false;
		}
		
		bool CrcValid()
		{
			if (!Complete())
			{
				return false;
			}
			
			if (!Contains(_data, _dataCount, PacketCrcStart))
			{
				return false;
			}
			
			unsigned char calculatedCrc = 0;
			
			for (int i = 1; i < _dataCount; i++)
			{
				if (_data[i] == PacketCrcStart)
				{
					if (_dataCount - i != 2)
					{
						int crcValue = 0;
						char tmp[3]  = {_data[i + 1], _data[i +2], 0};
						int res = sscanf(tmp, "%x", &crcValue);
						if (!res)
						{
							return false;
						}
						
						return calculatedCrc == crcValue;
					}
				}
				else
				{
					calculatedCrc ^= _data[i];
				}
			}
			
			return false;
		}
		
		int GetFieldsCount()
		{
			return _fieldsCount;
		}
		
		bool GetField(int fieldNumber, char *buffer, int bufferSize, int &filedSize)
		{
			if (fieldNumber > GetFieldsCount() || buffer == 0)
			{
				return false;
			}
			
			int i = 0;
			for (i = 0; i < _fieldSizeList[fieldNumber] && i < bufferSize; i++)
			{
				buffer[i] = _data[_fieldStartList[fieldNumber] + i + 1];
			}
			
			filedSize = i;
			return filedSize > 0;
		}
		
		bool GetField(int fieldNumber, int &value)
		{
			char tmp[maxPacketSize];
			int fieldSize = 0;
			if (GetField(fieldNumber, tmp, sizeof(tmp), fieldSize))
			{
				tmp[fieldSize] = 0;
				
				return sscanf(tmp, "%d", &value);
			}
			
			return false;
		}
		
		bool GetField(int fieldNumber, float &value)
		{
			char tmp[maxPacketSize];
			int fieldSize = 0;
			if (GetField(fieldNumber, tmp, sizeof(tmp), fieldSize))
			{
				tmp[fieldSize] = 0;
				
				return sscanf(tmp, "%f", &value);
			}
			
			return false;
		}
		
		bool GetField(int fieldNumber, char &value)
		{
			int fieldSize = 0;
			if (GetField(fieldNumber, &value, 1, fieldSize))
			{
				return fieldSize == 1;
			}
			
			return false;
		}
		
		char *Data()
		{
			return _data;
		}
		
		unsigned char DataCount()
		{
			return _dataCount;
		}
		
		bool GetMessageSource(char *res)
		{
			int filedsCount = GetFieldsCount();

			if (filedsCount > 0 && res > 0)
			{
				char fieldText[100];
				int fieldSize = 0;
				
				if (GetField(0, fieldText, sizeof(fieldText), fieldSize))
				{
					if (fieldSize >= 2)
					{
						res[0] = fieldText[0];
						res[1] = fieldText[1];
						return true;
					}
				}
			}
			
			return false;
		}
		
		bool MessageSourceEqual(char *source)
		{
			char tmp[2]; 
			
			if (source == 0)
			{
				return false;
			}
			
			if (GetMessageSource(tmp))
			{
				return tmp[0] == source[0] && tmp[1] == source[1];
			}
			
			return false;
		}
		
		bool MessageIdEqual(char *id)
		{
			int filedsCount = GetFieldsCount();

			if (filedsCount > 0)
			{
				char fieldText[100];
				int fieldSize = 0;
				
				if (GetField(0, fieldText, sizeof(fieldText), fieldSize))
				{
					if (fieldSize >= strlen(id))
					{
						return strncmp(id, fieldText + 2, strlen(id)) == 0;
					}
				}
			}
			
			return false;
		}
		
		SourceId GetMessageSource()
		{
			if (GetFieldsCount() < 1)
			{
				return SourceUnknown;
			}
			
			if (MessageSourceEqual("GP"))
			{
				return SourceGps;
			}
			
			if (MessageSourceEqual("GL"))
			{
				return SourceGlonass;
			}
			
			if (MessageSourceEqual("GA"))
			{
				return SourceGalileo;
			}
			
			if (MessageSourceEqual("Gn"))
			{
				return SourceGnss;
			}
			
			return SourceOther;
		}
		
		MessageId GetMessageId()
		{
			if (MessageIdEqual("GGA"))
			{
				return MessageGGA;
			}
			
			if (MessageIdEqual("GSA"))
			{
				return MessageGSA;
			}
			
			if (MessageIdEqual("GSV"))
			{
				return MessageGSV;
			}
			
			if (MessageIdEqual("ZDA"))
			{
				return MessageZDA;
			}
			
			if (MessageIdEqual("RMC"))
			{
				return MessageRMC;
			}
			
			return MessageUnknown;
		}
	public:
		bool Contains(char *str, int strLen, char c)
		{
			for (int i = 0; i < strLen; i++)
			{
				if (str[i] == c)
				{
					return true;
				}
			}
			
			return false;
		}
	};
	
	class PacketGGA
	{
	public:
		bool Valid;
		float Time;
		bool TimePresent;
		float Latitude;
		bool LatitudePresent;
		float Longitude;
		bool LongitudePresent;
		float Altitude;
		bool AltitudePresent;
		int SatellitesUsed;
		bool SatellitesUsedPresent;
	public:
		PacketGGA(Packet packet)
		{
			int filedsCount = packet.GetFieldsCount();
			Valid = filedsCount >= 15;
			Valid &= packet.GetMessageId() == Packet::MessageGGA;
			
			Time = 0;
			TimePresent = false;
			Latitude = 0;
			LatitudePresent = false;
			Longitude = 0;
			LongitudePresent = false;
			SatellitesUsed = 0;
			SatellitesUsedPresent = false;
			Altitude = 0;
			AltitudePresent = false;
			
			if (Valid)
			{
				TimePresent = packet.GetField(1, Time);
				LatitudePresent = packet.GetField(2, Latitude);
				char nsIndicator = ' ';
				LatitudePresent &= packet.GetField(3, nsIndicator);
				if (LatitudePresent)
				{
					Latitude = PacketCoordsToRadians(Latitude);
					if (nsIndicator == 'S')
					{
						Latitude = -Latitude;
					}
				}
				LongitudePresent = packet.GetField(4, Longitude);
				char ewIndicator = ' ';
				LongitudePresent &= packet.GetField(5, ewIndicator);
				if (LongitudePresent)
				{
					Longitude = PacketCoordsToRadians(Longitude);
					if (ewIndicator == 'W')
					{
						Longitude = -Longitude;
					}
				}
				SatellitesUsedPresent = packet.GetField(7, SatellitesUsed);
				AltitudePresent = packet.GetField(9, Altitude);
			}
		}
	protected:
		float PacketCoordsToRadians(float val)
		{
			static const float PI = 3.141592653589793238462643;

			float degree = (unsigned long)(val / 100);
    		float minutes = val - (degree * 100);
    		degree += minutes / 60.0;
    		float result = ((float)degree * PI) / 180.0;
    		return result;
		}
	};
	
	class PacketGSA
	{
	public:
		enum ModeCode
		{
			ModeNoPosition	= 1,
			Mode2D			= 2,
			Mode3D			= 3
		};
	public:
		bool Valid;
		ModeCode Mode;
		bool ModePresent;
	public:
		PacketGSA(Packet packet)
		{
			int filedsCount = packet.GetFieldsCount();
			Valid = filedsCount >= 3;
			Valid &= packet.GetMessageId() == Packet::MessageGSA;
			
			Mode = ModeNoPosition;
			ModePresent = false;
			
			if (Valid)
			{
				int mode = 0;
				ModePresent = packet.GetField(2, mode);
				Mode = (ModeCode)mode;
			}
		}
	};
	
	class PacketGSV
	{
	public:
		bool Valid;
		int SatellitesVisible;
		bool SatellitesVisiblePresent;
	public:
		PacketGSV(Packet packet)
		{
			int filedsCount = packet.GetFieldsCount();
			Valid = filedsCount >= 4;
			Valid &= packet.GetMessageId() == Packet::MessageGSV;
			
			SatellitesVisible = 0;
			SatellitesVisiblePresent = false;
			
			if (Valid)
			{
				SatellitesVisiblePresent = packet.GetField(3, SatellitesVisible);
			}
		}
	};
	
	class PacketRMC
	{
	public:
		bool  Valid;
		float Time;
		bool  TimePresent;
		float Latitude;
		bool  LatitudePresent;
		float Longitude;
		bool  LongitudePresent;
		float SpeedOverGround;
		bool  SpeedOverGroundPresent;
		float CourseOverGround;
		bool  CourseOverGroundPresent;
		int   Date;
		bool  DatePresent;
		float MagneticVariation;	// Not supported
		bool  MagneticVariationPresent;
		char  ModeIndicator;		// Not supported
		bool  ModeIndicatorPresent;
		bool  Active;
		bool  AllFiledsPresent;
	public:
		PacketRMC(Packet packet)
		{
			int filedsCount = packet.GetFieldsCount();
			Valid = filedsCount >= 9;
			Valid &= packet.GetMessageId() == Packet::MessageRMC;
			
			Time = 0;
			TimePresent = false;
			Latitude = 0;
			LatitudePresent = false;
			Longitude = 0;
			LongitudePresent = false;
			SpeedOverGround = 0;
			SpeedOverGroundPresent = false;
			CourseOverGround = 0;
			CourseOverGroundPresent = false;
			Date = 0;
			DatePresent = false;
			MagneticVariation = 0;
			ModeIndicator = ' ';
			Active = false;
			AllFiledsPresent = false;
			
			if (Valid)
			{
				TimePresent = packet.GetField(1, Time);
				LatitudePresent = packet.GetField(3, Latitude);
				char nsIndicator = ' ';
				LatitudePresent &= packet.GetField(4, nsIndicator);
				if (LatitudePresent)
				{
					Latitude = PacketCoordsToRadians(Latitude);
					if (nsIndicator == 'S')
					{
						Latitude = -Latitude;
					}
				}
				LongitudePresent = packet.GetField(5, Longitude);
				char ewIndicator = ' ';
				LongitudePresent &= packet.GetField(6, ewIndicator);
				if (LongitudePresent)
				{
					Longitude = PacketCoordsToRadians(Longitude);
					if (ewIndicator == 'W')
					{
						Longitude = -Longitude;
					}
				}
				SpeedOverGroundPresent = packet.GetField(7, SpeedOverGround);
				CourseOverGroundPresent = packet.GetField(8, CourseOverGround);
				DatePresent = packet.GetField(9, Date);
				MagneticVariationPresent = packet.GetField(10, MagneticVariation);
				char mewIndicator = ' ';
				MagneticVariationPresent &= packet.GetField(11, mewIndicator);
				if (MagneticVariationPresent)
				{
					if (mewIndicator == 'W')
					{
						MagneticVariation = -MagneticVariation;
					}
				}
				ModeIndicatorPresent = packet.GetField(12, ModeIndicator);
				AllFiledsPresent = TimePresent && LatitudePresent && LongitudePresent && SpeedOverGroundPresent && CourseOverGroundPresent && DatePresent;
				char statusIndicator = ' '; // Status: A=active, V=Void
				AllFiledsPresent &= packet.GetField(2, statusIndicator);
				Active = (statusIndicator == 'A');
			}
		}
	protected:
		float PacketCoordsToRadians(float val)
		{
			static const float PI = 3.141592653589793238462643;

			float degree = (unsigned long)(val / 100);
    		float minutes = val - (degree * 100);
    		degree += minutes / 60.0;
    		float result = ((float)degree * PI) / 180.0;
    		return result;
		}
	};
	
	// ZDA-пакет интересен информацией о часовом поясе, но по умолчанию отключен в GPS-модуле (IT530)
	// Как его включить:	1) http://cdn.sparkfun.com/datasheets/Sensors/GPS/NMEA%20manual%20for%20Fastrax%20IT500%20Series%20GPS%20receivers_V1.7.pdf
	//						2) https://www2.u-blox.com/images/downloads/Product_Docs/NMEA-CommandManual_%28FTX-HW-13002%29.pdf
	class PacketZDA
	{
	public:
		bool  Valid;
		float Time;
		bool  TimePresent;
		int   Day;
		bool  DayPresent;
		int   Month;
		bool  MonthPresent;
		int   Year;
		bool  YearPresent;
		int   LocalZoneHours;
		bool  LocalZoneHoursPresent;
		int   LocalZoneMinutes;
		bool  LocalZoneMinutesPresent;
		bool  AllFiledsPresent;
		
	public:
		PacketZDA(Packet packet)
		{
			int filedsCount = packet.GetFieldsCount();
			Valid = filedsCount >= 7;
			Valid &= packet.GetMessageId() == Packet::MessageZDA;
			
			Time = 0;
			TimePresent = false;
			Day = 0;
			DayPresent = false;
			Month = 0;
			MonthPresent = false;
			Year = 0;
			YearPresent = false;
			LocalZoneHours = 0;
			LocalZoneHoursPresent = false;
			LocalZoneMinutes = 0;
			LocalZoneMinutesPresent = false;
			AllFiledsPresent = false;
			
			if (Valid)
			{
				TimePresent = packet.GetField(1, Time);
				DayPresent = packet.GetField(2, Day);
				MonthPresent = packet.GetField(3, Month);
				YearPresent = packet.GetField(4, Year);
				LocalZoneHoursPresent = packet.GetField(5, LocalZoneHours);
				LocalZoneMinutesPresent = packet.GetField(6, LocalZoneMinutes);
				
				AllFiledsPresent = TimePresent && DayPresent && MonthPresent && YearPresent && LocalZoneHoursPresent && LocalZoneMinutesPresent;
			}
		}
	};
}
}
}

#endif