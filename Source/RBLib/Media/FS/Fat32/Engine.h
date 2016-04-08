
#ifndef _MEDIA_FS_FAT32_FAT_H_
#define _MEDIA_FS_FAT32_FAT_H_

#include "BootSector.h"
#include "DirectoryCluster.h"
#include "ClusterChain.h"
#include "FileInfo.h"
#include "Name.h"
#include "LongNamePart.h"
#include "FsInfo.h"
#include "DirIterator.h"

#include "../IoFlags.h"
#include "../PathIterator.h"
#include "../Path.h"
#include "../Mbr.h"
#include "../Error.h"

#include "../../../Utils/map_static.h"

namespace Rblib
{
namespace Media
{
namespace Fs
{
namespace Fat32
{

template<class DataProvider, class DateTimeProvider, int openedFilesMax = 30>
class Engine
{
protected:

	//
	// map ��� �������� ���������� �� �������� ������
	//
	static const unsigned long OpenedFilesMax = openedFilesMax;
	typedef Rblib::map_static<unsigned long, FileInfo, OpenedFilesMax> OpenedFilesMap;
	typedef void (*WdtClearType)(void);

public:

	typedef unsigned char ErrorType; // ��� ������
	typedef DirIterator<DataProvider, DateTimeProvider, OpenedFilesMax> DirIterator; // ��� ��������� ��� ������ ����������� ��������
	friend DirIterator;

public:

	Engine(DataProvider& provider)
	: _provider(provider)
	, _cluster(provider)
	, _clusterSize(0)
	, _rootCluster(0)
	, _clusterCount(0)
	, _lastOperationError(Error::None)
	, _wdtClear(0)
	{
	}

	//
	// �������������
	//

	// ��������� ������� MBR �� �����
	// ���� MBR ������������, �������� �������� � 1-� ������� ��������
	// ������ � MBR/��������� ���� ����-�� ������� (� ������ �����, �� FAT32)
	bool Init()
	{
		ClearError();

		// ��������� 0-� ������ �����
		// ����� ������ ������ MBR ��� BootSector FAT
		if (!_provider.ReadSector(0, (unsigned char*)&_mbrTmp))
			return false;
	  
		// _mbrTmp � _bootSectorTmp ����� � ����� union, ��� ��� ����� ��������� ����� ��� ���������
		// ���� MBR, �� ���� ������ ������� ��������� ������� � ���������� ������������� � ����� �������
		if (!_bootSectorTmp.Valid())
		{
			if (_mbrTmp.Valid()) 
			{
				for (unsigned char p = 0; p < Mbr::MaxPartitionsNumber; p++)
				{
					if (_mbrTmp.GetPartition(p).Valid() && _mbrTmp.GetPartition(p).Fat32())
					{
						return Init(_mbrTmp.GetPartition(p).Offset() * _sectorSize);
					}
				}

				return false; // ��� ���������� ��������
			}

			return false; // � 0-� ������� ����������� � MBR, � boot sector
		}

		// MBR �����������, � 0-� ������� ����� boot sector, � �� ��� ������
		// ���������� �������������
		return InitFromBootSector(0);
	}

	// �������������
	// ����� �������� �� ���������� ������ �������
	bool Init(unsigned long offset)
	{
		ClearError();

		if (!_provider.ReadSector(offset / _sectorSize, (unsigned char*)&_bootSectorTmp))
			return false;

		return InitFromBootSector(offset);
	}

	// ������ ������� ������ ����������� �������
	void SetWdtClear(WdtClearType wdtClear)
	{
		_wdtClear = wdtClear;
		_clusterChain.SetWdtClear(_wdtClear);
	}

	//
	// ������ ��������� ��������
	//

	inline ErrorType LastOperationError()
	{
		return _lastOperationError;
	}

	inline bool IoFault()
	{
		return _lastOperationError == Error::IOFault;
	}

	//
	// ������ �����
	//

	// ������ ����� � ������
	inline unsigned long long DiskSize()
	{
		return (unsigned long long) _clusterCount * _clusterSize;
	}

	// ���������� ����� � ������
	inline unsigned long long FreeSpace()
	{
		// ����� ������� FreeSpace ���� �������� FreeSpaceValid ��� ��������!
		// ���� ���� FreeCount � FSInfo �������������, ���������� ���������� �������� �� ClusterChain
		if (FreeSpaceValid())
			return (unsigned long long) _clusterChain.FreeClusters() * _clusterSize; // ��������� � �����
		return DiskSize(); // ������ �������, ��� �� ��������
	}
	
	// ���������� ����� ���������?
	inline bool FreeSpaceValid()
	{
		return _fsInfo.FreeCount() <= _clusterCount;
	}

	//
	// ������� ����
	//

	inline unsigned long OpenFileInOut(char* path)
	{
		return OpenFile(path, IOFlags::In | IOFlags::Out);
	}

