////////////////////////////////////////////////////////////////////////////////
//
//
//	Fateev A.I.		21/02/2012
////////////////////////////////////////////////////////////////////////////////

#ifndef WINDOW_GRAPH_H
#define WINDOW_GRAPH_H

#include "Window.h"
#include "OscFileFormat.h"
#include <math.h>

class DataSourceBase
{
public:
	virtual int GetMinPoint()
	{
		return 0;
	}
	
	virtual int GetMaxPoint()
	{
		return 0;
	}
		
	virtual float GetMinValue()
	{
		return 0;
	}
	
	virtual float GetMaxValue()
	{
		return 0;
	}
	
	virtual bool GetValue(int point, float &v)
	{
		return false;
	}
};

template<class DisplayType, DisplayType &display>
class Axis
{
protected:
	int _oX;
	int _oY;
	int _len;
	
	bool _hor;
	bool _inc;
	bool _mark;
	
	float _valueMin;
	float _valueMax;
	float _ratio;
	float _markPeriod;
public:
	Axis()
	{
		_oX = 0;
		_oY = 0;
		_len = 0;
		_hor = true;
		_inc = true;
		_mark = true;
		_valueMin = 0;
		_valueMax = 0;
		_ratio = 0;
		_markPeriod = 0;
	}
	
	void SetPos(int oX, int oY, int len)
	{
		_oX = oX;
		_oY = oY;
		_len = len;
		UpdateRatio();
	}
	
	void SetHor(bool value = true)
	{
		_hor = value;
	}
	
	void SetInc(bool value = true)
	{
		_inc = value;
	}
	
	void SetMark(bool value = true)
	{
		_mark = value;
	}
	
	void SetMinMaxValue(float min, float max)
	{
		_valueMin = min;
		_valueMax = max;
		UpdateRatio();
	}
	
	void SetMarkPeriod(float period)
	{
		_markPeriod = period;
	}
	
	int GetPointByValue(float val)
	{
		float point = (val - _valueMin) * _ratio;
		return GetMinPoint() + (int)point;
	}
	
	void Draw()
	{
		DrawBaseLine();
		//DrawArrow();
		DrawMarks();
		DrawMinMaxValues();
	}
protected:	
	void DrawBaseLine()
	{
		if (_hor)
		{
			int y = GetBaseLine();
			display.DrawLine(GetMinPoint(), y, GetMaxPoint(), y);
		}
		else
		{
			int x = GetBaseLine();
			display.DrawLine(x, GetMinPoint(), x, GetMaxPoint());
		}
	}
	
	void DrawArrow()
	{
		if (_hor)
		{
			const int x = GetMaxPoint();
			int dx = 5;
			const int dy = 3;
			if (x < GetMinPoint())
				dx = -dx;
			
			display.DrawLine(x - dx, GetBaseLine() - dy, x, GetBaseLine());
			display.DrawLine(x - dx, GetBaseLine() + dy, x, GetBaseLine());
		}
		else
		{
			const int y = GetMaxPoint();
			int dy = 5;
			const int dx = 3;
			if (y < GetMinPoint())
				dy = -dy;
			display.DrawLine(GetBaseLine() - dx, y - dy, GetBaseLine(), y);
			display.DrawLine(GetBaseLine() + dx, y - dy, GetBaseLine(), y);
		}
	}
	
	void DrawMarks()
	{
		float center = 0;
		/*
		if (center < _valueMin)
			center = _valueMin;
		*/
		DrawMarkByValue(center, true);
		if (_markPeriod <= 0)
			return;
		float next = center + _markPeriod;
		while (next < _valueMax)
		{
			bool printVal = (_valueMax - next) <= _markPeriod;
			
			DrawMarkByValue(next, true);
			next += _markPeriod;
		}
		next = center - _markPeriod;
		while (next >= _valueMin)
		{
			DrawMarkByValue(next, true);
			next -= _markPeriod;
		}
	}
	
	void DrawMinMaxValues()
	{
		char str[10];
		unsigned char len;
		
		if (_hor)
		{
			int y = GetBaseLine();
			int minX = GetMinPoint() / 8;
			int maxX = GetMaxPoint() / 8;
		}
		else
		{
			int x = GetBaseLine() / 8;
			int minY = GetMinPoint();
			int maxY = GetMaxPoint();
			
            if (_valueMin < -100000.0 | _valueMin > 100000.0)
            {
                len = sprintf(str, "error");
            }
            else
            {
			    len = sprintf(str, "%1.0f", _valueMin);
            }
			display.MoveCursorTo(x + (_mark ? 0 : (-len + 1)), minY + 1);
			display.WriteLine(str, len);
			
			if (_valueMax < -100000.0 | _valueMax > 100000.0)
            {
                len = sprintf(str, "error");
            }
            else
            {
			    len = sprintf(str, "%3.0f", _valueMax);
            }
			display.MoveCursorTo(x + (_mark ? 0 : (-len + 1)), maxY - 16);
			display.WriteLine(str, len);
		}
	}
	
