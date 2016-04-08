
#ifndef _RBLIB_BD_BPLUS_TREE_H_
#define _RBLIB_BD_BPLUS_TREE_H_

#include "file_client.h"
#include "b_plus_tree/node.h"
#include "b_plus_tree/node_wrapper.h"
#include "digit_selector.h"

namespace Rblib
{
// Класс, реализующий B+ дерево
// FS - доступ к файловой система
// Key - тип ключа, по которому будем сортировать и искать записи
// Value - тип хранимого значения (размер Value должен позволять умещать на страницу не менее 4-х пар ключ-значение)
// _64bit - выбор разрядности смещения в файле (максимальный размер файла с деревом 2^32 или 2^64), для FAT32 всегда _64bit=false
// _pageSize размер физиче7ской страницы, которую удобно считывать за 1 раз
template<class FS, class Key, class Value, bool _64bit = false, unsigned long _pageSize = 512>
class BPlusTree : private Db::FileClient<FS>
{

private:

	//
	// внутренние типы данных
	//
	typedef Db::BPlusTree::Node<Key, Value, _64bit, _pageSize> NodeType; // тип узла
	typedef Db::BPlusTree::NodeWrapper<NodeType> NodeWrapperType; // обёртка узла, хранящая доп параметры, нужные для обработки
	typedef typename NodeType::OffsetType OffsetType; // тип для представления смещения узлов в файле
	typedef typename NodeType::SizeType SizeType; // тип для пердставления размера списка в узле
	typedef Db::FileClient<FS> FileClient; // доступ к файлу

public:

	//
	// типы данных, которые понадобятся клиентам класса
	//
	// пара ключ-зачение, значение - данные пользователя, ассоциированные с ключом
	typedef typename NodeType::LeafPairType PairType;

public:

	BPlusTree(FS& fs)
	: FileClient(fs)
	, _currentOffset(0)
	, _currentElement(-1) // инитим недопустимым значение
	, _needLastKeyCheck(false)
	, _rootModified(false)
	, _rootUnvalid(true) // в начале корнеь не считан
	{
	}

	//
	// открыть файл с деревом или создать новое дерево
	//
	bool Open(char* fileName)
	{
		if (!FileClient::Open(fileName))
			return false;

		if (LoadRoot()) // считываем корень
			return true;

		if (FileClient::IoFault()) // если не загрузили и была ошибка чтения
			return false;

		_root.Reset(); // чистим узел, который будет корнем
		_root.SetRoot();
		_root.SetLeaf(); // в начале корень является листом

		if (!SaveRoot())
			return false; // пишем новый корень
		return FileClient::Flush(); // сохраняем FAT
	}

	//
	// закрыть файл с деревом
	//
	bool Close()
	{ 
		return FileClient::Close();
	}

	//
	// сохранить несохранённое
	//
	inline bool Flush()
	{
		bool rootOk = FlushRoot(); // сохраняем корень
		bool fatOk = FileClient::Flush(); // сохраняем всё остальное (внутренности FAT)
		return rootOk && fatOk;
	}

	//
	// встака в дерево
	//
	bool Insert(Key key, Value value)
	{
		if (!CheckRoot()) // корень должен быть актуальным
			return false;

		// для разделения нужно будет 2 объекта-узла (родитель и потомок)
		NodeType node1; // первый объект-узел
		NodeType* curNode = &_root; // текущий узел - корень
		OffsetType curNodeOffset = 0;

		NodeType* curParent = &_root; // родитель текущего узла
		OffsetType curParentOffset = 0; // смещение родителя текущего узла
		SizeType curIndex = 0; // индекс элемента в родителе, ссылающегося на текущий узел

		if (_root.Full()) // если корень уже полный, то его надо разделить
		{
			// сразу вычисляем дальнейший путь,
			// чтобы в текущем узле (curNode) после разделения оказался нужный узел (половина корня) и не надо было его снова считывать
			SizeType pathIndex = _root.FindPath(key);
			curIndex = _root.MajorIndex(pathIndex) ? 0 : 1; // запоминаем индекс нового узла в новом корне, понадобится при вставке
			curNode = &node1; // теперь текущий узел - пустой объект, заполнится в SplitRoot
			if ((curNodeOffset = SplitRoot(*curNode, _root.MajorIndex(pathIndex))) == 0) // разделяем корень
				return false;
		}

		NodeType node2; // второй объект-узел
		NodeType* child = &node2; // потомок - пустой объект, заполнится при считывании (LoadNode)

		// ищем узел-лист, в который надо произвести вставку,
		// по пути разделяем узлы, если нужно
		while(!curNode->Leaf())
		{
			// определяем потомка (поддерево), в котором искомый ключ
			SizeType pathInParentIndex = curNode->FindPath(key); // номер в массива
			OffsetType childOffset = curNode->Offset(pathInParentIndex); // смещение в файле

			// запоминаем родителя, понадобится при вставке
			curParent = curNode;
			curParentOffset = curNodeOffset;
			curIndex = pathInParentIndex;

			if (!LoadNodeWise(*child, childOffset, pathInParentIndex > 0, curNode->Key(pathInParentIndex - 1))) // считываем узел
				return false;

			// если узел уже заполнен, то его надо разделить
			if (child->Full())
			{
				// сразу вычисляем дальнейший путь,
				// чтобы в текущем потомке (child) после разделения оказался нужный узел (половина текущего потомка)
				//  и не надо было его снова считывать
				SizeType pathInChildIndex = child->FindPath(key);
				// разделяем текущего потомка (child)
				curIndex += child->MajorIndex(pathInChildIndex) ? 0 : 1;
				if ((childOffset = SplitNodeWise(*curNode, curNodeOffset, pathInParentIndex, *child, childOffset, child->MajorIndex(pathInChildIndex))) == 0)
					return false;
			}

			// на следуещей итерации смещением текущего узла будет смещение текущего потомка
			curNodeOffset = childOffset;

			if (curNode->Root()) // если текущий узел корень (значит, разделения корня не было)
			{
				// текущий узел будет ссылаться на объект, в котором потомок
				curNode = child;
				// а потомок будет ссылаться на ещё не использованный объект node1
				child = &node1;
			}
			else
			{
				// меняем ссылки на объекты узлов
				// текущий узлел будет ссылаться на объект, в котором потомок
				// а потомок будет ссылыться на объект, в котором уже ненужный текущий узел
				NodeType* tmp = curNode;
				curNode = child;
				child = tmp;
			}
		}
		
		curNode->InsertValue(key, value); // добавляем новое значние в узел-лист
		
		// если вставка не в корень и изменился мин. ключ, надо сначала поменять и сохранить родителя
		if (!curNode->Root() && curParent->Key(curIndex) > curNode->MinKey())
		{
			curParent->SetKey(curIndex, curNode->MinKey());
			if (!SaveNode(*curParent, curParentOffset))
				return false; // если родителя поменять не смогли, вставка не удалась
		}

		return SaveNode(*curNode, curNodeOffset); // сохраняем узел и выходим
	}

