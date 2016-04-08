
#ifndef PACKET_EXTRACTOR_CALLBACK_TYPE_PROVIDER_H
#define PACKET_EXTRACTOR_CALLBACK_TYPE_PROVIDER_H

#include "../Utils/Functor.h"

namespace Rblib
{
	namespace Protocol
	{

		template<bool functorCallback>
		class PacketExtractorCallbackTypeProvider;

		template<>
		class PacketExtractorCallbackTypeProvider<true>
		{
		public:
			typedef BasicFunctor<MakeTypeList<unsigned char*, unsigned int>::Result> FunctorType;
			typedef FunctorType& CallbackType;
		};

		template<>
		class PacketExtractorCallbackTypeProvider<false>
		{
		public:
			typedef void (CallbackType)(unsigned char *data, unsigned int length);
		};

	}
}

#endif // PACKET_EXTRACTOR_CALLBACK_H
