
#include "data_store/header.h"
#include "digit_selector.h"
#include "file_client.h"
#include "../static_check.h"

#ifndef _RBLIB_BD_DATA_STORE_H_
#define _RBLIB_BD_DATA_STORE_H_

namespace Rblib
{

//
// ����� ��������� ������
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
		// !!! ���������� ������ �������� ������ - 4, 8, 16 ��� 32 ����� !!!
		RBLIB_STATIC_CHECK(sizeof(Data) != 4 && sizeof(Data) != 8 && sizeof(Data) != 16 && sizeof(Data) != 32, UNVALID_DATA_TYPE);
		// !!! � ������ ������ ��������� �������� � ����� (32 ��� 64 ����)
		RBLIB_STATIC_CHECK(sizeof(Data) < sizeof(IdType), DATA_TYPE_TOO_SMALL);
		// !!! ������ �������� ������ ���� ������ 512  � �� ����� 0 !!!
		RBLIB_STATIC_CHECK(_pageSize == 0 || _pageSize % 512, UNVALID_PAGE_SIZE);
	}

	//
	// ������� ���� ���������
	//
	inline bool Open(char* fileName)
	{
		if (!FileClient::Open(fileName)) // ��������� ���� ��� ������ � ������
			return false;

		if (LoadHeader()) // �������� ��������� ���������
			return true;

		if (FileClient::IoFault()) // ���� �� ��������� � ���� ������ ������
			return false;

		return SaveHeader(); // ����� ����� ���������
	}

	//
	// ������� ���� ���������
	//
	inline bool Close()
	{
		return FileClient::Close();
	}

	// ��������� ������������
	inline bool Flush()
	{
		return FileClient::Flush();
	}

	//
	// �������� ������ �� id
	//
	bool Get(IdType id, Data& data)
	{
		// ���������� ����� id - �������� �� ������ � �����
		// ���� id ������ ��� ����� ������� �����, �� ����� ������ ���
		if (id >= FileClient::FileSize())
			return false;

		return LoadRecord(id, data);
	}

	//
	// �������� ������
	//
	IdType Add(const Data& data)
	{
		IdType id;

		// ���� ���� ������ ������
		if (id = _header.FirstFreeRecord())
		{
			// ������ �� ������ ������ ������ id ��������� ������ ������
			IdType nextFreeId;
			if (!LoadNextFreeRecordId(id, nextFreeId))
				return 0;

			// ��������� id ��������� ������ ������ � ���������
			_header.SetFirstFreeRecord(nextFreeId);
			if (!SaveFirstFreeRecordId())
				return 0;
		}
		else
		{
			id = FileClient::FileSize(); // ���� ��� ������ �������, ����� ������ � ����� �����
		}

		// ��������� ������
		if (!WriteRecord(id, data))
			return 0;

		return id; // ���������� id
	}

	//
	// ������� ������
	//
	bool Remove(IdType id)
	{
		// ���������� ����� id - �������� �� ������ � �����
		// ���� id ������ ��� ����� ������� �����, �� ����� ������ ���
		if (id >= FileClient::FileSize())
			return false;

		// � �������� ������ ��������� id ������ ������ �� ���������
		if (!WriteNextFreeRecordId(id, _header.FirstFreeRecord()))
			return false;

		// � � ��������� ��������� id ��������� ������
		_header.SetFirstFreeRecord(id);
		return SaveFirstFreeRecordId();
	}

protected:

	//
	// ������ � ���������
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
	// ������ � ������
	//

	// ��������� ������
	bool LoadRecord(IdType id, Data& data)
	{
		if (!FileClient::SeekFile(id))
			return false;
		return FileClient::ReadFile((unsigned char*) &data, sizeof(Data));
	}

	// ������������ ������
	bool WriteRecord(IdType id, const Data& data)
	{
		if (!FileClient::SeekFile(id))
			return false;
		return FileClient::WriteFile((unsigned char*) &data, sizeof(Data));
	}

	// ������� ������ 4 ����� ������ ������, � ������� id (��������) ��������� ������ ������
	bool LoadNextFreeRecordId(IdType id, IdType& nextId)
	{
		if (!FileClient::SeekFile(id))
			return false;
		return FileClient::ReadFile((unsigned char*) &nextId, IdTypeSize);
	}

	// �������� ������ 4 ����� ������ ������, � ������� id (��������) ��������� ������ ������
	bool WriteNextFreeRecordId(IdType id, IdType nextId)
	{
		if (!FileClient::SeekFile(id))
			return false;
		return FileClient::WriteFile((unsigned char*) &nextId, IdTypeSize);
	}

private:

	HeaderType _header; // ���������, ������ ����������
	static const unsigned long IdTypeSize = sizeof(IdType);
};

}

#endif // _RBLIB_BD_DATA_STORE_H_
