
#ifndef _RBLIB_BD_BPLUS_NODE_H_
#define _RBLIB_BD_BPLUS_NODE_H_

#include "header.h"
#include "pair_list.h"
#include "type_selector.h"
#include "../../static_check.h"
#include "../size_addition.h"
#include "../digit_selector.h"

namespace Rblib
{
namespace Db
{
namespace BPlusTree
{

#pragma pack(push, 1)
template<class KeyT, class ValueT, bool _64bit = false, unsigned long _pageSize = 512>
class Node : public Header<typename DigitSelector<_64bit>::T> // заголовок
{
private:

	//
	// внутренние типы данных
	//
	typedef Header<typename DigitSelector<_64bit>::T> Header; // тип заголовка
	// список пар ключ-значение для внутреннего узла (хранятся ключи и смещения узлов-потомков)
	typedef PairList<KeyT, typename Header::OffsetType, _pageSize - sizeof(Header)> InternalPairListType;
	// список пар ключ-значение для узла-листа (хранятся ключи и соответствующие им пользовательские данные)
	typedef PairList<KeyT, ValueT, _pageSize - sizeof(Header)> LeafPairListType;

	typedef typename InternalPairListType::SizeType InternalSizeType; // тип для хранения размера внутреннего узла
	typedef typename LeafPairListType::SizeType LeafSizeType; // тип для хранения размера узла-листа

public:

	//
	// типы данных, которые могут понадобиться клиентам класса
	//
	typedef KeyT KeyType; // тип ключа
	typedef ValueT ValueType; // тип хранимого значения
	typedef typename Header::OffsetType OffsetType;

	// тип для хранения размера узла - больший из двух (для внутреннего узла и узла-листа)
	typedef typename TypeSelector<InternalSizeType, LeafSizeType, sizeof(InternalSizeType) >= sizeof(LeafSizeType)>::T SizeType;

	typedef typename InternalPairListType::PairType InternalPairType; // пара ключ-значение для внутреннего узла
	typedef typename LeafPairListType::PairType LeafPairType; // пара ключ-значение для узла-листа

	static const unsigned long LeafMaxSize = LeafPairListType::MaxSize; // максимальный размер узла-листа (снаружи он нужен при обходе дерева)

public:

	Node()
	{
		// !!! Неправильный размер страницы, должен быть кратным 512 !!!
		RBLIB_STATIC_CHECK(_pageSize == 0 || _pageSize % 512, INVALID_PAGE_SIZE);
		// !!! Узел должен помещатья на 1 страницу, где-то ошибка !!!
		RBLIB_STATIC_CHECK(sizeof(Node<KeyT, ValueT, _64bit, _pageSize>) != _pageSize, INVALID_NODE_SIZE);

		InternalPairListType::StaticCheck(); // проверяем параметры внутреннего узла
		LeafPairListType::StaticCheck();  // проверяем параметры узла-листа

		InternalPairList().Clear(); // очищаем размер списка (internal или leaf неважно, т.к. они на одном буфере)
	}

	//
	// всё, что связано с размером списка
	//

	// размер (количество элементов в списке пар ключ-значение)
	inline SizeType Size()
	{
		return Header::Leaf() ? LeafPairList().Size() : InternalPairList().Size();
	}

	// максимальный размер списка пар ключ-значение
	inline SizeType MaxSize()
	{
		return Header::Leaf() ? LeafPairListType::MaxSize : InternalPairListType::MaxSize;
	}

	// попадает ли индекс в половину списка с бОльшими значениями?
	inline bool MajorIndex(SizeType i)
	{
		return Header::Leaf() ? LeafPairList().MajorIndex(i) : InternalPairList().MajorIndex(i);
	}

	// список заполнен (требует разделения)?
	inline bool Full()
	{
		return Header::Leaf() ? LeafPairList().Full() : InternalPairList().Full();
	}

	// список полупустой (требует перегруппировки или слияния с другим узлом)?
	inline bool Thin()
	{
		return Header::Leaf() ? LeafPairList().Thin() : InternalPairList().Thin();
	}

	// проверяет, станет ли список полупустым, если его размер станет равным sz
	inline bool ThinSize(SizeType sz)
	{
		return Header::Leaf() ? LeafPairListType::ThinSize(sz) : InternalPairListType::ThinSize(sz);
	}