	void DrawMarkByPoint(int point, bool printValue, float value)
	{
		char str[10];
		unsigned char len;
		
		if (_hor)
		{
			int y = GetBaseLine();
			int dy = 3;
			if (!_mark)
				dy = -dy;
			int x = point;
			display.DrawLine(x, y, x, y - dy);
			if (printValue)
			{
				if (_markPeriod == 50)
					len = sprintf(str, "%1.2f", value / 200.0);
				if (_markPeriod == 100)
					len = sprintf(str, "%1.1f", value / 200.0);
				if (_markPeriod >= 200)
					len = sprintf(str, "%1.0f", value / 200.0);
					
				display.MoveCursorTo(x / 8 - len / 2, y - dy - 16);
				display.WriteLine(str, len);
			}
		}
		else
		{
			int x = GetBaseLine();
			int dx = 3;
			if (!_mark)
				dx = -dx;
			int y = point;
			display.DrawLine(x, y, x + dx, y);
		}
	}
	
	void DrawMarkByValue(float val, bool printValue)
	{
		if (val >= _valueMin && val <= _valueMax)
		{
			DrawMarkByPoint(GetPointByValue(val), printValue, val);
		}
	}
	
	int GetBaseLine()
	{
		if (_hor)
		{
			return _oY;
		}
		else
		{
			return _oX;
		}
	}
	
	int GetMaxPoint()
	{
		if (_hor)
		{
			if (_inc)
				return _oX + _len;
			else
				return _oX - _len;
		}
		else
		{
			if (_inc)
				return _oY - _len;
			else
				return _oY + _len;
		}
	}
	
	int GetMinPoint()
	{
		if (_hor)
		{
			if (_inc)
				return _oX;
			else
				return _oX;
		}
		else
		{
			if (_inc)
			{
				return _oY;
			}
			else
			{
				return _oY;
			}
		}
	}
	
	void UpdateRatio()
	{
		static unsigned int markPeriodVer[] = {10, 25, 50, 100, 250, 500, 1000, 2500, 5000};
		static unsigned int markPeriodHor[] = {1 * 50, 1 * 100, 1 * 200, 2 * 200, 5 * 200, 10 * 200, 20 * 200, 50 * 200, 100 * 200, 250 * 200, 500 * 200};
		
		unsigned int *markPeriod = markPeriodVer;
		unsigned int indexCount = sizeof(markPeriodVer) / sizeof(unsigned int);
		if (_hor)
		{
			markPeriod = markPeriodHor;
			indexCount = sizeof(markPeriodHor) / sizeof(unsigned int);
		}
		
		int dp = GetMaxPoint() - GetMinPoint();
		float dv = _valueMax - _valueMin;
		
		int minDistanceBetweenMarks = _len / 6;
		if (dv == 0)
		{
			_ratio = 0;
			return;
		}
		_ratio = (float)dp / dv;
		
		unsigned int markindex = 0;
		if (fabs(_ratio) > 0)
		{
			while (markPeriod[markindex] * fabs(_ratio) < minDistanceBetweenMarks && markindex < indexCount)
			{
				markindex++;
			}
		}
		
		SetMarkPeriod(markPeriod[markindex]);
	}
};

struct GraphLinePointStruct
{
	int abs;
	float ord;
};

template<class DisplayType, DisplayType &display, int PointsMaxCount, class AxisType>
class GraphLine
{
public:
	
protected:
	AxisType *_axisAbs;
	AxisType *_axisOrd;
	GraphLinePointStruct _points[PointsMaxCount];
	int _pointsCount;
public:
	GraphLine()
	{
		_pointsCount = 0;
	}
	
	void SetAxis(AxisType *axisAbs, AxisType *axisOrd)
	{
		_axisAbs = axisAbs;
		_axisOrd = axisOrd;
	}
	
	void AddPoint(int abs, float ord)
	{
		if (_pointsCount >= PointsMaxCount)
			return;
		_points[_pointsCount].abs = abs;
		_points[_pointsCount].ord = ord;
		_pointsCount++;
	}
	
	void SetPoint(int i, int abs, float ord)
	{
		if (i < 0 || i >= _pointsCount)
			return;
		_points[i].abs = abs;
		_points[i].ord = ord;
	}
	
	void ClearPoints()
	{
		_pointsCount = 0;
	}
	
