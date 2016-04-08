///////////////////////////////////////////////////////////////////////////////
//	������ ���������� ������� ������� ���������� ��������
//
//	������ ������ ������ ��������� �� ������ ������������� ��� �� ����������
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef TIMER_PARTS_H
#define TIMER_PARTS_H

namespace Rblib
{
// ����� �������� ����������� ��� ��������� MSP430
namespace Msp430
{
	/// ��������� �������� ���������� �������� (TxxCTL).
	struct TimerControlRegister
	{
		unsigned short InterruptFlag	: 1;	///< ���� ����������
		unsigned short InterruptEnable	: 1;	///< ���������� ����������
		unsigned short ClearTimer		: 1;	///< ���� ������� �������
		unsigned short					: 1;
		unsigned short Mode				: 2;	///< ����� ������
		unsigned short InputDivider		: 2;	///< �������� ��������� �������
		unsigned short ClockSource		: 2;	///< �������� ������������
		unsigned short 					: 6;
	};

	/// ��������� �������� ���������� �������� TimerB.
	struct TimerBControlRegister
	{
		unsigned short InterruptFlag	: 1;	///< ���� ����������
		unsigned short InterruptEnable	: 1;	///< ���������� ����������
		unsigned short ClearTimer		: 1;	///< ���� ������� �������
		unsigned short					: 1;
		unsigned short Mode				: 2;	///< ����� ������
		unsigned short InputDivider		: 2;	///< �������� ��������� �������
		unsigned short ClockSource		: 2;	///< �������� ������������
		unsigned short 					: 1;
		unsigned short CounterLength	: 2;	///< ����������� �������� �������
		unsigned short GroupingCompare	: 2;	///< ���������� ������������ ������ ���������
		unsigned short 					: 1;
	};

	/// ��������� �������� ��������� �������/��������.
	struct TimerCaptureCompareControlRegister
	{
		unsigned short InterruptFlag			: 1;	///< ���� ����������
		unsigned short CaptureOverflowFlag		: 1;	///< ������������ �������� �������
		unsigned short Output					: 1;	///< ��������� ������ � ������ OutputMode == 0
		unsigned short Input					: 1;	///< ��������� ����� (������ ��� ������)
		unsigned short InterruptEnable			: 1;	///< ���������� ����������
		unsigned short OutputMode				: 3;	///< ����� ������
		unsigned short EnableCapture			: 1;	///< ��������� ������, ����� ���������
		unsigned short							: 1;
		unsigned short SynchronizedInput		: 1;	///< ���� ��������������� (������ ��� ������)
		unsigned short SynchronizeCaptureSource : 1;	///< ���������������� ���� � ��������
		unsigned short CaptureInputSelect		: 2;	///< ����� ����� ��� �������
		unsigned short CaptureMode				: 2;	///< ����� �������
	};
	
	/// ���������� ��������� ����������.
	/// \param PointerType ��� (������) �������� ������� ���������.
	/// \param ControlRegisterAddr ����� �������� ����������.
	/// \param TimerControlRegisterStructure ��������� �������� ����������.
	template <class PointerType,
			PointerType ControlRegisterAddr,
			class TimerControlRegisterStructure = TimerControlRegister
		>
	class TimerControl
	{
	protected:
		
