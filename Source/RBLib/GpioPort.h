///////////////////////////////////////////////////////////////////////////////
//	������ ���������� ������ �����-������
//	�.�. �� ������ ���������� ����� ����� ������� ����������� ������� ����������,
//	�� ������� ���������� ������� ��������� �� ��������� �������
//
//	GpioPortInputOutputDirection - ����� ������/������ � ������� ����������� ������ "�����" �����
//	GpioPortFunctionSelect - ����� ���������� ��������� "�����" �����
//	GpioPortInterrupt - ����� ���������� ������������ �����
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef GPIOPORT_H
#define GPIOPORT_H

namespace Rblib
{
	
	/// ����� ������/������ � ������� ����������� ������ "�����" �����
	template <class PortValueDataType,			// ��� (������) �������� ����� �����-������
			class PointerType,					// ��� (������) �������� ������� ���������
			PointerType InputRegisterAddr,		// ����� �������� ��� ������ ��������
			PointerType OutputRegisterAddr,		// ����� �������� ��� ������ ��������
			PointerType DirectionRegisterAddr	// ����� �������� ��� ������� ���������� �����-������
		>
	class GpioPortInputOutputDirection
	{
	protected:
		// ����� �������� ������ ������ � �����
		static volatile PortValueDataType * const _inputRegisterAddr;
		// ����� �������� ������ ������ � ����
		static volatile PortValueDataType * const _outputRegisterAddr;
		// ����� �������� ��������� ����������� ������ "�����" �����
		static volatile PortValueDataType * const _directionRegisterAddr;
	public:
		/*!	\brief ������ �������� ����� �����/������
			\return �������� �������� ����� �����/������, � ������� ������ ��� �������� ��������� ������ ����� �����/������ �����
		*/
		static inline PortValueDataType Read(void)
		{
			return *_inputRegisterAddr;
		}
		
		/*!	\brief ������ �������� ����� ����� �����/������ �����
			\param pinNumber - ����� ����� (������� � 0)
			\return true - �� ����� "�������" �������, false - �� ����� "������ �������"
		*/
		static inline PortValueDataType Read(unsigned char pinNumber)
		{
			return *_inputRegisterAddr & (1 << pinNumber);
		}
		
		/*! \brief ������ �������� ����� �����/������
			\param value - ����� �������� �������� �����
		*/
		static inline void Write(PortValueDataType value)
		{
			*_outputRegisterAddr = value;
		}

		/*!	\brief ��������� �������� ����� ����� � ���������� 1
			\param pinNumber - ����� ����� (������� � 0)
		*/
		static inline void SetBit(unsigned char pinNumber)
		{
			*_outputRegisterAddr |= (1 << pinNumber);
		}

		/*!	\brief ��������� �������� ����� ����� � ���������� 0
			\param pinNumber - ����� ����� (������� � 0)
		*/
		static inline void ClearBit(unsigned char pinNumber)
		{
			*_outputRegisterAddr &= ~(1 << pinNumber);
		}
		
		/*!	\brief ������������ ������ ����� �����
			\param pinNumber - ����� ����� (������� � 0)
		*/
		static inline void ToggleBit(unsigned char pinNumber)
		{
			*_outputRegisterAddr ^= (1 << pinNumber);
		}

		/*!	\brief ������ ����� ����������� �����/������ ����� �����
			\param mask - ����� ����������� ����� ������ (0 - ����� �������� ��� �������� ����, 1 - ��� �������� �����)
		*/
		static inline void SetDirection(PortValueDataType mask)
		{
			*_directionRegisterAddr = mask;
		}

		/*!	\brief ��������� ����� ��� �������� �����
			\param pinNumber - ����� ����� (������� � 0)
		*/
		static inline void SetOutputPin(unsigned char pinNumber)
		{
			*_directionRegisterAddr |= (1 << pinNumber);
		}