	unsigned long OpenFile(char* path, IOFlags::T flags = IOFlags::In)
	{
		ClearError();

		if (flags == IOFlags::In) // ��� �������� ����� ������ �� ������ ���������� ������, ���� ���� �� ���������� (����� �� ������)
		{
			if (!FindEntry(path, true))
				return 0;
		}
		else
		{
			if (!FindOrCreateEntry(path, true)) 
				return 0;
		}

		const DirEntry& entry = _cluster.CurrentRecord();

		if (entry.Directory())
			return 0;

		if ((flags & IOFlags::Out) && !(flags & IOFlags::In) && !(flags & IOFlags::App))
		{
			if (!ClearCurrentFile())
				return 0;
		}

		if (!RegisterFile(entry.Cluster(), _cluster.Number(), _cluster.CurrentRecordNum(), entry.Size(), flags))
			return 0;

		return entry.Cluster();
	}

	inline bool CloseFile(unsigned long cluster)
	{
		ClearError();

		// ��������� ���������, ���� ����
		if (!Flush(cluster))
		{
			SetError(Error::IOFault);
		}

		// ���� ���� ������, �� ����� ����� false
		return UnregisterFile(cluster) && _lastOperationError == 0;
	}

	//
	// ������ ���������� ������� � �����, ������� �� ������
	//

	bool SeekFile(unsigned long cluster, unsigned long pos)
	{
		ClearError();

		FileInfo* info = GetFileInfo(cluster);

		if (info == 0)
			return false;

		if (pos > info->Size())
			return false;

		info->SetPos(pos);

		return true;
	}

	//
	// ������� ���� ��������� ������� (���� ����� ��������� �����)
	//

	bool CreateFile(char* path, unsigned long len)
	{
		ClearError();

		if (!CreateEntry(path, true))
			return false;

		DirEntry& entry = _cluster.CurrentRecord();

		// ���� ���������� ���������� ��� �������� ����, ���������� ������
		if (entry.Directory() || entry.Size())
			return false;

		// ���� ����� �������� ����������
		if (len <= _clusterSize)
			return true;

		unsigned long currentLen = len;
		unsigned long cluster = entry.Cluster();

		for(; currentLen > _clusterSize; currentLen -= _clusterSize)
		{
			cluster = AddClusterToChain(cluster);
			if (cluster == 0)
				break;
		}

		// ���� �� ������ �������� �������, ����� ������, ������� ������ ��������
		entry.SetSize(cluster ? len : (len - currentLen));

		//������������� ���� ��������
		SetEntryCreateDate(entry);

		if (!_cluster.Save())
			return false;

		return true;
	}

	unsigned long CreateFileAndOpen(char* path, unsigned long len)
	{
		ClearError();

		if (!CreateFile(path, len))
			return 0;

		DirEntry& entry = _cluster.CurrentRecord();

		if (!RegisterFile(entry.Cluster(), _cluster.Number(), _cluster.CurrentRecordNum(), entry.Size(), IOFlags::Out))
			return 0;

		return entry.Cluster();
	}

	// �������� ������ ����� ��������� �����
	// ���������� ����� ������ ����� ��� 0 (� ������ ������)
	// ����� �� ������� ������������ ��� ��������� ������� �����
	unsigned long ChangeFileSize(unsigned long cluster, unsigned long size)
	{
		ClearError();

		FileInfo* info = GetFileInfo(cluster);

		// ���� ������ ���� ������ �� ������ ��� ��������
		if (info == 0 || !info->CheckOneFlag(IOFlags::Out | IOFlags::App))
			return false;

		if (info->Size() == size)
			return true;

		unsigned long currentSize = size;

		// ��� �� ������� ��������� � ��������� �����, ���� ����
		for(; currentSize > _clusterSize; currentSize -= _clusterSize)
		{
			if (ClusterEoc(cluster))
				cluster = AddClusterToChain(cluster); 
			else if (!LastOperationError()) // ���� �� ���� ������ � ClusterEoc
				cluster = NextCluster(cluster);
			else
				cluster = 0; // ���� ������

			if (cluster == 0)
				return 0; // ������
		}

		// ����������� �������� �������� (���� ����� ������ ������ �������)
		if (cluster && !ClusterEoc(cluster) && !SetClusterChainEocAndSave(cluster))
			return 0; // ������

		// ����� ��������� ����� ������

		if (!SetCluster(info->RecordCluster(), info->Record()))
			return 0;

		// ���� �� ������ �������� �������, ����� ������, ������� ������ ��������
		_cluster.CurrentRecord().SetSize(cluster ? size : (size - currentSize));

		// ������������� ���� �����������
		SetEntryWriteDate(_cluster.CurrentRecord());

		if (!_cluster.Save())
			return 0;

		// ��������� �������� �� �������� ����� ����������
		info->SetSize(_cluster.CurrentRecord().Size());
		info->SetPos(0);

		return _cluster.CurrentRecord().Size();
	}

	//
	// ������� ������ �� �����
	//

