
#ifndef _MEDIA_SD_CARD_SD_H_
#define _MEDIA_SD_CARD_SD_H_

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
	class Card<SdApi, false> : public CardBase<SdApi, false>, public Media
	{
	protected:

		typedef CardBase<SdApi, false> Base;

	public:
		
		static const unsigned short SectorSize = Base::SectorSize;
		
	public:

		bool Init()
		{
            SdApi::Init();

            if (!Base::SetIdleState())
                return false;

            if (!Base::VoltageValidate())
                return false;

            if (!InitCycle())
                return false;

            if (!Base::GetCardInfo())
                return false;

            if (!GetRca())
                return false;

            if (!SetTransferState())
                return false;

            if (!SetBusWidth(true))
                return false;

			if (!Base::SetBlockLen())
				return false;

            return true;
        }

        bool WriteSector(unsigned long address, unsigned char* buffer)
        {
            SdApi::SetWriteMode();

			if (!Base::WriteSectorInit(address))
				return false;

            if (!WaitState(State::Recveive))
                return false;

            if (!SdApi::SendDataBlock(buffer, Base::SectorSize))
                return false;

            if (!WaitState(State::Transfer))
                return false;

            return true;
        }

        bool ReadSector(unsigned long address, unsigned char* buffer)
        {
            SdApi::SetReadMode();

			if (!Base::ReadSectorInit(address))
				return false;

            if (!WaitState(State::Data))
                return false;

            if (!WaitState(State::Transfer))
                return false;

            if (!SdApi::ReadDataBlock(buffer, Base::SectorSize))
                return false;

            return true;
        }

    protected:

		//
        // init steps
		//

        bool InitCycle()
        {
            ACmd41 acmd41(ACmd41::VoltageLevel3);
            typename ResponseSelect<ACmd41, false>::C resp41;

            do
            {
                if (!Base::Sender::SendApp(acmd41, ResponseSelect<ACmd41, false>::T, resp41))
                    return false;
            } while(!resp41.OCR().PowerUpFinished());

			Base::_sdhc = resp41.OCR().SdhcCard();

            return true;
        }

        bool GetRca()
        {
            Cmd3 cmd3;
            typename ResponseSelect<Cmd3, false>::C resp3;

            do
            {
                if (!Base::Sender::Send(cmd3, ResponseSelect<Cmd3, false>::T, resp3))
                    return false;
            } while (resp3.State() != State::Standby);

            Base::_rca = resp3.Rca();

            return true;
        }

		bool SetBusWidth(bool enable4BitBus)
		{
			ACmd6 acmd6(enable4BitBus);
			typename ResponseSelect<ACmd6, false>::C resp6;

			if (!Base::Sender::SendApp(Base::_rca, acmd6, ResponseSelect<ACmd6, false>::T, resp6))
				return false;

			if (!resp6.ReadyForData() || resp6.State() != State::Transfer)
				return false;

			return true;
		}

		//
		// Текущее состояние
		//

        bool SetTransferState()
        {
            Cmd7 cmd7(Base::_rca);
            typename ResponseSelect<Cmd7, false>::C resp7;

            if (!Base::Sender::Send(cmd7, ResponseSelect<Cmd7, false>::T, resp7))
                return false;

            if (!WaitState(State::Transfer))
                return false;

            return true;
        }

        bool WaitState(unsigned char state)
        {
			Cmd13 cmd13(Base::_rca);
            typename ResponseSelect<Cmd13, false>::C resp13;

            do
            {
                if (!Base::Sender::Send(cmd13, ResponseSelect<Cmd13, false>::T, resp13))
                    return false;
            } 
            while(resp13.State() != state);

            return true;
        }

    };

}
}
}

#endif // _MEDIA_SD_CARD_SD_H_

