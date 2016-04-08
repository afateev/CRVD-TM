
#ifndef _RBLIB_BD_BPLUS_PAIR_LIST_H_
#define _RBLIB_BD_BPLUS_PAIR_LIST_H_

#include "pair.h"
#include "min_uint.h"
#include "../../static_check.h"

namespace Rblib
{
namespace Db
{
namespace BPlusTree
{

namespace _private
{
	//
	// вспомогательный класс для определения макс. размера списка
	// в зависимости от размера элемента и объёма доступного места в памяти под объект "список"
	// с учётом того, что текущий размер должен храниться в переменной минимального размера
	//
	template<unsigned long PairSize, unsigned long MaxSpace>
	class MaxSizeCalc
	{
		// макс. кол-во элементов, если переменная для хранения текущего кол-ва элементов будет занимать минимум (1 байт)
		static const unsigned long MaxSize1 = (MaxSpace - 1) / PairSize;
		// сколько на самом деле будет занимать переменная для хранения такого значения (кол-во элементов)
		static const unsigned char TypeSize1 = sizeof(MinUInt<MaxSize1>::T);

		// пересчитываем макс. кол-во элементов, исходя из нового размера переменной
		static const unsigned long MaxSize2 = (MaxSpace - TypeSize1) / PairSize;
		// размер переменной для хранения такого значения
		static const unsigned char TypeSize2 = sizeof(MinUInt<MaxSize2>::T);

	public:

		// максимальны размер списка
		static const unsigned long MaxSize = (TypeSize2 < TypeSize1) ? (MinUInt<MaxSize2>::MaxVal) : (MaxSpace - TypeSize2) / PairSize;
	};
}

//
// отсортированный по убыванию ключа список пар значений
//
#pragma pack(push, 1)
// MaxSpace - максимальный размер, который может иметь объект этого класса
template<class Key, class Value, unsigned long MaxSpace>
class PairList
{
public:

	typedef Pair<Key, Value> PairType; // тип элемента списка
	static const unsigned long MaxSize = _private::MaxSizeCalc<sizeof(PairType), MaxSpace>::MaxSize; // максимальный размер списка
	typedef typename MinUInt<MaxSize>::T SizeType;

public:

	PairList() : _size(0)
	{
		StaticCheck();
	}

	// проверка параметров списка на этапе компилляции
	static void StaticCheck()
	{
		// !!! список должен вмещать не менее 4-х элементов (иначе не получим B-дерево) !!!
		RBLIB_STATIC_CHECK(MaxSize < 4, PAIRLIST_MAXSIZE_LESS_THEN_4);
	}

	//
	// размер
	//

	inline unsigned char Size() const
	{
		return _size;
	}

	inline bool Empty()
	{
		return _size == 0;
	}

	inline void SetSize(unsigned char size)
	{
		_size = size;
	}

	inline void Clear()
	{
		_size = 0;
	}

	inline bool Full() const
	{
		return _size == MaxSize;
	}

	static inline bool ThinSize(SizeType sz)
	{
		return sz < (MaxSize >> 1);
	}

	inline bool Thin() const
	{
		return ThinSize(_size);
	}

	inline bool HalfFull() const
	{
		return _size > (MaxSize >> 1);
	}

	//
	// доступ к списку
	//

	inline PairType& operator[](unsigned long i)
	{
		return _list[i];
	}

	//
	// минимальный элемент
	//

	inline Key MinKey()
	{
		return _list[_size - 1].Key();
	}

	inline Value MinValue()
	{
		return _list[_size - 1].Value();
	}

	// опасный метод, не контролирует порядок ключей, вызывать с умом!!!
	inline void SetMinKey(Key key)
	{
		if (_size == 0)
			return;
		_list[_size - 1].SetKey(key);
	}

	inline void SetMinValue(Value value)
	{
		if (_size == 0)
			return;
		_list[_size - 1].SetValue(value);
	}