	// список пуст?
	inline bool Empty()
	{
		return Header::Leaf() ? LeafPairList().Empty() : InternalPairList().Empty();
	}

	// очистить список пар ключ-значение
	inline void Clear()
	{
		Header::Leaf() ? LeafPairList().Clear() : InternalPairList().Clear();
	}

	//
	// доступ к ключам и значениям в узле
	//

	// ключ i-го элемента в узле (в списке пар ключ-значение)
	inline KeyType Key(SizeType i)
	{
		if (i >= Size())
			return KeyType(); // неважно, что на выходе (чтобы не думать о корректности индекса при вызове LoadNodeWise)
		return Header::Leaf() ? LeafPairList()[i].Key() : InternalPairList()[i].Key(); 
	}

	// минимальный ключ
	inline KeyType MinKey()
	{
		return Header::Leaf() ? LeafPairList().MinKey() : InternalPairList().MinKey();
	}

	// максимальный ключ
	inline KeyType MaxKey()
	{
		return Header::Leaf() ? LeafPairList().MaxKey() : InternalPairList().MaxKey();
	}

	// установить ключ i-го элемента
	// опасный метод, не контролирует порядок элементов!!!
	inline void SetKey(SizeType i, KeyType key)
	{
		Header::Leaf() ? LeafPairList()[i].SetKey(key) : InternalPairList()[i].SetKey(key);
	}

	// пара ключ-смещение для i-го потомка
	// только для ВНУТРЕННЕГО узла, проверки на тип узла нет, опасный метод!!!
	inline InternalPairType ChildPair(SizeType i)
	{
		return InternalPairList()[i];
	}

	// пара ключ-значение для i-го элемента
	// только для ЛИСТА, проверки на тип узла нет, опасный метод!!!
	inline LeafPairType Pair(SizeType i)
	{
		return LeafPairList()[i];
	}

	// cмещение для i-го потомка
	// только для ВНУТРЕННЕГО узла, проверки на тип узла нет, опасный метод!!!
	inline OffsetType Offset(SizeType i)
	{
		return InternalPairList()[i].Value();
	}

	// значение для i-го элемента
	// только для ЛИСТА, проверки на тип узла нет, опасный метод!!!
	inline ValueType Value(SizeType i)
	{
		return LeafPairList()[i].Value();
	}

	//
	// модификация узла
	//

	// вставка во ВНУТРЕННИЙ узел
	inline bool InsertChild(KeyType key, OffsetType offset)
	{
		return InternalPairList().Insert(key, offset);
	}

	// вставка в УЗЕЛ-ЛИСТ
	inline bool InsertValue(KeyType key, ValueType value)
	{
		return LeafPairList().Insert(key, value);
	}

	// удалить элемент по индексу
	inline void Delete(SizeType i)
	{
		Header::Leaf() ? LeafPairList().Delete(i) : InternalPairList().Delete(i);
	}

	// удалить элемент по ключу
	inline bool DeleteWise(KeyType key)
	{
		return Header::Leaf() ? LeafPairList().DeleteWise(key) : InternalPairList().DeleteWise(key);
	}

	inline void DeleteMajorFrom(SizeType i)
	{
		Header::Leaf() ? LeafPairList().DeleteMajorFrom(i) : InternalPairList().DeleteMajorFrom(i);
	}

	// задать смещение для элемента с заданным ключом,
	// только для ВНУТРЕННЕГО узла
	inline bool SetOffsetForKey(KeyType key, OffsetType offset)
	{
		if (Header::Leaf())
			return false;
		return InternalPairList().SetValueForKey(key, offset);
	}

	// задать смещение для элемента с заданным индексом,
	// только для ВНУТРЕННЕГО узла
	inline bool SetOffset(SizeType i, OffsetType offset)
	{
		if (Header::Leaf())
			return false;
		InternalPairList()[i].SetValue(offset);
		return true;
	}

	//
	// поиск
	//

	// поиск пути (в какого потомка идти)
	inline SizeType FindPath(KeyType key)
	{
		return Header::Leaf() ? FindPath<LeafPairListType>(key, LeafPairList()) : FindPath<InternalPairListType>(key, InternalPairList());
	}

	// поиск элемента с искомым ключом
	inline SizeType Find(KeyType key)
	{
		return Header::Leaf() ? LeafPairList().Find(key) : InternalPairList().Find(key);
	}

