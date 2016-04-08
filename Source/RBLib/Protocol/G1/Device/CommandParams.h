///////////////////////////////////////////////////////////////////////////////
//	Формат пакета общий для всех устройств
//	Параметры команд
//
///////////////////////////////////////////////////////////////////////////////

#ifndef PROTOCOL_G1_DEVICE_COMMANDPARAMS_H
#define PROTOCOL_G1_DEVICE_COMMANDPARAMS_H

#include "FunctionCode.h"

namespace Rblib
{
namespace Protocol
{
namespace G1
{
namespace Device
{
	class CommandDeviceInfoParams
	{
	public:
		unsigned int SerialNumber;
		unsigned char FunctionsCount;
		FunctionCode *FunctionsList;
		
		CommandDeviceInfoParams()
		{
			SerialNumber = 0;
			FunctionsCount = 0;
			FunctionsList = 0;
		}
		
		int Size()
		{
			return sizeof(SerialNumber) + sizeof(FunctionsCount) + sizeof(FunctionCode) * FunctionsCount;
		}
		
		int ToByteArray(void *dst, int dstSize)
		{
			if (dstSize < Size())
			{
				return 0;
			}
			
			unsigned char *dstPtr = (unsigned char *)dst;
			
			int dstIndex = 0;
			for (int i = 0; i < 4; i++)
			{
				dstPtr[dstIndex] = ((unsigned char *)&SerialNumber)[i];
				dstIndex++;
			}
			
			dstPtr[dstIndex] = FunctionsCount;
			dstIndex++;
			
			for (int i = 0; i < FunctionsCount; i++)
			{
				dstPtr[dstIndex] = FunctionsList[i];
				dstIndex++;
			}
			
			return dstIndex;
		}
	};
	
	template
		<
			FunctionCode functionId,
			class FunctionDataType
		>
	class CommandToFunctionParams
	{
	public:
		FunctionCode FunctionId;
		FunctionDataType Data;
	protected:
		bool _valid;
	public:
		
		CommandToFunctionParams()
		{
			FunctionId = functionId;
			_valid = true;
		}
		
		CommandToFunctionParams(unsigned char *data, int dataCount)
		{
			_valid = false;
			
			if (data == 0 || dataCount < Size())
			{
				return;
			}
			
			FunctionId = (FunctionCode)data[0];
			if (FunctionId != functionId)
			{
				return;
			}
			_valid = true;
			Data.FromByteArray(&data[1], dataCount - 1);
		}
		
		bool Valid()
		{
			return _valid;
		}
		
		int Size()
		{
			return sizeof(FunctionId) + Data.Size();
		}
		
		int ToByteArray(void *dst, int dstSize)
		{
			if (dstSize < Size())
			{
				return 0;
			}
			
			unsigned char *dstPtr = (unsigned char *)dst;
			
			int dstIndex = 0;
			dstPtr[dstIndex] = FunctionId;
			dstIndex++;
			
			dstIndex += Data.ToByteArray(&dstPtr[dstIndex], dstSize - dstIndex);
			
			return dstIndex;
		}
	};
	
	template
		<
			FunctionCode functionId,
			class FunctionDataType
		>
	class CommandFromFunctionParams : public CommandToFunctionParams<functionId, FunctionDataType>
	{
	};
	
	class CommandSoftwareVersionParams
	{
	public:
		unsigned int A;
		unsigned int B;
		unsigned int C;
		unsigned int D;
		
		CommandSoftwareVersionParams()
		{
			A = 0;
			B = 0;
			C = 0;
			D = 0;
		}
		
		int Size()
		{
			return sizeof(A) + sizeof(B) + sizeof(C) + sizeof(D);
		}
		
		int ToByteArray(void *dst, int dstSize)
		{
			if (dstSize < Size())
			{
				return 0;
			}
			
			unsigned char *dstPtr = (unsigned char *)dst;
			
			int dstIndex = 0;
			for (int i = 0; i < 4; i++)
			{
				dstPtr[dstIndex] = ((unsigned char *)&A)[i];
				dstIndex++;
			}
			
			for (int i = 0; i < 4; i++)
			{
				dstPtr[dstIndex] = ((unsigned char *)&B)[i];
				dstIndex++;
			}
			
			for (int i = 0; i < 4; i++)
			{
				dstPtr[dstIndex] = ((unsigned char *)&C)[i];
				dstIndex++;
			}
			
			for (int i = 0; i < 4; i++)
			{
				dstPtr[dstIndex] = ((unsigned char *)&D)[i];
				dstIndex++;
			}
			
			return dstIndex;
		}
	};
}
}
}
}

#endif