	// опасный метод, не контролирует порядок ключей, вызывать с умом!!!
	inline void SetMin(Key key, Value value)
	{
		if (_size == 0)
			return;
		_list[_size - 1].SetKey(key);
		_list[_size - 1].SetValue(value);
	}

	//
	// максимальный элемент
	//

	inline Key MaxKey()
	{
		return _list[0].Key();
	}

	inline Value MaxValue()
	{
		return _list[0].Value();
	}

	// опасный метод, не контролирует порядок ключей, вызывать с умом!!!
	inline void SetMaxKey(Key key)
	{
		if (_size == 0)
			return;
		_list[0].SetKey(key);
	}

	inline void SetMaxValue(Value value)
	{
		if (_size == 0)
			return;
		_list[0].SetValue(value);
	}

	// опасный метод, не контролирует порядок ключей, вызывать с умом!!!
	inline void SetMax(Key key, Value value)
	{
		if (_size == 0)
			return;
		_list[0].SetKey(key);
		_list[0].SetValue(value);
	}

	//
	// вставка
	//
	bool Insert(Key key, Value value)
	{
		if (_size >= MaxSize)
			return false;

		SizeType pos = FindPos(key);

		// если все ключи больше, вставляем в конец
		if (pos == _size)
		{
			_list[_size].Set(key, value);
			_size++;
		}
		// если нашли такой же ключ, меняем соотв. значние
		else if (_list[pos].Key() == key)
		{
			_list[pos].Set(value);
		}
		else
		{
			// сдвигаем меньшие ключи на 1 позицию
			MoveElements(pos);
			// вставляем новое значние на освободившееся место
			_list[pos].Set(key, value);
		}

		return true;
	}

	// вставка в конец списка
	// опасный метод, не контролирует порядок ключей, вызывать с умом!!!
	bool InsertMin(Key key, Value value)
	{
		if (_size >= MaxSize)
			return false;
		_list[_size++].Set(key, value);
		return true;
	}

	//
	// поиск
	//
	SizeType Find(Key key)
	{
		for(SizeType i = 0; i < _size; i++)
			if (_list[i].Key() == key)
				return i;
		return _size;
	}

	//
	// модификация
	//
	// меняет значение у элемента с ключом key
	bool SetValueForKey(Key key, Value value)
	{
		SizeType i = Find(key);
		if (i >= _size)
			return false;
		_list[i].SetValue(value);
		return true;
	}

	//
	// удаление
	//
	// удаление по индексу
	void Delete(SizeType index)
	{
		for(SizeType i = index; i < (_size - 1); i++)
			_list[i] = _list[i + 1];
		if (_size)
			_size--;
	}

	// умное удаление по ключу (поиск индекса -> удаление)
	bool DeleteWise(Key key)
	{
		SizeType index = Find(key);
		if (index >= _size)
			return false;
		Delete(index);
		return true;
	}

	// удаляем минимальный элемент
	inline void DeleteMin()
	{
		_size--;
	}

	// удаляет максимальный элемент
	inline void DeleteMax()
	{
		Delete(0);
	}

	// удаляет большие элементы, начиная с index
	void DeleteMajorFrom(SizeType index)
	{
		if (index >= _size)
			return;
		_size = _size - index - 1; // уменьшаем размер списка
		// двигаем элементы
		for (SizeType i = 0; i < _size; i++)
			_list[i] = _list[index + 1 + i];
	}

	// слияние со списком, гарантированно содержащим меньшие ключи
	// опасный метод, не контролирует порядок ключей, вызывать с умом!!!
	bool MergeWithMinor(PairList& pairList)
	{
		if (pairList._size == 0)
			return true;
		if (pairList._size > MaxSize - _size)
			return false;
		// добавляем новые элементы в конец списка
		for(SizeType i = 0; i < pairList._size; i++)
			_list[_size + i] = pairList._list[i];
		_size += pairList._size;
		return true;
	}