	//
	// поиск значения по ключу
	// value - найденное значение
	//
	bool Find(Key key, Value& value)
	{
		if (!CheckRoot()) // корень должен быть актуальным
			return false;

		NodeType* curNode = &_root;
		NodeType node;
		
		while(!curNode->Leaf()) // спускаемся по дереву до листа
		{
			SizeType index = curNode->FindPath(key); // определяем поддерево (узел потомок) с искомым ключом
			OffsetType nextOffset = curNode->Offset(index);
			curNode = &node;
			if (!LoadNodeWise(*curNode, nextOffset, index > 0, curNode->Key(index - 1))) // считываем потомка
				return false;
		}
		
		// в curNode узел-лист, в котором должен быть искомый ключ
		SizeType index = curNode->Find(key);
		if (index == curNode->Size()) // если не нашли в узле ключа
			return false; // выходим

		// иначе записываем значение в value
		value = curNode->Value(index);
		return true;
	}

	//
	// удаление по ключу
	//
	bool Delete(Key key)
	{
		_lostElementsCount = 0; // сбрасываем кол-во потерянных элементов (для отладки)

		if (!CheckRoot()) // корень должен быть актуальным
			return false;

		// если корень - лист
		if (_root.Leaf())
		{
			// то просто удаляем элемент
			if (!_root.DeleteWise(key))
				return false;
			return SaveRoot(); // сохраняем корень
		}

		// 2 объекта, в которые будем считывать нужные узлы
		NodeType node1;
		NodeType node2;
		NodeType* parent = &_root; // текущий родитель
		OffsetType parentOffset = 0;

		NodeType* child = &node1; // текущий потомок
		NodeType* childSibling = &node2; // текущий брат потомка (соседний узел, потомок parent)

		SizeType index;
		typename NodeType::InternalPairType nodePair;

		// ищем узел-лист с искомым ключом, попутно перегруппировывая узлы, если надо
		do 
		{
			index = parent->FindPath(key); // определяем поддерево с искомым ключом
			nodePair = parent->ChildPair(index); // тут информация о нужном потомке

			bool root = parent->Root();
			// определяем, какой брат нам нужен (левый или правый)
			bool right = index > 0;
			SizeType index2 = right ? index - 1 : index + 1;
			typename NodeType::InternalPairType nodePair2 = parent->ChildPair(index2); // тут информация о брате

			if (!LoadNodeWise(*child, nodePair.Value(), index > 0, parent->Key(index - 1))) // считываем нужного потомка
				return false;

			if (child->Thin()) // если потомок полупустой, то потребуется перегруппировка с братом
			{
				if (!LoadNodeWise(*childSibling, nodePair2.Value(), index2 > 0, parent->Key(index2 - 1))) // считываем брата
					return false;

				// далее перегруппировка
				if (root) // если родитель нужного потомка и брата является коренем
				{
					NodeWrapperType childWrapper(child, index, nodePair.Key(), nodePair.Value());
					NodeWrapperType siblingWrapper(childSibling, index2, nodePair2.Key(), nodePair2.Value());
					if (!Redistribute(childWrapper, siblingWrapper, right))
						return false;
					nodePair.SetValue(childWrapper.Offset); // смещение узла изменилось, учитываем это
				}
				else
				{
					NodeWrapperType childWrapper(child, index, nodePair.Key(), nodePair.Value());
					NodeWrapperType siblingWrapper(childSibling, index2, nodePair2.Key(), nodePair2.Value());
					if (!Redistribute(childWrapper, siblingWrapper, parentOffset, right))
						return false;
					nodePair.SetValue(childWrapper.Offset); // смещение узла изменилось, учитываем это
				}
			}

			if (child->Leaf()) // если текущий потом - лист, то поиск окончен
				break;

			// текущий потомок будет родителем
			// если потомок стал корнем, то родителем будет корень
			// ! потомок мог стать корнем, если он был сыном корня,
			// ! и  у корня было всего 2 потомка, и они были слиты в один при перегруппировке
			parent = child->Root() ? &_root : child;
			parentOffset = child->Root() ? 0 : nodePair.Value();

			if (!child->Root())
			{
				child = childSibling; // слудующий потомок будет считан на место брата
				childSibling = parent; // брат будет считан на место родителя
			}

		} while (1);

		SizeType i = child->Find(key); // ищем ключ в листе
		bool minKeyRemoved = i == (child->Size() - 1);
		if (i >= child->Size()) // если не нашли
			return false;

		// если потомок стал корнем, то удаляем из корня и выходим
		if (child->Root())
		{
			_root.Delete(i);
			return SaveRoot();
		}

		// удаляем элемент и сохраняем узел
		child->Delete(i);
		if (!SaveNode(*child, nodePair.Value()))
			return false;

		// требуется ли модифицировать родителя
		// ! модификация родителя можт потребоваться, если удалили элемент с минимальным ключом,
		if (!minKeyRemoved)
			return true;

		// меняем ключ в родителе и сохраняем
		parent->SetKey(index, child->MinKey());
		SaveNode(*parent, parentOffset); // результат не важен, т.к. элемент уже удалён
		return true;
	}

	//
	// извлекает из дерева значение с наименьшим ключом,
	// подготавливает обход всего дерева
	//
	bool First(PairType& value)
	{
		if (!CheckRoot()) // корень должен быть актуальным
			return false;

		if (_root.Empty())
		{
			// если нечего обходить
			SetIteratorInvalid();
			return false;
		}

		if (_root.Leaf()) // если корень - лист
			return FirstRoot(value);

		return FirstGeneral(value);
	}

