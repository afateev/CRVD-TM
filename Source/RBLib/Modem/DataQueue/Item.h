#ifndef MODEM_DATAQUEUE_ITEM_H
#define MODEM_DATAQUEUE_ITEM_H

namespace Rblib
{
namespace Modem
{
namespace DataQueue
{
	template
		<
			class AddressType
		>
	class Item
	{
	public:
		AddressType SourceAddress;
		unsigned int DataCount;
		
		Item()
		{
			SourceAddress = 0;
			DataCount = 0;
		}
			
		Item(AddressType sourceAddress, unsigned int dataCount)
		{
			SourceAddress = sourceAddress;
			DataCount = dataCount;
		}
	};
}
}
}

#endif