	unsigned long ReadFile(unsigned long cluster, unsigned char* buffer, unsigned long len)
	{
		ClearError();

		FileInfo* info = GetFileInfo(cluster);

		if (info == 0 || len == 0)
			return 0;

		if (len > info->MaxReadLen()) // ������ ����� ������ �� ����� �����
			len = info->MaxReadLen();

		cluster = GetClusterForPos(cluster, info->LastPos(), info->LastCluster(), info->Pos()); // �������, ������� � �������� ����� ������
		if (cluster == 0)
			return 0;

		info->SaveStartPos(cluster); // ��������� ������� �������� � �������

		unsigned short offset = GetOffsetForPos(info->Pos()); // �������� � ��������, � �������� ����� ������
		unsigned long remainingLen = len; // ������� �������� ���������

		while(cluster && remainingLen)
		{
			info->SavePostion(cluster); // ��������� ���� �������-�������

			if (!SetCluster(cluster))
				return 0;

			signed short readRes = _cluster.Read(buffer + len - remainingLen, (remainingLen > _clusterSize) ? _clusterSize : (unsigned short) remainingLen, offset);

			if (readRes < 0)
			{
				SetError(Error::IOFault);
				return 0;
			}

			info->IncPos(readRes); // ��������� ������� �������

			remainingLen -= readRes;
			offset = 0; // �������� �������� (��������� �������� ����� ����� ������ � ������)
			cluster = NextCluster(cluster);
		}

		return len - remainingLen; // ���������� ���������� ����������� ������
	}

	//
	// �������� ������ � ����
	//

	unsigned long WriteFile(unsigned long cluster, unsigned char* buffer, unsigned long len)
	{
		ClearError();

		FileInfo* info = GetFileInfo(cluster);
		if (info == 0 || len == 0)
			return 0;

		cluster = GetOrCreateClusterForPos(cluster, info->LastPos(), info->LastCluster(), info->Pos());  // �������, ������� � �������� ����� ������
		if (cluster == 0)
			return 0;

		info->SaveStartPos(cluster); // ��������� ������� �������� � �������

		unsigned short offset = GetOffsetForPos(info->Pos()); // �������� � ��������, � �������� ����� ������
		unsigned long remainingLen = len; // ������� ��� �������� ��������

		while(cluster)
		{
			info->SavePostion(cluster); // ��������� ���� �������-�������

			if (!SetCluster(cluster))
				return 0;

			signed short writeRes = _cluster.Write(buffer + len - remainingLen, (remainingLen > _clusterSize) ? _clusterSize : (unsigned short)remainingLen, offset);
			if (writeRes < 0)
			{
				SetError(Error::IOFault);
				return 0;
			}

			info->IncPosAndSize(writeRes); // ��������� ������� ������� � ������, ���� ���������
			offset = 0; // �������� �������� (��������� �������� ����� ����� ������ � ������)
			remainingLen -= writeRes;

			if (remainingLen == 0) // ���� �� ��������, �������
				break;

			cluster = GetOrCreateNextCluster(cluster);
		}

		return len - remainingLen; // ���������� ���������� ���������� ������
	}

	//
	// ������� �������
	//

	bool CreateDirectory(char* path)
	{
		ClearError();

		unsigned long parentCluster = CreateNewDirEntry(path);

		if (parentCluster == 0)
			return false;

		DirEntry& entry = _cluster.CurrentRecord();
		entry.SetDirectoryAttr();
		SetEntryCreateDate(entry); // ������ ���� ��������
		if (!_cluster.Save())
			return false;

		unsigned long newDirCluster = entry.Cluster();

		if (!SetCluster(newDirCluster))
			return false;

		if (!_cluster.Clear()) //?
			return false;

		// ��������� �������� "." � ".."
		_cluster.Record(0).SetAsCurrentDirEntry(newDirCluster);
		_cluster.Record(1).SetAsParentDirEntry((parentCluster == _rootCluster) ? 0 : parentCluster);
		// ������ ���������� ���� ��������
		SetEntryCreateDate(_cluster.Record(0));
		SetEntryCreateDate(_cluster.Record(1));

		return _cluster.SaveToSector(0);
	}

	//
	// ������ ������ ��������� �����
	//

	unsigned long FileSize(unsigned long cluster)
	{
		ClearError();

		FileInfo* info = GetFileInfo(cluster);

		if (info == 0)
			return -1;

		return info->Size();
	}

	//
	// ���������� ������������ ��������� �� ��������
	//

	inline bool Flush(unsigned long cluster)
	{
		ClearError();

		if (!FlushClusterChain()) // FAT
		{
			RollbackFileSize(cluster);
			return false;			
		}

		if (!FlushFileInfo(cluster)) // ������ �����
		{
			return false;
		}

		if (!_cluster.Flush()) // ����� �������� ������������ ������ � _cluster
			return false;

		return FlushFsInfo(); // ���������� ��������� ���������
	}

	//
	// ���������� ������������ ��������� ����� ������
	//
	inline bool Flush()
	{
		ClearError();
		if (!_cluster.Flush()) // ����� �������� ������������ ������ � _cluster
			return false;
		if (!FlushClusterChain()) // FAT
			return false;
		return FlushFsInfo(); // ���������� ��������� ���������
	}

	//
	// ���������� �������� ��� �������� ����������� ��������
	//
	DirIterator OpenDir(char* path)
	{
		if (Path::IsRoot(path)) // ���� ����� �������� �������
			return DirIterator(*this, _rootCluster);

		if (!FindEntry(path, false)) // ����� ������ ��� ���?
			return DirIterator(*this); // ���������� ���������� ��������

		if (_cluster.CurrentRecord().File()) // �������� ���� ������������� �����?
			return DirIterator(*this); // ���������� ���������� ��������

		return DirIterator(*this, _cluster.CurrentRecord().Cluster());
	}

