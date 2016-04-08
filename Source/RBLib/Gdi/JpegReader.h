///////////////////////////////////////////////////////////////////////////////
//	Класс для чтения файлов JPEG
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef JPEGREADER_H
#define JPEGREADER_H

namespace Rblib
{
namespace Gdi
{
	class JpegReader
	{
	public:
		typedef unsigned char	ByteType;
		typedef unsigned short	WordType;
		typedef unsigned int	DwordType;
		typedef int				LongType;
		
		#pragma pack(push, 1)
		struct Sof0Struct
		{
			WordType SegmentLen;
			ByteType Precision;
			WordType Height;
			WordType Width;
			ByteType ComponentCount;
			
			struct
			{
				ByteType Id					: 8;
				ByteType VerSubsampling		: 4;
				ByteType HorSubsampling		: 4;
				ByteType QuantTableId		: 8;
			} ComponentDescription[3];
		};
		
		struct DriStruct
		{
			WordType SegmentLen;
			WordType RestartInterval;
		};
		
		struct SosStruct
		{
			WordType SegmentLen;
			ByteType ComponentCount;
			// и т.д.
			// остальные поля пропущены
		};
		#pragma pack (pop)
	protected:
		unsigned char *_dump;
		unsigned int _dumpSize;
		bool _valid;
		unsigned int _sioOffset;
		unsigned int _sofOffset;
		unsigned int _driOffset;
		unsigned int _sosOffset;
		Sof0Struct *_sof0;
		DriStruct *_dri;
		SosStruct *_sos;
	public:
		// Конструктор по умолчанию
		JpegReader()
		{
			_dump = 0;
			_valid = false;
		}
		
		// Конструктор по дампу файла изображения
		JpegReader (void *dump, unsigned int dumpSize)
		{
			SetDump(dump, dumpSize);
		}
		
		// Открыть файл из образа в памяти
		void SetDump(void *dump, unsigned int dumpSize)
		{
			_dump = (unsigned char *)dump;
			_dumpSize = dumpSize;
			
			_valid = _dump != 0;
			_valid &= dumpSize >= 2;
			
			_sioOffset = 0;
			_sofOffset = 0;
			_driOffset = 0;
			
			if (_valid)
			{
				_sioOffset = FindBlockBodyOffset(0xD8, 0, _dumpSize);
				_valid &= _sioOffset > 0;
			}
			
			if (_valid)
			{
				_sofOffset = FindBlockBodyOffset(0xC0, _sioOffset, _dumpSize);
				_valid &= _sofOffset > 0;
			}
			
			if (_valid)
			{
				_sof0 = (Sof0Struct*)&_dump[_sofOffset];
				_valid &= SwapBytes(_sof0->SegmentLen) >= 8;
				_valid &= _sofOffset + SwapBytes(_sof0->SegmentLen) < _dumpSize;
				
				if (_valid)
				{
					// только с такими умеем работать
					_valid &= _sof0->ComponentCount == 1 || _sof0->ComponentCount == 3;
				}
			}
			
			if (_valid)
			{
				_driOffset = FindBlockBodyOffset(0xDD, _sioOffset, _dumpSize);
				_valid &= _driOffset > 0;
			}
			
			if (_valid)
			{
				_dri = (DriStruct*)&_dump[_driOffset];
				_valid &= SwapBytes(_dri->SegmentLen) == 4;
			}
			
			if (_valid)
			{
				_sosOffset = FindBlockBodyOffset(0xDA, _sioOffset, _dumpSize);
				_valid &= _sosOffset > 0;
			}
			
			if (_valid)
			{
				_sos = (SosStruct*)&_dump[_sosOffset];
				_valid &= SwapBytes(_sos->SegmentLen) >= 3;
			}
		}
		
		bool GetMcuBlockSize(unsigned char &w, unsigned char &h)
		{
			bool res = _valid;
			
			if (_valid)
			{
				unsigned char minV = 255;
				unsigned char maxV = 0;
				unsigned char minH = 255;
				unsigned char maxH = 0;
				for (int i = 0; i < _sof0->ComponentCount; i++)
				{
					if (_sof0->ComponentDescription[i].VerSubsampling < minV)
					{
						minV = _sof0->ComponentDescription[i].VerSubsampling;
					}
					
					if (_sof0->ComponentDescription[i].HorSubsampling < minH)
					{
						minH = _sof0->ComponentDescription[i].HorSubsampling;
					}
					
					if (_sof0->ComponentDescription[i].VerSubsampling > maxV)
					{
						maxV = _sof0->ComponentDescription[i].VerSubsampling;
					}
					
					if (_sof0->ComponentDescription[i].HorSubsampling > maxH)
					{
						maxH = _sof0->ComponentDescription[i].HorSubsampling;
					}
				}
				
				w = 0;
				if (minH > 0)
				{
					w = maxH / minH * 8;
				}
				else
				{
					res = false;
				}
				
				h = 0;
				if (minV > 0)
				{
					h = maxV / minV * 8;
				}
				else
				{
					res = false;
				}
			}
			
			return res;
		}
		
