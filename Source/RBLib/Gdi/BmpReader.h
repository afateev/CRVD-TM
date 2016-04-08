///////////////////////////////////////////////////////////////////////////////
//	Класс для чтения изображений в формате BMP
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef BMPREADER_H
#define BMPREADER_H

namespace Rblib
{
namespace Gdi
{
	class BmpReader
	{
	public:
		typedef unsigned char	ByteType;
		typedef unsigned short	WordType;
		typedef unsigned int	DwordType;
		typedef int				LongType;
		
		enum CompressionType
		{
			CompressionTypeRGB = 0,
			CompressionTypeRLE8 = 1,
			CompressionTypeRLE4 = 2,
			CompressionTypeBITFIELDS = 3,
			CompressionTypeJPEG = 4,
			CompressionTypePNG = 5,
			CompressionTypeALPHABITFIELDS = 6
		};
		
		#pragma pack(push, 1)
		struct FileHeaderStruct
		{
			WordType Signature;
			DwordType Size;
			WordType Reserved1;
			WordType Reserved2;
			DwordType OffBits;
		};

		struct BitmapInfoStruct
		{
			DwordType Size;
			LongType Width;
			LongType Height;
			WordType Planes;
			WordType BitCount;
			DwordType Compression;
			DwordType SizeImage;
			LongType XPelsPerMeter;
			LongType YPelsPerMeter;
			DwordType ClrUsed;
			DwordType ClrImportant;
		};
		typedef DwordType FXPT2DOT30;
		typedef struct tagCIEXYZ {
			FXPT2DOT30 ciexyzX;
			FXPT2DOT30 ciexyzY;
			FXPT2DOT30 ciexyzZ;
		} CIEXYZ;
		
		typedef struct tagCIEXYZTRIPLE
			{
				CIEXYZ ciexyzRed;
				CIEXYZ ciexyzGreen;
				CIEXYZ ciexyzBlue;
			} CIEXYZTRIPLE;
		
		struct BitmapInfoV4Struct
		{
			DwordType Size;
			LongType Width;
			LongType Height;
			WordType Planes;
			WordType BitCount;
			DwordType Compression;
			DwordType SizeImage;
			LongType XPelsPerMeter;
			LongType YPelsPerMeter;
			DwordType ClrUsed;
			DwordType ClrImportant;
			DwordType RedMask;
			DwordType GreenMask;
			DwordType BlueMask;
			DwordType AlphaMask;
			DwordType CSType;
			CIEXYZTRIPLE Endpoints;
			DwordType GammaRed;
			DwordType GammaGreen;
			DwordType GammaBlue;
		};

		struct PixelFormat24
		{
			ByteType B;
			ByteType G;
			ByteType R;
		};
		#pragma pack (pop)
	protected:
		unsigned char *_dump;
		bool _valid;
		FileHeaderStruct *_fileHdr;
		BitmapInfoStruct *_bitmapInfo;
		BitmapInfoV4Struct *_bitmapInfoV4;
		unsigned char *_bits;
		unsigned char _rFieldOffset;
		unsigned char _gFieldOffset;
		unsigned char _bFieldOffset;
		unsigned char _rBitCount;
		unsigned char _gBitCount;
		unsigned char _bBitCount;
		unsigned char _rMaxValue;
		unsigned char _gMaxValue;
		unsigned char _bMaxValue;
		int _rowSize;
	public:
		// Конструктор по умолчанию
		BmpReader()
		{
			_dump = 0;
			_valid = false;
		}
		
		// Конструктор по дампу файла изображения
		BmpReader (void *dump)
		{
			SetDump(dump);
		}
		
