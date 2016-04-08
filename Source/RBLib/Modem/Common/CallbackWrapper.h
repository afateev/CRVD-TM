///////////////////////////////////////////////////////////////////////////////
//	������� ��� ������� ��������� ������
//
///////////////////////////////////////////////////////////////////////////////

#ifndef MODEM_CALLBACK_H
#define MODEM_CALLBACK_H

namespace Rblib
{
namespace Modem
{
	template
		<
			class Prototype	// �������� ������� ��������� ������
		>
	class CallbackWrapperImpl
	{
	public:
		typedef Prototype *CallbackPtrType;
	protected:
		CallbackPtrType _callback;		// ��������� �� �������
		void *_callbackParam;			// �������� ������������� ���������, ������� �������� ��� ��������� �����������
	public:
		// ����������� �� ���������
		CallbackWrapperImpl()
		{
			Reset();
		}
		
		// ������ ���������� � �������� ��� ������������� ���������
		void Set(CallbackPtrType callback, void *callbackParam)
		{
			_callback = callback;
			_callbackParam = callbackParam;
		}
		
		// �������� ���������� �����������
		void Reset()
		{
			_callback = 0;
			_callbackParam = 0;
		}
		
	};
	
	// ��������� ������ � ����� �����������
	template
		<
			class Param1 = NullType,
			class Param2 = NullType,
			class Param3 = NullType,
			class Param4 = NullType,
			class Param5 = NullType
		>
	class CallbackWrapper : public CallbackWrapperImpl<void (void *, Param1, Param2, Param3, Param4, Param5)>
	{
	protected:
		typedef CallbackWrapperImpl<void (void *, Param1, Param2, Param3, Param4, Param5)> Base;
	public:
		void Call(Param1 param1, Param2 param2, Param3 param3, Param4 param4, Param5 param5)
		{
			if (Base::_callback)
			{
				Base::_callback(Base::_callbackParam, param1, param2, param3, param4, param5);
			}
		}
	};
	
	// ��������� ������ � �������� �����������
	template
		<
			class Param1,
			class Param2,
			class Param3,
			class Param4
		>
	class CallbackWrapper<Param1, Param2, Param3, Param4> : public CallbackWrapperImpl<void (void *, Param1, Param2, Param3, Param4)>
	{
	protected:
		typedef CallbackWrapperImpl<void (void *, Param1, Param2, Param3, Param4)> Base;
	public:
		void Call(Param1 param1, Param2 param2, Param3 param3, Param4 param4)
		{
			if (Base::_callback)
			{
				Base::_callback(Base::_callbackParam, param1, param2, param3, param4);
			}
		}
	};
	
	// ��������� ������ � ����� �����������
	template
		<
			class Param1,
			class Param2,
			class Param3
		>
	class CallbackWrapper<Param1, Param2, Param3> : public CallbackWrapperImpl<void (void *, Param1, Param2, Param3)>
	{
	protected:
		typedef CallbackWrapperImpl<void (void *, Param1, Param2, Param3)> Base;
	public:
		void Call(Param1 param1, Param2 param2, Param3 param3)
		{
			if (Base::_callback)
			{
				Base::_callback(Base::_callbackParam, param1, param2, param3);
			}
		}
	};
	
	// ��������� ������ � ����� �����������
	template
		<
			class Param1,
			class Param2
		>
	class CallbackWrapper<Param1, Param2> : public CallbackWrapperImpl<void (void *, Param1, Param2)>
	{
	protected:
		typedef CallbackWrapperImpl<void (void *, Param1, Param2)> Base;
	public:
		void Call(Param1 param1, Param2 param2)
		{
			if (Base::_callback)
			{
				Base::_callback(Base::_callbackParam, param1, param2);
			}
		}
	};
	
	// ��������� ������ � ����� ����������
	template
		<
			class Param1
		>
	class CallbackWrapper<Param1> : public CallbackWrapperImpl<void (void *, Param1)>
	{
	protected:
		typedef CallbackWrapperImpl<void (void *, Param1)> Base;
	public:
		void Call(Param1 param1)
		{
			if (Base::_callback)
			{
				Base::_callback(Base::_callbackParam, param1);
			}
		}
	};
	
	// ��������� ������ ��� ����������
	template<>
		class CallbackWrapper<> : public CallbackWrapperImpl<void (void *)>
	{
	protected:
		typedef void (Prototype)(void *);
	public:
		void Call()
		{
			if (_callback)
			{
				_callback(_callbackParam);
			}
		}
	};
}
}

#endif