	void Scroll(int scroll)
	{
		if (scroll == 0)
		{
			return;
		}
		
		if (_pointsCount == 0)
		{
			return;
		}
		
		int from = 0;
		int to = 0;
		int delta = 1;
		
		if (scroll < 0)
		{
			from = 0;
			to = _pointsCount - 1;
			delta = 1;
			
			_pointsCount -= 10;
			if (_pointsCount < 0)
			{
				_pointsCount = 0;
			}
		}
		else
		{
			to = 0;
			from = _pointsCount - 1;
			delta = -1;
			
			_pointsCount += 10;
			if (_pointsCount > PointsMaxCount)
			{
				_pointsCount = PointsMaxCount;
			}
		}
		
		for (int i = from; (delta > 0 ? i <= to : i >= to); i += delta)
		{
			int j = i + scroll;
			if (j < 0 || j >= PointsMaxCount)
			{
				continue;
			}
			
			_points[j] = _points[i];
		}
	}
	
	void TrunkRight(int pos)
	{
		if (_pointsCount > pos)
		{
			_pointsCount = pos;
		}
	}
		
	void Draw()
	{
		if (_axisOrd)
		{
			if (_pointsCount == 1)
			{
				int a = GetPointAbs(0);
				int o = GetPointOrd(0);
				display.DrawPoint(a, o);
			}
			
			for (int i = 1; i < _pointsCount; i++)
			{
				int a1 = GetPointAbs(i - 1);
				int o1 = GetPointOrd(i - 1);
				int a2 = GetPointAbs(i);
				int o2 = GetPointOrd(i);
				display.DrawLine(a1, o1, a2, o2);
			}
		}
		else
		{
			int x1 = -1;
            int prevVal = -1;
            int y = 32;
            for (int i = 0; i < _pointsCount; i++)
			{
				int x = GetPointAbs(i);
                
                int curVal = GetPointOrd(i);
                
                if (prevVal != -1)
                {
                    if (prevVal != curVal)
                    {
                        display.DrawLine(x, y, x, y + 4);
                    }
                }
                
                prevVal = curVal;
                
				if (curVal > 0)
				{
					if (x1 == -1)
                    {
                        x1 = x;
                    }
				}
                
                if (curVal == 0 || i == (_pointsCount - 1))
                {
                    if (x1 != -1)
                    {
                        for (int px = x1; px <= x; px++)
                        {
                            if (px % 2 == 0)
                            {
                                display.DrawPoint(px, y);
                            }
                        }
                        x1 = -1;
                    }
                }
			}
		}
	}
	
	void SetMinMaxValue(float min, float max)
	{
		if (0 == _axisOrd)
			return;
		_axisOrd->SetMinMaxValue(min, max);
	}
	
	void SetMinMaxValueAuto(float minLimit, float maxLimit)
	{
		if (0 == _axisOrd)
			return;
		
		float min = 1000000.0;
		float max = -1000000.0;
		for (int i = 0; i < _pointsCount; i++)
		{
			if (min > _points[i].ord)
			{
				min = _points[i].ord;
			}
			
			if (max < _points[i].ord)
			{
				max = _points[i].ord;
			}
		}
		
		if (min < minLimit)
		{
			min = minLimit;
		}
		
		if (max > maxLimit)
		{
			max = maxLimit;
		}
		
		int minVal = 0;
		while (minVal > min)
		{
			if (minVal > -50)
			{
				minVal -= 10;
			}
			else
			{
				if (minVal > -1000)
				{
					minVal -= 50;
				}
				else
				{
					minVal -= 100;
				}
			}
		}
		
		int maxVal = 10;
		while (maxVal < max)
		{
			if (maxVal < 50)
			{
				maxVal += 10;
			}
			else
			{
				if (maxVal < 1000)
				{
					maxVal += 50;
				}
				else
				{
					maxVal += 100;
				}
			}
		}
		
		_axisOrd->SetMinMaxValue(minVal, maxVal);
	}
	
	int GetPoints(GraphLinePointStruct *points, int maxCount)
	{
		if (points == 0 || maxCount < 1)
		{
			return 0;
		}
		
		int count = maxCount;
		if (count > _pointsCount)
		{
			count = _pointsCount;
		}
		
		memcpy(points, _points, sizeof(GraphLinePointStruct) * count);
		
		return count;
	}
protected:
	int GetPointAbs(int i)
	{
		int res = 0;
		if (_axisAbs)
			res = _axisAbs->GetPointByValue(_points[i].abs);
		return res;
	}
	
	int GetPointOrd(int i)
	{
		int res = 0;
		if (_axisOrd)
		{
			res = _axisOrd->GetPointByValue(_points[i].ord);
		}
		else
		{
			res = _points[i].ord > 0 ? 1 : 0;
		}
		return res;
	}
};

template<class DisplayType, DisplayType &display>
class Graph
{
protected:
	int _left;
	int _top;
	int _right;
	int _bottom;
	
