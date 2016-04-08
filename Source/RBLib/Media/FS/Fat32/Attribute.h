
#ifndef _MEDIA_FS_FAT32_ATTRIBUTE_H_
#define _MEDIA_FS_FAT32_ATTRIBUTE_H_

namespace Rblib
{
namespace Media
{
namespace Fs
{
namespace Fat32
{
	namespace Attribute
	{

		enum
		{
			ReadOnly = 1,
			Hidden = 2,
			System = 4,
			VolumeId = 8,
			Directory = 16,
			Archive = 32,
			LongName = ReadOnly | Hidden | System | VolumeId
		};

	}
}
}
}
}

#endif // _MEDIA_FS_FAT32_ATTRIBUTE_H_

