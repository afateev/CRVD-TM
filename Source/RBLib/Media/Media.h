///////////////////////////////////////////////////////////////////////////////
//	Базовый класс драйвера носителя информации
//	
//
///////////////////////////////////////////////////////////////////////////////

#ifndef MEDIA_MEDIA_MEDIA_H
#define MEDIA_MEDIA_MEDIA_H

namespace Rblib
{
namespace Media
{
	class Media
	{
	public:
		Media()
		{
		}
		
		virtual ~Media()
		{
		}

		// Первоначальная инициализация
		virtual bool Init()
		{
			return false;
		}

		// Основной цикл драйвера
		virtual void Run()
		{
			// предполагается что здесь идёт контроль состояния носителя информации и обновление внутренних флагов состояний
		}

		// Готовность носителя к работе
		virtual bool Ready()
		{
			return false;
		}

		// Емкость носителя в секторах
		virtual unsigned long long SectorsCount()
		{
			return 0;
		}

		// Размер сектора
		virtual unsigned int SectorSize()
		{
			return 0;
		}

		// Прочитать сектор под номером sector в бувер buffer
		virtual bool ReadSector(unsigned long sector, unsigned char *buffer)
		{
			return false;
		}

		// Записать данные data в сектор под номером sector
		virtual bool WriteSector(unsigned long sector, unsigned char *data)
		{
			return false;
		}
	};
}
}

#endif