	// тот же First, только ключ и значение передаются по отдельности, а не парой
	inline bool First(Key& key, Value& value)
	{
		if (!CheckRoot()) // корень должен быть актуальным
			return false;

		PairType pair;
		bool res = First(pair);
		key = pair.Key();
		value = pair.Value();
		return res;
	}

	//
	// извлекает из дерева значение с наименьшим ключом большим или равным key,
	// подготавливает обход дерева, начиная с этого ключа
	//
	bool First(Key key, PairType& value)
	{
		if (!CheckRoot()) // корень должен быть актуальным
			return false;

		if (_root.Empty())
		{
			// если нечего обходить
			SetIteratorInvalid();
			return false;
		}

		if (_root.Leaf()) // если корень - лист
			return FirstRoot(key, value);

		return FirstGeneral(key, value);
	}

	// тот же First, только ключ и значение передаются по отдельности, а не парой
	bool First(Key keyIn, Key& key, Value& value)
	{
		if (!CheckRoot()) // корень должен быть актуальным
			return false;

		PairType pair;
		bool res = First(keyIn, pair);
		key = pair.Key();
		value = pair.Value();
		return res;
	}

	//
	// обход дерева, извлекает очередной элемент
	//
	bool Next(PairType& value)
	{
		if (!CheckRoot()) // корень должен быть актуальным
			return false;

		if (IteratorInvalid()) // если обход уже окончен
			return false;

		if (_currentOffset == 0) // если корень-лист и обход в нём
			return NextRoot(value);

		return NextGeneral(value);
	}

	// тот же Next, только ключ и значение передаются по отдельности, а не парой
	bool Next(Key& key, Value& value)
	{
		if (!CheckRoot()) // корень должен быть актуальным
			return false;

		PairType pair;
		bool res = Next(pair);
		key = pair.Key();
		value = pair.Value();
		return res;
	}

	//
	// методы для отладки
	//

	// количество потерянных элементов при последнем удалении (если была ошибка при перегруппировке)
	inline SizeType LostCount()
	{
		return _lostElementsCount;
	}

	// возвращает смещение текущего узла (обход)
	inline OffsetType CurrentOffset()
	{
		return _currentOffset;
	}

private:

	inline bool CheckRoot()
	{
		if (_rootUnvalid == false) // если корень актуальный, то ничего не надо делать
			return true;
		return LoadRoot(); // иначе считываем с диска
	}

	// считывает корень
	inline bool LoadRoot()
	{
		_rootModified = false;
		if (LoadNodeRaw(_root, 0))
		{
			_rootUnvalid = false;
			return true;
		}
		return false;
	}

	// сохраняет корень
	inline bool SaveRoot()
	{
		return SaveNode(_root, 0);
	}

	// сохраняет изменения в корне, если они есть
	inline bool FlushRoot()
	{
		// если меняли корень и не сохранили, пытаемся сохранить
		if (!_rootModified)
			return true;
		_rootModified = false; // пытаемся сохранить только 1 раз, иначе изменения откатываются
		return SaveRoot();
	}

	// считать узел по указанному смещению
	bool LoadNodeRaw(NodeType& node, OffsetType offset)
	{
		if (!FileClient::SeekFile(offset))
			return false;
		return FileClient::ReadFile((unsigned char*) &node, sizeof(NodeType));
	}

	// метод считывает узел, проверяет и корректирует ошибку перегруппировки (см. описание метода RedistributeLeaf)
	bool LoadNodeWise(NodeType& node, OffsetType offset, bool checkError, Key rightKey)
	{
		if (!LoadNodeRaw(node, offset))
			return false;

		// ошибка может быть только в листьях, не может быть в корне
		if (!checkError || !node.Leaf() || node.Root())
			return true;

		// проверяем на ошибку и корректируем, если надо
		CorrectRedistributionError(node, rightKey);

		return true;
	}

	// возвращает указатель на узел, соответствующий смещению;
	// если смещение = 0, то просто возвращается указатель на _root,
	// иначе в объект tempNode считывается узел по смещению и возвращется указатель на tempNode
	NodeType* GetNode(OffsetType offset, NodeType& tempNode)
	{
		// если нужен не корень, считываем его в объект tempNode
		if (offset)
		{
			if (!LoadNodeRaw(tempNode, offset)) 
				return 0;
			return &tempNode;
		}
		return &_root; // иначе возвращаем указатель на уже считанный корень
	}

	// сохранить узел по указанному смещению
	bool SaveNode(NodeType& node, OffsetType offset)
	{
		if (offset == 0) // если сохраняем корень
			_rootModified = false; // пытаемся сохранить только 1 раз, иначе изменения откатываются

		if (SaveNodeRaw(node, offset)) // если сохранили, то выходим
			return true;

		if (offset == 0) // если корень
			_rootUnvalid = true; // выставляем флаг неактуальности корня

		return false;
	}

	// сохраняет узел, возвращает смещение
	OffsetType SaveNode(NodeType& node)
	{
		OffsetType offset = FileClient::FileSize(); // если нет пустых страницы, то сохраним в конец файла
		
		if (_root.FirstFreePage()) // если есть свободные страницы (ранее удалённые узлы)
		{
			offset = _root.FirstFreePage(); // берём первую пустую страницу

			OffsetType nextFree = 0;
			// считываем это пустую страницу, чтобы узнать смещение следующей пустой страницы
			if (!LoadNextFreeRecordId(offset, nextFree))
				return 0;

			_root.SetFirstFreePage(nextFree); // меняем смещение первой пустой страницы
			if (!SaveRoot()) // сразу сохраняем корень
				return false;
		}

		if (!SaveNode(node, offset)) // сохраняем узел
			return 0;

		return offset;
	}

	// сохранить узел по указанному смещению,
	// используйте SaveNodeне, т.к. этот метод не контролирует флаги _rootModified и _rootUnvalid
	bool SaveNodeRaw(NodeType& node, OffsetType offset)
	{
		if (!FileClient::SeekFile(offset))
			return false;
		return FileClient::WriteFile((unsigned char*) &node, sizeof(NodeType));
	}