	//
	// функции, используемые при разделении, перегруппировке и слиянии узлов
	// функции должны вызываться только для ПОЛНОГО массива!!!
	//
	// копирует половину элементов, содержащую меньшие ключи
	inline void CopyMinor(Node& node)
	{
		if (Header::Leaf())
			LeafPairList().CopyMinor(node.LeafPairList());
		else
			InternalPairList().CopyMinor(node.InternalPairList());
	}

	// копирует половину элементов, содержащую большие ключи
	inline void CopyMajor(Node& node)
	{
		if (Header::Leaf())
			LeafPairList().CopyMajor(node.LeafPairList());
		else
			InternalPairList().CopyMajor(node.InternalPairList());
	}

	// оставляет в узле только половину меньших (по ключу) элементов
	inline void MakeMinorPart()
	{
		Header::Leaf() ? LeafPairList().MakeMinorPart() : InternalPairList().MakeMinorPart();
	}

	// оставляет в узле только половину бОльших (по ключу) элементов
	inline void MakeMajorPart()
	{
		Header::Leaf() ? LeafPairList().MakeMajorPart() : InternalPairList().MakeMajorPart();
	}

	// возвращает узлу первозданный вид после вызова MakeMinorPart
	inline void RestoreFromMinorPart()
	{
		Header::Leaf() ? LeafPairList().RestoreFromMinorPart() : InternalPairList().RestoreFromMinorPart();
	}

	// возвращает узлу первозданный вид после вызова MakeMajorPart
	inline void RestoreFromMajorPart()
	{
		Header::Leaf() ? LeafPairList().RestoreFromMajorPart() : InternalPairList().RestoreFromMajorPart();
	}

	// перемещает num мЕньших элементов в node, содержащий мЕньшие элементы, чем в нас
	// опасный метод, не контролирует порядок ключей, вызывать с умом!!!
	inline bool MoveMinor2Minor(Node& node, SizeType num)
	{
		if (Header::Leaf())
			return LeafPairList().MoveMinor2Minor(node.LeafPairList(), num);
		else
			return InternalPairList().MoveMinor2Minor(node.InternalPairList(), num);
	}

	// перемещает num бОльших элементов в node, содержащий бОльшие элементы, чем в нас
	// опасный метод, не контролирует порядок ключей, вызывать с умом!!!
	inline bool MoveMajor2Major(Node& node, SizeType num)
	{
		if (Header::Leaf())
			return LeafPairList().MoveMajor2Major(node.LeafPairList(), num);
		else
			return InternalPairList().MoveMajor2Major(node.InternalPairList(), num);
	}

	// слияние с узлом, гарантированно содержащим большие ключи
	// опасный метод, не контролирует порядок ключей, вызывать с умом!!!
	inline bool MergeWithMajor(Node& node)
	{
		if (Header::Leaf())
			return LeafPairList().MergeWithMajor(node.LeafPairList());
		else
			return InternalPairList().MergeWithMajor(node.InternalPairList());
	}

	// слияние с узлом, гарантированно содержащим меньшие ключи
	// опасный метод, не контролирует порядок ключей, вызывать с умом!!!
	inline bool MergeWithMinor(Node& node)
	{
		if (Header::Leaf())
			return LeafPairList().MergeWithMinor(node.LeafPairList());
		else
			return InternalPairList().MergeWithMinor(node.InternalPairList());
	}

private:

	// универсальный поиск пути независимо от того, что хранится в PairList
	template<class PairList>
	static SizeType FindPath(KeyType key, PairList& list)
	{
		SizeType lastIndex = list.Size() - 1;

		for(SizeType i = 0; i < lastIndex; i++)
		{
			if (key >= list[i].Key())
				return i;
		}

		return lastIndex;
	}

	// возвращает ссылку на список пар ключ-значения для внутреннего узла
	inline InternalPairListType& InternalPairList()
	{
		return (InternalPairListType&)*_buffer;
	}

	// возвращает ссылку на список пар ключ-значения для узла-листа
	inline LeafPairListType& LeafPairList()
	{
		return (LeafPairListType&)*_buffer;
	}

private:

	unsigned char _buffer[_pageSize - sizeof(Header)];
};
#pragma pack (pop)

}
}
}

#endif // _RBLIB_BD_BPLUS_NODE_H_

