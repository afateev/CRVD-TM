
#ifndef _RBLIB_BD_H_
#define _RBLIB_BD_H_

#include "data_store.h"
#include "index.h"

namespace Rblib
{

template<class FS, class Key, class Data, bool _64bit = false, unsigned long _pageSize = 512>
class DB
{

private:

	typedef DataStore<FS, Data, _64bit, _pageSize> StoreType;
	typedef Index<FS, Key, _64bit, _pageSize> IndexType;

public:

	DB(FS& fs)
	: _dataStore(fs)
	, _index(fs)
	{
	}

	inline bool Open(char* dataFileName, char* indexFileName)
	{
		if (!_dataStore.Open(dataFileName))
			return false;
		return _index.Open(indexFileName);
	}

	inline bool Close()
	{
		return !(!_dataStore.Close() || !_index.Close());
	}

	inline bool Flush()
	{
		return _dataStore.Flush() && _index.Flush();
	}

	bool Insert(Key key, const Data& data)
	{
		typename StoreType::IdType id = _dataStore.Add(data);
		if (id == 0)
			return false;
		return _index.Insert(key, id);
	}

	bool Find(Key key, Data& data)
	{
		typename StoreType::IdType id;
		if (!_index.Find(key, id))
			return false;
		return _dataStore.Get(id, data);
	}

	bool Delete(Key key)
	{
		typename StoreType::IdType id;
		if (!_index.Find(key, id))
			return false;
		if (!_index.Delete(key))
			return false;
		return _dataStore.Remove(id);
	}

	bool First(Key& key, Data& data)
	{
		typename IndexType::PairType pair;
		if (!_index.First(pair))
			return false;
		key = pair.Key();
		return _dataStore.Get(pair.Value(), data);
	}

	bool First(Key keyIn, Key& keyOut, Data& data)
	{
		typename IndexType::PairType pair;
		if (!_index.First(keyIn, pair))
			return false;
		keyOut = pair.Key();
		return _dataStore.Get(pair.Value(), data);
	}

	bool Next(Key& key, Data& data)
	{
		typename IndexType::PairType pair;
		if (!_index.Next(pair))
			return false;
		key = pair.Key();
		return _dataStore.Get(pair.Value(), data);
	}

private:

	StoreType _dataStore;
	IndexType _index;

};

}

#endif //  _RBLIB_BD_BD_H_