	public:
		/// ������ ������ �������.
		enum TimerMode
		{
			TimerModeStop		= 0,	///< ������ ����������
			TimerModeUp			= 1,	///< ������� �� ��������� ���������, ����� �����
			TimerModeContinuous = 2,	///< ������� �� 0xFFFF, ����� �����
			TimerModeUpDown		= 3		///< ������� �� ��������� ���������, ����� ������� �� ����
		};
		// �������� ���������� ������������
		enum TimerClockSource
		{
			TimerClockSourceExternalClock			= 0,	///< ������������ �� ������� "����" �������
			TimerClockSourceAuxClock				= 1,	///< �� ���������� ����������
			TimerClockSourceSubsystemMasterClock	= 2,	///< �� ���������� ����������
			TimerClockSourceInvertedExternalClock	= 3		///< ��������������� ������������ �� "����" �������
		};
		/// �������� ���������.
		enum TimerDivider
		{
			TimerDiv1 = 0,	///< /1
			TimerDiv2 = 1,	///< /2
			TimerDiv4 = 2,	///< /4
			TimerDiv8 = 3	///< /8
		};
	protected:
		/// ����� �������� ����������.
		static volatile TimerControlRegisterStructure * const _controlRegisterAddr;
	public:
		/// ������ �������� �������� ����������.
		static inline unsigned short GetControl()
		{
			return *(unsigned short *)_controlRegisterAddr;
		}
		/// ������ �������� �������� ����������.
		static inline void SetControl(unsigned short value)
		{
			*(unsigned short *)_controlRegisterAddr = value;
		}
		/// ���������� �������� ������������.
		static inline void SetClockSource(TimerClockSource source)
		{
			_controlRegisterAddr->ClockSource = source;
		}
		/// ���������� ����� ������ �������.
		static inline void SetMode(TimerMode mode)
		{
			_controlRegisterAddr->Mode = mode;
		}
		/// ��������� ����������.
		static inline void InterruptEnable()
		{
			_controlRegisterAddr->InterruptEnable = 1;
		}
		/// ��������� ����������.
		static inline void InterruptDisable()
		{
			_controlRegisterAddr->InterruptEnable = 0;
		}
		/// ����� �������.
		static inline void Reset()
		{
			_controlRegisterAddr->ClearTimer = 1;
		}
		/// ���������� �������� ��������� �������.
		static inline void SetDivider(TimerDivider value)
		{
			_controlRegisterAddr->InputDivider = value;
		}
	};

	/// ���������� ��������� ���������� TimerB.
	/// \tparam PointerType ��� (������) �������� ������� ���������.
	/// \tparam ControlRegisterAddr ����� �������� ����������.
	/// \tparam TimerControlRegisterStructure ��������� �������� ����������.
	template <class PointerType,
			PointerType ControlRegisterAddr,
			class TimerControlRegisterStructure = TimerBControlRegister
		>
	class TimerBControl : public TimerControl<PointerType, ControlRegisterAddr, TimerControlRegisterStructure>
	{
	private:
	  
	  typedef TimerControl<PointerType, ControlRegisterAddr, TimerControlRegisterStructure> _TimerControl;
	  
	public:

		/// ���������� ��������.
		enum CounterLength
		{
			/// 16 ���.
			CounterLength16Bit = 0,
			/// 12 ���.
			CounterLength12Bit = 1,
			/// 10 ���.
			CounterLength10bit = 2,
			/// 8 ���.
			CounterLength8Bit = 3,
		};

	public:

		/// ���������� ����������� ��������.
		static inline void SetCounterLength(CounterLength counterLength)
		{
			_TimerControl::_controlRegisterAddr->CounterLength = counterLength;
		}

	};

	/// ���������� ��������� ��������
	/// \tparam RegisterDataType ��� (������) �������� ��������.
	/// \tparam PointerType ��� (������) �������� ������� ���������.
	/// \tparam PointerType ����� �������� ��������.
	template <class RegisterDataType,
			class PointerType,
			PointerType CounterRegisterAddr
		>
	class TimerCounter
	{
	protected:
		/// ����� �������� ��������.
		static volatile RegisterDataType * const _counterRegisterAddr;
	public:
		/// ������ �������� ��������.
		static inline RegisterDataType GetCounter()
		{
			return *_counterRegisterAddr;
		}
		/// ������ �������� ��������.
		static inline void SetCounter(RegisterDataType value)
		{
			*_counterRegisterAddr = value;
		}
	};

	/// ���������� ��������� ������� ����������.
	/// \tparam RegisterDataType ��� (������) �������� ��������.
	/// \tparam PointerType ��� (������) �������� ������� ���������.
	/// \tparam InterruptVectorRegisterAddr ����� �������� ������� ����������.
	template <class RegisterDataType,
			class PointerType,
			PointerType InterruptVectorRegisterAddr
		>
	class TimerInterruptVector
	{
	protected:
		/// ����� �������� ��������.
		static volatile RegisterDataType * const _interruptVectorRegisterAddr;
	public:
		/// ������ �������� ������� ����������.
		static inline RegisterDataType GetInterruptVector()
		{
			return *_interruptVectorRegisterAddr;
		}
	};