	//
	// логика дерева
	//

	// разделить корень
	// node - объект, в котором будет возвращён один из получившихся узлов
	// major - флаг, определяющий, какой узел вернуть (с большими или меньшими ключами)
	// возвращает смещение одного из новых узлов
	OffsetType SplitRoot(NodeType& node, bool major)
	{
		if (_root.Leaf()) // если корень лист, то и новые узлы будут листьями
			node.SetLeaf();

		// сначала копируем из корня половину больших ключей и сохраняем
		_root.CopyMajor(node);
		Key majorMinKey = node.MinKey(); // минимальный ключ в узле
		OffsetType majorOffset = SaveNode(node); // сохраняем, получаем смещение в файле
		if (majorOffset == 0)
			return 0;

		// потом копируем половину меньших ключей и сохраняем
		_root.CopyMinor(node);
		// если новые узлы листья, то надо связать их в список
		if (node.Leaf())
			node.SetNextLeafPage(majorOffset);
		Key minorMinKey = node.MinKey(); // минимальный ключ в узле
		OffsetType minorOffset = SaveNode(node); // сохраняем, получаем смещение в файле
		if (minorOffset == 0)
			return 0;

		// если были добавлены новые страницы, то надо сразу сохарнить размер файла и FAT-таблицу
		if (!FileClient::Flush())
			return false;

		// если на выходе нужна половина с большими ключами, снова копируем её
		if (major)
		{
			_root.CopyMajor(node);
			if (node.Leaf())
				node.SetNextFreePage(0);
		}

		// обновляем корень
		_root.Clear();
		_root.ClearLeaf(); // теперь точно не лист
		_root.InsertChild(majorMinKey, majorOffset);
		_root.InsertChild(minorMinKey, minorOffset);

		if (!SaveRoot()) // сохраняем корень
			return 0;

		return major ? majorOffset : minorOffset;
	}

	// разделить узел-антикорень, универсальный метод
	// node - узел, который надо разделить и в котором будет возвращён один из получившихся узлов
	// major - флаг, определяющий, какой узел вернуть (с большими или меньшими ключами)
	// offset - смещение разделяемого узла (node)
	// parentOffset - смещение родителя
	// возвращает смещение одного из двух узлов, полученных при разделении
	inline OffsetType SplitNodeWise(NodeType& parent, OffsetType parentOffset, SizeType indexInParent, NodeType& node, OffsetType offset, bool major)
	{
		if (node.Leaf())
			return SplitLeafNode(parent, parentOffset, indexInParent, node, offset, major);
		return SplitInternalNode(parent, parentOffset, indexInParent, node, offset, major);
	}

	// разделить узел-антикорень (не лист)
	// node - узел, который надо разделить и в котором будет возвращён один из получившихся узлов
	// major - флаг, определяющий, какой узел вернуть (с большими или меньшими ключами)
	// offset - смещение разделяемого узла (node)
	// parentOffset - смещение родителя
	// возвращает смещение одного из двух узлов, полученных при разделении
	OffsetType SplitInternalNode(NodeType& parent, OffsetType parentOffset, SizeType indexInParent, NodeType& node, OffsetType offset, bool major)
	{
		// сначала берём половину больших значений и сохраняем в новом узле
		node.MakeMajorPart();
		Key majorMinKey = node.MinKey(); // минимальный ключ нового узла
		OffsetType majorOffset = SaveNode(node); // сохраняем на новое место, получаем смещение нового узла
		if (majorOffset == 0)
			return 0;

		// возвращаем узлу первозданный вид
		node.RestoreFromMajorPart();

		// берём другую половину значений (меньишх) и сохраняем в новом узле
		node.MakeMinorPart();

		Key minorMinKey = node.MinKey(); // минимальный ключ нового узла
		OffsetType minorOffset = SaveNode(node); // сохраняем на новое место, получаем смещение нового узла
		if (minorOffset == 0)
			return 0;

		// если были добавлены новые страницы, то надо сразу сохарнить размер файла и FAT-таблицу
		if (!FileClient::Flush())
			return false;

		// теперь надо внести изменения в родителя (добавить новые узлы, удалить старые)
		// заменяем ссылку на старый узел, ссылкой на новый (с меньшими значениями),
		// обязательно корректируем ключ до (!!!) вставки нового элемента
		parent.SetKey(indexInParent, node.MinKey());
		parent.SetOffset(indexInParent, minorOffset);
		parent.InsertChild(majorMinKey, majorOffset); // добавляем новый узел (большие значения)
		if (!SaveNode(parent, parentOffset)) // сохраняем родителя
		{
			// TODO: здесь можно попытаться освободить занятые новыми узлами страницы,
			// в текущей реализаии они просто не будут больше использоваться
			return false; // если не сохранили, то выходим
		}

		//
		// теперь надо освободить старую страницу, где лежал node, её смещене в offset,
		// для этого надо её добавить в список пустых страниц
		//

		// сохраняем корень на место пустой страницы,
		// чтобы в поле NextFreePage пустой страницы оказалось значение поля FirstFreePage корня
		if (!SaveNode(_root, offset))
			return false;

		// делаем новую пустую страницу первой в списке пустых страниц и сохраняем корень
		_root.SetFirstFreePage(offset);
		if (!SaveRoot())
			return false;

		// всё сохранилось, проконтролируем результат;
		// если на выходе нужна половина с большими ключами, снова модицифицируем узел
		if (major)
		{
			node.RestoreFromMinorPart();
			node.MakeMajorPart();
		}

		return major ? majorOffset : minorOffset;
	}

