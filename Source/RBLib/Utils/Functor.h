///////////////////////////////////////////////////////////////////////////////
//	Класс "Функтор"
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef FUNCTOR_H
#define FUNCTOR_H

#include "..\TypeList.h"
#include "..\EmptyType.h"

namespace Rblib
{
	namespace Private
	{
		template<class TList> class FunctorHandlerBase;
		template<>
		struct FunctorHandlerBase<NullType>
		{
		  	virtual ~FunctorHandlerBase() {}
			virtual void operator()() = 0;
			virtual FunctorHandlerBase* clone() const = 0;
		};
		template<typename P1>
		struct FunctorHandlerBase<TypeList<P1, NullType> >
		{
		  	virtual ~FunctorHandlerBase() {}
			virtual void operator()(P1) = 0;
			virtual FunctorHandlerBase* clone() const = 0;
		};
		template<typename P1, typename P2>
		struct FunctorHandlerBase<TypeList<P1, TypeList<P2, NullType> > >
		{
		  	virtual ~FunctorHandlerBase() {}
			virtual void operator()(P1, P2) = 0;
			virtual FunctorHandlerBase* clone() const = 0;
		};
		
		template<class TList> class ProcedureTypeHandlerSelector;
		template<>
		struct ProcedureTypeHandlerSelector<NullType>
		{
		  typedef void (*Result)(void);
		};
		template<typename P1>
		struct ProcedureTypeHandlerSelector <TypeList<P1, NullType> >
		{
		  typedef void (*Result)(P1);
		};
		template<typename P1, typename P2>
		struct ProcedureTypeHandlerSelector<TypeList<P1, TypeList<P2, NullType> > >
		{
		  typedef void (*Result)(P1, P2);
		};
	}
  
	/// Класс обобщённого функтора.
	/// \details Обобщённый функтор - обёртка для пользовательских обратных вызовов, 
	/// позволяющая их хранить, вызывать и передавать в качестве параметра в функции, 
	/// используя унифицированный интерфейс.\n
	/// В качестве пользовательскиъ вызываемых сущностей могут использоватся указатели на функции и 
	/// объекты классов с переопределённым оператором вызова.\n
	/// Данная реалихайия обобщённого функтора рассчитана на работу с вызываемыми сущностями с 1-м, 2-мя параметрами или без параметров.
	/// \tparam ParameterList список типов параметров вызова.
	template<class ParameterList = NullType>
	class BasicFunctor
	{
	private:
		typedef typename TypeAtNonStrict<ParameterList, 0, EmptyType>::Result Param1;
		typedef typename TypeAtNonStrict<ParameterList, 1, EmptyType>::Result Param2;
		template<class Fun>
		struct FunctorHandler : public Private::FunctorHandlerBase<ParameterList>
		{
			Fun& _fun;
			FunctorHandler(Fun& fun) : _fun(fun) 
			{
			}
			void operator()()
			{
				_fun();
			}
			void operator()(Param1 p1)
			{
				_fun(p1);
			}
			void operator()(Param1 p1, Param2 p2)
			{
				_fun(p1, p2);
			}
			Private::FunctorHandlerBase<ParameterList>* clone() const
			{
				return new FunctorHandler<Fun>(_fun);
			}
		};

		typedef typename Private::ProcedureTypeHandlerSelector<ParameterList>::Result ProcedureHandler;
		
	private:
		Private::FunctorHandlerBase<ParameterList>* _functorHandler;
		ProcedureHandler _procedureHandler;

	public:

		/// конструктор по умолчнию.
		BasicFunctor() : _functorHandler(0), _procedureHandler(0)
		{
		}

		/// Конструктор копирования.
		BasicFunctor(const BasicFunctor& functor) 
			: _procedureHandler(functor._procedureHandler)
			, _functorHandler(functor._functorHandler ? functor._functorHandler->clone() : 0)
		{
		}

		/// Оператор присваивания.
		BasicFunctor& operator = (const BasicFunctor& functor)
		{
			_procedureHandler = functor._procedureHandler;
			_functorHandler = functor._functorHandler ? functor._functorHandler->clone() : 0;
			return *this;
		}

		/// Конструктор функтора из пользовательского объекта-вызова.
		/// \tparam Fun Тип пользовательского объекта-вызова.
		/// \param functor пользовательский объект-вызов.
		template<class Fun>
		BasicFunctor(Fun& functor) : _functorHandler(new FunctorHandler<Fun>(functor)), _procedureHandler(0)
		{
		}

		/// Конструктор функтора из указателя на функцию.
		/// \param procedure Указатель на функцию обратного вызова пользователя.
		BasicFunctor(ProcedureHandler procedure) : _functorHandler(0), _procedureHandler(procedure)
		{
		}

		~BasicFunctor()
		{
			if (_functorHandler)
				delete _functorHandler;
		}

		/// Оператор вызова функтора без параметров.
		inline void operator()()
		{
			if (_functorHandler)
				(*_functorHandler)();
			else
				if (_procedureHandler)
					_procedureHandler();
		}

		/// Оператор вызова функтора с 1-м параметром.
		inline void operator()(Param1 p1)
		{
			if (_functorHandler)
				(*_functorHandler)(p1);
			else
			{
				if (_procedureHandler)
					_procedureHandler(p1);
			}
		}

		/// Оператор вызова функтора с 2-мя параметрами.
		inline void operator()(Param1 p1, Param2 p2)
		{
			if (_functorHandler)
				(*_functorHandler)(p1, p2);
			else
			{
				if (_procedureHandler)
					_procedureHandler(p1, p2);
			}
		}
	};

	/// Обобщнный функтор без параметров.
	typedef BasicFunctor<NullType> Functor;
}

#endif // FUNCTOR_H
