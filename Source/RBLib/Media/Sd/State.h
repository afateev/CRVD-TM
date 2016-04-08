
#ifndef _MEDIA_SD_STATE_H_
#define _MEDIA_SD_STATE_H_

namespace Rblib
{
namespace Media
{
namespace Sd
{
namespace State
{
    enum
    {
        Idle = 0,
        Ready = 1,
        Ident = 2,
        Standby = 3,
        Transfer = 4,
        Data = 5,
        Recveive = 6,
        Programming = 7,
        Disconnect = 8
    };
}
}
}
}

#endif // _MEDIA_SD_STATE_H_