	// разделить узел-антикорень, являющийся листом
	// node - узел, который надо разделить и в котором будет возвращён один из получившихся узлов
	// major - флаг, определяющий, какой узел вернуть (с большими или меньшими ключами)
	// offset - смещение разделяемого узла (node)
	// parentOffset - смещение родителя
	// возвращает смещение одного из двух узлов, полученных при разделении
	OffsetType SplitLeafNode(NodeType& parent, OffsetType parentOffset, SizeType indexInParent, NodeType& node, OffsetType offset, bool major)
	{
		// сначала берём половину больших значений и сохраняем в новом узле
		node.MakeMajorPart();
		Key majorMinKey = node.MinKey();
		OffsetType majorOffset = SaveNode(node);
		if (majorOffset == 0)
			return 0;

		// если были добавлены новые страницы, то надо сразу сохарнить размер файла и FAT-таблицу
		if (!FileClient::Flush())
			return false;

		// теперь надо включить новый узел в цепочку листьев,
		// для этого искомый узел должен на него ссылаться

		// возвращаем узлу первозданный вид
		node.RestoreFromMajorPart();
		OffsetType majorNextLeaf = node.NextLeafPage(); // запоминаем значние поля NextLeaf для старшего узла
		node.SetNextLeafPage(majorOffset); // теперь искомый узел должен ссылаться на вновь созданный выше узел
		if (!SaveNode(node, offset)) // сохраняем узел на старом месте
		{
			// если в этом месте сохранить не удалось, теряем одну страницу,
			// можно попытаться её освободить (поставить в список пустых)
			return 0;
		}

		// добавляем в родителя ссылку на полученный новый узел с большими значениями
		parent.InsertChild(majorMinKey, majorOffset);
		if (!SaveNode(parent, parentOffset)) // сохраняем родителя
		{
			// если в этом месте сохранить не удалось, получаем страницу в списке листьев,
			// сожержимое которой должно пропускаться при обходе
			return 0;
		}

		// теперь берём другую половину значений (меньишх) и опять сохраняем на старом месте
		// поле NextLeafPage уже правильное
		node.MakeMinorPart();
		if (!SaveNode(node, offset)) // сохраняем
		{
			// если тут ошибка то получаем лист, содержащий ключи,
			// дублирующие ключи из соседнего листа (major листа),
			// эти ключи должны пропускаться при обходе,
			// а ошибка корректироваться при разделениях при разделениях и слияниях,
			// для этого страницы загружаются методом LoadNodeWise
			return 0;
		}

		// если на выходе нужна половина с большими ключами, снова модицифицируем узел
		if (major)
		{
			node.RestoreFromMinorPart();
			node.MakeMajorPart();
			node.SetNextLeafPage(majorNextLeaf); // восстанавливаем ссылку на следующий лист
		}

		return major ? majorOffset : offset;
	}

	//
	// перегруппировка узлов
	//
	// формирует 1 или 2 полноценных узла из одного полупустого и его правого брата.
	// если в правом узле ключей больше половины, то берём из него один ключ
	// если правый узел тоже полупустой, сливаем оба узла в один node1
	bool RedistributeWithRight(NodeType& node, NodeType& rightNode)
	{
		SizeType elementsToMove = (rightNode.Size() - node.Size()) >> 1;
		// если правый узел не станет полупустым, после выравнивания размеров, то выравниваем
		if (!rightNode.ThinSize(rightNode.Size() - elementsToMove))
		{
			_lastMovedCount = elementsToMove; // запоминаем кол-во перемещённых элементов (для отладки)
			// выравниваем размеры узлов (перемещаем половину разницы размеров)
			return rightNode.MoveMinor2Minor(node, elementsToMove);
		}

		_lastMovedCount = rightNode.Size(); // запоминаем кол-во перемещённых элементов (для отладки)

		// иначе сливаем в один узел
		if (!node.MergeWithMajor(rightNode))
			return false; // ошибок быть не долнжно, иначе вызвали метод с неправильными узлами!!!

		rightNode.Clear();

		return true;
	}

	// формирует 1 или 2 полноценных узла из одного полупустого и его левого брата.
	// если в левом узле ключей больше половины, то берём из него один ключ
	// если левый узел тоже полупустой, сливаем оба узла в один leftNode
	bool RedistributeWithLeft(NodeType& node, NodeType& leftNode)
	{
		SizeType elementsToMove = (leftNode.Size() - node.Size()) >> 1;
		// если левый узел не станет полупустым, после выравнивания размеров, то выравниваем
		if (!leftNode.ThinSize(leftNode.Size() - elementsToMove))
		{
			_lastMovedCount = elementsToMove; // запоминаем кол-во перемещённых элементов (для отладки)
			// выравниваем размеры узлов (перемещаем половину разницы размеров)
			return leftNode.MoveMajor2Major(node, elementsToMove);
		}

		_lastMovedCount = leftNode.Size(); // запоминаем кол-во перемещённых элементов (для отладки)

		// иначе сливаем в один узел
		if (!node.MergeWithMinor(leftNode))
			return false; // ошибок быть не должно, иначе вызвали метод с неправильными узлами!!!

		leftNode.Clear();

		return true;
	}

	//
	// перегруппировка узлов
	//
	// перегруппировка потомков корня
	inline bool Redistribute(NodeWrapperType& node, NodeWrapperType& nodeSibling, bool right)
	{
		if (node.Node->Leaf())
			return RedistributeLeaf(node, nodeSibling, 0, right);
		return RedistributeInternal(node, nodeSibling, 0, right);
	}

	// перегруппировка потомков обычного узла
	bool inline Redistribute(NodeWrapperType& node, NodeWrapperType& nodeSibling, OffsetType parentOffset, bool right)
	{
		if (node.Node->Leaf())
			return RedistributeLeaf(node, nodeSibling, parentOffset, right);
		return RedistributeInternal(node, nodeSibling, parentOffset, right);
	}