	typedef Axis<DisplayType, display> AxisType;
	typedef Axis<DisplayType, display> y1AxisType;
	
	AxisType xAxis;
	AxisType y1Axis;
	AxisType y2Axis;
public:
	typedef GraphLine<DisplayType, display, 270, AxisType> LineType;
	char *CaptionStr;
protected:	
	LineType line1;
	LineType line2;
	LineType line3;
public:
	Graph()
	{
		CaptionStr = 0;
		
		for (int i = -180; i < 180; i++)
		{
			float v = sin((float)i * 3.14 / 180.0) * 50.0;
			line1.AddPoint(i, v);
		}
		
		for (int i = -180; i < 180; i++)
		{
			float v = sin((float)(i + 90) * 3.14 / 180.0) * 50.0;
			line2.AddPoint(i, v);
		}
	}
	
	void SetPos(int left, int top, int right, int bottom)
	{
		_left = left;
		_top = top;
		_right = right;
		_bottom = bottom;
		
		int width = right - left - 1;
		
		xAxis.SetPos(_left, _bottom, width);
		xAxis.SetHor(true);
		//xAxis.SetMinMaxValue(-180, 180);
		xAxis.SetMarkPeriod(10);
		
		y1Axis.SetPos(_left, _bottom + 240, 190);
		y1Axis.SetHor(false);
		//y1Axis.SetMinMaxValue(-100, 100);
		y1Axis.SetMarkPeriod(10);
		
		y2Axis.SetPos(_left + width, _bottom, 190);
		y2Axis.SetHor(false);
		y2Axis.SetMark(false);
		y2Axis.SetMinMaxValue(-100, 100);
		y2Axis.SetMarkPeriod(10);
		
		line1.SetAxis(&xAxis, &y1Axis);
		line2.SetAxis(&xAxis, &y2Axis);
		line3.SetAxis(&xAxis, 0);
	}
	
	void Draw()
	{
		display.MoveCursorTo(_left / 8, _top);
		if (CaptionStr != 0)
		{
			display.WriteLine(CaptionStr);
		}
		
		for (unsigned char x = 0; x < 40; x++)
		{
			display.WriteByte(x, 15, 0xFF);
		}
		
		line1.Draw();
		line2.Draw();
		line3.Draw();
		xAxis.Draw();
		y1Axis.Draw();
		y2Axis.Draw();
		
		//DrawBorder();
	}
	
	void SetMinMaxX(int min, int max)
	{
		xAxis.SetMinMaxValue(min, max);
	}
	
	LineType *GetLine1()
	{
		return &line1;
	};
	
	LineType *GetLine2()
	{
		return &line2;
	};
	
	LineType *GetLine3()
	{
		return &line3;
	};
protected:	
	void DrawBorder()
	{
		int dx = (_right - _left) / abs(_right - _left);
		int dy = (_bottom - _top) / abs(_bottom - _top);
		
		for (int x = _left; x <= _right; x += dx)
		{
			display.DrawPoint(x, _top);
			display.DrawPoint(x, _bottom);
		}
		
		for (int y = _top; y <= _bottom; y += dy)
		{
			display.DrawPoint(_left, y);
			display.DrawPoint(_right, y);
		}
	}
};

class Sinus : public DataSourceBase
{
int dx;
public:
	virtual int GetMinPoint()
	{
		return -180 * 4;
	}
	
	virtual int GetMaxPoint()
	{
		return 180 * 4;
	}
		
	virtual float GetMinValue()
	{
		return -100;
	}
	
	virtual float GetMaxValue()
	{
		return 100;
	}
	
	virtual bool GetValue(int point, float &v)
	{
		v = sin((float)(point) * 3.14 / 180.0) * 50.0;
		return true;
	}
};

class OscSource : public DataSourceBase
{
public:
	static const unsigned short PointsCacheSize = 260;
	
	enum SourceCode
	{
		SourceCodeUst = 0,
		SourceCodeIst,
		SourceCodeUrot,
		SourceCodeIrot,
		SourceCodeEngineRun,
        SourceCodeEnergising
	};
	
	static const unsigned char SourceCodeMinAnalog = SourceCodeUst;
	static const unsigned char SourceCodeMaxAnalog = SourceCodeIrot;
	static const unsigned char SourceCodeMinDiscrette = SourceCodeEngineRun;
	static const unsigned char SourceCodeMaxDiscrette = SourceCodeEnergising;
protected:
	static const unsigned char FileNameMaxLen = 50;
	char _fileName[FileNameMaxLen];
	
	OscFileFormat::HeaderStruct _header;
	OscFileFormat::AnalogValuesStruct _analogValues;
	OscFileFormat::DataStruct _record;
	int curRecord;
	
