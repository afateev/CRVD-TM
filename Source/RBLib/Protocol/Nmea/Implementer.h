///////////////////////////////////////////////////////////////////////////////
//	Реализация обработчика NMEA пакетов
//	
//
///////////////////////////////////////////////////////////////////////////////

#ifndef PROTOCOL_NMEA_IMPLEMENTER_H
#define PROTOCOL_NMEA_IMPLEMENTER_H

#include "../Nmea/PacketExtractor.h"
#include "../../Utils/CallbackWrapper.h"

namespace Rblib
{
namespace Protocol
{
namespace Nmea
{
	template
		<
			bool reserved = false // Пока неиспользуемый параметр шаблона
		>
	class Implementer
	{
	public:
		typedef Rblib::Protocol::Nmea::PacketExtractor<false> PacketExtractorType;
		typedef PacketExtractorType::PacketType PacketType;
		typedef CallbackWrapper<PacketType> OnPacketReceivedCallbackType;
		typedef CallbackWrapper<PacketType::SourceId, bool /*dataValid*/, float /*latitude*/, float /*longitude*/, float /*altitude*/, int /*satellitesUsed*/> OnGGAReceivedCallbackType;
		typedef CallbackWrapper<PacketType::SourceId, bool /*dataValid*/, PacketGSA::ModeCode /*mode*/> OnGSAReceivedCallbackType;
		typedef CallbackWrapper<PacketType::SourceId, bool /*dataValid*/, int /*satellitesVisible*/> OnGSVReceivedCallbackType;
		typedef CallbackWrapper<PacketType::SourceId, bool /*dataValid*/, float /*Time*/, int /*Day*/, 
								int /*Month*/, int /*Year*/, int /*LocalZoneHours*/, int /*LocalZoneMinutes*/> OnZDAReceivedCallbackType;
		typedef CallbackWrapper<PacketType::SourceId, bool /*dataValid*/, float /*Time*/, float /*Latitude*/,
								float /*Longitude*/, float /*SpeedOverGround*/, float /*CourseOverGround*/, int /*Date*/> OnRMCReceivedCallbackType;
		typedef CallbackWrapper<bool> OnStatusChangedCallbackType;
		
	protected:
		PacketExtractorType _packetExtractor;
		PacketType _packet;
		static Implementer *_selfPtr;
		bool _active_prev;
	public:
		OnPacketReceivedCallbackType OnPacketReceivedCallback;
		OnGGAReceivedCallbackType OnGGAReceivedCallback;
		OnGSAReceivedCallbackType OnGSAReceivedCallback;
		OnGSVReceivedCallbackType OnGSVReceivedCallback;
		OnZDAReceivedCallbackType OnZDAReceivedCallback;
		OnRMCReceivedCallbackType OnRMCReceivedCallback;
		OnStatusChangedCallbackType OnStatusChangedCallback;
	public:
		Implementer()
		{
			_selfPtr = this;
			_active_prev = false;
		}
		
		void OnDataReceived(unsigned char *data, unsigned int dataCount)
		{
			if (dataCount > 0)
			{
				_packetExtractor.ExtractPackets(data, dataCount, OnPacketReceivedStaticCallback);
			}
		}
		
		static void OnDataReceived(void *callabckParam, unsigned char *data, int dataCount)
		{
			Implementer *im = (Implementer *)callabckParam;
			
			if (!im)
			{
				return;
			}
			
			im->OnDataReceived(data, dataCount);
		}
	protected:
		static void OnPacketReceivedStaticCallback(unsigned char *data, unsigned int dataCount)
		{
			_selfPtr->OnPacketReceived((char *)data, dataCount);
		}
		
		void OnPacketReceived(char *data, unsigned int dataCount)
		{
			_packet.FromByteArray(data, dataCount);
			// Этот колбэк можно использовать для отладки
			OnPacketReceivedCallback.Call(_packet);
			
			// Парсинг конкретных типов пакетов
			switch(_packet.GetMessageId())
			{
			case PacketType::MessageGGA:
				{
					PacketGGA p(_packet);
			
					if (p.Valid)
					{
						OnGGAReceivedCallback.Call(_packet.GetMessageSource(),
												   p.LatitudePresent && p.LongitudePresent && p.AltitudePresent && p.SatellitesUsedPresent,
												   p.Latitude , p.Longitude, p.Altitude, p.SatellitesUsed);
					}
				}
				break;
			case PacketType::MessageGSA:
				{
					PacketGSA p(_packet);
				
					if (p.Valid)
					{
						OnGSAReceivedCallback.Call(_packet.GetMessageSource(), p.ModePresent, p.Mode);
					}
				}
				break;
			case PacketType::MessageGSV:
				{
					PacketGSV p(_packet);
					
					// Подсчитываем только GPS спутники
					if (p.Valid && _packet.MessageSourceEqual("GP"))
					{
						OnGSVReceivedCallback.Call(_packet.GetMessageSource(), p.SatellitesVisiblePresent, p.SatellitesVisible);
					}
				}
				break;
			case PacketType::MessageRMC:
				{
					PacketRMC p(_packet);
				
					if (p.Valid)
					{
						OnRMCReceivedCallback.Call(_packet.GetMessageSource(), p.AllFiledsPresent,
												   p.Time, p.Latitude, p.Longitude, p.SpeedOverGround, p.CourseOverGround, p.Date);
						
						if (_active_prev != p.Active)
						{
							OnStatusChangedCallback.Call(p.Active);
							_active_prev = p.Active;
						}
					}
				}
				break;
			case PacketType::MessageZDA:
				{
					PacketZDA p(_packet);
				
					if (p.Valid)
					{
						OnZDAReceivedCallback.Call(_packet.GetMessageSource(), p.AllFiledsPresent,
												   p.Time, p.Day, p.Month, p.Year, p.LocalZoneHours, p.LocalZoneMinutes);
					}
				}
				break;
			}
		}
	};
	
	template <bool reserved>
		Implementer<reserved> *Implementer<reserved>::_selfPtr = 0;
}
}
}

#endif