	//
	// ��a���� ����
	//
	bool DeleteFile(char* path)
	{
		ClearError();

		if (!FindEntry(path, true)) // ���� ������ � �����
			return false;

		if (!FreeChain(_cluster.CurrentRecord().Cluster())) // ����������� ������� ��������� �����
			return false;

		UnregisterFile(_cluster.CurrentRecord().Cluster()); // ��������� ����, ���� ������

		// ����� ����������� ��� ������ � ����� (�������� � �������, ���� ����)

		unsigned long emptyCount = _cluster.LongCount() + 1; // ������� � ���� ��������

		// ����������� ������� ������ (����� ���� � 2-� ���������, �� �� �������)
		do
		{
			for (signed short r = _cluster.CurrentRecordNum(); r >=0 && emptyCount; r--, emptyCount--)
			{
				DirEntry& entry = _cluster.Record(r);
				if (!entry.Valid())
				{
					SetError(Error::IOFault);
					return false;
				}
				_cluster.Record(r).SetEmpty();
				_cluster.SetModified();
			}
			// ���� �� �� ��������, ������������� �� ������ �������
			if (emptyCount && !SetClusterToLastRecord(_cluster.EntryStartCluster()))
				return false;
		} while(emptyCount);

		return true;
	}

protected:

	//
	// �������������
	//

	// �������������, � _bootSectorTmp ��� �������� ����������� ������
	bool InitFromBootSector(unsigned long offset)
	{
		_rootCluster = _bootSectorTmp.RootCluster();
		_clusterSize = _bootSectorTmp.ClusterSize();
		_clusterCount = _bootSectorTmp.ClusterCount();

		FileInfo::SetClustrerSize(_clusterSize);

		if (!_bootSectorTmp.Valid())
			return false;

		if (!InitClusterChain(offset))
			return false;

		_cluster.Init((offset + _bootSectorTmp.FirstDataSectorOffset()) / _sectorSize
							, _clusterSize / _sectorSize);

		// ��������� ��������� (��� ����� ���������� �������� ���������),
		// ������������ ���������������� � ���������� ��������� ���� FSInfo ������������ ����������
		return ResetFsInfo(offset);
	}

	// ������������ FSInfo ������������ ����������
	bool ResetFsInfo(unsigned long offset)
	{
		_fsInfoSector = offset / _sectorSize + _bootSectorTmp.FsInfoSector(); // ���������� ������ ���������

		// ��������� FSInfo
		// !!! ��� _bootSectorTmp ���������� ������������ (�.�. �� � 1-� union-�) !!!
		if (!_provider.ReadSector(_fsInfoSector, (unsigned char*)&_fsInfo)) // ��������� FSInfo
			return false;

		if (!_fsInfo.Valid())
			_fsInfo.Reset(); // ���� ������, �� �������� � ���������� ���

		// ���� ���������� ��������� ��������� ��������������� � ���� ������ ����� ��������������,
		// ������������� ����� ��������
		if (_fsInfo.FreeCount() > _clusterCount && DiskEmptyAfterFormat())
			_fsInfo.SetFreeCount(_clusterCount - 1); // ����� ������ ���� ������� (��� �������� �������)

		if (LastOperationError()) // ���� ���� ������ �����/������ � DiskEmptyAfterFormat
			return false;

		// ��������� ���������
		if (!_provider.WriteSector(_fsInfoSector, (unsigned char*)&_fsInfo))
			return false;

		// ���� ������� ����� ���������, �� ������� �� � ClusterChain
		if (_fsInfo.FreeCount() <= _clusterCount)
			_clusterChain.SetFreeClustersCount(_fsInfo.FreeCount());
		if (_fsInfo.NextFree() <= _clusterCount)
			_clusterChain.SetNextFree(_fsInfo.NextFree());

		return true;
	}

	bool InitClusterChain(unsigned long offset)
	{
		_clusterChain.SetProvider(&_provider);
		_clusterChain.Init((offset + _bootSectorTmp.FatOffset(0)) / _sectorSize
							, _bootSectorTmp.FatSizeInBytes() / _sectorSize
							, _bootSectorTmp.FatsCount());

		return true;
	}

	//
	// ����� ������
	//

	// ����� ������, ��������������� ��������� ����
	bool FindEntry(char* path, bool file)
	{
		PathIterator iter(path);
		unsigned long cluster = _rootCluster;

		for(; !iter.End(); iter.Next())
		{
			if (!FindEntryInDir(cluster, iter.Name(), (unsigned char) iter.NameLength()))
				return false;

			cluster = _cluster.CurrentRecord().Cluster();
		}

		return file && iter.File() && _cluster.CurrentRecord().File()
				|| !file && _cluster.CurrentRecord().Directory();
	}

	// ����� ��� �������� ������, ��������������� ��������� ����
	bool FindOrCreateEntry(char* path, bool file)
	{
		PathIterator iter(path);
		unsigned long cluster = _rootCluster;

		for(; !iter.End(); iter.Next())
		{
			if (!FindEntryInDir(cluster, iter.Name(), (unsigned char) iter.NameLength()))
			{
				if (LastOperationError()) // ���� ������
					return false;

				// ���������� � ��������� ����� �� ��������, ���� � iter.Next() �� ������ ��������� �������
				iter.Next();
				if (!iter.End() || (file && !iter.File()))
					return false;
				// iter.Name() �� ��������, ���� � iter.Next() �� ������ ��������� �������
				return CreateEntry(cluster, iter.Name(), (unsigned char)iter.NameLength());
			}

			cluster = _cluster.CurrentRecord().Cluster();
		}

		return file && iter.File() && _cluster.CurrentRecord().File() // ���� ���� ����, ������ ���� � �����, � �� ����� ����
				|| !file && _cluster.CurrentRecord().Directory(); // ���� ���� �����, � �� ����� �����
	}

