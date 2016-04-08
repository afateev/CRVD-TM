///////////////////////////////////////////////////////////////////////////////
//	Пустой тип
//	Используется в списках типов
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef EMPTY_TYPE_H
#define EMPTY_TYPE_H

namespace Rblib
{
  class EmptyType
  {
  };

  inline bool operator==(const EmptyType&, const EmptyType&)
  {
      return true;
  }   

  inline bool operator<(const EmptyType&, const EmptyType&)
  {
      return false;
  }
  
  inline bool operator>(const EmptyType&, const EmptyType&)
  {
      return false;
  }
}

#endif // EMPTY_TYPE_H
