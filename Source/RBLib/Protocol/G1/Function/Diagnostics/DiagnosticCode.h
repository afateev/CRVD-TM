///////////////////////////////////////////////////////////////////////////////
//	ƒиагностические коды датчика
//
///////////////////////////////////////////////////////////////////////////////

#ifndef PROTOCOL_G1_FUNCTION_DIAGNOSTICS_PARAMCODE_H
#define PROTOCOL_G1_FUNCTION_DIAGNOSTICS_PARAMCODE_H

namespace Rblib
{
namespace Protocol
{
namespace G1
{
namespace Function
{
namespace Diagnostics
{
	enum DiagnosticCode
	{
		DiagnosticCodeDevice								= 0,
		DiagnosticCodeFramInit								= 1,
		DiagnosticCodeRealTimeClockInit						= 2,
		DiagnosticCodeRadioTransiverApiInit					= 3,
		DiagnosticCodeRadioTransiverPowerOn					= 4,
		DiagnosticCodeRadioTransiverApiTest					= 5,
		DiagnosticCodeRadioTransiverConfig					= 6,
		DiagnosticCodePowerLo								= 7,
		DiagnosticCodeSdCardResponseErrorCounter			= 8,
		DiagnosticCodeSdCardEraseResetErrorCounter			= 9,
		DiagnosticCodeSdCardEraseSequenceErrorCounter		= 10,
		DiagnosticCodeSdCardIllegalCommandErrorCounter		= 11,
		DiagnosticCodeSdCardCommandCrcErrorCounter			= 12,
		DiagnosticCodeSdCardCommandAddressErrorCounter		= 13,
		DiagnosticCodeSdCardCommandParameterErrorCounter	= 14,
		DiagnosticCodeSdCardCommandWaitErrorCounter			= 15,
		DiagnosticCodeSdCardWriteCrcErrorCounter			= 16,
		DiagnosticCodeSdCardWriteErrorCounter				= 17,
		DiagnosticCodeSdCardWriteWaitErrorCounter			= 18,
		DiagnosticCodeSdCardReadCrcErrorCounter				= 19,
		DiagnosticCodeSdCardReadWaitErrorCounter			= 20,
		DiagnosticCodeSdCardInit							= 21,
		DiagnosticCodeFileSystemCardInit					= 22,
		DiagnosticCodeModemLockOnTransiverErrorCounter		= 23,
		DiagnosticCodeModemLockOnNetConfigErrorCounter		= 24,
		DiagnosticCodeModemLockOnCommonErrorCounter			= 25,
	};
	
	// „исло используемых диагностических кодов
	static const unsigned int DiagnosticCodesUsedCount = 26;
	
	// «арезервированное число диагностических кодов
	// ¬нимание!!! ѕри изменении кол-ва, может потребоватьс€ изменение версии структуры результатов диагностики сохран€емой в пам€ти
	static const unsigned int MaxDiagnosticCodesCount = 100;
}
}
}
}
}

#endif