	// �������� ������, ��������������� ��������� ����
	// ���� ������ ����������, ����� ������
	bool CreateEntry(char* path, bool file)
	{
		PathIterator iter(path);
		unsigned long cluster = _rootCluster;

		for(; !iter.End(); iter.Next())
		{
			if (!FindEntryInDir(cluster, iter.Name(), (unsigned char) iter.NameLength()))
			{
				if (LastOperationError()) // ���� ������
					return false;

				// ���������� � ��������� ����� �� ��������, ���� � iter.Next() �� ������ ��������� �������
				iter.Next();
				if (!iter.End() || (file && !iter.File()))
					return false;
				// iter.Name() �� ��������, ���� � iter.Next() �� ������ ��������� �������
				return CreateEntry(cluster, iter.Name(), (unsigned char)iter.NameLength());
			}

			cluster = _cluster.CurrentRecord().Cluster();
		}

		return false;
	}

	// ������ ������ ��� ����� ����������, ���������� ������ ������� ���������� �������� ��� 0
	unsigned long CreateNewDirEntry(char* path)
	{
		PathIterator iter(path);
		unsigned long cluster = _rootCluster;

		for(PathIterator iter(path); !iter.End(); iter.Next())
		{
			if (!FindEntryInDir(cluster, iter.Name(), (unsigned char) iter.NameLength()))
			{
				if (LastOperationError()) // ���� ������
					return 0;

				// ���������� � ��������� ����� �� ��������, ���� � iter.Next() �� ������ ��������� �������
				if (iter.Next(), !iter.End())
					return 0;

				// iter.Name() �� ��������, ���� � iter.Next() �� ������ ��������� �������
				if (!CreateEntry(cluster, iter.Name(), (unsigned char)iter.NameLength()))
					return 0;

				return cluster;
			}

			cluster = _cluster.CurrentRecord().Cluster();
		}

		SetError(Error::AlreadyExist);

		return 0;
	}

	// ����� ������ � �������� �� �����
	bool FindEntryInDir(unsigned long cluster, char* name, unsigned char len)
	{
		unsigned char checkedLen = 0;

		while(cluster)
		{
			WdtClear();

			if (!SetCluster(cluster))
				return false;

			if (_cluster.FindEntry(name, len, checkedLen))
				return true; // ��������� ������ � _cluster.CurrentRecord()

			if (!_cluster.Valid()) // ���� ��� ������� ����������, ������ ���� ������ ������
			{
				SetError(Error::IOFault);
				return false;
			}

			cluster = NextCluster(cluster);
		}

		return false;
	}

	// ����� ����� ��� ������� ���������� ������� (���� �� ����� � ��������, �� � ������ ��������)
	bool FindEmptyBlock(unsigned long cluster, unsigned short cnt)
	{
		unsigned short foundCnt = 0;

		unsigned long startCluster = 0;
		unsigned short startRecord = 0;
		unsigned short foundSummary = 0;

		unsigned long lastDirCluster = 0;

		while(cluster)
		{
			if (!SetCluster(cluster))
				return false;

			foundCnt = _cluster.FindEmptyBlock(cnt);

			// ���� �� ����� ������ ������
			if (foundCnt == 0)
			{
				if (!_cluster.Valid()) // ���� ��� ������� ����������, ������ ���� ������ ������
					return false;
				foundSummary = 0;
			}
			else // �����
			{
				// ���� ��� �� �������� ��� ������ ����� �� � ������ ������� (�.�. ������ ����� �������������������)
				if (!foundSummary || _cluster.CurrentRecordNum() != 0)
				{
					startCluster = cluster;
					startRecord = _cluster.CurrentRecordNum();
					foundSummary = foundCnt;
				}
				else
				{
					foundSummary += foundCnt;
				}

				// ������ ������ �����
				if (foundSummary >= cnt)
				{
					if (!SetCluster(startCluster, startRecord))
						return false;
					return true; // ��������� ������ � _cluster.CurrentRecord()
				}
			}

			lastDirCluster = cluster;
			cluster = NextCluster(cluster);

			if (LastOperationError()) // ���� ������ IO � NextCluster
				return false;
		}

		if (foundCnt)
		{
			if (!SetCluster(startCluster, startRecord))
				return false;
			return AddNextCluster(lastDirCluster, true); // ��������� ����� �������
		}
		else
			return AddNextClusterAndSet(lastDirCluster, true); // ��������� ����� �������, ������� ������ � �� 0
	}

	//
	// ���������� ������ �������� � �������
	//
	
	// �������� ����� �������
	unsigned long AddClusterToChain(unsigned long cluster)
	{
		unsigned long freeCluster = FindFreeCluster();

		if (freeCluster == 0)
			return 0;

		if (!SetNextCluster(cluster, freeCluster)) // ����� �� ���������
			return 0;

		if (!SetClusterEoc(freeCluster)) // ��� ���� ������ �� ���������
			return 0;

		return freeCluster;
	}

