///////////////////////////////////////////////////////////////////////////////
//	Типы тестов
//
///////////////////////////////////////////////////////////////////////////////

#ifndef MODEM_TESTS_TESTTYPECODE_H
#define MODEM_TESTS_TESTTYPECODE_H

namespace Rblib
{
namespace Modem
{
namespace Tests
{
	enum TestTypeCode
	{
		TestTypeCodeUnknown 		= 0,
		TestTypeCodeSensitivity 	= 1,
		TestTypeCodeLink 			= 2,
		TestTypeCodeOutputPower		= 3,
		TestTypeCodeDataDelivery	= 4,
	};
}
}
}

#endif