		/*!	\brief ��������� ����� ��� �������� ����
			\param pinNumber - ����� ����� (������� � 0)
		*/
		static inline void SetInputPin(unsigned char pinNumber)
		{
			*_directionRegisterAddr &= ~(1 << pinNumber);
		}
	};

	/// ����� ��������� ����� ����� �� ������ � ���������� ��� ��� ����� �����/������
	template <class PortValueDataType,					// ��� (������) �������� ����� �����-������
			class PointerType,							// ��� (������) �������� ������� ���������
			PointerType FunctionSelectRegisterAddr		// ����� �������� ��� ��������� ����������� ������� "���" �����
		>
	class GpioPortFunctionSelect
	{
	protected:
		// ����� �������� ��������� ����������� ������� "���" �����
		static volatile PortValueDataType * const _functionSelectRegisterAddr;
	public:
		/*!	\brief ��������� ����� ��� �������� ����/�����
			\param pinNumber - ����� ����� (������� � 0)
		*/
		static inline void SetFunctionGeneralIo(unsigned char pinNumber)
		{
			*_functionSelectRegisterAddr &= ~(1 << pinNumber);
		}
		
		/*!	\brief ��������� ����� �� ������ � ����������
			\param pinNumber - ����� ����� (������� � 0)
		*/
		static inline void SetFunctionPeripheral(unsigned char pinNumber)
		{
			*_functionSelectRegisterAddr |= (1 << pinNumber);
		}
	};

	/// ����� ���������� ������������ �����
	template <class PortValueDataType,					// ��� (������) �������� ����� �����-������
			class PointerType,							// ��� (������) �������� ������� ���������
			PointerType InterruptFlagRegisterAddr,		// ����� �������� ������ ����������
			PointerType InterruptEdgeSelectAddr,		// ����� �������� ��������� ���������� �� ������
			PointerType InterruptEnableRegisterAddr		// ����� �������� ���������� ����������
		>
	class GpioPortInterrupt
	{
	protected:
		// ����� �������� ������ ����������
		static volatile PortValueDataType * const _interruptFlagRegisterAddr;
		// ����� �������� ��������� ���������� �� ������
		static volatile PortValueDataType * const _interruptEdgeSelectRegisterAddr;
		// ����� �������� ���������� ����������
		static volatile PortValueDataType * const _interruptEnableRegisterAddr;
	public:
		/*!	\brief ������ �������� ������ ����������
			\return ������� ����� ������ ���������� �����
		*/
		static inline PortValueDataType GetInterruptFlags(void)
		{
			return *_interruptFlagRegisterAddr;
		}
		
		/*!	\brief ������ ����� ����������
			\param bitNumber - ����� ����� (������� � 0)
			\return true - ���� ��������� ���� ����������
		*/
		static inline PortValueDataType GetInterruptFlag(unsigned char bitNumber)
		{
			return *_interruptFlagRegisterAddr & (1 << bitNumber);
		}
		
		/*!	\brief ������ �������� ������ ����������
			\param value - ����� �������� �������� ������ ����������
		*/
		static inline void SetInterruptFlags(PortValueDataType value)
		{
			*_interruptFlagRegisterAddr = value;
		}

		/*!	\brief ��������� ����� ����������
			\param bitNumber - ����� ����� (������� � 0)
		*/
		static inline void SetInterruptFlag(unsigned char bitNumber)
		{
			*_interruptFlagRegisterAddr |= (1 << bitNumber);
		}

		/*!	\brief ������� ����� ����������
			\param bitNumber - ����� ����� (������� � 0)
		*/
		static inline void ClearInterruptFlag(unsigned char bitNumber)
		{
			*_interruptFlagRegisterAddr &= ~(1 << bitNumber);
		}

		/*!	\brief ������ �������� ��������� ���������� �� ������
			\return �������� �������� ��������� ���������� �� ������
		*/
		static inline PortValueDataType GetInterruptEdgeSelect(void)
		{
			return *_interruptEdgeSelectRegisterAddr;
		}

