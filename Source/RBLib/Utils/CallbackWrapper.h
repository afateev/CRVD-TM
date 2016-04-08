///////////////////////////////////////////////////////////////////////////////
//	Обертка для функций обратного вызова
//
///////////////////////////////////////////////////////////////////////////////

#ifndef RBLIB_CALLBACKWRAPPER_H
#define RBLIB_CALLBACKWRAPPER_H

#include "../NullType.h"

namespace Rblib
{
	template
		<
			class Prototype,		// Прототип функции обратного вызова
			class ShortPrototype	// Прототип функции обратного вызова без обязательного параметра
		>
	class CallbackWrapperImpl
	{
	public:
		typedef Prototype *CallbackPtrType;
		typedef ShortPrototype *ShortCallbackPtrType;
	protected:
		CallbackPtrType _callback;				// указатель на функцию
		ShortCallbackPtrType _shortCallback;	// указатель на функцию
		void *_callbackParam;			// значение обязательного параметра, которое задается при установке обработчика
	public:
		// Конструктор по умолчанию
		CallbackWrapperImpl()
		{
			Reset();
		}
		
		// Задать обработчик и значение его обязательного параметра
		void Set(CallbackPtrType callback, void *callbackParam)
		{
			_callback = callback;
			_callbackParam = callbackParam;
		}
		
		// Задать обработчик
		void Set(ShortCallbackPtrType callback)
		{
			_shortCallback = callback;
		}
		
		// Отменить назначение обработчика
		void Reset()
		{
			_callback = 0;
			_shortCallback = 0;
			_callbackParam = 0;
		}
		
		inline bool Valid()
		{
			return _callback != 0 || _shortCallback != 0;
		}
	};
	
	// Рализация вызова с восемью параметрами
	template
		<
			class Param1 = NullType,
			class Param2 = NullType,
			class Param3 = NullType,
			class Param4 = NullType,
			class Param5 = NullType,
			class Param6 = NullType,
			class Param7 = NullType,
			class Param8 = NullType
		>
	class CallbackWrapper : public CallbackWrapperImpl<void (void *, Param1, Param2, Param3, Param4, Param5, Param6, Param7, Param8), void (Param1, Param2, Param3, Param4, Param5, Param6, Param7, Param8)>
	{
	protected:
		typedef CallbackWrapperImpl<void (void *, Param1, Param2, Param3, Param4, Param5, Param6, Param7, Param8), void (Param1, Param2, Param3, Param4, Param5, Param6, Param7, Param8)> Base;
	public:
		void Call(Param1 param1, Param2 param2, Param3 param3, Param4 param4, Param5 param5, Param6 param6, Param7 param7, Param8 param8)
		{
			if (Base::_callback)
			{
				Base::_callback(Base::_callbackParam, param1, param2, param3, param4, param5, param6, param7, param8);
			}
			
			if (Base::_shortCallback)
			{
				Base::_shortCallback(param1, param2, param3, param4, param5, param6, param7, param8);
			}
		}
		
		void operator ()(Param1 param1, Param2 param2, Param3 param3, Param4 param4, Param5 param5, Param6 param6, Param7 param7, Param8 param8)
		{
			Call(param1, param2, param3, param4, param5, param6, param7, param8);
		}
		
		void operator = (typename Base::ShortCallbackPtrType callback)
		{
			Base::Set(callback);
		}
	};
	
