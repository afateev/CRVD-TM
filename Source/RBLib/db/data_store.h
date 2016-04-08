
#include "data_store/header.h"
#include "digit_selector.h"
#include "file_client.h"
#include "../static_check.h"

#ifndef _RBLIB_BD_DATA_STORE_H_
#define _RBLIB_BD_DATA_STORE_H_

namespace Rblib
{

//
// класс хранилища данных
//
template<class FS, class Data, bool _64bit = false, unsigned long _pageSize = 512>
class DataStore : private Db::FileClient<FS>
{
public:

	typedef typename Db::DigitSelector<_64bit>::T IdType;

private:

	typedef Bd::DataStore::Header<IdType> HeaderType;
	typedef Db::FileClient<FS> FileClient;
	
public:

	DataStore(FS& fs) : FileClient(fs)
	{
		// !!! допустимый размер хранимых данных - 4, 8, 16 или 32 байта !!!
		RBLIB_STATIC_CHECK(sizeof(Data) != 4 && sizeof(Data) != 8 && sizeof(Data) != 16 && sizeof(Data) != 32, UNVALID_DATA_TYPE);
		// !!! в записи должно умещаться смещение в файле (32 или 64 бита)
		RBLIB_STATIC_CHECK(sizeof(Data) < sizeof(IdType), DATA_TYPE_TOO_SMALL);
		// !!! размер страницы должен быть кратен 512  и не равен 0 !!!
		RBLIB_STATIC_CHECK(_pageSize == 0 || _pageSize % 512, UNVALID_PAGE_SIZE);
	}

	//
	// открыть файл хранилища
	//
	inline bool Open(char* fileName)
	{
		if (!FileClient::Open(fileName)) // открываем файл для чтения и записи
			return false;

		if (LoadHeader()) // пытаемся загрузить заголовок
			return true;

		if (FileClient::IoFault()) // если не загрузили и была ошибка чтения
			return false;

		return SaveHeader(); // пишем новый заголовок
	}

	//
	// закрыть файл хранилища
	//
	inline bool Close()
	{
		return FileClient::Close();
	}

	// сохранить несохранённое
	inline bool Flush()
	{
		return FileClient::Flush();
	}

	//
	// получить запись по id
	//
	bool Get(IdType id, Data& data)
	{
		// физический смысл id - смещение до записи в файле
		// если id больше или равен размеру файла, то такой записи нет
		if (id >= FileClient::FileSize())
			return false;

		return LoadRecord(id, data);
	}

	//
	// добавить запись
	//
	IdType Add(const Data& data)
	{
		IdType id;

		// если есть пустые записи
		if (id = _header.FirstFreeRecord())
		{
			// достаём из первой пустой записи id следеющей пустой записи
			IdType nextFreeId;
			if (!LoadNextFreeRecordId(id, nextFreeId))
				return 0;

			// сохраняем id следующей пустой записи в заголовке
			_header.SetFirstFreeRecord(nextFreeId);
			if (!SaveFirstFreeRecordId())
				return 0;
		}
		else
		{
			id = FileClient::FileSize(); // если нет пустых записей, будем писать в конец файла
		}

		// сохраняем запись
		if (!WriteRecord(id, data))
			return 0;

		return id; // возвращаем id
	}

	//
	// удалить запись
	//
	bool Remove(IdType id)
	{
		// физический смысл id - смещение до записи в файле
		// если id больше или равен размеру файла, то такой записи нет
		if (id >= FileClient::FileSize())
			return false;

		// в удалённой записи сохраняем id пустой записи из заголовка
		if (!WriteNextFreeRecordId(id, _header.FirstFreeRecord()))
			return false;

		// а в заголовке сохраняем id удаляемой записи
		_header.SetFirstFreeRecord(id);
		return SaveFirstFreeRecordId();
	}

protected:

	//
	// доступ к заголовку
	//

	bool SaveHeader()
	{
		if (!FileClient::SeekFile(0))
			return false;

		return FileClient::WriteFile((unsigned char*) &_header, sizeof(HeaderType));
	}

	bool LoadHeader()
	{
		if (!FileClient::SeekFile(0))
			return false;
		return FileClient::ReadFile((unsigned char*) &_header, sizeof(HeaderType));
	}

	bool SaveFirstFreeRecordId()
	{
		if (!FileClient::SeekFile(0))
			return false;
		return FileClient::WriteFile((unsigned char*) &_header, IdTypeSize);
	}

	bool LoadFirstFreeRecordId()
	{
		if (!FileClient::SeekFile(0))
			return false;
		return FileClient::ReadFile((unsigned char*) &_header, IdTypeSize);
	}

	//
	// доступ к записи
	//

	// загрузить запись
	bool LoadRecord(IdType id, Data& data)
	{
		if (!FileClient::SeekFile(id))
			return false;
		return FileClient::ReadFile((unsigned char*) &data, sizeof(Data));
	}

	// перезаписать запись
	bool WriteRecord(IdType id, const Data& data)
	{
		if (!FileClient::SeekFile(id))
			return false;
		return FileClient::WriteFile((unsigned char*) &data, sizeof(Data));
	}

	// считать первые 4 байта пустой записи, в которых id (смещение) следующей пустой записи
	bool LoadNextFreeRecordId(IdType id, IdType& nextId)
	{
		if (!FileClient::SeekFile(id))
			return false;
		return FileClient::ReadFile((unsigned char*) &nextId, IdTypeSize);
	}

	// записать первые 4 байта пустой записи, в которых id (смещение) следующей пустой записи
	bool WriteNextFreeRecordId(IdType id, IdType nextId)
	{
		if (!FileClient::SeekFile(id))
			return false;
		return FileClient::WriteFile((unsigned char*) &nextId, IdTypeSize);
	}

private:

	HeaderType _header; // заголовок, всегда актуальный
	static const unsigned long IdTypeSize = sizeof(IdType);
};

}

#endif // _RBLIB_BD_DATA_STORE_H_