		/*!	\brief ������ �������� �������� ������� ���������� �� ������
			\param bitNumber - ����� ����� (������� � 0)
			\return ����� ���������� �� ������
		*/
		static inline PortValueDataType GetInterruptEdgeSelect(unsigned char bitNumber)
		{
			return *_interruptEdgeSelectRegisterAddr & (1 << bitNumber);
		}

		/*!	\brief ������ �������� �������� ������� ���������� �� ������
			\param value - ����� �������� �������� ������� ���������� �� ������
		*/
		static inline void SetInterruptEdgeSelect(PortValueDataType value)
		{
			*_interruptEdgeSelectRegisterAddr = value;
		}

		/*!	\brief ��������� ���������� �� ����� ������
			\param bitNumber - ����� ����� (������� � 0)
		*/
		static inline void SetInterruptEdgeHiToLow(unsigned char bitNumber)
		{
			*_interruptEdgeSelectRegisterAddr |= (1 << bitNumber);
		}

		/*!	\brief ��������� ���������� �� ����������� ������
			\param bitNumber - ����� ����� (������� � 0)
		*/
		static inline void SetInterruptEdgeLowToHi(unsigned char bitNumber)
		{
			*_interruptEdgeSelectRegisterAddr &= ~(1 << bitNumber);
		}

		/*!	\brief ������ �������� ���������� ����������
			\return ������� ����� ������ ���������� ���������� �����
		*/
		// ������ �������� ���������� ����������
		static inline PortValueDataType GetInterruptEnable(void)
		{
			return *_interruptEnableRegisterAddr;
		}

		/*!	\brief ������ ���� ���������� ���������� �� ��������� ������ �� �����
			\param bitNumber - ����� ����� (������� � 0)
			\return true - ���� ���������� ���������
		*/
		static inline PortValueDataType GetInterruptEnable(unsigned char bitNumber)
		{
			return *_interruptEnableRegisterAddr & (1 << bitNumber);
		}

		/*!	\brief ������ �������� ���������� ����������
			\param value - ����� �������� �������� ���������� ����������
		*/
		static inline void SetInterruptEnable(PortValueDataType value)
		{
			*_interruptEnableRegisterAddr = value;
		}
		
		/*!	\brief ��������� ���������� �� ��������� ������ �� �����
			\param bitNumber - ����� ����� (������� � 0)
		*/
		static inline void InterruptEnable(unsigned char bitNumber)
		{
			*_interruptEnableRegisterAddr |= (1 << bitNumber);
		}

		/*!	\brief ��������� ���������� �� ��������� ������ �� �����
			\param bitNumber - ����� ����� (������� � 0)
		*/
		static inline void InterruptDisable(unsigned char bitNumber)
		{
			*_interruptEnableRegisterAddr &= ~(1 << bitNumber);
		}
	};

	/// ����� ���������� �������������� �����������
	template <class PortValueDataType,					// ��� (������) �������� ����� �����-������
			class PointerType,							// ��� (������) �������� ������� ���������
			PointerType ResistorRegisterAddr			// ����� �������� ��� ��������� ������������� ����������
		>
	class GpioPortResistor
	{
	protected:
		// ����� �������� ��������� ����������� ������� "���" �����
		static volatile PortValueDataType * const _resistorRegisterAddr;
	public:
		/*!	\brief �������� ������������� ��������
			\param pinNumber - ����� ����� (������� � 0)
		*/
		static inline void ResistorEnable(unsigned char pinNumber)
		{
			*_resistorRegisterAddr |= (1 << pinNumber);
		}
		
		/*!	\brief ��������� ������������� ��������
			\param pinNumber - ����� ����� (������� � 0)
		*/
		static inline void ResistorDisable(unsigned char pinNumber)
		{
			*_resistorRegisterAddr &= ~(1 << pinNumber);
		}
	};


