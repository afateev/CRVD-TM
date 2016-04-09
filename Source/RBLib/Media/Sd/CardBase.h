
#ifndef _MEDIA_SD_CARD_BASE_H_
#define _MEDIA_SD_CARD_BASE_H_

#include "Cmd.h"
#include "ResponseSelect.h"
#include "CommandSender.h"
#include "State.h"

namespace Rblib
{
namespace Media
{
namespace Sd
{

	template<class SdApi, bool _spiMode>
	class CardBase
	{
	public:

		static const unsigned short SectorSize = 512;

	protected:

		typedef CommandSender<SdApi, _spiMode> Sender;

	public:

		CardBase()
		: _sdhc(false)
		, _rca(0)
		, _serial(0)
		{
			for (int i = 0; i < sizeof(_name); i++)
			{
				_name[i] = 0;
			}
			for (int i = 0; i < sizeof(_appId); i++)
			{
				_appId[i] = 0;
			}
		}

    public:

		//
        // init steps
		//

        inline bool SetIdleState()
        {
            Cmd0 cmd0;
			typename ResponseSelect<Cmd0, _spiMode>::C resp0;

            if (!Sender::Send(cmd0, ResponseSelect<Cmd0, _spiMode>::T, resp0))
                return false;

            return true;
        }

        inline bool VoltageValidate()
        {
            Cmd8 cmd8(0xAA);
            typename ResponseSelect<Cmd8, _spiMode>::C resp8;

			if (!Sender::Send(cmd8, ResponseSelect<Cmd8, _spiMode>::T, resp8))
                return false;

            if (resp8.Condition().CheckPattern() != 0xAA)
              return false;

            return true;
        }

        bool GetCardInfo()
        {
            Cmd10 cmd10;
            typename ResponseSelect<Cmd10, _spiMode>::C resp10;

			if (!Sender::Send(cmd10, ResponseSelect<Cmd10, _spiMode>::T, resp10))
                return false;

            SetParams(resp10.CID());

            return true;
        }

		bool SetBlockLen()
		{
			if (_sdhc)
				return true;

			Cmd16 cmd16(SectorSize);
			typename ResponseSelect<Cmd16, _spiMode>::C resp16;

			if (!Sender::Send(cmd16, ResponseSelect<Cmd16, _spiMode>::T, resp16))
				return false;

			return resp16.SetLenOk();
		}

		//
		// 
		//

		inline bool WriteSectorInit(unsigned long address)
		{
			Cmd24 cmd24(address);
			typename ResponseSelect<Cmd24, _spiMode>::C resp24;

			if (!Sender::Send(cmd24, ResponseSelect<Cmd24, _spiMode>::T, resp24))
				return false;

			return true;
		}

		inline bool ReadSectorInit(unsigned long address)
		{
			Cmd17 cmd17(address);
			typename ResponseSelect<Cmd17, _spiMode>::C resp17;

			if (!Sender::Send(cmd17, ResponseSelect<Cmd17, _spiMode>::T, resp17))
				return false;

			return true;
		}

        //
		// seters for card params fields
		//

		void SetParams(Response::Cid& cid)
        {
            SetDate(cid.ManufacturingDate());
            SetRevision(cid.Revision());
            SetName(cid.Name());
            SetAppId(cid.ApplicationId());
            _serial = cid.Serial();
            _manufacturerId = cid.ManufacturerId();
        }

        void SetDate(unsigned short responseValue)
        {
            _year = (responseValue >> 4) + 2000;
            _month = responseValue & 0x000F;
        }

        void SetRevision(unsigned char responseValue)
        {
            _revisionMinor = responseValue >> 4;
            _revisionMajor = responseValue & 0x0F;
        }

        void SetName(char* responseValue)
        {
            strncpy(_name, responseValue, 5);
            _name[5] = 0;
        }

        void SetAppId(char* responseValue)
        {
            _appId[0] = responseValue[0];
            _appId[1] = responseValue[1];
            _appId[2] = 0;
        }

	protected:

		unsigned short _year;
		unsigned char _month;
		unsigned char _revisionMinor;
		unsigned char _revisionMajor;
		char _name[6];
		char _appId[3];
		unsigned long _serial;
		unsigned char _manufacturerId;

		unsigned short _rca;

		bool _sdhc;
		unsigned long _size;
    };

}
}
}

#endif // _MEDIA_SD_CARD_BASE_H_