	// �������� �������
	// ��������� ���, ���� ����
	// ���������� �� ���� _cluster, ���� ����
	bool AddNextCluster(unsigned long cluster, bool clear, bool set)
	{
		unsigned long newCluster = AddClusterToChain(cluster);

		if (newCluster == 0)
			return false;

		if (set)
		{
			if (!SetCluster(newCluster, 0))
				return false;
		}

		if (clear)
		{
			if (!_cluster.Clear(newCluster))
				return false;
		}

		return true;
	}

	// �������� �������
	inline bool AddNextCluster(unsigned long cluster, bool clear)
	{
		return AddNextCluster(cluster, clear, false);
	}

	// �������� �������, �� �������
	inline bool AddNextCluster(unsigned long cluster)
	{
		return AddNextCluster(cluster, false, false);
	}

	// �������� ������� � ���������� �� ���� _cluster
	inline bool AddNextClusterAndSet(unsigned long cluster, bool clear)
	{
		return AddNextCluster(cluster, clear, true);
	}

	// �������� ������� � ���������� �� ���� _cluster, �� �������
	inline bool AddNextClusterAndSet(unsigned long cluster)
	{
		return AddNextCluster(cluster, false, true);
	}

	// ���������� ��������� ��� ���������������� ��������� �������
	inline unsigned long GetOrCreateNextCluster(unsigned long cluster)
	{
		unsigned long clusterTmp = NextCluster(cluster);

		if (LastOperationError()) // ���� ������ IO � NextCluster
			return 0;

		if (clusterTmp)
			return clusterTmp;

		return AddClusterToChain(cluster);
	}

	//
	// ����������� ������ � �����
	//
	bool SetNewFileSize(FileInfo* info)
	{
		if (info->SizeFixed())
			return true;

		if (!SetCluster(info->RecordCluster(), info->Record()))
			return false;

		// ���� ������ � �����
		DirEntry& fileEntry = _cluster.CurrentRecord();
		if (fileEntry.Valid()) // ���� ��������� ���������
		{
			fileEntry.SetSize(info->Size()); // ������ ����� ������
			SetEntryWriteDate(fileEntry); // ������ ���� �����������
			if (_cluster.Save())
			{
				info->FixSize(); // ���������� ��������� ������ ���������� ������ �����
				return true;
			}
		}

		info->RollbackSize(); // ������ ������ �� ��������� ������ ����������

		return false;
	}

	// �������� ����, �������� ������������� ������� ������ � _cluster
	bool ClearCurrentFile()
	{
		_cluster.CurrentRecord().SetSize(0);
		SetEntryWriteDate(_cluster.CurrentRecord()); // ������ ���� �����������

		if (!_cluster.Save())
			return false;

		return SetClusterChainEocAndSave(_cluster.CurrentRecord().Cluster());
	}

	//
	// �������� ������
	//

	bool CreateEntry(unsigned long cluster, char* name, unsigned char len)
	{
		Name::Type type = Name::Check(name, len);

		if (type == Name::NotValid)
			return false;

		// �������� ���

		// ��������� ������

		DirEntry shortEntry;
		shortEntry.Clear();

		// ������������� ���� � ����� �������� ������
		SetEntryCreateDate(shortEntry);

		if (type == Name::Short)
		{
			shortEntry.SetShortName(name, len);
		}
		else
		{
			char shortName[13];
			unsigned char shortNameLen = Name::ToShort(name, len, shortName);
			shortEntry.SetShortName(shortName, shortNameLen);
		}

		unsigned char checkSum = shortEntry.CheckSumShort();

		unsigned char longEntriesCnt = (type == Name::Short) ? 0 : LongNamePart::Count(len);

		if (!FindEmptyBlock(cluster, longEntriesCnt + 1)) // ��������� � _cluster
			return false;

		cluster = _cluster.Number();

		bool setLast = true;

		LongNamePart part(name, len);

		while(cluster)
		{
			unsigned short r = 0;

			for(r = _cluster.CurrentRecordNum(); r < _cluster.RecordsPerCluster() && longEntriesCnt > 0; r++, longEntriesCnt--)
			{
				DirEntry& record = _cluster.Record(r);
				record.Clear();

				record.SetLongNameAttr();
				record.SetOrder(longEntriesCnt);
				record.SetCheckSum(checkSum);
				record.SetLongName(part.GetUnicode(), part.Length());
				if (setLast)
				{
					record.SetLast();
					setLast = false;
				}

				part.Next();
				_cluster.SetModified();
			}

			if (!_cluster.Flush())
				return false;

			// ������������� �� ������ ������ ���������� ��������
			if (r == _cluster.RecordsPerCluster())
			{
				cluster = NextCluster(cluster);
				if (LastOperationError()) // ���� ������ IO � NextCluster
					return false;
				if (!SetCluster(cluster, 0)) // ��������� �� ������ ������ ���������� ��������
					return false;
			}
			else
			{
				if (!SetCluster(cluster, r)) // ��������� �� ��������� ������ � ���� ��������
					return false;
			}

			// ���� ��������� ��� ������� ������
			if (longEntriesCnt == 0)
				break;
		}

		unsigned long newCluster = FindFreeCluster();
		if (newCluster == 0)
			return false;

		if (!SetClusterEoc(newCluster))
			return false;

		shortEntry.SetCluster(newCluster);
		_cluster.CurrentRecord() = shortEntry;
		_cluster.SetModified();

		return _cluster.Flush();
	}

