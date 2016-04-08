///////////////////////////////////////////////////////////////////////////////
//	Элемент таблицы активности удаленного узла сети
//
///////////////////////////////////////////////////////////////////////////////

#ifndef MODEM_ACTIVITY_REMOTEACTIVITYTABLEITEM_H
#define MODEM_ACTIVITY_REMOTEACTIVITYTABLEITEM_H

namespace Rblib
{
namespace Modem
{
namespace Activity
{
#pragma pack(push, 1)

	class RemoteActivityTableItem
	{
	public:
		unsigned char Rate;
	public:
		RemoteActivityTableItem()
		{
			Reset();
		}
		
		void Reset()
		{
			Rate = 0;
		}
	};
	
#pragma pack (pop)
}
}
}

#endif