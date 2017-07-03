////////////////////////////////////////////////////////////////////////////////
//
//
//	Fateev A.I.		26/11/2011
////////////////////////////////////////////////////////////////////////////////

#ifndef REGISTERREFERENCE_H
#define REGISTERREFERENCE_H

typedef unsigned int RegisterPointerType;

template<class RegisterDataType, RegisterPointerType RegisterAddr>
class RegisterReference
{
public:
	static volatile RegisterDataType * const _register;
};

template<class RegisterDataType, RegisterPointerType RegisterAddr>
volatile RegisterDataType * const RegisterReference<RegisterDataType, RegisterAddr>
	::_register = reinterpret_cast<volatile RegisterDataType*>(RegisterAddr);


#endif