	// ��������������� ����������� ����������

	// GpioPortInputOutputDirection

	template<class PortValueDataType, class PointerType, PointerType InputRegisterAddr, PointerType OutputRegisterAddr, PointerType DirectionRegisterAddr>
	volatile PortValueDataType* const GpioPortInputOutputDirection<PortValueDataType, PointerType, InputRegisterAddr, OutputRegisterAddr, DirectionRegisterAddr>
		::_inputRegisterAddr = reinterpret_cast<volatile PortValueDataType*>(InputRegisterAddr);

	template<class PortValueDataType, class PointerType, PointerType InputRegisterAddr, PointerType OutputRegisterAddr, PointerType DirectionRegisterAddr>
	volatile PortValueDataType* const GpioPortInputOutputDirection<PortValueDataType, PointerType, InputRegisterAddr, OutputRegisterAddr, DirectionRegisterAddr>
		::_outputRegisterAddr = reinterpret_cast<volatile PortValueDataType*>(OutputRegisterAddr);

	template<class PortValueDataType, class PointerType, PointerType InputRegisterAddr, PointerType OutputRegisterAddr, PointerType DirectionRegisterAddr>
	volatile PortValueDataType* const GpioPortInputOutputDirection<PortValueDataType, PointerType, InputRegisterAddr, OutputRegisterAddr, DirectionRegisterAddr>
		::_directionRegisterAddr = reinterpret_cast<volatile PortValueDataType*>(DirectionRegisterAddr);

	// GpioPortFunctionSelect

	template<class PortValueDataType, class PointerType, PointerType FunctionSelectRegisterAddr>
	volatile PortValueDataType* const GpioPortFunctionSelect<PortValueDataType, PointerType, FunctionSelectRegisterAddr>
		::_functionSelectRegisterAddr = reinterpret_cast<volatile PortValueDataType*>(FunctionSelectRegisterAddr);

	// GpioPortInterrupt

	template<class PortValueDataType, class PointerType, PointerType InterruptFlagRegisterAddr, PointerType InterruptEdgeSelectAddr, PointerType InterruptEnableRegisterAddr>
	volatile PortValueDataType* const GpioPortInterrupt<PortValueDataType, PointerType, InterruptFlagRegisterAddr, InterruptEdgeSelectAddr, InterruptEnableRegisterAddr>
		::_interruptFlagRegisterAddr = reinterpret_cast<volatile PortValueDataType*>(InterruptFlagRegisterAddr);

	template<class PortValueDataType, class PointerType, PointerType InterruptFlagRegisterAddr, PointerType InterruptEdgeSelectAddr, PointerType InterruptEnableRegisterAddr>
	volatile PortValueDataType* const GpioPortInterrupt<PortValueDataType, PointerType, InterruptFlagRegisterAddr, InterruptEdgeSelectAddr, InterruptEnableRegisterAddr>
		::_interruptEdgeSelectRegisterAddr = reinterpret_cast<volatile PortValueDataType*>(InterruptEdgeSelectAddr);

	template<class PortValueDataType, class PointerType, PointerType InterruptFlagRegisterAddr, PointerType InterruptEdgeSelectAddr, PointerType InterruptEnableRegisterAddr>
	volatile PortValueDataType* const GpioPortInterrupt<PortValueDataType, PointerType, InterruptFlagRegisterAddr, InterruptEdgeSelectAddr, InterruptEnableRegisterAddr>
		::_interruptEnableRegisterAddr = reinterpret_cast<volatile PortValueDataType*>(InterruptEnableRegisterAddr);

	// GpioPortResistor

	template<class PortValueDataType, class PointerType, PointerType ResistorRegisterAddr>
	volatile PortValueDataType* const GpioPortResistor<PortValueDataType, PointerType, ResistorRegisterAddr>
		::_resistorRegisterAddr = reinterpret_cast<volatile PortValueDataType*>(ResistorRegisterAddr);
}

#endif