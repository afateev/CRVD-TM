#ifndef MODEM_TESTS_DATAITEM_H
#define MODEM_TESTS_DATAITEM_H

#include "TestTypeCode.h"

namespace Rblib
{
namespace Modem
{
namespace Tests
{
	class DataItem
	{
	public:
		TestTypeCode TestType;
		int TestId;
		unsigned int DataCount;
		
		DataItem()
		{
			TestType = TestTypeCodeUnknown;
			TestId = -1;
			DataCount = 0;
		}
			
		DataItem(unsigned char testType, int testId, unsigned int dataCount)
		{
			TestType = (TestTypeCode)testType;
			TestId = testId;
			DataCount = dataCount;
		}
	};
}
}
}

#endif