	// перегруппировка узла и его брата, яляющихся внутренними узлами
	// для сохранения целостности в случае ошибок модифицированные узлы сохраняются на НОВЫЕ страницы,
	// потом модифицируется родитель, затем освобождаются старые узлы
	bool RedistributeInternal(NodeWrapperType& node, NodeWrapperType& nodeSibling, OffsetType parentOffset, bool right)
	{
		// пераспределяем элементы между узлами
		if (right)
			RedistributeWithRight(*node.Node, *nodeSibling.Node); // перегруппируем искомый узел и узел справа от него
		else
			RedistributeWithLeft(*node.Node, *nodeSibling.Node); // перегруппируем искомый узел и узел слева от него

		// родитель - корень, в результате перегруппировки у корня остался только один потомок,
		// делаем этого потомка корнем
		if (parentOffset == 0 && _root.Size() == 2 && nodeSibling.Node->Empty())
			return RedistributeRoot(node, nodeSibling);

		// сохраняем искомый узел на новое место
		OffsetType nodeNewOffset = SaveNode(*node.Node);
		if (nodeNewOffset == 0)
			return false;

		// если брат не опустел, сохраняем на новое место
		OffsetType siblingNewOffset;
		if (!nodeSibling.Node->Empty())
		{
			siblingNewOffset = SaveNode(*nodeSibling.Node);
			if (siblingNewOffset == 0)
				return false;
		}

		// узлы могли сохраниться на освобождённые страницы,
		// в этом случаем мы должны обязятельно зафиксировать изменения в корне (список пустых страниц) до изменения родительского узла;
		// для родителя-корня не надо
		if (parentOffset != 0)
		{
			if (!FlushRoot())
				return false;
		}
		// если были добавлены новые страницы, то надо сразу сохарнить размер файла и FAT-таблицу
		if (!FileClient::Flush())
			return false;

		// запоминаем мин. ключ брата, т.к. узел сейчас перепишется (если родитель не корень)
		Key siblingMinKey = nodeSibling.Node->MinKey();
		bool siblingEmpty = nodeSibling.Node->Empty(); // запоминаем размер брата

		// если родитель не корень, считываем его в уже ненужный объект nodeSibling,
		// иначе берём _root
		NodeType* nodeParent = GetNode(parentOffset, *nodeSibling.Node);
		if (nodeParent == 0)
			return false;

		// модифицируем родителя

		// информация об искомом узле
		nodeParent->SetKey(node.Index, node.Node->MinKey());
		nodeParent->SetOffset(node.Index, nodeNewOffset);

		// информация о брате
		if (!siblingEmpty)
		{
			nodeParent->SetKey(nodeSibling.Index, siblingMinKey);
			nodeParent->SetOffset(nodeSibling.Index, siblingNewOffset);
		}
		else
			nodeParent->Delete(nodeSibling.Index);

		// сохраняем родителя
		if (!SaveNode(*nodeParent, parentOffset))
			return false;

		// теперь надо освободить старые страницы узлов (добавить в список пустых)

		// старая страница брата будет ссылаться на первую пусту страницу в списке,
		// для этого удобно просто сохранить корень на опустевшую страницу
		if (!SaveNode(_root, nodeSibling.Offset))
			return false;

		// старая страница искомого узла будет ссылаться на старую страницу брата,
		// используем объект nodeSibling.Node, т.к. он больше не нужен
		nodeSibling.Node->SetNextFreePage(nodeSibling.Offset);
		if (!SaveNode(*nodeSibling.Node, node.Offset))
			return false;

		// старая страница искомого узла будет первой в списке пустых
		_root.SetFirstFreePage(node.Offset);
		node.Offset = nodeNewOffset; // !!! наружу должны вернуть правильное смещение старницы узла
		return SaveRoot();
	}

	// перегруппировка потомков корня, являющихся листьями
	// в случае листьев мы не можем оба узла сохранить на новые страницы,
	// левый узел (брат или искомый) должен остаться на своей странице, т.к. на неё может ссылается лист из соседнего поддерева,
	// в связи с этим, порядок сохранения страниц разный для разных ситуаций,
	// при этом в отличие от внутренних узлов, ошибка при сохранении при перегруппировка литьев может привести к ОШИБКЕ В ДЕРЕВЕ:
	// узел может содеражать лишние ключи, дублирующие ключи соседнего узла с большими ключами, эта ошибка должна корректироваться при последующих проходах,
	// функция для корректировки определена ниже
	bool RedistributeLeaf(NodeWrapperType& node, NodeWrapperType& nodeSibling, OffsetType parentOffset, bool right)
	{
		// пераспределяем элементы между узлами
		if (right)
			RedistributeWithRight(*node.Node, *nodeSibling.Node); // перегруппируем искомый узел и узел справа от него
		else
			RedistributeWithLeft(*node.Node, *nodeSibling.Node); // перегруппируем искомый узел и узел слева от него

		// если в результате перегруппировки у корня остался только один потомок,
		// делаем этого потомка корнем
		if (parentOffset == 0 && _root.Size() == 2 && nodeSibling.Node->Empty())
			return RedistributeRoot(node, nodeSibling);

		// если брат опустел после перегруппировки
		if (nodeSibling.Node->Empty())
			return SaveRedistributeLeafSiblingEmpty(node, nodeSibling, parentOffset, right);

		// если брат не опустел
		if (right)
			return SaveRedistributeLeafSiblingRigth(node, nodeSibling, parentOffset); // если брат справа
		return SaveRedistributeLeafSiblingLeft(node, nodeSibling, parentOffset); // если брат слева
	}

	// вызывается, если у корня остался только один потомок после перегруппировки
	// делает этого потомка корнем
	bool RedistributeRoot(NodeWrapperType& node, NodeWrapperType& nodeSibling)
	{
		OffsetType nextFree = _root.FirstFreePage();
		node.Node->SetRoot(); // будет корнем
		_root = *node.Node; // делаем единственного оставшегося потомка корнем
		_root.SetFirstFreePage(nextFree); // после копирования это поля затёрлось, восстанавливаем

		if (!SaveRoot()) // сразу сохраняем корень, и только потом модифицируем список пустых страниц
			return false;

		// далее сохраняем пустые страницы,
		// назначая правильные сслыки на следующие пустые страницы
		nodeSibling.Node->SetNextFreePage(nextFree);
		if (SaveNode(*nodeSibling.Node, nodeSibling.Offset))
			nextFree = nodeSibling.Offset;

		node.Node->SetNextFreePage(nextFree);
		if (SaveNode(*node.Node, node.Offset))
			nextFree = node.Offset;

		// устанавливаем ссылку на первую пустую страницу
		_root.SetFirstFreePage(nextFree);
		// node должен сожержать корень на выходе
		node.Node->SetFirstFreePage(_root.FirstFreePage());
		node.Offset = 0;

		// сохраняем корень
		return SaveRoot();
	}

