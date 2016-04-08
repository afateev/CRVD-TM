#ifndef ID_OBJECT_BASE_H
#define ID_OBJECT_BASE_H

namespace Rblib
{
	template<unsigned int IdObj>
	class IdObjectBase
	{
	public:
		static const unsigned int Id;
	};
	
	template<unsigned int IdObj>
	const unsigned int IdObjectBase<IdObj>
		::Id = IdObj;
}

#endif