///////////////////////////////////////////////////////////////////////////////
//	Список функций обратного вызова
//
///////////////////////////////////////////////////////////////////////////////

#ifndef CALLBACKLISTWRAPPER_H
#define CALLBACKLISTWRAPPER_H

namespace Rblib
{
	template
		<
			class Prototype,		// Прототип функции обратного вызова,
			class ShortPrototype,	// Прототип функции обратного вызова без обязательного параметра
			int maxCount
		>
	class CallbackListWrapperImpl
	{
	public:
		typedef Prototype *CallbackPtrType;
		typedef ShortPrototype *ShortCallbackPtrType;
		static const int MaxCount = maxCount;
	protected:
		CallbackPtrType _callback[MaxCount];			// указатель на функцию
		ShortCallbackPtrType _shortCallback[MaxCount];	// указатель на функцию
		void *_callbackParam[MaxCount];					// значение обязательного параметра, которое задается при установке обработчика
		int _count;
		int _shortCount;
	public:
		CallbackListWrapperImpl()
		{
			Reset();
		}
		
		// Отменить назначение всех обработчиков
		void Reset()
		{
			_count = 0;
			_shortCount = 0;
		}
		
		// Добавить обработчик и значение его обязательного параметра
		void Add(CallbackPtrType callback, void *callbackParam)
		{
			if (_count >= MaxCount)
			{
				return;
			}
			
			_callback[_count] = callback;
			_callbackParam[_count] = callbackParam;
			_count++;
		}
		
		// Задать обработчик
		void Add(ShortCallbackPtrType callback)
		{
			if (_shortCount >= MaxCount)
			{
				return;
			}
			
			_shortCallback[_shortCount] = callback;
			_shortCount++;
		}
	};
	
	// Рализация вызова с восемью параметрами
	template
		<
			int maxCount,
			class Param1 = NullType,
			class Param2 = NullType,
			class Param3 = NullType,
			class Param4 = NullType,
			class Param5 = NullType,
			class Param6 = NullType,
			class Param7 = NullType,
			class Param8 = NullType
		>
	class CallbackListWrapper : public CallbackListWrapperImpl<void (void *, Param1, Param2, Param3, Param4, Param5, Param6, Param7, Param8), void (Param1, Param2, Param3, Param4, Param5, Param6, Param7, Param8), maxCount>
	{
	protected:
		typedef CallbackListWrapperImpl<void (void *, Param1, Param2, Param3, Param4, Param5, Param6, Param7, Param8), void (Param1, Param2, Param3, Param4, Param5, Param6, Param7, Param8), maxCount> Base;
	public:
		void Call(Param1 param1, Param2 param2, Param3 param3, Param4 param4, Param5 param5, Param6 param6, Param7 param7, Param8 param8)
		{
			for (int i = 0; i < Base::_count; i++)
			{
				if (Base::_callback[i])
				{
					Base::_callback[i](Base::_callbackParam[i], param1, param2, param3, param4, param5, param6, param7, param8);
				}
			}
			
			for (int i = 0; i < Base::_shortCount; i++)
			{
				if (Base::_shortCallback[i])
				{
					Base::_shortCallback[i](param1, param2, param3, param4, param5, param6, param7, param8);
				}
			}
		}
		
		void operator ()(Param1 param1, Param2 param2, Param3 param3, Param4 param4, Param5 param5, Param6 param6, Param7 param7, Param8 param8)
		{
			Call(param1, param2, param3, param4, param5, param6, param7, param8);
		}
		
		void operator += (typename Base::ShortCallbackPtrType callback)
		{
			Base::Add(callback);
		}
	};
	
	// Рализация вызова с семью параметрами
	template
		<
			int maxCount,
			class Param1,
			class Param2,
			class Param3,
			class Param4,
			class Param5,
			class Param6,
			class Param7
		>
	class CallbackListWrapper<maxCount, Param1, Param2, Param3, Param4, Param5, Param6, Param7> : public CallbackListWrapperImpl<void (void *, Param1, Param2, Param3, Param4, Param5, Param6, Param7), void (Param1, Param2, Param3, Param4, Param5, Param6, Param7), maxCount>
	{
	protected:
		typedef CallbackListWrapperImpl<void (void *, Param1, Param2, Param3, Param4, Param5, Param6, Param7), void (Param1, Param2, Param3, Param4, Param5, Param6, Param7), maxCount> Base;
	public:
		void Call(Param1 param1, Param2 param2, Param3 param3, Param4 param4, Param5 param5, Param6 param6, Param7 param7)
		{
			for (int i = 0; i < Base::_count; i++)
			{
				if (Base::_callback[i])
				{
					Base::_callback[i](Base::_callbackParam[i], param1, param2, param3, param4, param5, param6, param7);
				}
			}
			
			for (int i = 0; i < Base::_shortCount; i++)
			{
				if (Base::_shortCallback[i])
				{
					Base::_shortCallback[i](param1, param2, param3, param4, param5, param6, param7);
				}
			}
		}
		