	//
	// перегруппировка, вспомогательные функции
	//
	// сохранить результаты перегруппировки,
	// если узлы - листья, и брат опустел
	bool SaveRedistributeLeafSiblingEmpty(NodeWrapperType& node, NodeWrapperType& nodeSibling, OffsetType parentOffset, bool right)
	{
		// если брат левый, то единственный оставшийся узел надо сохранять на его место
		if (!right)
		{
			// обмен страниц
			OffsetType offset = node.Offset;
			node.Offset = nodeSibling.Offset;
			nodeSibling.Offset = offset;
			SizeType index = node.Index;
			node.Index = nodeSibling.Index;
			nodeSibling.Index = index;
		}

		// искомый узел сохраняем на одну из старых страниц (если брат слева, то узел сохранится на его страницу) 
		if (!SaveNode(*node.Node, node.Offset))
			return false; // узел не сохранился, значит элемент не будет удалён, возвращаем false

		// запоминаем значение поля NextLeaf брата, т.к. он будет переписан, значение поля может понадобиться
		OffsetType SiblingNextLeaf = nodeSibling.Node->NextLeafPage();

		// берём родителя: если родитель не корень, считываем его в уже ненужный объект nodeSibling,
		// иначе берём _root
		NodeType* nodeParent = GetNode(parentOffset, *nodeSibling.Node);
		if (nodeParent == 0)
			return false;

		// удаляем ссылку на опустевший узел из корня и сохраняем
		// ключ не надо модифицировать, он и так правильный в любом случае
		nodeParent->Delete(nodeSibling.Index);
		if (!SaveNode(*nodeParent, parentOffset))
			return false; //здесь возникает ошибка перегруппировки (см. описание метода RedistributeLeaf)

		// если брат был справа, то теперь надо поправить поле NextLeaf оставшегося узла
		if (right)
		{
			// узел будет ссылаться на страницу, на которую ссылался брат
			node.Node->SetNextLeafPage(SiblingNextLeaf);
			if (!SaveNode(*node.Node, node.Offset))
				return false; // здесь появляется лишнее звено в цепочке листьев, с дублирующими ключами
		}

		// модифицируем опустевшую страницу, добавляем её в список пустых
		// в поле NextFreePage должно быть значение FirstFreePage из корня
		if (!SaveNode(_root, nodeSibling.Offset))
			return false;
		// опустевшая страница становится первой в списке пустых
		_root.SetFirstFreePage(nodeSibling.Offset);
		return SaveRoot();
	}

	// сохранить результаты перегруппировки,
	// если узлы - листья, брат справа
	bool SaveRedistributeLeafSiblingRigth(NodeWrapperType& node, NodeWrapperType& nodeSibling, OffsetType parentOffset)
	{
		// брата сохраняем на старую страницу
		if (!SaveNode(*nodeSibling.Node, nodeSibling.Offset))
			return false; // потерь нет

		_lostElementsCount = _lastMovedCount; // количество потерянных элементов, если будут ошибки при сохранении

		// запоминаем мин. ключ брата, т.к. узел сейчас перепишется (если родитель не корень)
		Key siblingMinKey = nodeSibling.Node->MinKey();

		// берём родителя: если родитель не корень, считываем его в уже ненужный объект nodeSibling,
		// иначе берём _root
		NodeType* nodeParent = GetNode(parentOffset, *nodeSibling.Node);
		if (nodeParent == 0)
			return false; // потери

		// модифицируем родителя: корректируем ключ
		nodeParent->SetKey(nodeSibling.Index, siblingMinKey);
		if (!SaveNode(*nodeParent, parentOffset))
			return false; // потери

		// искомый узел сохраняем на старое место
		if (!SaveNode(*node.Node, node.Offset))
			return false; // потери

		_lostElementsCount = 0; // потерь нет

		return true;
	}

	// сохранить результаты перегруппировки,
	// если узлы - листья, и брат слева
	bool SaveRedistributeLeafSiblingLeft(NodeWrapperType& node, NodeWrapperType& nodeSibling, OffsetType parentOffset)
	{
		// сохраняем брата на старое место
		if (!SaveNode(*nodeSibling.Node, nodeSibling.Offset))
			return false; // потерь нет

		_lostElementsCount = _lastMovedCount; // количество потерянных элементов, если будут ошибки при сохранении

		// берём родителя: если родитель не корень, считываем его в уже ненужный объект nodeSibling.Node,
		// иначе берём _root
		NodeType* nodeParent = GetNode(parentOffset, *nodeSibling.Node);
		if (nodeParent == 0)
			return false; // потери

		// меняем в родителе ключ искомого узла
		nodeParent->SetKey(node.Index, node.Node->MinKey());
		if (!SaveNode(*nodeParent, parentOffset))
			return false; // потери
		
		// сохраняем искомый узел на старую страницу
		if (!SaveNode(*node.Node))
			return false; // потери

		_lostElementsCount = 0; // потерь нет

		return true;
	}

	// метод для корректировки ошибки перераспределения листьевых узлов (см. описание метода RedistributeLeaf)
	// node - проверяемый и корректируемый узел
	// key - минимальный ключ узла
	// rightKey - минимальный ключ правого брата узла узла
	void CorrectRedistributionError(NodeType& node, Key rightKey)
	{
		if (!node.Leaf())
			return;

		SizeType i = node.FindPath(rightKey);

		if (node.Key(i) != rightKey)
		{
			if (i == 0)
				return; // лишних ключей нет, выходим
			else
				i--; // ключи большие, чем Key[i], тут лишние
		}

		// есть лишние ключи, начиная с i, надо удалить все элементы в диапазоне [0,i], т.е. ключи >= Key[i]
		node.DeleteMajorFrom(i);
	}

	//
	// обход дерева
	//

	// перевести итератор на следующий узел в цепочке
	inline void SetIteratorNextNode(NodeType& currentNode)
	{
		_currentOffset = currentNode.NextLeafPage();
		if (_currentOffset)
			SetIteratorBegin();
	}

	// перевести итератор на следующий узел в цепочке и запомнить последний ключ в узле (самый большой)
	inline void SetIteratorNextNodeEx(NodeType& currentNode)
	{
		SetIteratorNextNode(currentNode);
		_currentLastKey = currentNode.Key(0);
		_needLastKeyCheck = true;
	}

