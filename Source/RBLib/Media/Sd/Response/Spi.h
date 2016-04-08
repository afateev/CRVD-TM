
#ifndef _MEDIA_SD_RESPONSE_SPI_H_
#define _MEDIA_SD_RESPONSE_SPI_H_

#include "Buffer.h"

#include "Ocr.h"
#include "Cid.h"
#include "Csd.h"
#include "InterfaceCondition.h"
#include "Status.h"

namespace Rblib
{
namespace Media
{
namespace Sd
{
namespace Response
{
namespace Spi
{
    //
    // R1
    //

    class R1 : public Response::Buffer
    {
    public:

        R1() : Response::Buffer(_response, sizeof(_response))
        {
        }

		inline Status& Status()
		{
			return _status;
		}

		inline bool SetLenOk()
		{
			return !_status.Error();
		}

		inline bool ApplicationCommandEnabled()
		{
			return !_status.Error();
		}

		inline bool Initialized()
		{
			return !_status.Idle();
		}

    private:

#pragma pack(push, 1)
        union
        {
			unsigned char _response[sizeof(Response::Status)];
			Response::Status _status;
        };
#pragma pack (pop)

    };

    //
    // R1 CID
    //

	class R1CID : public Response::Buffer
	{
	public:

		R1CID() : Response::Buffer(_response, sizeof(_response))
		{
		}

		inline Status& Status()
		{
			return _status;
		}

		inline Cid& CID()
		{
			return _cid;
		}

	private:

#pragma pack(push, 1)
		union
		{
			unsigned char _response[sizeof(Response::Status) + sizeof(Response::Cid)];
			struct
			{
				Response::Status _status;
				Response::Cid _cid;
			};
		};
#pragma pack (pop)

	};

	//
	// R1 CSD
	//

	class R1CSD : public Response::Buffer
	{
	public:

		R1CSD() : Response::Buffer(_response, sizeof(_response))
		{
		}

		inline Response::Status& Status()
		{
			return _status;
		}

		inline const Response::Csd& CSD()
		{
			return _csd;
		}

	private:

#pragma pack(push, 1)
		union
		{
			unsigned char _response[sizeof(Response::Status) + sizeof(Response::Csd)];
			struct
			{
				Response::Status _status;
				Response::Csd _csd;
			};
		};
#pragma pack (pop)

	};

    //
    // R2
    //

    class R2 : public Response::Buffer
    {
    public:

        R2() : Response::Buffer(_response, sizeof(_response))
        {
        }

        inline Response::StatusEx& Status()
        {
            return _status;
        }

    protected:

#pragma pack(push, 1)
        union
        {
            unsigned char _response[sizeof(Response::StatusEx)];
            Response::StatusEx _status;
        };
#pragma pack (pop)

    };
    
    //
    // R3
    //

    class R3 : public Response::Buffer
    {
    public:

        R3() : Response::Buffer(_response, sizeof(_response))
        {
        }

        inline Response::Ocr& OCR()
        {
            return _ocr;
        }

        inline Response::Status& Status()
        {
            return _status;
        }

		inline bool Sdhc()
		{
			return _ocr.SdhcCard();
		}

    private:

#pragma pack(push, 1)
        union
        {
            unsigned char _response[sizeof(Response::Status) + sizeof(Response::Ocr)];
            struct
            {
                Response::Status _status;
                Response::Ocr _ocr;
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

        R7() : Buffer(_response, sizeof(_response))
        {
        }

        inline Response::InterfaceCondition& Condition()
        {
            return _condition;
        }

        inline Response::Status& Status()
        {
            return _status;
        }

    private:

#pragma pack(push, 1)
        union
        {
            unsigned char _response[sizeof(Response::Status) + sizeof(Response::InterfaceCondition)];
            struct
            {
                Response::Status _status;
                Response::InterfaceCondition _condition;
            };
        };
#pragma pack (pop)
    };
}
}
}
}
}

#endif // _MEDIA_SD_RESPONSE_SPI_H_