	// слияние со списком, гарантированно содержащим большие ключи
	// опасный метод, не контролирует порядок ключей, вызывать с умом!!!
	bool MergeWithMajor(PairList& pairList)
	{
		if (pairList._size == 0)
			return true;
		if (pairList._size > MaxSize - _size)
			return false;
		// сдвигаем текущий список на pairList._size вниз
		for(SizeType i = 0; i < _size; i++)
			_list[pairList._size + _size - 1 - i] = _list[_size - 1 - i];
		// копируем новые элементы в текущий список
		for(SizeType i = 0; i < pairList._size; i++)
			_list[i] = pairList._list[i];
		_size += pairList._size; // меняем размер
		return true;
	}

	//
	// поддержка разделения узла
	// функции должны вызываться только для ПОЛНОГО массива!!!
	//
	// копирует половину элементов, содержащую большие ключи
	void CopyMajor(PairList& pairList)
	{
		static const unsigned long sizeToMove = MaxSize >> 1; // сколько нужно скопировать

		for(unsigned long i = 0; i < sizeToMove; i++)
		{
			pairList._list[i] = _list[i];
		}

		pairList._size = sizeToMove;
	}

	// копирует половину элементов, содержащую меньшие ключи
	void CopyMinor(PairList& pairList)
	{
		static const unsigned long sizeMajor = MaxSize >> 1; // размер половины с большими ключами
		static const unsigned long sizeToMove = MaxSize - sizeMajor; // сколько нужно скопировать

		for(unsigned long i = 0; i < sizeToMove; i++)
		{
			pairList._list[i] = _list[sizeMajor + i];
		}

		pairList._size = sizeToMove;
	}

	// копирует половину списка, с большими или меньшими ключами в зависимости от параметра
	inline void CopyPart(bool major, PairList& pairList)
	{
		if (major)
			CopyMajor(pairList);
		else
			CopyMinor(pairList);
	}

	// перемещает num мЕньших элементов в pairList, содержащий мЕньшие элементы, чем в нас
	// опасный метод, не контролирует порядок ключей, вызывать с умом!!!
	bool MoveMinor2Minor(PairList& pairList, SizeType num)
	{
		// если размер нашего списка меньше num, то скопируем весь наш список
		num = _size >= num ? num : _size;
		// если во входной список не войдёт - это ошибка
		if (pairList._size + num > MaxSize)
			return false;
		// сдвигаем элементы на num вниз во входном списке
		for (SizeType i = 0; i < pairList._size; i++)
			pairList._list[pairList._size + num - 1 - i] = pairList._list[pairList._size - 1 - i];
		// копируем элементы в начало входного списка
		for (SizeType i = 0; i < num; i++)
			pairList._list[i] = _list[_size - num + i];
		// меняем размеры списков
		_size -= num;
		pairList._size += num;
		return true;
	}

	// перемещает num бОльших элементов в pairList, содержащий бОльшие элементы, чем в нас
	// опасный метод, не контролирует порядок ключей, вызывать с умом!!!
	bool MoveMajor2Major(PairList& pairList, SizeType num)
	{
		// если размер нашего списка меньше num, то скопируем весь наш список
		num = _size >= num ? num : _size;
		// если во входной список не войдёт - это ошибка
		if (pairList._size + num > MaxSize)
			return false;
		// копируем элементы в конец входного списка
		for(SizeType i = 0; i < num; i++)
			pairList._list[pairList._size + i] = _list[i];
		// меняем размеры списков
		_size -= num;
		pairList._size += num;
		// сдвигаем элементы на num вверх в нашем списке
		for(SizeType i = 0; i < _size ;i++)
			_list[i] = _list[i + num];
		return true;
	}

	//
	// поддержка разделения узла
	// функции должны вызываться только для ПОЛНОГО массива!!!
	//
	// оставляет в массива только половину бОльших значений
	inline void MakeMajorPart()
	{
		// массив отсортирован по убыванию, поэтому просто меняем размер
		_size = MaxSize >> 1;
	}

	// возвращает массиву первозданный вид после вызова предыдущей функции
	inline void RestoreFromMajorPart()
	{
		_size = MaxSize; // возвращаем размер
	}