	// недопустимое состояние, дальнейший обход не возможен
	inline bool IteratorInvalid()
	{
		return _currentElement > NodeType::LeafMaxSize;
	}
	
	// допустимое состояние, дальнейший обход возможен
	inline bool IteratorValid()
	{
		return _currentElement <= NodeType::LeafMaxSize;
	}

	// перевести в недопустимое состояние
	inline void SetIteratorInvalid()
	{
		_needLastKeyCheck = false;
		_currentElement = -1;
	}

	// нужно ли начать обход текущего узла с начала?
	inline bool IteratorBegin()
	{
		return _currentElement == NodeType::LeafMaxSize;
	}

	// перемещаем итератор на начало узла
	inline void SetIteratorBegin()
	{
		_needLastKeyCheck = false;
		_currentElement = NodeType::LeafMaxSize;
	}

	// первый элемент дерева, если корень - лист
	bool FirstRoot(PairType& value)
	{
		_currentOffset = 0;
		_currentElement = _root.Size() - 1;
		value = _root.Pair(_currentElement--);
		return true;
	}

	// первый элемент дерева больший или равный key, если корень - лист
	bool FirstRoot(Key key, PairType& value)
	{
		_currentOffset = 0;
		_currentElement = _root.FindPath(key);
		if (_root.Key(_currentElement) < key) // если попали в самый левый элемент (минимальный), ключ там может быть < key
			_currentElement--; // тогда переходим правее (к большему элементу) на 1 элемент
		if (IteratorInvalid())
			return false;
		value = _root.Pair(_currentElement--);
		return true;
	}

	// первый элемент дерева, если корень не является листом
	bool FirstGeneral(PairType& value)
	{
		SetIteratorInvalid();

		OffsetType nextOffset = _root.Offset(_root.Size() - 1);
		NodeType node;

		do
		{
			if (!LoadNodeRaw(node, nextOffset) || node.Size() == 0)
				return false;
			_currentOffset = nextOffset;
			nextOffset = node.Offset(node.Size() - 1);
		} while (!node.Leaf());

		_currentElement = node.Size() - 1;
		value = node.Pair(_currentElement--);

		return true;
	}

	// первый элемент дерева больший или равный key, если корень не является листом
	bool FirstGeneral(Key key, PairType& value)
	{
		_currentElement = _root.FindPath(key);
		_currentOffset = _root.Offset(_currentElement);

		NodeType node;

		do
		{
			if (!LoadNodeRaw(node, _currentOffset) || node.Size() == 0)
			{
				SetIteratorInvalid();
				return false;
			}

			_currentElement = node.FindPath(key);

			if (node.Leaf())
				break;

			_currentOffset = node.Offset(_currentElement);
		} while (1);

		// если попали в самый левый элемент (минимальный), там могут быть ключи < key, пропускаем их
		while (node.Key(_currentElement) < key)
		{
			_currentElement--; // переходим правее (к большему элементу) на 1 элемент
			// если закончились элемены в текущем узле, переключаемся на следующий в цепочке
			if (IteratorInvalid())
			{
				SetIteratorNextNode(node); // ключ запоминать не надо
				return Next(value);
			}
		}

		value = node.Pair(_currentElement--);

		// если взяли последний элемент в узле, то переключаемся на следующий
		if (IteratorInvalid())
			SetIteratorNextNodeEx(node); // переходим на след узел, запоминаем последний ключ, для корректировки ошибок, если они есть

		return true;
	}

	// следующий элемент, если корень - лист
	bool NextRoot(PairType& value)
	{
		if (!_root.Leaf())
			return false;

		if (_currentElement >= _root.Size())
			return false;

		value = _root.Pair(_currentElement--);
		return true;
	}

	// следующий элемент, если корень не является листом
	bool NextGeneral(PairType& value)
	{
		NodeType node;

		do
		{
			if (!LoadNodeRaw(node, _currentOffset))
				return false;

			if (node.Size() == 0)
				return false;

			if (!IteratorBegin())
				break;

			_currentElement = node.Size() - 1;
			SkipDuplicateKeys(node);

			// если закончились элемены в текущем узле, переключаемся на следующий в цепочке
			if (IteratorInvalid())
			{
				SetIteratorNextNode(node); // ключ сохранять не надо
				_needLastKeyCheck = true;
			}
			else
				break;

		} while(IteratorValid());

		if (IteratorInvalid()) // если не осталось элементов, выходим
			return false;

		value = node.Pair(_currentElement--);

		// если взяли последний элемент в узле, то переключаемся на следующий
		if (IteratorInvalid())
			SetIteratorNextNodeEx(node); // последний ключ надо сохранить

		return true;
	}

	// пропускает дублирующие ключи, которые могли возникнуть при ошибке сохранения во время перегруппировки,
	// см. описание метода RedistributeLeaf
	void SkipDuplicateKeys(NodeType& currentNode)
	{
		if (_needLastKeyCheck == false)
			return;
		while (IteratorValid() && _currentLastKey >= currentNode.Key(_currentElement))
			_currentElement--;
	}

	//
	// список пустых страниц
	//

	// считать первые 4 байта пустой записи, в которых id (смещение) следующей пустой записи
	bool LoadNextFreeRecordId(OffsetType id, OffsetType& nextId)
	{
		if (!FileClient::SeekFile(id))
			return false;
		return FileClient::ReadFile((unsigned char*) &nextId, sizeof(unsigned long));
	}

private:

	NodeType _root; // корень дерева, всегда актуальный

	// признак наличия несохранённых изменений в корне (значит, надо сохранить корень в методе Flush()),
	// сейчас используется только при изменении поля FirstFree при сохранении узла на место удалённого 
	bool _rootModified;
	bool _rootUnvalid; // флаг выставляется, если не смогли сохранить изменения в корне

	OffsetType _currentOffset; // текущая странца-узел для обхода дерева
	SizeType _currentElement; // текущий элемент в текущем узле
	Key _currentLastKey;
	bool _needLastKeyCheck;

	SizeType _lastMovedCount; // количество перемещённых элементов при последней перегруппировке (для отладки)
	SizeType _lostElementsCount; // количество потерянных при последней перегруппировке элементов (для отладки)
};

}

#endif // _RBLIB_BD_BPLUS_TREE_H_