	/// ���������� ��������� ��������� �������/���������.
	/// \tparam RegisterDataType ��� (������) �������� ��������.
	/// \tparam PointerType ��� (������) �������� ������� ���������.
	/// \tparam PointerType ����� �������� ��������� �������/���������.
	template <class RegisterDataType,
			class PointerType,
			PointerType CaptureCompareControlRegisterAddr
		>
	class TimerCaptureCompareControl
	{
	public:
		/// ������ ������ PWM ������.
		enum TimerPwmMode
		{
			TimerPwmModeOut			= 0,	///< ������ ��
			TimerPwmModeSet			= 1,	///< ������ PWM
			TimerPwmModeToggleReset	= 2,	///< ������� ��
			TimerPwmModeSetReset	= 3,	///< ������, ����������
			TimerPwmModeToggle		= 4,	///< ����� �����������,
			TimerPwmModeReset		= 5,	///< � ������ ������ �������.
			TimerPwmModeToggleSet	= 6,	///< ��. ��������
			TimerPwmModeResetSet	= 7		///< �����������
		};

		/// ������ �������.
		enum TimerCaptureMode
		{
			TimerCaptureModeNone = 0, ///< ��� �������
			TimerCaptureModeRising = 1, ///< �� ��������� ������
			TimerCaptureModeFalling = 2, ///< �� ������� ������
			TimerCaptureModeBoth = 3 ///< �� ����� �������
		};

		/// ������� ������� ��� ������ �������.
		enum TimerCaptureSignal
		{
			TimerCaptureInputA = 0, ///< CCIxA
			TimerCaptureInputB = 1, ///< CCIxB
			TimerCaptureInputC = 2, ///< GND
			TimerCaptureInputD = 3  ///< Vcc
		};

	protected:
		/// ����� �������� ��������� �������/���������.
		static volatile TimerCaptureCompareControlRegister * const _captureCompareControlRegisterAddr;
	public:
		/// ������ �������� �������� ����������.
		static inline RegisterDataType GetControl()
		{
			return *(RegisterDataType *)_captureCompareControlRegisterAddr;
		}
		/// ������ �������� �������� ����������.
		static inline void SetControl(RegisterDataType value)
		{
			*(RegisterDataType *)_captureCompareControlRegisterAddr = value;
		}
		/// ��������� ����������.
		static inline void InterruptEnable()
		{
			_captureCompareControlRegisterAddr->InterruptEnable = 1;
		}
		/// ��������� ����������.
		static inline void InterruptDisable()
		{
			_captureCompareControlRegisterAddr->InterruptEnable = 0;
		}
		/// ���������� "�������" ������� �� �������� ���� (����������� � ������ OutputNode == 0).
		static inline void CompareSetOutputHi()
		{
			_captureCompareControlRegisterAddr->Output = 1;
		}
		/// ���������� "������" ������� �� �������� ���� (����������� � ������ OutputNode == 0).
		static inline void CompareSetOutputLow()
		{
			_captureCompareControlRegisterAddr->Output = 0;
		}
		/// ���������� ����� ������ PWM.
		static inline void CompareSetMode(TimerPwmMode mode)
		{
			_captureCompareControlRegisterAddr->OutputMode = mode;
		}
		/// �������� ����� ��������� (����� Pwm).
		static inline void CompareEnable()
		{
			_captureCompareControlRegisterAddr->EnableCapture = 0;
		}

		/// �������� ����� �������.
		static inline void CaptureEnable()
		{
			_captureCompareControlRegisterAddr->EnableCapture = 1;
		}

		/// ������� ����� �������.
		static inline void CaptureSetMode(TimerCaptureMode mode)
		{
			_captureCompareControlRegisterAddr->CaptureMode = mode;
		}

		/// ������� ������ ��� �������.
		static inline void CaptureSetSignal(TimerCaptureSignal signal)
		{
			_captureCompareControlRegisterAddr->CaptureInputSelect = signal;
		}

		/// ��������� ���� ����������.
		static inline bool InterruptOccured()
		{
			return _captureCompareControlRegisterAddr->InterruptFlag;
		}

		/// �������� ���� ����������.
		static inline void ClearInterruptFlag()
		{
			_captureCompareControlRegisterAddr->InterruptFlag = 0;
		}
	};

	/// ���������� ��������� �������/���������.
	/// \tparam ��� (������) �������� ��������.
	/// \tparam ��� (������) �������� ������� ���������.
	/// \tparam ����� �������� �������/���������.
	template <class RegisterDataType,
			class PointerType,
			PointerType CaptureCompareRegisterAddr
		>
	class TimerCaptureCompareRegister
	{
	protected:
		/// ����� �������� ��������.
		static volatile RegisterDataType * const _captureCompareRegisterAddr;
	public:
		/// ������ �������� ��������.
		static inline RegisterDataType GetValue()
		{
			return *_captureCompareRegisterAddr;
		}
		/// ������ �������� ��������.
		static inline void SetValue(RegisterDataType value)
		{
			*_captureCompareRegisterAddr = value;
		}
	};

