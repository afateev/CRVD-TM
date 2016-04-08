#ifndef MODEM_DEBUGDATAQUEUE_ITEM_H
#define MODEM_DEBUGDATAQUEUE_ITEM_H

namespace Rblib
{
namespace Modem
{
namespace DebugDataQueue
{
	template
		<
			class CodeType
		>
	class Item
	{
	public:
		CodeType Code;
		unsigned short DataCount;
		
		Item()
		{
			Code = 0;
			DataCount = 0;
		}
			
		Item(CodeType code, unsigned int dataCount)
		{
			Code = code;
			DataCount = dataCount;
		}
	};
}
}
}

#endif