		// Открыть файл из образа в памяти
		void SetDump(void *dump)
		{
			_dump = (unsigned char *)dump;
			
			_valid = _dump != 0;
			
			if (_valid)
			{
				_fileHdr = (FileHeaderStruct *)_dump;

				_valid &= sizeof(FileHeaderStruct) == 14;
				_valid &= _fileHdr->Signature == 0x4D42;
			}

			if (_valid)
			{
				_bitmapInfo = (BitmapInfoStruct *)&_dump[sizeof(FileHeaderStruct)];
				
				_valid &= sizeof(BitmapInfoStruct) == 40;
				_valid &= _bitmapInfo->Size >= sizeof(BitmapInfoStruct);
				_valid &= _bitmapInfo->Width > 0;
				_valid &= _bitmapInfo->Height > 0;
				_valid &= _bitmapInfo->BitCount == 16 || _bitmapInfo->BitCount == 24;
				_valid &= _bitmapInfo->Compression == CompressionTypeRGB || _bitmapInfo->Compression == CompressionTypeBITFIELDS;
				
				if (_bitmapInfo->Compression == CompressionTypeBITFIELDS)
				{
					_bitmapInfoV4 = (BitmapInfoV4Struct *)&_dump[sizeof(FileHeaderStruct)];
				
					int tmp = sizeof(BitmapInfoV4Struct);
					_valid &= tmp == 108;
					_valid &= _bitmapInfoV4->Size >= sizeof(BitmapInfoV4Struct);
					_valid &= _bitmapInfoV4->RedMask > 0;
					_valid &= _bitmapInfoV4->GreenMask > 0;
					_valid &= _bitmapInfoV4->BlueMask > 0;
					
					if (_valid)
					{
						_rFieldOffset = 0;
						_gFieldOffset = 0;
						_bFieldOffset = 0;
						DwordType r = _bitmapInfoV4->RedMask;
						DwordType g = _bitmapInfoV4->GreenMask;
						DwordType b = _bitmapInfoV4->BlueMask;
						while ((r >> _rFieldOffset & 1) == 0 || (g >> _gFieldOffset & 1) == 0 || (b >> _bFieldOffset & 1) == 0)
						{
							if ((r >> _rFieldOffset & 1) == 0)
							{
								_rFieldOffset++;
							}
							
							if ((g >> _gFieldOffset & 1) == 0)
							{
								_gFieldOffset++;
							}
							
							if ((b >> _bFieldOffset & 1) == 0)
							{
								_bFieldOffset++;
							}
						}
						
						_rMaxValue = r >> _rFieldOffset;
						_gMaxValue = g >> _gFieldOffset;
						_bMaxValue = b >> _bFieldOffset;
						
						r  = _rMaxValue;
						g  = _gMaxValue;
						b  = _bMaxValue;
						
						while (r > 0 || g > 0 || b > 0)
						{
							if (r > 0)
							{
								_rBitCount++;
								r >>= 1;
							}
							
							if (g > 0)
							{
								_gBitCount++;
								g >>= 1;
							}
							
							if (b > 0)
							{
								_bBitCount++;
								b >>= 1;
							}
						}
					}
				}
			}

			if (_valid)
			{
				_rowSize = (_bitmapInfo->BitCount * _bitmapInfo->Width + 31) / 32 * 4;
			}

			if (_valid)
			{
				_bits = &_dump[_fileHdr->OffBits];
			}
		}
		
		// Изображение может быть использовано
		bool Valid()
		{
			return _valid;
		}
		
		// Ширина изображения в точках
		int Width()
		{
			return _valid ? _bitmapInfo->Width : 0;
		}

		// Высота изображения в точках
		int Height()
		{
			return _valid ? _bitmapInfo->Height : 0;
		}
		
		// Получить точку изображениея по координатам
		bool GetPixel(int x, int y, unsigned char &r, unsigned char &g, unsigned char &b)
		{
			if (!_valid)
			{
				return false;
			}

			if (x < 0 || x >= _bitmapInfo->Width)
			{
				return false;
			}

			if (y < 0 || y >= _bitmapInfo->Height)
			{
				return false;
			}

			// порядок строк снизу-вверх или сверху-вниз
			int row = _bitmapInfo->Height > 0 ? _bitmapInfo->Height - y - 1 : y;
			
			if (_bitmapInfo->Compression == CompressionTypeRGB)
			{
				PixelFormat24 *_pixel = (PixelFormat24 *)&_bits[row * _rowSize + x * sizeof(PixelFormat24)];
				r = _pixel->R;
				g = _pixel->G;
				b = _pixel->B;
			}
			
			if (_bitmapInfo->Compression == CompressionTypeBITFIELDS)
			{
				if (_bitmapInfo->BitCount == 16)
				{
					unsigned short pixel = *((unsigned short *)&_bits[row * _rowSize + x * 2]);
					r = (pixel & _bitmapInfoV4->RedMask) >> _rFieldOffset;
					g = (pixel & _bitmapInfoV4->GreenMask) >> _gFieldOffset;
					b = (pixel & _bitmapInfoV4->BlueMask) >> _bFieldOffset;
					
					r <<= 8 - _rBitCount;
					g <<= 8 - _gBitCount;
					b <<= 8 - _bBitCount;
				}
			}
			
			return true;
		}
	};
}
}

#endif