	/// ���������� ��������� ����������
	/// \tparam RegisterDataType ��� (������) �������� ��������.
	/// \tparam PointerType ��� (������) �������� ������� ���������.
	/// \tparam ExpansionRegisterAddr ����� �������� ����������.
	template <class RegisterDataType,
			class PointerType,
			PointerType ExpansionRegisterAddr
		>
	class TimerExpansionRegister
	{
	public:
		/// �������� �������������� ���������.
		enum TimerDividerEx
		{
			TimerDivEx1 = 0,	///< /1
			TimerDivEx2 = 1,	///< /2
			TimerDivEx3 = 2,	///< /3
			TimerDivEx4 = 3,	///< /4
			TimerDivEx5 = 4,	///< /5
			TimerDivEx6 = 5,	///< /6
			TimerDivEx7 = 6,	///< /7
			TimerDivEx8 = 7,	///< /8
		};
	protected:
		/// ����� �������� ��������.
		static volatile RegisterDataType * const _expansionRegisterAddr;
	public:
		/// ������ �������� ��������.
		static inline RegisterDataType GetExpansion()
		{
			return *_expansionRegisterAddr;
		}
		/// ������ �������� ��������.
		static inline void SetExpansion(RegisterDataType value)
		{
			*_expansionRegisterAddr = value;
		}
		/// ���������� �������������� �������� ��������� �������.
		static inline void SetDividerEx(TimerDividerEx value)
		{
			*_expansionRegisterAddr &= ~0x07;
			*_expansionRegisterAddr |= value;
		}
	};

	// ��������������� ����������� ����������

	// TimerControl

	template<class PointerType, PointerType ControlRegisterAddr, class TimerControlRegisterStructure>
	volatile TimerControlRegisterStructure* const TimerControl<PointerType, ControlRegisterAddr, TimerControlRegisterStructure>
		::_controlRegisterAddr = reinterpret_cast<volatile TimerControlRegisterStructure*>(ControlRegisterAddr);

	// TimerCounter

	template<class RegisterDataType, class PointerType, PointerType CounterRegisterAddr>
	volatile RegisterDataType* const TimerCounter<RegisterDataType, PointerType, CounterRegisterAddr>
		::_counterRegisterAddr = reinterpret_cast<volatile RegisterDataType*>(CounterRegisterAddr);

	// TimerInterruptVector

	template<class RegisterDataType, class PointerType, PointerType InterruptVectorRegisterAddr>
	volatile RegisterDataType* const TimerInterruptVector<RegisterDataType, PointerType, InterruptVectorRegisterAddr>
		::_interruptVectorRegisterAddr = reinterpret_cast<volatile RegisterDataType*>(InterruptVectorRegisterAddr);

	// TimerCaptureCompareControl

	template<class RegisterDataType, class PointerType, PointerType CaptureCompareControlRegisterAddr>
	volatile TimerCaptureCompareControlRegister* const TimerCaptureCompareControl<RegisterDataType, PointerType, CaptureCompareControlRegisterAddr>
		::_captureCompareControlRegisterAddr = reinterpret_cast<volatile  TimerCaptureCompareControlRegister*>(CaptureCompareControlRegisterAddr);

	// TimerCaptureCompareRegister

	template<class RegisterDataType, class PointerType, PointerType CaptureCompareRegisterAddr>
	volatile RegisterDataType* const TimerCaptureCompareRegister<RegisterDataType, PointerType, CaptureCompareRegisterAddr>
		::_captureCompareRegisterAddr = reinterpret_cast<volatile  RegisterDataType*>(CaptureCompareRegisterAddr);

	// TimerExpansionRegister

	template<class RegisterDataType, class PointerType, PointerType ExpansionRegisterAddr>
	volatile RegisterDataType* const TimerExpansionRegister<RegisterDataType, PointerType, ExpansionRegisterAddr>
		::_expansionRegisterAddr = reinterpret_cast<volatile  RegisterDataType*>(ExpansionRegisterAddr);
}
}

#endif