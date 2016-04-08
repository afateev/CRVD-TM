#ifndef ASYNCOPERATION_H
#define ASYNCOPERATION_H

#include "CallbackWrapper.h"

namespace Rblib
{
	template
		<
			class dataType
		>
	class AsyncOperation
	{
	public:
		typedef dataType DataType;
		typedef Rblib::CallbackWrapper<DataType> ResultHandlerType;
		typedef Rblib::CallbackWrapper<DataType&, ResultHandlerType> RequestHandlerType;
	public:
		RequestHandlerType RequestHandler;
		ResultHandlerType ResultHandler;
	protected:
		bool _pending;
		DataType _data;
	public:
		AsyncOperation()
		{
		}
		
		void operator ()(DataType data)
		{
			_data = data;
			_pending = true;
		}
		
		void Process()
		{
			if (_pending)
			{
				RequestHandler(_data, ResultHandler);
				ResultHandler(_data);
				_pending = false;
			}
		}
	};
}

#endif