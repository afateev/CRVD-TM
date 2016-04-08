
#ifndef _MEDIA_SD_CARD_SPI_H_
#define _MEDIA_SD_CARD_SPI_H_

#include "CardBase.h"
#include "../Media.h"

namespace Rblib
{
namespace Media
{
namespace Sd
{
	template<class SdApi, bool _spiMode>
	class Card;

	template<class SdApi>
	class Card<SdApi, true> : public CardBase<SdApi, true>, public Media
	{
	protected:

        typedef CardBase<SdApi, true> Base;
		typedef typename SdApi::ErrorsCounter ErrorsCounter;

	public:
	  
		static const unsigned short SectorSize = Base::SectorSize;
		
	public:

		virtual bool Init()
		{
            SdApi::Init();

            if (!Base::SetIdleState())
			{
				if (!Base::SetIdleState())
					return false;
			}

            if (!Base::VoltageValidate())
                return false;

            if (!InitCycle())
                return false;

			if (!CheckCsd())
			  return false;

            if (!Base::GetCardInfo())
                return false;

			if (!Base::SetBlockLen())
				return false;

			if (!CrcCheckOn())
				return false;

            return true;
        }

        virtual bool WriteSector(unsigned long address, unsigned char* buffer)
        {
			// для обычных карт нужен полный адрес, а не номер сектора
			if (Base::_sdhc == false)
				address *= Base::SectorSize;

			for (int i = 0; i < SdApi::MaxCmdRepeats; i++)
			{
				if (!Base::WriteSectorInit(address))
					return false;

				// если успешно, то проверим статус
				if (SdApi::SendDataBlock(buffer, Base::SectorSize) && CheckStatus())
					return true;
			}

			return false;
        }

        virtual bool ReadSector(unsigned long address, unsigned char* buffer)
        {
			// для обычных карт нужен полный адрес, а не номер сектора
			if (Base::_sdhc == false)
				address *= Base::SectorSize;

			for (int i = 0; i < SdApi::MaxCmdRepeats; i++)
			{
				if (!Base::ReadSectorInit(address))
					return false;

				// если успешно, то выходим
				if (SdApi::ReadDataBlock(buffer, Base::SectorSize))
					return true;
			}

            return false;
        }

		inline const ErrorsCounter& Errors()
		{
			return SdApi::Errors();
		}

    protected:

		//
        // init steps
		//

        bool InitCycle()
        {
            ACmd41 acmd41(ACmd41::VoltageLevel3);
            typename ResponseSelect<ACmd41, true>::C resp41;

            do
            {
                if (!Base::Sender::SendApp(acmd41, ResponseSelect<ACmd41, true>::T, resp41))
                    return false;
            } while(!resp41.Initialized());

            return true;
        }

		// OCR сейчас не анализируется, т.к. флаг CCS выставляется неправильный
		// тип флешки (SDHC или нет) определяется при анализе регистра CSD (см. ниже)
        bool CheckOcr()
        {
            Cmd58 cmd58;
            typename ResponseSelect<Cmd58, true>::C resp58;

            if (!Base::Sender::Send(cmd58, ResponseSelect<Cmd58, true>::T, resp58))
                return false;

            Base::_sdhc = resp58.Sdhc();
			
			return true;
        }

        bool CheckCsd()
        {
            Cmd9 cmd9;
            typename ResponseSelect<Cmd9, true>::C resp9;

            if (!Base::Sender::Send(cmd9, ResponseSelect<Cmd9, true>::T, resp9))
                return false;

			Base::_sdhc = resp9.CSD().Sdhc();
			Base::_size = resp9.CSD().Size();

			return true;
        }

		inline bool CheckStatus()
		{
			Cmd13 cmd13;
			typename ResponseSelect<Cmd13, true>::C resp13;

			if (!Base::Sender::Send(cmd13, ResponseSelect<Cmd13, true>::T, resp13))
				return false;

			return !resp13.Status().Error();
		}

		inline bool CrcCheckOn()
		{
			Cmd59 cmd59(true);
			typename ResponseSelect<Cmd59, true>::C resp59;

			if (!Base::Sender::Send(cmd59, ResponseSelect<Cmd59, true>::T, resp59))
				return false;

			return true;
		}

    };

}
}
}

#endif // _MEDIA_SD_CARD_SPI_H_

