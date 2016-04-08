
#ifndef _MEDIA_SD_RESPONSE_SD_H_
#define _MEDIA_SD_RESPONSE_SD_H_

#include "Buffer.h"

#include "Ocr.h"
#include "Cid.h"
#include "InterfaceCondition.h"
#include "../State.h"

namespace Rblib
{
namespace Media
{
namespace Sd // SD карта
{
namespace Response
{
namespace Sd // режим SD
{
    //
    // R1
    //

    class R1 : public Buffer
    {
    public:

        R1() : Buffer((unsigned char*)&_response, sizeof(_response))
        {
        }

		inline bool SetLenOk()
		{
			return ReadyForData() && _state != Media::Sd::State::Transfer;
		}

        inline bool ApplicationCommandEnabled()
        {
            return _appCommandEnabled;
        }

        inline unsigned char State()
        {
            return _state;
        }

        inline bool ReadyForData()
        {
            return _readyForData;
        }

    private:

#pragma pack(push, 1)
        union
        {
            unsigned long _response;
            struct
            {
                unsigned char : 3;
                unsigned char _authError : 1;
                unsigned char : 1;
                unsigned char _appCommandEnabled : 1;
                unsigned char : 2;
                unsigned char _readyForData : 1;
                unsigned char _state : 4;
                unsigned char _erase : 1;
                unsigned char _eccDisabled : 1;
                unsigned char _wpEraseSkip : 1;
                unsigned char _csdOverWrite : 1;
                unsigned char : 2;
                unsigned char _error : 1;
                unsigned char _controllerError : 1;
                unsigned char _eccFailed : 1;
                unsigned char _illegalCommand : 1;
                unsigned char _crcError : 1;
                unsigned char _lockUnlockFailed : 1;
                unsigned char _locked : 1;
                unsigned char _wpViolation : 1;
                unsigned char _eraseParamsError : 1;
                unsigned char _eraseError : 1;
                unsigned char _blockLenError : 1;
                unsigned char _addressError : 1;
                unsigned char _outOfRage : 1;
            };
        };
#pragma pack (pop)

    };

    //
    // R2 CID
    //

    class R2CID : public Buffer
    {
    public:

        R2CID() : Buffer((unsigned char*) &_response, sizeof(_response))
        {
        }

		inline Cid& CID()
		{
			return _cid;
		}

    protected:

#pragma pack(push, 1)
        union
        {
            unsigned char _response[15];
			Cid _cid;
        };
#pragma pack (pop)

    };
    
    //
    // R2 CSD
    //

    class R2CSD : public Buffer
    {
    public:

        R2CSD() : Buffer((unsigned char*) &_response, sizeof(_response))
        {
        }

    protected:

#pragma pack(push, 1)
        union
        {
            unsigned char _response[15];
            struct
            {

            };
        };
#pragma pack (pop)

    };

    //
    // R3
    //

    class R3 : public Buffer
    {
    public:

        R3() : Buffer((unsigned char*) &_response, sizeof(_response))
        {
        }

		inline Ocr& OCR()
		{
			return _ocr;
		}

		inline bool Sdhc()
		{
			return _ocr.SdhcCard();
		}

    private:

#pragma pack(push, 1)
        union
        {
            unsigned long _response;
			Ocr _ocr;
        };
#pragma pack (pop)

    };

    //
    // R6
    //

    class R6 : public Buffer
    {
    public:

        R6() : Buffer((unsigned char*) &_response, sizeof(_response))
        {
        }

        inline unsigned short Rca()
        {
            return _rca;
        }

        inline unsigned char State()
        {
            return _state;
        }

#pragma pack(push, 1)
        union
        {
            unsigned long _response;
            struct
            {
                // status
                unsigned char : 3;
                unsigned char _authError : 1;
                unsigned char : 1;
                unsigned char _appCommandEnabled : 1;
                unsigned char : 2;
                unsigned char _readyForData : 1;
                unsigned char _state : 4;
                unsigned char _error : 1;
                unsigned char _illegalCommand : 1;
                unsigned char _crcError : 1;
                // rca
                unsigned short _rca;
            };
        };
#pragma pack (pop)

    };

    //
    // R7
    //

    class R7 : public Buffer
    {
    public:

        enum Voltage
        {
            VoltageDefault1 = 1
        };

    public:

        R7() : Buffer((unsigned char*) &_response, sizeof(_response))
        {
        }

		inline InterfaceCondition& Condition()
		{
			return _condition;
		}

    private:

#pragma pack(push, 1)
        union
        {
            unsigned long _response;
			InterfaceCondition _condition;
        };
#pragma pack (pop)
    };
}
}
}
}
}

#endif // _MEDIA_SD_RESPONSE_SD_H_