	FL_FILE *oscFile;
public:	
	//GraphLinePointStruct PointsCache[2][PointsCacheSize];
	//unsigned short PointsInCache[2];
public:
	OscSource()
	{
		Init(0, 0);
		oscFile = 0;
		//PointsInCache[0] = 0;
		//PointsInCache[1] = 0;
	}
	
	virtual ~OscSource()
	{
		
	}
	
	void Init(char *fileName, unsigned char fileNameLen)
	{
		_fileName[FileNameMaxLen - 1] = 0;
		
		curRecord = 0;
		_header = OscFileFormat::HeaderStruct();
		_analogValues = OscFileFormat::AnalogValuesStruct();
		_record = OscFileFormat::DataStruct();
		curRecord = -1;
		
		oscFile = 0;
		
		if (fileName == 0 || fileNameLen == 0)
		{
			return;
		}
		
		char *path = "/osc/";
		unsigned char i = 0;
		unsigned char j = 0;
		for (; j < strlen(path) && i < FileNameMaxLen - 1; i++, j++)
		{
			_fileName[i] = path[j];
		}
		
		j = 0;
		for (; j < fileNameLen && i < FileNameMaxLen - 1; i++, j++)
		{
			_fileName[i] = fileName[j];
		}
		
		if (i < FileNameMaxLen)
		{
			_fileName[i] = 0;
		}
		
		oscFile = (FL_FILE*)fl_fopen(_fileName, "r");
	
		if (oscFile)
		{
			bool res = fl_fseek(oscFile, 0, 0) == 0;
			if (res)
			{
				res = fl_fread(&_header, sizeof(_header), 1, oscFile) == sizeof(_header);
				res &= _header.Version == 0x0200;
			}
			
			if (res)
			{
				res = fl_fread(&_analogValues, sizeof(_analogValues), 1, oscFile) == sizeof(_analogValues);
			}
			
			if (!res)
			{
				_header = OscFileFormat::HeaderStruct();
				_analogValues = OscFileFormat::AnalogValuesStruct();
			}
			
			fl_fclose(oscFile);
			oscFile = 0;
		}
	}
	
	virtual int GetMinPoint()
	{
		return -_header.Center;
	}
	
	virtual int GetMaxPoint()
	{
		return -_header.Center + _header.Length;
	}
	
	virtual float GetMinValue()
	{
		return 0;
	}
	
	virtual float GetMinValue(unsigned char sourceId)
	{
		switch(sourceId)
		{
		case SourceCodeUst:	
			return -_analogValues.Ust;
		case SourceCodeIst:	
			return -_analogValues.Ist;
		case SourceCodeUrot:	
			return -_analogValues.Urot;
		case SourceCodeIrot:	
			return -_analogValues.Irot;
		}
		
		return 0;
	}
	
	virtual float GetMaxValue()
	{
		return 0;
	}
	
	virtual float GetMaxValue(unsigned char sourceId)
	{
		switch(sourceId)
		{
		case SourceCodeUst:	
			return _analogValues.Ust;
		case SourceCodeIst:	
			return _analogValues.Ist;
		case SourceCodeUrot:	
			return _analogValues.Urot;
		case SourceCodeIrot:	
			return _analogValues.Irot;
		}
		
		return 0;
	}
	
	void OpenFile()
	{
		oscFile = (FL_FILE*)fl_fopen(_fileName, "r");
	}
	
	void CloseFile()
	{
		fl_fclose(oscFile);
		oscFile = 0;
	}
	
	virtual bool GetValue(int point, float &v)
	{
		return false;
	}
	
	float GetValueFromCurrentRecord(unsigned char sourceId)
	{
		float v = 0;
		
		switch(sourceId)
		{
		case SourceCodeUst:
			v = _record.Ust;
			break;
		case SourceCodeIst:
			v = _record.Ist / 10.0;
			break;
		case SourceCodeUrot:
			v = ((signed short)_record.Urot) / 10.0;
			break;
		case SourceCodeIrot:
			v = ((signed short)_record.Irot) / 100.0;
			break;
		case SourceCodeEngineRun:
			v = (_record.Discrete & (1 << 10)) == (1 << 10) ? 1 : 0;
			break;
        case SourceCodeEnergising:
            v = (_record.Discrete & (1 << 0)) == (1 << 0) ? 1 : 0;
            break;
		}
		
		return v;
	}
	