	//
	// ������ � �������
	//

	// �������� �������� ����
	// cluster - ������ ������� �����
	// recCluster - ������� � ������� � �����
	// rec - ������ � ����� � recCluster
	// size - ������ �����
	bool RegisterFile(unsigned long cluster, unsigned long recCluster, unsigned short rec, unsigned long size, IOFlags::T flags)
	{
		typename OpenedFilesMap::iterator iter = _openedFilesMap.find(cluster);
		
		if (iter != _openedFilesMap.end())
			return false;

		return _openedFilesMap.insert(OpenedFilesMap::value_type(cluster, FileInfo(recCluster, rec, size, flags))).second;
	}

	bool UnregisterFile(unsigned long cluster)
	{
		typename OpenedFilesMap::iterator iter = _openedFilesMap.find(cluster);

		if (iter == _openedFilesMap.end())
			return false;

		_openedFilesMap.erase(iter);

		return true;
	}

	FileInfo* GetFileInfo(unsigned long cluster)
	{
		typename OpenedFilesMap::iterator iter = _openedFilesMap.find(cluster);

		if (iter == _openedFilesMap.end())
			return 0;

		return &iter->second;
	}

	void RollbackFileSize(unsigned long cluster)
	{
		FileInfo* info = GetFileInfo(cluster);
		if (!info)
			return;
		info->RollbackSize();
		if (info->LastPos() >= 65536)
			info = 0;
	}

	//
	// ������������ ����������� FAT ������
	//

	// �������� ������� ��� ��������� � ������� ��� ���������� �� ��������
	inline bool SetClusterEoc(unsigned long cluster)
	{
		if (!_clusterChain.SetEoc(cluster))
			return false;

		return true;
	}

	inline bool SetClusterEocAndSave(unsigned long cluster)
	{
		if (!_clusterChain.SetEocAndSave(cluster))
			return false;

		return true;
	}

	inline bool SetClusterChainEocAndSave(unsigned long cluster)
	{
		if (!_clusterChain.SetChainEocAndSave(cluster))
			return false;

		return true;
	}

	inline bool SetNextCluster(unsigned long cluster, unsigned long nextCluster)
	{
		if (!_clusterChain.SetNextCluster(cluster, nextCluster))
			return false;

		return true;
	}

	inline bool FreeChain(unsigned long cluster)
	{
		if (!_clusterChain.FreeChain(cluster))
		{
			SetError(Error::IOFault);
			return false;
		}
		return true;
	}

	//
	// ����� ������������ ��������� �� ����
	//

	// ��������� ��������� � FAT ��������, ���� ��� ����
	inline bool FlushClusterChain()
	{
		if (!_clusterChain.Flush())
		{
			SetError(Error::IOFault);
			return false;
		}

		return true;
	}

	// ��������� ��������� ���������� � �����
	// ���������� ��������� ������ ����� ������ �����
	inline bool FlushFileInfo(unsigned long cluster)
	{
		FileInfo* info = GetFileInfo(cluster);

		if (info == 0)
			return false;

		if (!SetNewFileSize(info))
		{
			SetError(Error::IOFault);
			return false;
		}

		return true;
	}

	inline bool FlushFsInfo()
	{
		bool modified = false;
		// ���� ���� ���� FreeCount ����������� ��� �������������, �� � ������ ��� �� ����,
		// �.�. �� �� ����� ������� ����� �� ����� ���� ��������;
		// ���� �������� �� ����������, �� ���� ������ � ��������� �� ����
		if (_fsInfo.FreeCount() <= _clusterCount && _clusterChain.FreeClusters() != _fsInfo.FreeCount())
		{
			_fsInfo.SetFreeCount(_clusterChain.FreeClusters()); // ��������� ���������� ��������� ���������
			modified = true;
		}

		// �������� NextFree � ��� ���������� � ����� ������,
		// ���� �� ����������, �� ���������
		if (_fsInfo.NextFree() != _clusterChain.NextFree())
		{
			_fsInfo.SetNextFree(_clusterChain.NextFree());
			modified = true;
		}

		// ���� ��� ���������,
		if (modified == false)
			return true; // �� ���� ������ ���������, �������

		// ��������� ���������
		if (!_provider.WriteSector(_fsInfoSector, (unsigned char*)&_fsInfo))
		{
			SetError(Error::IOFault);
			return false;
		}

		return true;
	}

	//
	// ���������� �� FAT �������
	//

	inline bool ClusterEoc(unsigned long cluster)
	{
		if (_clusterChain.ClusterEoc(cluster))
			return true;
		if (_clusterChain.LastOperationFailed())
			SetError(Error::IOFault);
		return false;
	}

	inline unsigned long NextCluster(unsigned long cluster)
	{
		signed long res = _clusterChain.NextCluster(cluster);
		
		if (res < 0)
		{
			SetError(Error::IOFault);
			return 0;
		}

		return res;
	}