		bool GetImageSize(unsigned short &w, unsigned short &h)
		{
			bool res = _valid;
			
			if (_valid)
			{
				w = SwapBytes(_sof0->Width);
				h = SwapBytes(_sof0->Height);
			}
			
			return res;
		}
		
		bool GetDri(unsigned short &dri)
		{
			bool res = _valid;
			
			if (_valid)
			{
				dri = SwapBytes(_dri->RestartInterval);
			}
			
			return res;
		}
		
		// Изображение может быть использовано
		bool Valid()
		{
			return _valid;
		}
		
		unsigned int CopyHeaderTo(unsigned char *dst, unsigned int modifyWidth, unsigned int modifyHeigth)
		{
			unsigned int hdrSize = 0;
			
			if (_valid)
			{	
				unsigned int heigthOffset = _sofOffset;
				heigthOffset += 3;
				unsigned int widthOffset = _sofOffset;
				widthOffset += 5;
				
				unsigned short widthValue = SwapBytes(modifyWidth);
				unsigned short heigthValue = SwapBytes(modifyHeigth);
				
				unsigned int start = _sioOffset - 2;
				unsigned int end = _sosOffset + SwapBytes(_sos->SegmentLen);
				
				for (unsigned int i = start; i <= end; i++)
				{
					if (i == heigthOffset)
					{
						dst[hdrSize] = heigthValue & 0xFF;
					}
					else
					{
						if (i == heigthOffset + 1)
						{
							dst[hdrSize] = (heigthValue >> 8) & 0xFF;
						}
						else
						{
							if (i == widthOffset)
							{
								dst[hdrSize] = widthValue & 0xFF;
							}
							else
							{
								if (i == widthOffset + 1)
								{
									dst[hdrSize] = (widthValue >> 8) & 0xFF;
								}
								else
								{
									dst[hdrSize] = _dump[i];
								}
							}
						}
					}
					hdrSize++;
				}
			}
			
			return hdrSize;
		}
		
		unsigned int CopyDataTo(unsigned char *dst, unsigned int startFrom, unsigned int &nextRestart, bool insertRestart, unsigned char restartId)
		{
			unsigned int copyCount = 0;
			
			unsigned int restartOffset = FindRestartOffset(startFrom, _dumpSize);
			
			if (restartOffset > 0)
			{
				copyCount = restartOffset - startFrom;
				nextRestart = startFrom + copyCount + 2;
			}
			else
			{
				unsigned int eoiOffset = FindBlockBodyOffset(0xD9, startFrom, _dumpSize);
				if (eoiOffset > 0)
				{
					copyCount = eoiOffset - 2 - startFrom;
				}
				else
				{
					if (_dumpSize > startFrom)
					{
						copyCount = _dumpSize - startFrom;
					}
					else
					{
						copyCount = 0;
					}
				}
				nextRestart = startFrom + copyCount;
			}
			
			bool restratInserted = copyCount > 0 && insertRestart;
			
			if (restratInserted)
			{
				dst[0] = 0xFF;
				unsigned char marker = 0xD0;
				marker |= restartId &0x07;
				dst[1] = marker;
				
				dst += 2;
			}
			
			if (copyCount > 0)
			{
				for (unsigned int i = 0; i < copyCount; i++)
				{
					dst[i] = _dump[startFrom + i];
				}
			}
			
			return copyCount + (restratInserted ? 2 : 0);
		}
		
		unsigned int WriteRestartMarkerTo(unsigned char *dst, unsigned char val)
		{
			dst[0] = 0xFF;
			unsigned char marker = 0xD0;
			marker |= val &0x0F;
			dst[1] = marker;
			return 2;
		}
		
		unsigned int WriteEndMarkerTo(unsigned char *dst)
		{
			dst[0] = 0xFF;
			dst[1] = 0xD9;
			return 2;
		}
	protected:
		WordType SwapBytes(WordType src)
		{
			WordType res = 0;
			res = src & 0xFF;
			res <<= 8;
			res |= (src >> 8) & 0xFF;
			return res;
		}
		
		unsigned int FindBlockBodyOffset(unsigned char marker, unsigned int startOffset, unsigned int endOffset)
		{
			for (int i = startOffset; i < endOffset; i++)
			{
				if (_dump[i] == 0xFF)
				{
					if (i < endOffset - 1)
					{
						if (_dump[i + 1] == marker)
						{
							return i + 2;
						}
					}
				}
			}
			
			return 0;
		}
		
		unsigned int FindRestartOffset(unsigned int startOffset, unsigned int endOffset)
		{
			for (int i = startOffset; i < endOffset; i++)
			{
				if (_dump[i] == 0xFF)
				{
					if (i < endOffset - 1)
					{
						if ((_dump[i + 1] >> 4 & 0x0F) == 0x0D)
						{
							return i;
						}
					}
				}
			}
			
			return 0;
		}
	};
}
}

#endif