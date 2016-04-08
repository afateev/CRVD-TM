
#ifndef _RBLIB_BD_FILE_CLIENT_H_
#define _RBLIB_BD_FILE_CLIENT_H_

namespace Rblib
{
namespace Db
{

template<class FS>
class FileClient
{
public:

	FileClient(FS& fs) : _fs(fs)
	{
	}

	inline bool Open(char* fileName)
	{
		_file = _fs.OpenFileInOut(fileName); // открываем файл для чтения и записи
		return _file != 0;
	}

	inline bool Close()
	{
		return _fs.CloseFile(_file);
	}

	inline bool Flush()
	{
		return _fs.Flush(_file);
	}

	inline unsigned long FileSize()
	{
		return _fs.FileSize(_file);
	}

	inline bool SeekFile(unsigned long pos)
	{
		return _fs.SeekFile(_file, pos);
	}

	inline bool ReadFile(unsigned char* buffer, unsigned long len)
	{
		return _fs.ReadFile(_file, buffer, len) == len;
	}

	inline bool WriteFile(unsigned char* buffer, unsigned long len)
	{
		return _fs.WriteFile(_file, buffer, len) == len;
	}

	inline bool IoFault()
	{
		return _fs.IoFault();
	}

private:

	FS& _fs; // файловая система
	unsigned long _file; // файл

};

}
}

#endif // _RBLIB_BD_FILE_CLIENT_H_