	inline unsigned long FindFreeCluster()
	{
		return _clusterChain.FindFreeCluster();
	}

	//
	// ����� (��� �������) ������� � �������, ��������������� �������
	//

	inline unsigned long GetClusterForPos(unsigned long cluster, unsigned long lastPos, unsigned long lastCluster, unsigned long newPos)
	{
		return GetClusterForPos(cluster, lastPos, lastCluster, newPos, false);
	}

	inline unsigned long GetOrCreateClusterForPos(unsigned long cluster, unsigned long lastPos, unsigned long lastCluster, unsigned long newPos)
	{
		return GetClusterForPos(cluster, lastPos, lastCluster, newPos, true);
	}

	unsigned long GetClusterForPos(unsigned long cluster
									, unsigned long lastPos
									, unsigned long lastCluster
									, unsigned long newPos
									, bool AddClusterIfNeed)
	{
		if (lastPos && newPos >= lastPos && lastCluster > 0)
		{
			cluster = lastCluster;
			newPos -= lastPos - lastPos % _clusterSize;
		}

		unsigned long clusterPrev = 0;

		while(newPos >= _clusterSize && cluster)
		{
			clusterPrev = cluster;
			cluster = NextCluster(cluster);

			if (LastOperationError()) // ���� ������ IO � NextCluster
				return 0;

			newPos -= _clusterSize;
		}

		if (cluster)
			return cluster;

		return (AddClusterIfNeed && newPos < _clusterSize) ? AddClusterToChain(clusterPrev) : 0;
	}

	// �������� � �������� ��� �������
	inline unsigned short GetOffsetForPos(unsigned long pos)
	{
		return (unsigned short)(pos % _clusterSize);
	}

	// ����������� ����� ���� � ������� ��� ������

	void SetEntryCreateDate(DirEntry& entry)
	{
		typename DateTimeProvider::DateType date = DateTimeProvider::GetDate();
		entry.SetCreateDateWise(date.Year(), date.Month(), date.DayOfMonth());

		typename DateTimeProvider::TimeType time = DateTimeProvider::GetTime();
		entry.SetCreateTimeWise(time.Hours(), time.Minutes(), time.Seconds());
	}

	void SetEntryWriteDate(DirEntry& entry)
	{
		typename DateTimeProvider::DateType date = DateTimeProvider::GetDate();
		entry.SetWriteDateWise(date.Year(), date.Month(), date.DayOfMonth());

		typename DateTimeProvider::TimeType time = DateTimeProvider::GetTime();
		entry.SetWriteTime(time.Hours(), time.Minutes(), time.Seconds());
	}

	void SetEntryAccessDate(DirEntry& entry)
	{
		typename DateTimeProvider::DateType date = DateTimeProvider::GetDate();
		entry.SetLastAccessDate(date.Year(), date.Month(), date.DayOfMonth());
	}

	//
	// ������ ���� ����� ��������������?
	// ��� ���, ���� ������ ������ ��������� �������� �������� ������ � ����� �� ��� ������� �������
	// � � ������� ��������� �������� ������ ���� �������
	//
	bool DiskEmptyAfterFormat()
	{
		ClearError();

		if (!SetCluster(_rootCluster))
			return false;
		DirEntry& rec = _cluster.Record(0);

		if (!rec.Valid()) // ���� ������ �����/������
		{
			SetError(Error::IOFault);
			return false;
		}

		if (!rec.LastInDir())
			return false;

		return ClusterEoc(_rootCluster) && !LastOperationError();
	}

	//
	// ��������� ������� _cluster �� ������ ������� � ������ � ��������� ������
	//
	inline bool SetCluster(unsigned long cluster, unsigned short record = 0)
	{
		if (!_cluster.Set(cluster, record))
		{
			SetError(Error::IOFault);
			return false;
		}
		return true;
	}

	inline bool SetClusterToLastRecord(unsigned long cluster)
	{
		if (!_cluster.SetToLastRecord(cluster))
		{
			SetError(Error::IOFault);
			return false;
		}
		return true;
	}

	//
	// ������
	//

	inline void ClearError()
	{
		_lastOperationError = Error::None;
	}

	inline void SetError(ErrorType err)
	{
		_lastOperationError = err;
	}

	//
	// ����� ����������� �������
	//
	inline void WdtClear()
	{
		if (_wdtClear)
			_wdtClear();
	}

private:

	OpenedFilesMap _openedFilesMap;

	// � �������� ��� ���� ��������� ������ ��� �������������
	// ������������ ����� ������ _fsInfo
	union
	{
		Mbr _mbrTmp;
		BootSector _bootSectorTmp;
		FsInfo _fsInfo;
	};

	DataProvider& _provider;

	DirectoryCluster<DataProvider> _cluster;

	unsigned short _clusterSize;
	unsigned long _rootCluster;
	unsigned long _clusterCount;

	unsigned long _fsInfoSector;

	ClusterChain<DataProvider> _clusterChain; // ���� ������� (FAT)

	static const unsigned short _sectorSize = 512;

	ErrorType _lastOperationError;

	WdtClearType _wdtClear; // ��������� �� ������� ������ ����������� �������
};

}
}
}
}

#endif // _MEDIA_FS_FAT32_FAT_H_

