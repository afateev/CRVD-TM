
#ifndef _MEDIA_SD_COMMAND_SENDER_H_
#define _MEDIA_SD_COMMAND_SENDER_H_

#include "Cmd.h"
#include "ResponseSelect.h"

namespace Rblib
{
namespace Media
{
namespace Sd
{
    template<class SdApi, bool spiMode>
    class CommandSender
    {
    public:

        static inline bool Send(const Cmd& cmd)
        {
			for (char i = 0; i < SdApi::MaxCmdRepeats; i++)
			{
				if (SdApi::SendCommand(cmd.Code(), cmd.Argument(), Response::Type::None, 0)) 
					return true;
			}

			return false;
        }

        static inline bool Send(const Cmd& cmd, unsigned char respType, Response::Buffer& buffer)
        {
			for (char i = 0; i < SdApi::MaxCmdRepeats; i++)
			{
				if (SdApi::SendCommand(cmd.Code(), cmd.Argument(), respType, buffer.Ptr()))
					return true;
			}

            return false;
        }

        static inline bool SendApp(const Cmd& cmd, unsigned char respType, Response::Buffer buffer)
        {
            return SendApp(0, cmd, respType, buffer);
        }

        static inline bool SendApp(unsigned short rca, const Cmd& cmd, unsigned char respType, Response::Buffer buffer)
        {
            Cmd55 cmd55(rca);
			typename ResponseSelect<Cmd55, spiMode>::C response55;

			if(!Send(cmd55, ResponseSelect<Cmd55, spiMode>::T, response55))
                return false;

            if (!response55.ApplicationCommandEnabled())
                return false;

            return Send(cmd, respType, buffer);
        }
    };
}
}
}

#endif // _MEDIA_SD_COMMAND_SENDER_H_