	// оставляет в массиве только половину меньших значений
	inline void MakeMinorPart()
	{
		SwitchParts<MaxSize % 2>(); // меняем местами половины массива
		_size = MaxSize - (MaxSize >> 1); // меняем размер
	}

	// возвращает массиву первозданный вид после вызова предыдущей функции
	inline void RestoreFromMinorPart()
	{
		SwitchPartsBack<MaxSize % 2>(); // обратно меняем местави половины массива
		_size = MaxSize; // меняем размер
	}

	// оставляет в массиве половину значений, больших или меньших в зависимости от параметра
	inline void MakePart(bool major)
	{
		if (major)
			MakeMajorPart();
		else
			MakeMinorPart();
	}

	// возвращает массиву первозданный вид после вызова предыдущей функции
	inline void RestoreFromPart(bool major)
	{
		if (major)
			RestoreFromMajorPart();
		else
			RestoreFromMinorPart();
	}

	// к какой половине (большие или меньшие значения) относится индекс?
	static inline bool MajorIndex(SizeType index)
	{
		return index < (MaxSize >> 1);
	}

private:

	//
	// поменть местами половины массива
	// функции используются только для заполненного масива, поэтому используем константу MaxSize
	//
  	
  	template<bool val> class StaticBool{};
  
	template<bool odd> void SwitchParts()
	{
		SwitchParts(StaticBool<odd>());
	}

	// для чётного количества элементов
	void SwitchParts(StaticBool<false>)
	{
		static const unsigned long halfSize = MaxSize >> 1;
		for(unsigned long i = 0; i < halfSize; i++)
		{
			PairType tmp = _list[i];
			_list[i] = _list[halfSize + i];
			_list[halfSize + i] = tmp;
		}
	}

	// для нечётного количества элементов
	// "половины" будут разной длины (длина отличается на 1)
	void SwitchParts(StaticBool<true>)
	{
		static const unsigned long halfSize = MaxSize >> 1;

		PairType tmp1 = _list[halfSize];

		for(unsigned long i = 0; i < halfSize; i++)
		{
			PairType tmp2 = _list[i];
			_list[i] = tmp1;
			tmp1 = _list[halfSize + 1 + i];
			_list[halfSize + 1 + i] = tmp2;
		}

		_list[halfSize] = tmp1;
	}

	//
	// вернуть массив в исходный вид после вызова предыдущих функций
	// функции используются только для заполненного масива, поэтому используем константу MaxSize
	//
	template<bool odd> void SwitchPartsBack()
	{
		SwitchPartsBack(StaticBool<odd>());
	}

	// для нечётного кол-ва элементов
	// "половины" разной длины (длина отличается на 1)
	void SwitchPartsBack(StaticBool<true>)
	{
		static const unsigned long halfSize = MaxSize >> 1;

		PairType tmp1 = _list[halfSize];

		for(unsigned long i = 0; i < halfSize; i++)
		{
			PairType tmp2 = _list[MaxSize - 1 - i];
			_list[MaxSize - 1 - i] = tmp1;
			tmp1 = _list[halfSize - i - 1];
			_list[halfSize - i - 1] = tmp2;
		}

		_list[halfSize] = tmp1;
	}

	// для чётного кол-ва элементов
	inline void SwitchPartsBack(StaticBool<false>)
	{
		SwitchParts<false>();
	}

	//
	// вспомогательные функции для вставки в массив
	//

	// ищет позицию для вставки ключа
	SizeType FindPos(Key key)
	{
		for(SizeType i = 0; i < _size; i++)
			if (_list[i].Key() <= key)
				return i;
		return _size;
	}

	// сдвигает элементы списка на одну позицию, начиная с pos
	// если список полный, вызывать метод нельзя!
	void MoveElements(SizeType pos)
	{
		for(SizeType i = _size++; i > pos; i--)
			_list[i] = _list[i - 1];
	}

protected:

	SizeType _size; // текущий размер
	PairType _list[MaxSize]; // список

};
#pragma pack (pop)

}
}
}

#endif // _RBLIB_BD_BPLUS_PAIR_LIST_H_