	virtual bool GetValue(unsigned char sourceId, int point, float &v)
	{
		if (curRecord == point)
		{
			v = GetValueFromCurrentRecord(sourceId);
			return true;
		}
		
		int p = point + _header.Center;
		
		if (p < 0)
		{
			return false;
		}
		
		if (p >= _header.Length)
		{
			return false;
		}
		
		bool res = false;
		
		
	
		if (oscFile)
		{
			long int offset = sizeof(_header);
			offset += sizeof(_analogValues);
			offset += sizeof(_record) * p;
			
			res = fl_fseek(oscFile, offset, 0) == 0;
			if (res)
			{
				res = fl_fread(&_record, sizeof(_record), 1, oscFile) == sizeof(_record);
			}
			
			if (res)
			{
				curRecord = point;
				v = GetValueFromCurrentRecord(sourceId);
			}
		}
		
		return res;
	}
};

template<class DisplayType, DisplayType &display, class DrawContext, DrawContext* drawContext>
class WindowGraph : public Window<DisplayType, display>
{
public:
	typedef Window<DisplayType, display> Base;
	
	enum State
	{
		StateClosed,
		StateInit,
		StateIdle,
		StateUpdateData,
	};
protected:
	typedef Graph<DisplayType, display> GraphType;
	GraphType _graph;
	DataSourceBase *dataSource1;
	Sinus sinDataSource;
	OscSource _oscSource;
	DataSourceBase *dataLine1;
	unsigned char _zoom;
	unsigned char _maxZoom;
	int _center;
	State _state;
	bool _updateDataRequestPending;
	int _updateDataRequestHorScroll;
	
	unsigned char source1Id;
	unsigned char source2Id;
	unsigned char source3Id;
public:
	char *OscFileName;
public:
	WindowGraph()
	{
		dataSource1 = 0;
		dataLine1 = 0;
		
		_graph.SetPos(8 * 6, 0, 8 * 39 - 1, 222);
		
		dataLine1 = &sinDataSource;
		source1Id = OscSource::SourceCodeUst;
		source2Id = OscSource::SourceCodeUrot;
		source3Id = OscSource::SourceCodeEngineRun;
		
		_maxZoom = 32;
		_zoom = _maxZoom;
		_center = 0;
		_state = StateClosed;
		_updateDataRequestPending = false;
		_updateDataRequestHorScroll = 0;
	}
	
	virtual void Draw()
	{
		_graph.Draw();
		PrintZoom();
		DrawSource(0, 32 + (source1Id == OscSource::SourceCodeUst ? 240 : 0), " Uст", source1Id == OscSource::SourceCodeUst || source2Id == OscSource::SourceCodeUst);
		DrawSource(0, 48 + (source1Id == OscSource::SourceCodeIst ? 240 : 0), " Iст", source1Id == OscSource::SourceCodeIst || source2Id == OscSource::SourceCodeIst);
		DrawSource(0, 64 + (source1Id == OscSource::SourceCodeUrot ? 240 : 0), " Uрот", source1Id == OscSource::SourceCodeUrot || source2Id == OscSource::SourceCodeUrot);
		DrawSource(0, 80 + (source1Id == OscSource::SourceCodeIrot ? 240 : 0), " Iрот", source1Id == OscSource::SourceCodeIrot || source2Id == OscSource::SourceCodeIrot);
		
		DrawSource(0, 112, "Д вкл", source3Id == OscSource::SourceCodeEngineRun);
		DrawSource(0, 128, "Возб ", source3Id == 5);
	}
	
	void DrawSource(int posX, int posY, char *text, bool selected)
	{
		display.MoveCursorTo(posX, posY);
		display.WriteLine(text);
		
		if (selected)
		{
			for (unsigned char y = 0; y < 16; y++)
			for (unsigned char x = 0; x < 5; x++)
			{
				display.InvertByte(posX + x, posY + y);
			}
		}
		else
		{
			for (int x = 0; x < 5 * 8; x ++)
			{
				display.DrawPoint(posX + x, posY);
				display.DrawPoint(posX + x, posY + 16);
			}
			
			for (int y = 0; y < 16; y ++)
			{
				display.DrawPoint(0, posY + y);
				display.DrawPoint(5 * 8 - 1, posY + y);
			}
		}
	}
	
	void PrintZoom()
	{
		char str[7];
		char len;
		
		len = sprintf(str, "x1/%d", _zoom);
		
		display.MoveCursorTo(0, 239 - 16);
		display.WriteLine(str, len);
	}
	
	virtual void OnShow()
	{
		if (drawContext)
		{
			drawContext->SecondBufferEnable(true);
		}
		
		source1Id = OscSource::SourceCodeUst;
		source2Id = OscSource::SourceCodeUrot;
		source3Id = OscSource::SourceCodeEngineRun;
		_graph.CaptionStr = OscFileName;
		_zoom = _maxZoom;
		_center = 0;
		UpdateData(0);
		_state = StateInit;
	}
	
	virtual void Close()
	{
		Base::Close();
		
		_state = StateClosed;
	}
	
