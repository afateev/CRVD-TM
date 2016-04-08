
#ifndef _MEDIA_FS_IO_FLAGS_H_
#define _MEDIA_FS_IO_FLAGS_H_

namespace Rblib
{
namespace Media
{
namespace Fs
{

	namespace IOFlags
	{
		typedef unsigned char T;

		enum
		{
			In = 1,
			Out = 2,
			App = 4
		};
	}

}
}
}

#endif // _MEDIA_FS_IO_FLAGS_H_

