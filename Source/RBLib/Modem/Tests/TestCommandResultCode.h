///////////////////////////////////////////////////////////////////////////////
//	Код результата тестовой команды
//
///////////////////////////////////////////////////////////////////////////////

#ifndef MODEM_TESTS_TESTCOMMANDRESULTCODE_H
#define MODEM_TESTS_TESTCOMMANDRESULTCODE_H

namespace Rblib
{
namespace Modem
{
namespace Tests
{
	enum TestCommandResultCode
	{
		TestCommandResultCodeUnknown					= 0,
		TestCommandResultCodeNoError					= 1,
		TestCommandResultCodeMaxTestsCountExceeded 		= 2,
		TestCommandResultCodeInvalidTestType			= 3,
		TestCommandResultCodeNoMoreMemory				= 4,
		TestCommandResultCodeInvalidArgument			= 5,
	};
}
}
}

#endif