		void operator ()(Param1 param1, Param2 param2, Param3 param3, Param4 param4, Param5 param5, Param6 param6, Param7 param7)
		{
			Call(param1, param2, param3, param4, param5, param6, param7);
		}
		
		void operator += (typename Base::ShortCallbackPtrType callback)
		{
			Base::Add(callback);
		}
	};
	
	// Рализация вызова с шестью параметрами
	template
		<
			int maxCount,
			class Param1,
			class Param2,
			class Param3,
			class Param4,
			class Param5,
			class Param6
		>
	class CallbackListWrapper<maxCount, Param1, Param2, Param3, Param4, Param5, Param6> : public CallbackListWrapperImpl<void (void *, Param1, Param2, Param3, Param4, Param5, Param6), void (Param1, Param2, Param3, Param4, Param5, Param6), maxCount>
	{
	protected:
		typedef CallbackListWrapperImpl<void (void *, Param1, Param2, Param3, Param4, Param5, Param6), void (Param1, Param2, Param3, Param4, Param5, Param6), maxCount> Base;
	public:
		void Call(Param1 param1, Param2 param2, Param3 param3, Param4 param4, Param5 param5, Param6 param6)
		{
			for (int i = 0; i < Base::_count; i++)
			{
				if (Base::_callback[i])
				{
					Base::_callback[i](Base::_callbackParam[i], param1, param2, param3, param4, param5, param6);
				}
			}
			
			for (int i = 0; i < Base::_shortCount; i++)
			{
				if (Base::_shortCallback[i])
				{
					Base::_shortCallback[i](param1, param2, param3, param4, param5, param6);
				}
			}
		}
		
		void operator ()(Param1 param1, Param2 param2, Param3 param3, Param4 param4, Param5 param5, Param6 param6)
		{
			Call(param1, param2, param3, param4, param5, param6);
		}
		
		void operator += (typename Base::ShortCallbackPtrType callback)
		{
			Base::Add(callback);
		}
	};
	
	// Рализация вызова с пятью параметрами
	template
		<
			int maxCount,
			class Param1,
			class Param2,
			class Param3,
			class Param4,
			class Param5
		>
	class CallbackListWrapper<maxCount, Param1, Param2, Param3, Param4, Param5> : public CallbackListWrapperImpl<void (void *, Param1, Param2, Param3, Param4, Param5), void (Param1, Param2, Param3, Param4, Param5), maxCount>
	{
	protected:
		typedef CallbackListWrapperImpl<void (void *, Param1, Param2, Param3, Param4, Param5), void (Param1, Param2, Param3, Param4, Param5), maxCount> Base;
	public:
		void Call(Param1 param1, Param2 param2, Param3 param3, Param4 param4, Param5 param5)
		{
			for (int i = 0; i < Base::_count; i++)
			{
				if (Base::_callback[i])
				{
					Base::_callback[i](Base::_callbackParam[i], param1, param2, param3, param4, param5);
				}
			}
			
			for (int i = 0; i < Base::_shortCount; i++)
			{
				if (Base::_shortCallback[i])
				{
					Base::_shortCallback[i](param1, param2, param3, param4, param5);
				}
			}
		}
		
		void operator ()(Param1 param1, Param2 param2, Param3 param3, Param4 param4, Param5 param5)
		{
			Call(param1, param2, param3, param4, param5);
		}
		
		void operator += (typename Base::ShortCallbackPtrType callback)
		{
			Base::Add(callback);
		}
	};
	
	// Рализация вызова с четырьмя параметрами
	template
		<
			int maxCount,
			class Param1,
			class Param2,
			class Param3,
			class Param4
		>
	class CallbackListWrapper<maxCount, Param1, Param2, Param3, Param4> : public CallbackListWrapperImpl<void (void *, Param1, Param2, Param3, Param4), void (Param1, Param2, Param3, Param4), maxCount>
	{
	protected:
		typedef CallbackListWrapperImpl<void (void *, Param1, Param2, Param3, Param4), void (Param1, Param2, Param3, Param4), maxCount> Base;
	public:
		void Call(Param1 param1, Param2 param2, Param3 param3, Param4 param4)
		{
			for (int i = 0; i < Base::_count; i++)
			{
				if (Base::_callback[i])
				{
					Base::_callback[i](Base::_callbackParam[i], param1, param2, param3, param4);
				}
			}
			
			for (int i = 0; i < Base::_shortCount; i++)
			{
				if (Base::_shortCallback[i])
				{
					Base::_shortCallback[i](param1, param2, param3, param4);
				}
			}
		}
		
