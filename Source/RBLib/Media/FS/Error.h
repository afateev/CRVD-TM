
#ifndef _MEDIA_FS_ERROR_H_
#define _MEDIA_FS_ERROR_H_

namespace Rblib
{
namespace Media
{
namespace Fs
{

	namespace Error
	{
		enum
		{
			None = 0,
			NoOpenFile = 1,
			IOFault = 2,
			AlreadyExist = 3
		};
	}

}
}
}

#endif // _MEDIA_FS_ERROR_H_

