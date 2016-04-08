
#ifndef _MEDIA_SD_RESPONSE_RESPONSE_SELECT_H_
#define _MEDIA_SD_RESPONSE_RESPONSE_SELECT_H_

#include "Cmd.h"
#include "Response/Sd.h"
#include "Response/Spi.h"
#include "Response/Type.h"

namespace Rblib
{
namespace Media
{
namespace Sd
{
//
// Селектор структуры ответа
//

// Cmd - класс команды
// spiMode == true - режим SPI, иначе - SD
template<class Cmd, bool _spiMode>
class ResponseSelect
{
public:
};

//
// чаще всего ответ R1 (в обоих режимах)
//

template<class Cmd>
class ResponseSelect<Cmd, false>
{
public:
	typedef Response::Sd::R1 C;
	static const unsigned char T = Response::Type::R1;
};

template<class Cmd>
class ResponseSelect<Cmd, true>
{
public:
	typedef Response::Spi::R1 C;
	static const unsigned char T = Response::Type::R1;
};

// специализации для других вариантов ответов
//
// cmd2
//

template<>
class ResponseSelect<Cmd2, false>
{
public:
	typedef Response::Sd::R2CID C;
	static const unsigned char T = Response::Type::R2;
};

//
// cmd3
//

template<>
class ResponseSelect<Cmd3, false>
{
public:
	typedef Response::Sd::R6 C;
	static const unsigned char T = Response::Type::R6;
};

//
// cmd8
//

template<>
class ResponseSelect<Cmd8, false>
{
public:
	typedef Response::Sd::R7 C;
	static const unsigned char T = Response::Type::R7;
};

template<>
class ResponseSelect<Cmd8, true>
{
public:
	typedef Response::Spi::R7 C;
	static const unsigned char T = Response::Type::R7;
};

//
// cmd9
//

template<>
class ResponseSelect<Cmd9, false>
{
public:
	typedef Response::Sd::R2CSD C;
	static const unsigned char T = Response::Type::R2;
};

template<>
class ResponseSelect<Cmd9, true>
{
public:
	typedef Response::Spi::R1CSD C;
	static const unsigned char T = Response::Type::R1;
};

//
// cmd10
//

template<>
class ResponseSelect<Cmd10, false>
{
public:
	typedef Response::Sd::R2CID C;
	static const unsigned char T = Response::Type::R2;
};

template<>
class ResponseSelect<Cmd10, true>
{
public:
	typedef Response::Spi::R1CID C;
	static const unsigned char T = Response::Type::R1;
};

//
// cmd13
//

template<>
class ResponseSelect<Cmd13, true>
{
public:
	typedef Response::Spi::R2 C;
	static const unsigned char T = Response::Type::R2;
};

//
// acmd41
//

template<>
class ResponseSelect<ACmd41, false>
{
public:
	typedef Response::Sd::R3 C;
	static const unsigned char T = Response::Type::R3;
};

//
// cmd58
//

template<>
class ResponseSelect<Cmd58, true>
{
public:
	typedef Response::Spi::R3 C;
	static const unsigned char T = Response::Type::R3;
};

}
}
}

#endif // _MEDIA_SD_RESPONSE_RESPONSE_SELECT_H_