		void operator ()(Param1 param1, Param2 param2, Param3 param3, Param4 param4)
		{
			Call(param1, param2, param3, param4);
		}
		
		void operator += (typename Base::ShortCallbackPtrType callback)
		{
			Base::Add(callback);
		}
	};
	
	// Рализация вызова с тремя параметрами
	template
		<
			int maxCount,
			class Param1,
			class Param2,
			class Param3
		>
	class CallbackListWrapper<maxCount, Param1, Param2, Param3> : public CallbackListWrapperImpl<void (void *, Param1, Param2, Param3), void (Param1, Param2, Param3), maxCount>
	{
	protected:
		typedef CallbackListWrapperImpl<void (void *, Param1, Param2, Param3), void (Param1, Param2, Param3), maxCount> Base;
	public:
		void Call(Param1 param1, Param2 param2, Param3 param3)
		{
			for (int i = 0; i < Base::_count; i++)
			{
				if (Base::_callback[i])
				{
					Base::_callback[i](Base::_callbackParam[i], param1, param2, param3);
				}
			}
			
			for (int i = 0; i < Base::_shortCount; i++)
			{
				if (Base::_shortCallback[i])
				{
					Base::_shortCallback[i](param1, param2, param3);
				}
			}
		}
		
		void operator ()(Param1 param1, Param2 param2, Param3 param3)
		{
			Call(param1, param2, param3);
		}
		
		void operator += (typename Base::ShortCallbackPtrType callback)
		{
			Base::Add(callback);
		}
	};
	
	// Рализация вызова с двумя параметрами
	template
		<
			int maxCount,
			class Param1,
			class Param2
		>
	class CallbackListWrapper<maxCount, Param1, Param2> : public CallbackListWrapperImpl<void (void *, Param1, Param2), void (Param1, Param2), maxCount>
	{
	protected:
		typedef CallbackListWrapperImpl<void (void *, Param1, Param2), void (Param1, Param2), maxCount> Base;
	public:
		void Call(Param1 param1, Param2 param2)
		{
			for (int i = 0; i < Base::_count; i++)
			{
				if (Base::_callback[i])
				{
					Base::_callback[i](Base::_callbackParam[i], param1, param2);
				}
			}
			
			for (int i = 0; i < Base::_shortCount; i++)
			{
				if (Base::_shortCallback[i])
				{
					Base::_shortCallback[i](param1, param2);
				}
			}
		}
		
		void operator ()(Param1 param1, Param2 param2)
		{
			Call(param1, param2);
		}
		
		void operator += (typename Base::ShortCallbackPtrType callback)
		{
			Base::Add(callback);
		}
	};
	
	// Рализация вызова с одним параметром
	template
		<
			int maxCount,
			class Param1
		>
	class CallbackListWrapper<maxCount, Param1> : public CallbackListWrapperImpl<void (void *, Param1), void (Param1), maxCount>
	{
	protected:
		typedef CallbackListWrapperImpl<void (void *, Param1), void (Param1), maxCount> Base;
	public:
		void Call(Param1 param1)
		{
			for (int i = 0; i < Base::_count; i++)
			{
				if (Base::_callback[i])
				{
					Base::_callback[i](Base::_callbackParam[i], param1);
				}
			}
			
			for (int i = 0; i < Base::_shortCount; i++)
			{
				if (Base::_shortCallback[i])
				{
					Base::_shortCallback[i](param1);
				}
			}
		}
		
		void operator ()(Param1 param1)
		{
			Call(param1);
		}
		
		void operator += (typename Base::ShortCallbackPtrType callback)
		{
			Base::Add(callback);
		}
	};
	
	// Рализация вызова без параметров
	template<int maxCount>
		class CallbackListWrapper<maxCount> : public CallbackListWrapperImpl<void (void *), void (), maxCount>
	{
	protected:
		typedef CallbackListWrapperImpl<void (void *), void (), maxCount> Base;
	public:
		void Call()
		{
			for (int i = 0; i < Base::_count; i++)
			{
				if (Base::_callback[i])
				{
					Base::_callback[i](Base::_callbackParam[i]);
				}
			}
			
			for (int i = 0; i < Base::_shortCount; i++)
			{
				if (Base::_shortCallback[i])
				{
					Base::_shortCallback[i]();
				}
			}
		}
		
		void operator ()()
		{
			Call();
		}
		
		void operator += (typename Base::ShortCallbackPtrType callback)
		{
			Base::Add(callback);
		}
	};
}

#endif