	virtual bool OnKeyDown(char &key)
	{
		if ('E' == key)
		{
			WindowBase::Close();
			return true;
		}
		
		if ('+' == key)
		{
			OnZoomIn();
			return true;
		}
		
		if ('-' == key)
		{
			OnZoomOut();
			return true;
		}
		
		if ('1' == key)
		{
			OnNextSource(source1Id, source2Id);
			UpdateData(0);
			return true;
		}
		
		if ('2' == key)
		{
			OnNextSource(source2Id, source1Id);
			UpdateData(0);
			return true;
		}
		
		if ('3' == key)
		{
			source3Id++;
			if (source3Id > OscSource::SourceCodeMaxDiscrette)
			{
				source3Id = OscSource::SourceCodeMinDiscrette;
			}
			UpdateData(0);
			return true;
		}
		
		if ('4' == key)
		{
			UpdateData(-10);
			return true;
		}
		
		if ('6' == key)
		{
			UpdateData(10);
			return true;
		}
		return false;
	}
	
	void Run()
	{
		switch (_state)
		{
		case StateClosed:
			{
				_updateDataRequestPending = false;
			}
			break;
		case StateInit:
			{
				if (OscFileName != 0)
				{
					int strLen = 0;
					strLen = strlen(OscFileName);
					_oscSource.Init(OscFileName, strLen);
					_state = StateIdle;
				}
				else
				{
					_state = StateClosed;
				}
			}
			break;
		case StateIdle:
			{
				if (_updateDataRequestPending)
				{
					int horScroll = _updateDataRequestHorScroll;
					_updateDataRequestPending = false;
					ProcessUpdateDataRequest(horScroll);
				}
			}
			break;
		case StateUpdateData:
			{
			}
			break;
		}
	}
	
	void OnZoomIn()
	{
		if (_zoom >= 2)
		{
			_zoom >>= 1;
		}
		UpdateData(0);
	}
	
	void OnZoomOut()
	{
		if (_zoom < _maxZoom)
		{
			_zoom <<= 1;
		}
		UpdateData(0);
	}
	
	void OnNextSource(unsigned char &sourceChanging, unsigned char sourceOther)
	{
		const unsigned char maxSourceId = OscSource::SourceCodeMaxAnalog;
		
		sourceChanging++;
		if (sourceChanging > maxSourceId)
		{
			sourceChanging = OscSource::SourceCodeMinAnalog;
		}
		
		if (sourceChanging == sourceOther)
		{
			OnNextSource(sourceChanging, sourceOther);
		}
	}
	
	void UpdateData(short horScroll)
	{
		_updateDataRequestHorScroll = horScroll;
		_updateDataRequestPending = true;
	}
	