	// Рализация вызова с семью параметрами
	template
		<
			class Param1,
			class Param2,
			class Param3,
			class Param4,
			class Param5,
			class Param6,
			class Param7
		>
	class CallbackWrapper<Param1, Param2, Param3, Param4, Param5, Param6, Param7> : public CallbackWrapperImpl<void (void *, Param1, Param2, Param3, Param4, Param5, Param6, Param7), void (Param1, Param2, Param3, Param4, Param5, Param6, Param7)>
	{
	protected:
		typedef CallbackWrapperImpl<void (void *, Param1, Param2, Param3, Param4, Param5, Param6, Param7), void (Param1, Param2, Param3, Param4, Param5, Param6, Param7)> Base;
	public:
		void Call(Param1 param1, Param2 param2, Param3 param3, Param4 param4, Param5 param5, Param6 param6, Param7 param7)
		{
			if (Base::_callback)
			{
				Base::_callback(Base::_callbackParam, param1, param2, param3, param4, param5, param6, param7);
			}
			
			if (Base::_shortCallback)
			{
				Base::_shortCallback(param1, param2, param3, param4, param5, param6, param7);
			}
		}
		
		void operator ()(Param1 param1, Param2 param2, Param3 param3, Param4 param4, Param5 param5, Param6 param6, Param7 param7)
		{
			Call(param1, param2, param3, param4, param5, param6, param7);
		}
		
		void operator = (typename Base::ShortCallbackPtrType callback)
		{
			Base::Set(callback);
		}
	};
	
	// Рализация вызова с шестью параметрами
	template
		<
			class Param1,
			class Param2,
			class Param3,
			class Param4,
			class Param5,
			class Param6
		>
	class CallbackWrapper<Param1, Param2, Param3, Param4, Param5, Param6> : public CallbackWrapperImpl<void (void *, Param1, Param2, Param3, Param4, Param5, Param6), void (Param1, Param2, Param3, Param4, Param5, Param6)>
	{
	protected:
		typedef CallbackWrapperImpl<void (void *, Param1, Param2, Param3, Param4, Param5, Param6), void (Param1, Param2, Param3, Param4, Param5, Param6)> Base;
	public:
		void Call(Param1 param1, Param2 param2, Param3 param3, Param4 param4, Param5 param5, Param6 param6)
		{
			if (Base::_callback)
			{
				Base::_callback(Base::_callbackParam, param1, param2, param3, param4, param5, param6);
			}
			
			if (Base::_shortCallback)
			{
				Base::_shortCallback(param1, param2, param3, param4, param5, param6);
			}
		}
		
		void operator ()(Param1 param1, Param2 param2, Param3 param3, Param4 param4, Param5 param5, Param6 param6)
		{
			Call(param1, param2, param3, param4, param5, param6);
		}
		
		void operator = (typename Base::ShortCallbackPtrType callback)
		{
			Base::Set(callback);
		}
	};
	
	// Рализация вызова с пятью параметрами
	template
		<
			class Param1,
			class Param2,
			class Param3,
			class Param4,
			class Param5
		>
	class CallbackWrapper<Param1, Param2, Param3, Param4, Param5> : public CallbackWrapperImpl<void (void *, Param1, Param2, Param3, Param4, Param5), void (Param1, Param2, Param3, Param4, Param5)>
	{
	protected:
		typedef CallbackWrapperImpl<void (void *, Param1, Param2, Param3, Param4, Param5), void (Param1, Param2, Param3, Param4, Param5)> Base;
	public:
		void Call(Param1 param1, Param2 param2, Param3 param3, Param4 param4, Param5 param5)
		{
			if (Base::_callback)
			{
				Base::_callback(Base::_callbackParam, param1, param2, param3, param4, param5);
			}
			
			if (Base::_shortCallback)
			{
				Base::_shortCallback(param1, param2, param3, param4, param5);
			}
		}
		
		void operator ()(Param1 param1, Param2 param2, Param3 param3, Param4 param4, Param5 param5)
		{
			Call(param1, param2, param3, param4, param5);
		}
		
		void operator = (typename Base::ShortCallbackPtrType callback)
		{
			Base::Set(callback);
		}
	};
	