	void ProcessUpdateDataRequest(short horScroll)
	{
		_state = StateUpdateData;
		
		typename GraphType::LineType *line1 = _graph.GetLine1();
		typename GraphType::LineType *line2 = _graph.GetLine2();
		typename GraphType::LineType *line3 = _graph.GetLine3();
		
		if (0 == line1 || 0 == line2 || 0 == line3)
			return;
		/*
		line1->ClearPoints();
		
		if (dataLine1)
		{
			int min = dataLine1->GetMinPoint();
			int max = dataLine1->GetMaxPoint();
			
			int left = -_zoom * 100 + _center;
			int right = _zoom * 100 + _center;
			
			if (right > max)
			{
				int dx = max - right;
				left += dx;
				right += dx;
				_center += dx;
			}
			
			if (left < min)
			{
				int dx = min - left;
				left += dx;
				right += dx;
				_center += dx;
			}
			
			_graph.SetMinMaxX(left, right);
			line1->SetMinMaxValue(dataLine1->GetMinValue(), dataLine1->GetMaxValue());
	
			for(int i = left; i <= right; i += _zoom)
			{
				if (i >= min && i <= max)
				{
					float v = 0; 
					if (dataLine1->GetValue(i, v))
						line1->AddPoint(i, v);
				}
			}
		}
		*/
		
		_oscSource.OpenFile();
		
		if (horScroll == 0)
		{
			int pointsStep = _zoom;
			
			line1->ClearPoints();
			line2->ClearPoints();
			line3->ClearPoints();
			
			int min = _oscSource.GetMinPoint();
			int max = _oscSource.GetMaxPoint();
			
			int left = -pointsStep * 100 + _center;
			int right = pointsStep * 100 + _center;
			
			if (right > max)
			{
				int dx = max - right;
				left += dx;
				right += dx;
				_center += dx;
			}
			
			if (left < min)
			{
				int dx = min - left;
				left += dx;
				right += dx;
				_center += dx;
			}
			
			_graph.SetMinMaxX(left, right);
			line1->SetMinMaxValue(_oscSource.GetMinValue(source1Id), _oscSource.GetMaxValue(source1Id));
			line2->SetMinMaxValue(_oscSource.GetMinValue(source2Id), _oscSource.GetMaxValue(source2Id));
			
			//_oscSource.PointsInCache[0] = line1->GetPoints(_oscSource.PointsCache[0], _oscSource.PointsCacheSize);
			
			for(int i = left; i <= right; i += pointsStep)
			{
				if (i >= min && i <= max)
				{
					float v = 0; 
					if (_oscSource.GetValue(source1Id, i, v))
					{
						line1->AddPoint(i, v);
					}
					
					if (_oscSource.GetValue(source2Id, i, v))
					{
						line2->AddPoint(i, v);
					}
					
					if (_oscSource.GetValue(source3Id, i, v))
					{
						line3->AddPoint(i, v);
					}
				}
			}
			
			line1->SetMinMaxValueAuto(_oscSource.GetMinValue(source1Id), _oscSource.GetMaxValue(source1Id));
			line2->SetMinMaxValueAuto(_oscSource.GetMinValue(source2Id), _oscSource.GetMaxValue(source2Id));
		}
		else
		{
			int oldCenter = _center;
			
			_center += _zoom * horScroll;
			
			int pointsStep = _zoom;
			int min = _oscSource.GetMinPoint();
			int max = _oscSource.GetMaxPoint();
			
			int left = -pointsStep * 100 + _center;
			int right = pointsStep * 100 + _center;
			
			if (right > max)
			{
				int dx = max - right;
				left += dx;
				right += dx;
				_center += dx;
			}
			
			if (left < min)
			{
				int dx = min - left;
				left += dx;
				right += dx;
				_center += dx;
			}
			
			_graph.SetMinMaxX(left, right);
			
			int deltaCenter = _center - oldCenter;
			if (abs(deltaCenter) != abs(pointsStep * horScroll) && deltaCenter != 0)
			{
				line1->ClearPoints();
				line2->ClearPoints();
				line3->ClearPoints();
				
				for(int i = left; i <= right; i += pointsStep)
				{
					if (i >= min && i <= max)
					{
						float v = 0; 
						if (_oscSource.GetValue(source1Id, i, v))
						{
							line1->AddPoint(i, v);
						}
						if (_oscSource.GetValue(source2Id, i, v))
						{
							line2->AddPoint(i, v);
						}
						if (_oscSource.GetValue(source3Id, i, v))
						{
							line3->AddPoint(i, v);
						}
					}
				}
				
				line1->SetMinMaxValueAuto(_oscSource.GetMinValue(source1Id), _oscSource.GetMaxValue(source1Id));
				line2->SetMinMaxValueAuto(_oscSource.GetMinValue(source2Id), _oscSource.GetMaxValue(source2Id));
			}
			else
			{
				int scroll = deltaCenter / pointsStep;
				
				if (scroll != 0)
				{
					line1->Scroll(-scroll);
					line2->Scroll(-scroll);
					line3->Scroll(-scroll);
					
					if (horScroll < 0)
					{
						for(int p = 0, i = left; p < abs(scroll) && i <= right; i += pointsStep)
						{
							if (i >= min && i <= max)
							{
								float v = 0; 
								if (_oscSource.GetValue(source1Id, i, v))
								{
									line1->SetPoint(p, i, v);
								}
								
								if (_oscSource.GetValue(source2Id, i, v))
								{
									line2->SetPoint(p, i, v);
								}
								
								if (_oscSource.GetValue(source3Id, i, v))
								{
									line3->SetPoint(p, i, v);
								}
								
								p++;
							}
						}
					}
					else
					{
						for(int i = right - abs(scroll) * pointsStep;i <= right; i += pointsStep)
						{
							if (i >= min && i <= max)
							{
								float v = 0; 
								if (_oscSource.GetValue(source1Id, i, v))
								{
									line1->AddPoint(i, v);
								}
								if (_oscSource.GetValue(source2Id, i, v))
								{
									line2->AddPoint(i, v);
								}
								if (_oscSource.GetValue(source3Id, i, v))
								{
									line3->AddPoint(i, v);
								}
							}
						}
					}
					
					int howManyPointsCanDraw = (right - left) / pointsStep;
					line1->TrunkRight(howManyPointsCanDraw);
					line2->TrunkRight(howManyPointsCanDraw);
					line3->TrunkRight(howManyPointsCanDraw);
					line1->SetMinMaxValueAuto(_oscSource.GetMinValue(source1Id), _oscSource.GetMaxValue(source1Id));
					line2->SetMinMaxValueAuto(_oscSource.GetMinValue(source2Id), _oscSource.GetMaxValue(source2Id));
				}
			}
			
		}
				
		_oscSource.CloseFile();
		
		if (_state == StateUpdateData)
		{
			_state = StateIdle;
		}
	}
};

#endif