	// Рализация вызова с четырьмя параметрами
	template
		<
			class Param1,
			class Param2,
			class Param3,
			class Param4
		>
	class CallbackWrapper<Param1, Param2, Param3, Param4> : public CallbackWrapperImpl<void (void *, Param1, Param2, Param3, Param4), void (Param1, Param2, Param3, Param4)>
	{
	protected:
		typedef CallbackWrapperImpl<void (void *, Param1, Param2, Param3, Param4), void (Param1, Param2, Param3, Param4)> Base;
	public:
		void Call(Param1 param1, Param2 param2, Param3 param3, Param4 param4)
		{
			if (Base::_callback)
			{
				Base::_callback(Base::_callbackParam, param1, param2, param3, param4);
			}
			
			if (Base::_shortCallback)
			{
				Base::_shortCallback(param1, param2, param3, param4);
			}
		}
		
		void operator ()(Param1 param1, Param2 param2, Param3 param3, Param4 param4)
		{
			Call(param1, param2, param3, param4);
		}
		
		void operator = (typename Base::ShortCallbackPtrType callback)
		{
			Base::Set(callback);
		}
	};
	
	// Рализация вызова с тремя параметрами
	template
		<
			class Param1,
			class Param2,
			class Param3
		>
	class CallbackWrapper<Param1, Param2, Param3> : public CallbackWrapperImpl<void (void *, Param1, Param2, Param3), void (Param1, Param2, Param3)>
	{
	protected:
		typedef CallbackWrapperImpl<void (void *, Param1, Param2, Param3), void (Param1, Param2, Param3)> Base;
	public:
		void Call(Param1 param1, Param2 param2, Param3 param3)
		{
			if (Base::_callback)
			{
				Base::_callback(Base::_callbackParam, param1, param2, param3);
			}
			
			if (Base::_shortCallback)
			{
				Base::_shortCallback(param1, param2, param3);
			}
		}
		
		void operator ()(Param1 param1, Param2 param2, Param3 param3)
		{
			Call(param1, param2, param3);
		}
		
		void operator = (typename Base::ShortCallbackPtrType callback)
		{
			Base::Set(callback);
		}
	};
	
	// Рализация вызова с двумя параметрами
	template
		<
			class Param1,
			class Param2
		>
	class CallbackWrapper<Param1, Param2> : public CallbackWrapperImpl<void (void *, Param1, Param2), void (Param1, Param2)>
	{
	protected:
		typedef CallbackWrapperImpl<void (void *, Param1, Param2), void (Param1, Param2)> Base;
	public:
		void Call(Param1 param1, Param2 param2)
		{
			if (Base::_callback)
			{
				Base::_callback(Base::_callbackParam, param1, param2);
			}
			
			if (Base::_shortCallback)
			{
				Base::_shortCallback(param1, param2);
			}
		}
		
		void operator ()(Param1 param1, Param2 param2)
		{
			Call(param1, param2);
		}
		
		void operator = (typename Base::ShortCallbackPtrType callback)
		{
			Base::Set(callback);
		}
	};
	
	// Рализация вызова с одним параметром
	template
		<
			class Param1
		>
	class CallbackWrapper<Param1> : public CallbackWrapperImpl<void (void *, Param1), void (Param1)>
	{
	protected:
		typedef CallbackWrapperImpl<void (void *, Param1), void (Param1)> Base;
	public:
		void Call(Param1 param1)
		{
			if (Base::_callback)
			{
				Base::_callback(Base::_callbackParam, param1);
			}
			
			if (Base::_shortCallback)
			{
				Base::_shortCallback(param1);
			}
		}
		
		void operator ()(Param1 param1)
		{
			Call(param1);
		}
		
		void operator = (typename Base::ShortCallbackPtrType callback)
		{
			Base::Set(callback);
		}
	};
	
	// Рализация вызова без параметров
	template<>
		class CallbackWrapper<> : public CallbackWrapperImpl<void (void *), void ()>
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
			
			if (_shortCallback)
			{
				_shortCallback();
			}
		}
		
		void operator ()()
		{
			Call();
		}
		
		void operator = (ShortCallbackPtrType callback)
		{
			Set(callback);
		}
	};
}

#endif