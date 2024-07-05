#include "global_arithmetic.h"
#include "CodingUnit.h"
#include "utility.h"
#include <iostream> 
#include <assert.h>
#include <cmath>

using namespace std;

CodingUnit::CodingUnit(void)
{

}

CodingUnit::~CodingUnit()
{

}
void CodingUnit::destroy(CodingUnit* CU)
{
	for (int childIndex = 0; childIndex < 4; childIndex++)
		if (CU->child[childIndex] != NULL) CU->child[childIndex]->destroy(CU->child[childIndex]);
	if (CU->m_value != NULL) { delete[]CU->m_value; CU->m_value = NULL; }
	if (CU->Edge != NULL) { delete[]CU->Edge; CU->Edge = NULL; }
	if (CU->difEdge != NULL) { delete[]CU->difEdge; CU->difEdge = NULL; }
	if (CU->ForbiddenLine != NULL) { delete[]CU->ForbiddenLine; CU->ForbiddenLine = NULL; }
	if (CU->runLength != NULL) { delete[]CU->runLength; CU->runLength = NULL; }
	for (int childIndex = 0; childIndex < 4; childIndex++)
		CU->child[childIndex] = NULL;
	delete element;
}

void CodingUnit::create(unsigned char* pImg, int h, int w, int CU_h, int CU_w, int loc_row, int loc_col, int dep)
{
	m_height = CU_h;
	m_width = CU_w;
	m_depth = dep;
	location_row = loc_row;
	location_col = loc_col;
	pic_width = w;
	pic_height = h;
	rate = 0;
	context = 0;
	element = new AC;
	m_value = new unsigned char[CU_h*CU_w];
	for (int i = 0; i < CU_h; i++)
	{
		for (int j = 0; j < CU_w; j++)
		{
			m_value[i * CU_w + j] = pImg[(i + loc_row)*w + j+ loc_col];
		}
	}
	for (int i = loc_row; i < loc_row + CU_h; i++)
	{
		for (int j = loc_col; j < loc_col + CU_w; j++)
		{
			depthMap[i * w + j] = dep;
		}
	}

	pImage = pImg;
	split_flag = false;
	oneColor = false;
	contiEdge = false;
	for (int childIndex = 0; childIndex < 4; childIndex++)
		child[childIndex] = NULL;
	Edge = NULL;
	difEdge = NULL;
	runLength = NULL;
}

void CodingUnit::create(unsigned char* pImg, int h, int w, int CU_h, int CU_w, int loc_row, int loc_col, int dep, int max_cu)
{
	pImage = pImg;
	m_height = CU_h;
	m_width = CU_w;
	m_depth = dep;
	location_row = loc_row;
	location_col = loc_col;
	pic_width = w;
	pic_height = h;
	context = 0;
	split_flag = false;
	for (int childIndex = 0; childIndex < 4; childIndex++)
		child[childIndex] = NULL;
	m_value = NULL;
	Edge = NULL;
	difEdge = NULL;
	runLength = NULL;
	for (int i = loc_row; i < loc_row + CU_h; i++)
	{
		for (int j = loc_col; j < loc_col + CU_w; j++)
		{
			depthMap[i * w + j] = dep;
		}
	}
}

int CodingUnit::getWidth()
{
	return m_width;
}

int CodingUnit::getHeight()
{
	return m_height;
}

int CodingUnit::getDepth()
{
	return m_depth;
}

unsigned char * CodingUnit::getValue()
{
	return m_value;
}

unsigned char * CodingUnit::getImg()
{
	return pImage;
}

int CodingUnit::getColorValue()
{
	return colorValue;
}

void CodingUnit::setColorValue(int color)
{
	colorValue = color;
}

int * CodingUnit::getEdge()
{
	return Edge;
}

int CodingUnit::getEdgeLength()
{
	return edgeLength;
}

void get_context(unsigned char* block, int location_row, int location_col, int width, int block_side, int initial_y, int initial_x, int color2, int* edgeline, int* context, int* mode)
{
	unsigned char* ref = block + location_row * width + location_col;
	int line[128 * 128];
	int length = 0;
	int x, y;
	if (initial_x == 0)
	{
		ref -= block_side;
		line[0] = 0;
		x = block_side;
		y = initial_y;
		while (!((x == 0) || (x == block_side + 1) || (y == 0) || (y == block_side)))
		{
			switch (line[length])
			{
			case 0:
				if (ref[y * width + x - 1] != color2) line[++length] = 1;
				else if (ref[(y - 1) * width + x - 1] != color2) line[++length] = 0;
				else line[++length] = 3;
				break;
			case 1:
				if (x == block_side)
				{
					if (ref[y * width + x - 1] != color2) line[++length] = 1;
					else line[++length] = 0;
				}
				else
				{
					if (ref[y * width + x] != color2) line[++length] = 2;
					else if (ref[y * width + x - 1] != color2) line[++length] = 1;
					else line[++length] = 0;
				}
				break;
			case 2:
				if (ref[(y - 1) * width + x] != color2) line[++length] = 3;
				else if (ref[y * width + x] != color2) line[++length] = 2;
				else line[++length] = 1;
				break;
			case 3:
				if (x == block_side)
				{
					if (ref[(y - 1) * width + x - 1] != color2) line[++length] = 0;
					else line[++length] = 3;
				}
				else
				{
					if (ref[(y - 1) * width + x - 1] != color2) line[++length] = 0;
					else if (ref[(y - 1) * width + x] != color2) line[++length] = 3;
					else line[++length] = 2;
				}
			}
			switch (line[length])
			{
			case 0: x--; break;
			case 1: y++; break;
			case 2: x++; if (x == block_side) x++; break;
			case 3: y--;
			}
		}
	}
	else if (initial_y == 0)
	{
		ref -= block_side* width;
		line[0] = 3;
		x = initial_x;
		y = block_side;
		while (!((x == 0) || (x == block_side) || (y == 0) || (y == block_side + 1)))
		{
			switch (line[length])
			{
			case 0:
				if (y == block_side)
				{
					if (ref[(y - 1) * width + x - 1] != color2) line[++length] = 3;
					else line[++length] = 0;
				}
				else
				{
					if (ref[(y - 1) * width + x - 1] != color2) line[++length] = 3;
					else if (ref[y * width + x - 1] != color2) line[++length] = 0;
					else line[++length] = 1;
				}
				break;
			case 1:
				if (ref[y * width + x - 1] != color2) line[++length] = 0;
				else if (ref[y * width + x] != color2) line[++length] = 1;
				else line[++length] = 2;
				break;
			case 2:
				if (y == block_side)
				{
					if (ref[(y - 1) * width + x] != color2) line[++length] = 2;
					else line[++length] = 3;
				}
				else
				{
					if (ref[y * width + x] != color2) line[++length] = 1;
					else if (ref[(y - 1) * width + x] != color2) line[++length] = 2;
					else line[++length] = 3;
				}
				break;
			case 3:
				if (ref[(y - 1) * width + x] != color2) line[++length] = 2;
				else if (ref[(y - 1) * width + x - 1] != color2) line[++length] = 3;
				else line[++length] = 0;
			}
			switch (line[length])
			{
			case 0: x--; break;
			case 1: y++; if (y == block_side) y++; break;
			case 2: x++; break;
			case 3: y--;
			}
		}
	}
	if (x == 0 || y == block_side) *mode = 0;
	else *mode = 1;
	for (int i = length; i > 0; i--)
	{
		if (line[i - 1] - line[i] == 1 || line[i - 1] - line[i] == -3)  //left
		{
			if (*mode == 0) line[i] = 2;
			if (*mode == 1)
			{
				*mode = 0;
				line[i] = 1;
			}
		}
		else if (line[i - 1] - line[i] == -1 || line[i - 1] - line[i] == 3) //right
		{
			if (*mode == 1) line[i] = 2;
			if (*mode == 0)
			{
				*mode = 1;
				line[i] = 1;
			}
		}
		else
		{
			line[i] = 0;
		}
	}
	if (length - 1 > 3) *context = 3;
	else *context = length - 1;
	for (int i = 0; i < *context; i++)
	{
		edgeline[i] = line[*context - i];
	}
}

void CodingUnit::setEdge()
{
	int color1 = m_value[0];
	int color2;
	int x = 0, y = 0;
	bool findColor2 = false;
	int i, j;
	int edgeline[128*128];
	int forbiddenline[128 * 128];
	unsigned char* ChainPath = new unsigned char [(m_height + 1) * (m_width + 1)];
	for (int i = 0; i < (m_height + 1) * (m_width + 1); i++) ChainPath[i] = 0;

	for (i = 1; i < m_height; i++)     
	{                                     
		if (m_value[i*m_width] != color1)           
		{                              
			color2 = m_value[i*m_width];
			findColor2 = true; y = i; x = 0;
			break;
		}
	}
	if (!findColor2)
	{
		for (j = 1; j < m_width; j++)
		{
			if (m_value[j] != color1)
			{
				color2 = m_value[j];
				findColor2 = true; y = 0; x = j;
				break;
			}
		}
	}
	if (!findColor2)
	{
		for (j = 1; j < m_width; j++)
		{
			if (m_value[m_width*(m_height - 1) + j] != color1)
			{
				color2 = m_value[m_width*(m_height - 1) + j];
				findColor2 = true; y = m_height - 1; x = j;
				break;
			}
		}
	}
	if (!findColor2)
	{
		for (i = 1; i < m_height - 1; i++)
		{
			if (m_value[i*m_width + m_width - 1] != color1)
			{
				color2 = m_value[i*m_width + m_width - 1];
				findColor2 = true; y = i; x = m_width - 1;
				break;
			}
		}
	}
	int initial_y = y, initial_x = x;
	edgeLength = 0;
	if (m_height >= 4)  //3OT
	{
		mode = 1;
		if (initial_x == 0 || initial_y == m_height - 1) mode = 0;
		if ((initial_x == 0 && location_col > 0) || (initial_y == 0 && location_row > 0))
		{
			get_context(getImg(), location_row, location_col, pic_width, m_width, initial_y, initial_x, color2, edgeline, &context, &mode);
		}
		edgeLength = context;
		int mode_item = mode;

		if (initial_x == 0)
		{
			edgeline[edgeLength] = 0; x++;
		}
		else if (initial_y == 0)
		{
			edgeline[edgeLength] = 3; y++;
		}
		else if (initial_y == m_height - 1)
		{
			edgeline[edgeLength] = 1;
		}
		else
		{
			edgeline[edgeLength] = 2;
		}
		while (!((x == m_width) || (y == 0) || (x == 0) || (y == m_height)))
		{
			forbiddenline[edgeLength] = -1;
			if (initial_x == 0)
			{
				if (x == 1 && y < initial_y)
				{
					if (edgeline[edgeLength] == 1) forbiddenline[edgeLength] = 2;
					if (edgeline[edgeLength] == 2) forbiddenline[edgeLength] = 4;
				}
			}
			else if (initial_y == 0)
			{
				if (x == 1)
				{
					if (edgeline[edgeLength] == 2) forbiddenline[edgeLength] = 4;
					if (edgeline[edgeLength] == 3) forbiddenline[edgeLength] = 2;
				}
				else if (x < initial_x && y == 1)
				{
					if (edgeline[edgeLength] == 1) forbiddenline[edgeLength] = 4;
					if (edgeline[edgeLength] == 2) forbiddenline[edgeLength] = 1;
				}
			}
			else if (initial_y == m_height - 1)
			{
				if (x == 1)
				{
					if (edgeline[edgeLength] == 1) forbiddenline[edgeLength] = 2;
					if (edgeline[edgeLength] == 2) forbiddenline[edgeLength] = 4;
				}
				else if (y == 1)
				{
					if (edgeline[edgeLength] == 0) forbiddenline[edgeLength] = 1;
					if (edgeline[edgeLength] == 1) forbiddenline[edgeLength] = 4;
				}
				else if (x < initial_x && y == m_height - 1)
				{
					if (edgeline[edgeLength] == 2) forbiddenline[edgeLength] = 3;
					if (edgeline[edgeLength] == 3) forbiddenline[edgeLength] = 4;
				}
			}
			else
			{
				if (x == 1)
				{
					if (edgeline[edgeLength] == 2) forbiddenline[edgeLength] = 4;
					if (edgeline[edgeLength] == 3) forbiddenline[edgeLength] = 2;
				}
				else if (y == 1)
				{
					if (edgeline[edgeLength] == 1) forbiddenline[edgeLength] = 4;
					if (edgeline[edgeLength] == 2) forbiddenline[edgeLength] = 1;
				}
				else if (y == m_height - 1)
				{
					if (edgeline[edgeLength] == 0) forbiddenline[edgeLength] = 3;
					if (edgeline[edgeLength] == 3) forbiddenline[edgeLength] = 4;
				}
				else if (x == m_width - 1 && y < initial_y)
				{
					if (edgeline[edgeLength] == 0) forbiddenline[edgeLength] = 4;
					if (edgeline[edgeLength] == 1) forbiddenline[edgeLength] = 0;
				}
			}

			ChainPath[y * (m_width + 1) + x] = 1;
			int forbidden1, forbidden2;
			for (int i = 0; i < 4; i++)
			{
				if (i == 2) continue;
				else if (i == 0) { forbidden1 = (edgeline[edgeLength] + 1) % 4; forbidden2 = (edgeline[edgeLength] + 3) % 4; }
				else if (i == 1) { forbidden1 = edgeline[edgeLength]; forbidden2 = (edgeline[edgeLength] + 3) % 4; }
				else { forbidden1 = edgeline[edgeLength]; forbidden2 = (edgeline[edgeLength] + 1) % 4; }
				switch ((edgeline[edgeLength] + i) % 4)
				{
				case 0: if (ChainPath[y * (m_width + 1) + x + 1] == 1) { if (forbiddenline[edgeLength] == forbidden1 || forbiddenline[edgeLength] == forbidden2) forbiddenline[edgeLength] = 4; else if (forbiddenline[edgeLength] != 4) forbiddenline[edgeLength] = 0; } break;
				case 1: if (ChainPath[(y - 1) * (m_width + 1) + x] == 1) { if (forbiddenline[edgeLength] == forbidden1 || forbiddenline[edgeLength] == forbidden2) forbiddenline[edgeLength] = 4; else if (forbiddenline[edgeLength] != 4) forbiddenline[edgeLength] = 1; }break;
				case 2: if (ChainPath[y * (m_width + 1) + x - 1] == 1) { if (forbiddenline[edgeLength] == forbidden1 || forbiddenline[edgeLength] == forbidden2) forbiddenline[edgeLength] = 4; else if (forbiddenline[edgeLength] != 4) forbiddenline[edgeLength] = 2; }break;
				case 3: if (ChainPath[(y + 1) * (m_width + 1) + x] == 1) { if (forbiddenline[edgeLength] == forbidden1 || forbiddenline[edgeLength] == forbidden2) forbiddenline[edgeLength] = 4; else if (forbiddenline[edgeLength] != 4) forbiddenline[edgeLength] = 3; }
				}
			}

			switch (edgeline[edgeLength])
			{
			case 0:
				if (initial_x == 0 || initial_y == m_height - 1)
				{
					if (m_value[y * m_height + x] == color1) edgeline[++edgeLength] = 3;
					else if (m_value[(y - 1) * m_height + x] == color1) edgeline[++edgeLength] = 0;
					else edgeline[++edgeLength] = 1;
				}
				else
				{
					if (m_value[(y - 1) * m_height + x] == color1) edgeline[++edgeLength] = 1;
					else if (m_value[y * m_height + x] == color1) edgeline[++edgeLength] = 0;
					else edgeline[++edgeLength] = 3;
				}
				break;
			case 1:
				if (initial_x == 0 || initial_y == m_height - 1)
				{
					if (m_value[(y - 1) * m_height + x] == color1) edgeline[++edgeLength] = 0;
					else if (m_value[(y - 1) * m_height + x - 1] == color1) edgeline[++edgeLength] = 1;
					else edgeline[++edgeLength] = 2;
				}
				else
				{
					if (m_value[(y - 1) * m_height + x - 1] == color1) edgeline[++edgeLength] = 2;
					else if (m_value[(y - 1) * m_height + x] == color1) edgeline[++edgeLength] = 1;
					else edgeline[++edgeLength] = 0;
				}
				break;
			case 2:
				if (initial_x == 0 || initial_y == m_height - 1)
				{
					if (m_value[(y - 1) * m_height + x - 1] == color1) edgeline[++edgeLength] = 1;
					else if (m_value[y * m_height + x - 1] == color1) edgeline[++edgeLength] = 2;
					else edgeline[++edgeLength] = 3;
				}
				else
				{
					if (m_value[y * m_height + x - 1] == color1) edgeline[++edgeLength] = 3;
					else if (m_value[(y - 1) * m_height + x - 1] == color1) edgeline[++edgeLength] = 2;
					else edgeline[++edgeLength] = 1;
				}
				break;
			case 3:
				if (initial_x == 0 || initial_y == m_height - 1)
				{
					if (m_value[y * m_height + x - 1] == color1) edgeline[++edgeLength] = 2;
					else if (m_value[y * m_height + x] == color1) edgeline[++edgeLength] = 3;
					else edgeline[++edgeLength] = 0;
				}
				else
				{
					if (m_value[y * m_height + x] == color1) edgeline[++edgeLength] = 0;
					else if (m_value[y * m_height + x - 1] == color1) edgeline[++edgeLength] = 3;
					else edgeline[++edgeLength] = 2;
				}
			}
			switch (edgeline[edgeLength])
			{
			case 0: x++; break;
			case 1: y--; break;
			case 2: x--; break;
			case 3: y++;
			}
		}

		for (int i = context; i < edgeLength; i++)
		{
			if (forbiddenline[i] >= 0 && forbiddenline[i] < 4)
			{
				if (forbiddenline[i] - edgeline[i] == 1 || forbiddenline[i] - edgeline[i] == -3)  //left
				{
					if (mode_item == 0) forbiddenline[i] = 2;
					if (mode_item == 1) forbiddenline[i] = 1;
				}
				else if (forbiddenline[i] - edgeline[i] == -1 || forbiddenline[i] - edgeline[i] == 3) //right
				{
					if (mode_item == 1) forbiddenline[i] = 2;
					if (mode_item == 0) forbiddenline[i] = 1;
				}
				else
				{
					forbiddenline[i] = 0;
				}
			}


			if (edgeline[i + 1] - edgeline[i] == 1 || edgeline[i + 1] - edgeline[i] == -3)  //left
			{
				if (mode_item == 0) edgeline[i] = 2;
				if (mode_item == 1)
				{
					mode_item = 0;
					edgeline[i] = 1;
				}
			}
			else if (edgeline[i + 1] - edgeline[i] == -1 || edgeline[i + 1] - edgeline[i] == 3) //right
			{
				if (mode_item == 1) edgeline[i] = 2;
				if (mode_item == 0)
				{
					mode_item = 1;
					edgeline[i] = 1;
				}
			}
			else
			{
				edgeline[i] = 0;
			}
		}
	}

	Edge = new int[edgeLength + 5];
	Edge[0] = color1;
	Edge[1] = initial_y;
	Edge[2] = initial_x;
	Edge[3] = color2;
    Edge[4] = edgeLength;
	for (i = 0; i < edgeLength; i++)
		Edge[5 + i] = edgeline[i];

	ForbiddenLine = new int[edgeLength];
	for (i = context; i < edgeLength; i++)
		ForbiddenLine[i] = forbiddenline[i];

	delete[] ChainPath;
}

int power(int a, int b)
{
	if (b == 0) return 1;
	int output = a;
	for (int i = 0; i < b - 1; i++)
		output *= a;
	return output;
}

void CodingUnit::setRunLength()
{
	if (m_height == 2)
	{
		runLength = new int[4];
		for (int i = 0; i < 4; i++)
			runLength[i] = m_value[i];
	}
}

int * CodingUnit::getRunLength()
{
	return runLength;
}

void CodingUnit::enSplitFlag(bool encode)
{
  int index;
  int splitFlag = split_flag;
  if (location_col > 0 && depthMap[location_row*pic_width + location_col - 1] > m_depth) index = 1;
  else index = 0;
  if (location_row > 0 && depthMap[(location_row - 1)*pic_width + location_col] > m_depth) index++;
  if (encode)
  {
	  switch (index)
	  {
	  case 0:acodec.encode(splitFlag, aSplitFlag1); break;
	  case 1:acodec.encode(splitFlag, aSplitFlag2); break;
	  case 2:acodec.encode(splitFlag, aSplitFlag3); break;
	  }
  }
  else
  {
	  switch (index)
	  {
	  case 0:this->update(element->aSplitFlag1, splitFlag); break;
	  case 1:this->update(element->aSplitFlag2, splitFlag); break;
	  case 2:this->update(element->aSplitFlag3, splitFlag); break;
	  }
  }
}

void CodingUnit::enOneColor(bool encode)
{
	int color1, color2;
	int predict = predictColor1(this, &color1, &color2);   //predict the color
	if (encode)
	{
		if (predict > 0)
		{
			if (color1 == colorValue)
			{
				acodec.encode(1, aPredictColor1Flag);
			}
			else
			{
				acodec.encode(0, aPredictColor1Flag);
				if (!g_biValue)
				{
					if (predict == 2)
					{
						if (color2 == colorValue) acodec.encode(1, aPredictColor1Flag2);
						else
						{
							acodec.encode(0, aPredictColor1Flag2);
							if (numC != 3)
							{
								Adaptive_Data_Model Color(numC - 2);
								Color.copy(aColor, getColorList(color1, colorList), getColorList(color2, colorList));
								int index = get_index(getColorList(colorValue, colorList), true, getColorList(color1, colorList), getColorList(color2, colorList));
								acodec.encode(index, Color);
								acodec.noencode(getColorList(colorValue, colorList), aColor);
							}
						}
					}
					else
					{
						Adaptive_Data_Model Color(numC - 1);
						Color.copy(aColor, getColorList(color1, colorList));
						int index = get_index(getColorList(colorValue, colorList), true, getColorList(color1, colorList));
						acodec.encode(index, Color);
						acodec.noencode(getColorList(colorValue, colorList),aColor);
					}
				}
			}
		}
		else if(predict < 0)
		{
			acodec.encode(getColorList(colorValue, colorList), aColor);
		}
		else
		{
			if(ref_frame) acodec.encode(getColorList(colorValue, colorList), aColor);
		}
		adjustColorList(colorValue, colorList);
	}
	else
	{
		if (predict > 0)
		{
			if (color1 == colorValue)
			{
				this->update(element->aPredictColor1Flag, 1);
			}
			else
			{
				this->update(element->aPredictColor1Flag, 0);
				if (!g_biValue)
				{
					if (predict == 2)
					{
						if (color2 == colorValue) this->update(element->aPredictColor1Flag2, 1);
						else
						{
							this->update(element->aPredictColor1Flag2, 0);
							if(numC != 3) this->update(element->aColor, getColorList(colorValue, element->colorList), getColorList(color1, element->colorList), getColorList(color2, element->colorList));
						}
					}
					else this->update(element->aColor, getColorList(colorValue, element->colorList), getColorList(color1, element->colorList));
				}
			}
		}
		else if(predict < 0)
		{
			this->update(element->aColor, getColorList(colorValue, element->colorList));
		}
		else
		{
			if (ref_frame) this->update(element->aColor, getColorList(colorValue, element->colorList));
		}
		adjustColorList(colorValue, element->colorList);
	}
}

void get_predict_position(int block_size, int* pos1, int* pos2, int* pos3, int predict, int* num)
{
	if (predict == 1)
	{
		*pos1 = 1;
		*pos2 = 2;
		*num = 2;
	}
	else if (predict == block_size - 1)
	{
		*pos1 = block_size - 1;
		*pos2 = block_size - 2;
		*num = 2;
	}
	else
	{
		*pos1 = predict;
		*pos2 = predict + 1;
		*pos3 = predict - 1;
		*num = 3;
	}
}

int get_position(int position, int pos1, int pos2, int pos3, int block_size)
{
	int index = 0;
	for (int i = 1; i < block_size; i++)
	{
		if (i == position) return index;
		if (i == pos1 || i == pos2 || i == pos3) continue;
		index++;
	}

}

void CodingUnit::enChainCoding(bool encode)
{
	if (encode)
	{
		int color1, color2;
		int predict = predictColor1(this, &color1, &color2);
		if (predict > 0)
		{
			if (color1 == Edge[0])
			{
				acodec.encode(1, aPredictColor1Flag);
			}
			else
			{
				acodec.encode(0, aPredictColor1Flag);
				if (!g_biValue)
				{
					if (predict == 2)
					{
						if (color2 == Edge[0]) acodec.encode(1, aPredictColor1Flag2);
						else
						{
							acodec.encode(0, aPredictColor1Flag2);
							if (numC != 3)
							{
								Adaptive_Data_Model Color(numC - 2);
								Color.copy(aColor, getColorList(color1, colorList), getColorList(color2, colorList));
								int index = get_index(getColorList(Edge[0], colorList), true, getColorList(color1, colorList), getColorList(color2, colorList));
								acodec.encode(index, Color);
								acodec.noencode(getColorList(Edge[0], colorList), aColor);
							}
						}
					}
					else
					{
						Adaptive_Data_Model Color(numC - 1);
						Color.copy(aColor, getColorList(color1, colorList));
						int index = get_index(getColorList(Edge[0], colorList), true, getColorList(color1, colorList));
						acodec.encode(index, Color);
						acodec.noencode(getColorList(Edge[0], colorList), aColor);
					}
				}
			}
		}
		else if (predict < 0) acodec.encode(getColorList(Edge[0], colorList), aColor);
		else
		{
			if (ref_frame) acodec.encode(getColorList(Edge[0], colorList), aColor);
		}
		adjustColorList(Edge[0], colorList);

		color1 = Edge[0];

		if (m_height == 2 && m_width == 2)   //if CU height equal to 2, encode the initial position directly.    ChainPositionMode2x2
		{
			if (m_value[1] == Edge[3] && m_value[2] == Edge[0] && m_value[3] == Edge[0]) acodec.encode(0, aChainPositionMode2x2);
			if (m_value[1] == Edge[0] && m_value[2] == Edge[3] && m_value[3] == Edge[0]) acodec.encode(1, aChainPositionMode2x2);
			if (m_value[1] == Edge[0] && m_value[2] == Edge[0] && m_value[3] == Edge[3]) acodec.encode(2, aChainPositionMode2x2);
			if (m_value[1] == Edge[0] && m_value[2] == Edge[3] && m_value[3] == Edge[3]) acodec.encode(3, aChainPositionMode2x2);
			if (m_value[1] == Edge[3] && m_value[2] == Edge[0] && m_value[3] == Edge[3]) acodec.encode(4, aChainPositionMode2x2);
			if (m_value[1] == Edge[3] && m_value[2] == Edge[3] && m_value[3] == Edge[3]) acodec.encode(5, aChainPositionMode2x2);
		}
		else
		{
			int ini_y, ini_x, ini_y2, ini_x2, predictMode1 = -2, predictMode2 = -2;
			int predict = predictInitialPosition(this, &ini_y, &ini_x, &ini_y2, &ini_x2, color1, &predictMode1, &predictMode2);      //predict the initial position.
			int x1 = -1, x2 = -1, x3 = -1, y1 = -1, y2 = -1, y3 = -1, num0 = 0, num1 = 0;
			if (predict == 0 || predict == 2) get_predict_position(m_height, &y1, &y2, &y3, ini_y, &num0);
			if (predict == 1 || predict == 2) get_predict_position(m_height, &x1, &x2, &x3, ini_x2, &num1);
			int CUhSize = getBitSize(m_height);
			int CUwSize = getBitSize(m_width);
			bool predictTrue = 0;
			if (predict == 1)
			{
				ini_y = ini_y2;
				ini_x = ini_x2;
			}
			if (predict >= 0)
			{
				if (predict == 2)
				{
					if (Edge[1] == 0 && abs(ini_x2 - Edge[2]) <= 1)
					{
						ini_y = ini_y2;
						ini_x = ini_x2;
					}
				}
				if (ini_y == 0)
				{
					if (Edge[1] != 0)
					{
						acodec.encode(0, aPredictPositionFlag);        //predict wrong
					}
					else
					{
						if (ini_x == 1 || ini_x == m_width - 1)
						{
							if (ini_x == Edge[2])
							{
								acodec.encode(1, aPredictPositionFlag);
								if (predict == 2) acodec.encode(1, aPredictPositionSide);
								acodec.encode(0, aPredictPositionBorder); predictTrue = 1;
							}
							else if ((ini_x == 1 && Edge[2] == 2) || (ini_x == m_width - 1 && Edge[2] == m_width - 2))
							{
								acodec.encode(1, aPredictPositionFlag);
								if (predict == 2) acodec.encode(1, aPredictPositionSide);
								acodec.encode(1, aPredictPositionBorder); predictTrue = 1;
							}
							else
							{
								acodec.encode(0, aPredictPositionFlag);
							}
						}
						else
						{
							if (ini_x == Edge[2])
							{
								acodec.encode(1, aPredictPositionFlag);      //predict right
								if (predict == 2) acodec.encode(1, aPredictPositionSide);
								acodec.encode(0, aPredictPositionMode);
								predictTrue = 1;
							}
							else if (ini_x + 1 == Edge[2])
							{
								acodec.encode(1, aPredictPositionFlag);
								if (predict == 2) acodec.encode(1, aPredictPositionSide);
								acodec.encode(1, aPredictPositionMode);
								predictTrue = 1;
							}
							else if (ini_x - 1 == Edge[2])
							{
								acodec.encode(1, aPredictPositionFlag);
								if (predict == 2) acodec.encode(1, aPredictPositionSide);
								acodec.encode(2, aPredictPositionMode);
								predictTrue = 1;
							}
							else
							{
								acodec.encode(0, aPredictPositionFlag);
							}
						}
					}
				}
				else
				{
					if (Edge[2] != 0)
					{
						acodec.encode(0, aPredictPositionFlag);
					}
					else
					{
						if (ini_y == 1 || ini_y == m_height - 1)
						{
							if (ini_y == Edge[1])
							{
								acodec.encode(1, aPredictPositionFlag);
								if (predict == 2) acodec.encode(0, aPredictPositionSide);
								acodec.encode(0, aPredictPositionBorder); predictTrue = 1;
							}
							else if ((ini_y == 1 && Edge[1] == 2) || (ini_y == m_height - 1 && Edge[1] == m_height - 2))
							{
								acodec.encode(1, aPredictPositionFlag);
								if (predict == 2) acodec.encode(0, aPredictPositionSide);
								acodec.encode(1, aPredictPositionBorder); predictTrue = 1;
							}
							else
							{
								acodec.encode(0, aPredictPositionFlag);
							}
						}
						else
						{
							if (ini_y == Edge[1])
							{
								acodec.encode(1, aPredictPositionFlag);
								if (predict == 2) acodec.encode(0, aPredictPositionSide);
								acodec.encode(0, aPredictPositionMode);
								predictTrue = 1;
							}
							else if (ini_y + 1 == Edge[1])
							{
								acodec.encode(1, aPredictPositionFlag);
								if (predict == 2) acodec.encode(0, aPredictPositionSide);
								acodec.encode(1, aPredictPositionMode);
								predictTrue = 1;
							}
							else if (ini_y - 1 == Edge[1])
							{
								acodec.encode(1, aPredictPositionFlag);
								if (predict == 2) acodec.encode(0, aPredictPositionSide);
								acodec.encode(2, aPredictPositionMode);
								predictTrue = 1;
							}
							else
							{
								acodec.encode(0, aPredictPositionFlag);
							}
						}
					}
				}
			}
			if (!predictTrue)         //if redict wrong or not predict, encode the initial position.
			{
				if (Edge[2] == 0)            //because the initial position is on the boundary, so it is enough to encode a side mode and the one position
				{
					if (m_height == 4 && num1 == 3)	acodec.encode(0, aSideMode, 1); else acodec.encode(0, aSideMode);
					if (!((predict == 0 || predict == 2) && m_height == 4))
					{
						if (predictMode1 == 1 && y1 >= m_height / 2 && y1 < m_height - 2)
						{
							if (Edge[1] > y1)
							{
								acodec.encode(1, aPredictPosition);
								int num = m_height - y1 - 2;
								if (num > 1)
								{
									Static_Data_Model Position(num);
									acodec.encode(Edge[1] - y1 - 2, Position);
								}
							}
							else
							{
								acodec.encode(0, aPredictPosition);
								int num = y1 - 2;
								if (num > 1)
								{
									Static_Data_Model Position(num);
									acodec.encode(y1 - Edge[1] - 2, Position);
								}
							}
						}
						else if (predictMode1 == -1 && y1 <= m_height / 2 && y1 > 2)
						{
							if (Edge[1] > y1)
							{
								acodec.encode(0, aPredictPosition);
								int num = m_height - y1 - 2;
								if (num > 1)
								{
									Static_Data_Model Position(num);
									acodec.encode(Edge[1] - y1 - 2, Position);
								}
							}
							else
							{
								acodec.encode(1, aPredictPosition);
								int num = y1 - 2;
								if (num > 1)
								{
									Static_Data_Model Position(num);
									acodec.encode(y1 - Edge[1] - 2, Position);
								}
							}
						}
						else
						{
							Static_Data_Model Position(m_height - 1 - num0);
							int index = get_position(Edge[1], y1, y2, y3, m_height);
							acodec.encode(index, Position);
						}
					}
				}
				else if (Edge[1] == 0)
				{
					if (m_height == 4 && num0 == 3)	acodec.encode(1, aSideMode, 0); else acodec.encode(1, aSideMode);
					if (!((predict == 1 || predict == 2) && m_height == 4))
					{
						if (predictMode2 == 1 && x1 >= m_height / 2 && x1 < m_height - 2)
						{
							if (Edge[2] > x1)
							{
								acodec.encode(1, aPredictPosition);
								int num = m_height - x1 - 2;
								if (num > 1)
								{
									Static_Data_Model Position(num);
									acodec.encode(Edge[2] - x1 - 2, Position);
								}
							}
							else
							{
								acodec.encode(0, aPredictPosition);
								int num = x1 - 2;
								if (num > 1)
								{
									Static_Data_Model Position(num);
									acodec.encode(x1 - Edge[2] - 2, Position);
								}
							}
						}
						else if (predictMode2 == -1 && x1 <= m_height / 2 && x1 > 2)
						{
							if (Edge[2] > x1)
							{
								acodec.encode(0, aPredictPosition);
								int num = m_height - x1 - 2;
								if (num > 1)
								{
									Static_Data_Model Position(num);
									acodec.encode(Edge[2] - x1 - 2, Position);
								}
							}
							else
							{
								acodec.encode(1, aPredictPosition);
								int num = x1 - 2;
								if (num > 1)
								{
									Static_Data_Model Position(num);
									acodec.encode(x1 - Edge[2] - 2, Position);
								}
							}
						}
						else
						{
							Static_Data_Model Position(m_height - 1 - num1);
							int index = get_position(Edge[2], x1, x2, x3, m_height);
							acodec.encode(index, Position);
						}
					}
				}
				else if (Edge[1] == m_height - 1)
				{
					if (m_height == 4 && num0 == 3 && num1 != 3) acodec.encode(2, aSideMode, 0);
					else if (m_height == 4 && num0 != 3 && num1 == 3) acodec.encode(2, aSideMode, 1);
					else if (m_height == 4 && num0 == 3 && num1 == 3)
					{
						Adaptive_Data_Model Side(2);
						Side.copy(aSideMode, 0, 1);
						acodec.encode(0, Side);
						acodec.noencode(2, aSideMode);
					}
					else acodec.encode(2, aSideMode);
					Static_Data_Model Position(m_height - 1);
					acodec.encode(Edge[2] - 1, Position);
				}
				else
				{
					if (m_height == 4 && num0 == 3 && num1 != 3) acodec.encode(3, aSideMode, 0);
					else if (m_height == 4 && num0 != 3 && num1 == 3) acodec.encode(3, aSideMode, 1);
					else if (m_height == 4 && num0 == 3 && num1 == 3)
					{
						Adaptive_Data_Model Side(2);
						Side.copy(aSideMode, 0, 1);
						acodec.encode(1, Side);
						acodec.noencode(3, aSideMode);
					}
					else acodec.encode(3, aSideMode);
					Static_Data_Model Position(m_height - 2);
					acodec.encode(Edge[1] - 1, Position);
				}
			}
		}
		if (!g_biValue)
		{
			int color2 = predictColor2(this, Edge[1], Edge[2], Edge[0]);        //predict second color
			if (color2 >= 0)
			{
				if (color2 == Edge[3])
				{
					acodec.encode(1, aPredictColor2Flag);

				}
				else
				{
					acodec.encode(0, aPredictColor2Flag);
					if (numC != 3)
					{
						Adaptive_Data_Model Color(numC - 2);
						Color.copy(aColor, getColorList(color1, colorList), getColorList(color2, colorList));
						int index = get_index(getColorList(Edge[3], colorList), true, getColorList(color1, colorList), getColorList(color2, colorList));
						acodec.encode(index, Color);
						acodec.noencode(getColorList(Edge[3], colorList), aColor);
					}
				}
			}
			else
			{
				Adaptive_Data_Model Color(numC - 1);
				Color.copy(aColor, getColorList(color1, colorList));
				int index = get_index(getColorList(Edge[3], colorList), true, getColorList(color1, colorList));
				acodec.encode(index, Color);
				acodec.noencode(getColorList(Edge[3], colorList), aColor);
			}
		}
		adjustColorList(Edge[3], colorList);
	}
	else
	{
	int color1, color2;
	int predict = predictColor1(this, &color1, &color2);
	if (predict > 0)
	{
		if (color1 == Edge[0])
		{
			this->update(element->aPredictColor1Flag, 1);
		}
		else
		{
			this->update(element->aPredictColor1Flag, 0);
			if (!g_biValue)
			{
				if (predict == 2)
				{
					if (color2 == Edge[0]) this->update(element->aPredictColor1Flag, 1);
					else
					{
						this->update(element->aPredictColor1Flag, 0);
						if (numC != 3) this->update(element->aColor, getColorList(Edge[0], element->colorList), getColorList(color1, element->colorList), getColorList(color2, element->colorList));
					}
				}
				else
				{
					this->update(element->aColor, getColorList(Edge[0], element->colorList), getColorList(color1, element->colorList));
				}
			}
		}
	}
	else if (predict < 0) this->update(element->aColor, getColorList(Edge[0], element->colorList));
	else
	{
		if (ref_frame) this->update(element->aColor, getColorList(Edge[0], element->colorList));
	}
	adjustColorList(Edge[0], element->colorList);

	color1 = Edge[0];

	if (m_height == 2 && m_width == 2)   //if CU height equal to 2, encode the initial position directly.    ChainPositionMode2x2
	{
		if (m_value[1] == Edge[3] && m_value[2] == Edge[0] && m_value[3] == Edge[0]) this->update(element->aChainPositionMode2x2, 0); 
		if (m_value[1] == Edge[0] && m_value[2] == Edge[3] && m_value[3] == Edge[0]) this->update(element->aChainPositionMode2x2, 1); 
		if (m_value[1] == Edge[0] && m_value[2] == Edge[0] && m_value[3] == Edge[3]) this->update(element->aChainPositionMode2x2, 2); 
		if (m_value[1] == Edge[0] && m_value[2] == Edge[3] && m_value[3] == Edge[3]) this->update(element->aChainPositionMode2x2, 3); 
		if (m_value[1] == Edge[3] && m_value[2] == Edge[0] && m_value[3] == Edge[3]) this->update(element->aChainPositionMode2x2, 4); 
		if (m_value[1] == Edge[3] && m_value[2] == Edge[3] && m_value[3] == Edge[3]) this->update(element->aChainPositionMode2x2, 5); 
	}
	else
	{
		int ini_y, ini_x, ini_y2, ini_x2, predictMode1 = -2, predictMode2 = -2;
		int predict = predictInitialPosition(this, &ini_y, &ini_x, &ini_y2, &ini_x2, color1, &predictMode1, &predictMode2);      //predict the initial position.
		int x1 = -1, x2 = -1, x3 = -1, y1 = -1, y2 = -1, y3 = -1, num0 = 0, num1 = 0;
		if (predict == 0 || predict == 2) get_predict_position(m_height, &y1, &y2, &y3, ini_y, &num0);
		if (predict == 1 || predict == 2) get_predict_position(m_height, &x1, &x2, &x3, ini_x2, &num1);
		int CUhSize = getBitSize(m_height);
		int CUwSize = getBitSize(m_width);
		bool predictTrue = 0;
		if (predict == 1)
		{
			ini_y = ini_y2;
			ini_x = ini_x2;
		}
		if (predict >= 0)
		{
			if (predict == 2)
			{
				if (Edge[1] == 0 && abs(ini_x2 - Edge[2]) <= 1)
				{
					ini_y = ini_y2;
					ini_x = ini_x2;
				}
			}
			if (ini_y == 0)
			{
				if (Edge[1] != 0)
				{
					this->update(element->aPredictPositionFlag, 0);       //predict wrong
				}
				else
				{
					if (ini_x == 1 || ini_x == m_width - 1)
					{
						if (ini_x == Edge[2])
						{
							this->update(element->aPredictPositionFlag, 1); 
							if (predict == 2) this->update(element->aPredictPositionSide, 1);
							this->update(element->aPredictPositionBorder, 0); predictTrue = 1;
						}
						else if ((ini_x == 1 && Edge[2] == 2) || (ini_x == m_width - 1 && Edge[2] == m_width - 2))
						{
							this->update(element->aPredictPositionFlag, 1); 
							if (predict == 2) this->update(element->aPredictPositionSide, 1);
							this->update(element->aPredictPositionBorder, 1); predictTrue = 1;
						}
						else
						{
							this->update(element->aPredictPositionFlag, 0);
						}
					}
					else
					{
						if (ini_x == Edge[2])
						{
							this->update(element->aPredictPositionFlag, 1);    //predict right
							if (predict == 2) this->update(element->aPredictPositionSide, 1);
							this->update(element->aPredictPositionMode, 0); 
							predictTrue = 1;
						}
						else if (ini_x + 1 == Edge[2])
						{
							this->update(element->aPredictPositionFlag, 1);
							if (predict == 2) this->update(element->aPredictPositionSide, 1);
							this->update(element->aPredictPositionMode, 1);
							predictTrue = 1;
						}
						else if (ini_x - 1 == Edge[2])
						{
							this->update(element->aPredictPositionFlag, 1);
							if (predict == 2) this->update(element->aPredictPositionSide, 1);
							this->update(element->aPredictPositionMode, 2);
							predictTrue = 1;
						}
						else
						{
							this->update(element->aPredictPositionFlag, 0);
						}
					}
				}
			}
			else
			{
				if (Edge[2] != 0)
				{
					this->update(element->aPredictPositionFlag, 0);
				}
				else
				{
					if (ini_y == 1 || ini_y == m_height - 1)
					{
						if (ini_y == Edge[1])
						{
							this->update(element->aPredictPositionFlag, 1);
							if (predict == 2) this->update(element->aPredictPositionSide, 0);
							this->update(element->aPredictPositionBorder, 0); predictTrue = 1;
						}
						else if ((ini_y == 1 && Edge[1] == 2) || (ini_y == m_height - 1 && Edge[1] == m_height - 2))
						{
							this->update(element->aPredictPositionFlag, 1);
							if (predict == 2) this->update(element->aPredictPositionSide, 0);
							this->update(element->aPredictPositionBorder, 1); predictTrue = 1;
						}
						else
						{
							this->update(element->aPredictPositionFlag, 0);
						}
					}
					else
					{
						if (ini_y == Edge[1])
						{
							this->update(element->aPredictPositionFlag, 1);
							if (predict == 2) this->update(element->aPredictPositionSide, 0);
							this->update(element->aPredictPositionMode, 0);
							predictTrue = 1;
						}
						else if (ini_y + 1 == Edge[1])
						{
							this->update(element->aPredictPositionFlag, 1);
							if (predict == 2) this->update(element->aPredictPositionSide, 0);
							this->update(element->aPredictPositionMode, 1);
							predictTrue = 1;
						}
						else if (ini_y - 1 == Edge[1])
						{
							this->update(element->aPredictPositionFlag, 1);
							if (predict == 2) this->update(element->aPredictPositionSide, 0);
							this->update(element->aPredictPositionMode, 2);
							predictTrue = 1;
						}
						else
						{
							this->update(element->aPredictPositionFlag, 0);
						}
					}
				}
			}
		}
		if (!predictTrue)         //if redict wrong or not predict, encode the initial position.
		{
			if (Edge[2] == 0)            //because the initial position is on the boundary, so it is enough to encode a side mode and the one position
			{
				if (m_height == 4 && num1 == 3) this->update(element->aSideMode, 0, 1); else this->update(element->aSideMode, 0);
				if (!((predict == 0 || predict == 2) && m_height == 4))
				{
					if (predictMode1 == 1 && y1 >= m_height / 2 && y1 < m_height - 2)
					{
						if (Edge[1] > y1)
						{
							this->update(element->aPredictPosition, 1);
							int num = m_height - y1 - 2;
							rate -= log2(1.0 / num);
						}
						else
						{
							this->update(element->aPredictPosition, 0);
							int num = y1 - 2;
							rate -= log2(1.0 / num);
						}
					}
					else if (predictMode1 == -1 && y1 <= m_height / 2 && y1 > 2)
					{
						if (Edge[1] > y1)
						{
							this->update(element->aPredictPosition, 0);
							int num = m_height - y1 - 2;
							rate -= log2(1.0 / num);
						}
						else
						{
							this->update(element->aPredictPosition, 1);
							int num = y1 - 2;
							rate -= log2(1.0 / num);
						}
					}
					else
					{
						rate -= log2(1.0 / (m_height - 1 - num0));
					}
				}
			}
			else if (Edge[1] == 0)
			{
				if (m_height == 4 && num0 == 3) this->update(element->aSideMode, 1, 0); else this->update(element->aSideMode, 1);
				if (!((predict == 1 || predict == 2) && m_width == 4))
				{
					if (predictMode2 == 1 && x1 >= m_height / 2 && x1 < m_height - 2)
					{
						if (Edge[2] > x1)
						{
							this->update(element->aPredictPosition, 1);
							int num = m_height - x1 - 2;
							rate -= log2(1.0 / num);
						}
						else
						{
							this->update(element->aPredictPosition, 0);
							int num = x1 - 2;
							rate -= log2(1.0 / num);
						}
					}
					else if (predictMode2 == -1 && x1 <= m_height / 2 && x1 > 2)
					{
						if (Edge[2] > x1)
						{
							this->update(element->aPredictPosition, 0);
							int num = m_height - x1 - 2;
							rate -= log2(1.0 / num);
						}
						else
						{
							this->update(element->aPredictPosition, 1);
							int num = x1 - 2;
							rate -= log2(1.0 / num);
						}
					}
					else
					{
						rate -= log2(1.0 / (m_height - 1 - num1));
					}
				}
			}
			else if (Edge[1] == m_height - 1)
			{
				if (m_height == 4 && num0 == 3 && num1 != 3) this->update(element->aSideMode, 2, 0);
				else if (m_height == 4 && num0 != 3 && num1 == 3) this->update(element->aSideMode, 2, 1);
				else if (m_height == 4 && num0 == 3 && num1 == 3) this->update(element->aSideMode, 2, 0, 1);
				else this->update(element->aSideMode, 2);
				rate -= log2(1.0 / (m_height - 1));
				
			}
			else
			{
				if (m_height == 4 && num0 == 3 && num1 != 3) this->update(element->aSideMode, 3, 0);
				else if (m_height == 4 && num0 != 3 && num1 == 3) this->update(element->aSideMode, 3, 1);
				else if (m_height == 4 && num0 == 3 && num1 == 3) this->update(element->aSideMode, 3, 0, 1);
				else this->update(element->aSideMode, 3);
				rate -= log2(1.0 / (m_height - 2));
				
			}
		}
	}
	if (!g_biValue)
	{
		int color2 = predictColor2(this, Edge[1], Edge[2], Edge[0]);        //predict second color
		if (color2 >= 0)
		{
			if (color2 == Edge[3])
			{
				this->update(element->aPredictColor2Flag, 1);

			}
			else
			{
				this->update(element->aPredictColor2Flag, 0);
				if (numC != 3) this->update(element->aColor, getColorList(Edge[3], element->colorList), getColorList(color1, element->colorList), getColorList(color2, element->colorList));
			}
		}
		else this->update(element->aColor, getColorList(Edge[3], element->colorList), getColorList(color1, element->colorList));
	}
	adjustColorList(Edge[3], element->colorList);
    }

    if (m_height != 2) 
    {
        encodeEdgeLength(this, encode);
    }                                            //encode the edge
}

void CodingUnit::enRunLength(bool encode)
{
	int length = 0;
	int index = 1;

	int color1, color2;
	int predict = predictColor1(this, &color1, &color2);
	if (encode)
	{
		if (predict > 0)
		{
			if (color1 == runLength[index - 1])
			{
				acodec.encode(1, aPredictColor1Flag);
			}
			else
			{
				acodec.encode(0, aPredictColor1Flag);
				if (!g_biValue)
				{
					if (predict == 2)
					{
						if (color2 == runLength[index - 1]) acodec.encode(1, aPredictColor1Flag2);
						else
						{
							acodec.encode(0, aPredictColor1Flag2);
							if (numC != 3)
							{
								Adaptive_Data_Model Color(numC - 2);
								Color.copy(aColor, getColorList(color1, colorList), getColorList(color2, colorList));
								int indexc = get_index(getColorList(runLength[index - 1], colorList), true, getColorList(color1, colorList), getColorList(color2, colorList));
								acodec.encode(indexc, Color);
								acodec.noencode(getColorList(runLength[index - 1], colorList), aColor);
							}
						}
					}
					else
					{
						Adaptive_Data_Model Color(numC - 1);
						Color.copy(aColor, getColorList(color1, colorList));
						int indexc = get_index(getColorList(runLength[index - 1], colorList), true, getColorList(color1, colorList));
						acodec.encode(indexc, Color);
						acodec.noencode(getColorList(runLength[index - 1], colorList), aColor);
					}
				}
			}
		}
		else if(predict < 0) acodec.encode(getColorList(runLength[index - 1], colorList), aColor);
		else
		{
			if (ref_frame) acodec.encode(getColorList(runLength[index - 1], colorList), aColor);
		}
		adjustColorList(runLength[index - 1], colorList);

		if (m_width == 2)                        //when g_min_CU == 2, encode the color value directly.
		{
			if (!g_biValue)
			{
				acodec.encode(getColorList(runLength[index], colorList), aColor);
				adjustColorList(runLength[index], colorList);
				acodec.encode(getColorList(runLength[index + 1], colorList), aColor);
				adjustColorList(runLength[index + 1], colorList);
				acodec.encode(getColorList(runLength[index + 2], colorList), aColor);
				adjustColorList(runLength[index + 2], colorList);
			}
		}
	}
	else
	{
		if (predict > 0)
		{
			if (color1 == runLength[index - 1])
			{
				this->update(element->aPredictColor1Flag, 1);
			}
			else
			{
				this->update(element->aPredictColor1Flag, 0);
				if (!g_biValue)
				{
					if (predict == 2)
					{
						if (color2 == runLength[index - 1]) this->update(element->aPredictColor1Flag2, 1);
						else
						{
							this->update(element->aPredictColor1Flag2, 0);
							if (numC != 3) this->update(element->aColor, getColorList(runLength[index - 1], element->colorList), getColorList(color1, element->colorList), getColorList(color2, element->colorList));
						}
					}
					else this->update(element->aColor, getColorList(runLength[index - 1], element->colorList), getColorList(color1, element->colorList));
				}
			}
		}
		else if (predict < 0)
		{
			this->update(element->aColor, getColorList(runLength[index - 1], element->colorList));
		}
		else
		{
			if (ref_frame) this->update(element->aColor, getColorList(runLength[index - 1], element->colorList));
		}
		adjustColorList(runLength[index - 1], element->colorList);

		if (m_width == 2)                        //when g_min_CU == 2, encode the color value directly.
		{
			if (!g_biValue)
			{
				this->update(element->aColor, getColorList(runLength[index], element->colorList));
				adjustColorList(runLength[index], element->colorList);
				this->update(element->aColor, getColorList(runLength[index+1], element->colorList));
				adjustColorList(runLength[index+1], element->colorList);
				this->update(element->aColor, getColorList(runLength[index+2], element->colorList));
				adjustColorList(runLength[index+2], element->colorList);
			}
		}
	}
}

void CodingUnit::enMV(bool encode)
{
	if (encode)
	{
		int absV = 0;
		if (mvx > 0) absV = (mvx - 1) * 2 + 1;
		else if (mvx < 0) absV = -mvx * 2;
		int stopLoop = 0;     //一阶指数哥伦布
		int k = 1;
		do {
			if (absV >= (1 << k))
			{
				acodec.encode(1, aGolomb);
				absV = absV - (1 << k);
				k++;
			}
			else {
				acodec.encode(0, aGolomb);
				while (k--)
					acodec.encode(((absV >> k) & 1), aGolomb);
				stopLoop = 1;
			}
		} while (!stopLoop);

		absV = 0;
		if (mvy > 0) absV = (mvy - 1) * 2 + 1;
		else if (mvy < 0) absV = -mvy * 2;
		stopLoop = 0;     //一阶指数哥伦布
		k = 1;
		do {
			if (absV >= (1 << k))
			{
				acodec.encode(1, aGolomb);
				absV = absV - (1 << k);
				k++;
			}
			else {
				acodec.encode(0, aGolomb);
				while (k--)
					acodec.encode(((absV >> k) & 1), aGolomb);
				stopLoop = 1;
			}
		} while (!stopLoop);

		if (noResidue) acodec.encode(1, aNoResidue);
		else acodec.encode(0, aNoResidue);
		if (!noResidue)
		{
			if (difEdge[0] > 0)
			{
				Static_Data_Model Index(difEdge[0]);
				acodec.encode(difEdge[1], Index);
			}
			encodeDifEdgeLength(this, encode);
		}

	}
	else
	{
		int absV = 0;
		if (mvx > 0) absV = (mvx - 1) * 2 + 1;
		else if (mvx < 0) absV = -mvx * 2;
		int stopLoop = 0;     //一阶指数哥伦布
		int k = 1;
		do {
			if (absV >= (1 << k))
			{
				this->update(element->aGolomb, 1);
				absV = absV - (1 << k);
				k++;
			}
			else {
				this->update(element->aGolomb, 0);
				while (k--)
					this->update(element->aGolomb, ((absV >> k) & 1));
				stopLoop = 1;
			}
		} while (!stopLoop);

		absV = 0;
		if (mvy > 0) absV = (mvy - 1) * 2 + 1;
		else if (mvy < 0) absV = -mvy * 2;
		stopLoop = 0;     //一阶指数哥伦布
		k = 1;
		do {
			if (absV >= (1 << k))
			{
				this->update(element->aGolomb, 1);
				absV = absV - (1 << k);
				k++;
			}
			else {
				this->update(element->aGolomb, 0);
				while (k--)
					this->update(element->aGolomb, ((absV >> k) & 1));
				stopLoop = 1;
			}
		} while (!stopLoop);
		if (noResidue) this->update(element->aNoResidue, 1);
		else this->update(element->aNoResidue, 0);
		if (!noResidue)
		{
			if (difEdge[0] > 0)
			{
				rate -= log2(1.0 / difEdge[0]);
			}
			encodeDifEdgeLength(this, encode);
		}
	}
}


int CodingUnit::deSplitFlag()
{
  int index;
  int splitFlag;
  if (location_col > 0 && depthMap[location_row*pic_width + location_col - 1] > m_depth) index = 1;
  else index = 0;
  if (location_row > 0 && depthMap[(location_row - 1)*pic_width + location_col] > m_depth) index++;
  switch (index)
  {
  case 0:splitFlag = acodec.decode(aSplitFlag1); break;
  case 1:splitFlag = acodec.decode(aSplitFlag2); break;
  case 2:splitFlag = acodec.decode(aSplitFlag3); break;
  }  
  return splitFlag;
}

void CodingUnit::deOneColor(unsigned char*pImg)
{
	m_value = new unsigned char[m_height*m_width];
	int color1, color2;
	int predict = predictColor1(this, &color1, &color2);
	if (predict == -1) color1 = colorList[acodec.decode(aColor)];
	else if (predict == 0)
	{
		color1 = 0;
		if(ref_frame) color1 = colorList[acodec.decode(aColor)];
	}
	else
	{
		if (!acodec.decode(aPredictColor1Flag))
		{
			if (g_biValue) color1 = 1 - color1;
			else
			{
				if (predict == 2)
				{
					if (acodec.decode(aPredictColor1Flag2)) color1 = color2;
					else
					{
						if (numC == 3)
						{
							int index = get_index(0, false, getColorList(color1, colorList), getColorList(color2, colorList));
							color1 = colorList[index];
						}
						else
						{
							Adaptive_Data_Model Color(numC - 2);
							Color.copy(aColor, getColorList(color1, colorList), getColorList(color2, colorList));
							int index = get_index(acodec.decode(Color), false, getColorList(color1, colorList), getColorList(color2, colorList));
							acodec.nodecode(aColor, index);
							color1 = colorList[index];
						}
					}
				}
				else 
				{
					Adaptive_Data_Model Color(numC - 1);
					Color.copy(aColor, getColorList(color1, colorList));
					int index = get_index(acodec.decode(Color),false, getColorList(color1, colorList));
					acodec.nodecode(aColor, index);
					color1 = colorList[index];
				}
			}
		}
	}
	adjustColorList(color1, colorList);
	for (int i = 0; i < m_height*m_width; i++)
	{
		m_value[i] = color1;
	}

	for (int i = 0; i < m_height; i++)
	{
		for (int j = 0; j < m_width; j++)
		{
			pImg[(i + location_row)*pic_width + j + location_col] = m_value[i * m_width + j];
		}
	}
}

int get_position_dec(int index, int pos1, int pos2, int pos3, int block_size)
{
	int item = 0;
	for (int i = 1; i < block_size; i++)
	{
		if (i == pos1 || i == pos2 || i == pos3) continue;
		if (item == index) return i;
		item++;
	}
}

void CodingUnit::deChainCoding(unsigned char*pImg)
{
	m_value = new unsigned char[m_height*m_width];
	int CUhSize = getBitSize(m_height);
	int CUwSize = getBitSize(m_width);
  int chainPositionMode2x2;
  int color1, color2;
  int predict = predictColor1(this, &color1, &color2);
  if (predict == -1) color1 = colorList[acodec.decode(aColor)];
  else if (predict == 0)
  {
	  color1 = 0;
	  if (ref_frame) color1 = colorList[acodec.decode(aColor)];
  }
  else
  {
	  if (!acodec.decode(aPredictColor1Flag))
	  {
		  if (g_biValue) color1 = 1 - color1;
		  else
		  {
			  if (predict == 2)
			  {
				  if (acodec.decode(aPredictColor1Flag2)) color1 = color2;
				  else
				  {
					  if (numC == 3)
					  {
						  int index = get_index(0, false, getColorList(color1, colorList), getColorList(color2, colorList));
						  color1 = colorList[index];
					  }
					  else
					  {
						  Adaptive_Data_Model Color(numC - 2);
						  Color.copy(aColor, getColorList(color1, colorList), getColorList(color2, colorList));
						  int index = get_index(acodec.decode(Color), false, getColorList(color1, colorList), getColorList(color2, colorList));
						  acodec.nodecode(aColor, index);
						  color1 = colorList[index];
					  }
				  }
			  }
			  else 
			  { 
				  Adaptive_Data_Model Color(numC - 1);
				  Color.copy(aColor, getColorList(color1, colorList));
				  int index = get_index(acodec.decode(Color), false, getColorList(color1, colorList));
				  acodec.nodecode(aColor, index);
				  color1 = colorList[index];
			  }
		  }
	  }
  }

  adjustColorList(color1, colorList);

	int initial_loc_y, initial_loc_x;
	if (m_height == 2 && m_width == 2)
	{
    chainPositionMode2x2 = acodec.decode(aChainPositionMode2x2);
    switch (chainPositionMode2x2)
    {
    case 0:initial_loc_y = 0; initial_loc_x = 1; break;
    case 1:initial_loc_y = 1; initial_loc_x = 0; break;
    case 2:initial_loc_y = 1; initial_loc_x = 1; break;
    case 3:initial_loc_y = 1; initial_loc_x = 0; break;
    case 4:initial_loc_y = 0; initial_loc_x = 1; break;
    case 5:initial_loc_y = 1; initial_loc_x = 0; break;
    }
	}
	else
	{
		int predictTrue = 0;
        int initial_loc_y2, initial_loc_x2, predictMode1 = -2, predictMode2 = -2;
		int predict = predictInitialPosition(this, &initial_loc_y, &initial_loc_x,&initial_loc_y2, &initial_loc_x2, color1, &predictMode1, &predictMode2);
		int x1 = -1, x2 = -1, x3 = -1, y1 = -1, y2 = -1, y3 = -1, num0 = 0, num1 = 0;
		if (predict == 0 || predict == 2) get_predict_position(m_height, &y1, &y2, &y3, initial_loc_y, &num0);
		if (predict == 1 || predict == 2) get_predict_position(m_height, &x1, &x2, &x3, initial_loc_x2, &num1);
		if (predict == 1)
		{
			initial_loc_y = initial_loc_y2;
			initial_loc_x = initial_loc_x2;
		}
		if (predict >= 0)
		{
			predictTrue = acodec.decode(aPredictPositionFlag);
			if (predictTrue)
			{
        if (predict == 2)
        {
          if (acodec.decode(aPredictPositionSide))
          {
            initial_loc_y = initial_loc_y2;
            initial_loc_x = initial_loc_x2;
          }
        }
        if (initial_loc_x == 1 || initial_loc_x == m_width - 1 || initial_loc_y == 1 || initial_loc_y == m_height - 1)
        {
          if (acodec.decode(aPredictPositionBorder))
          {
            if (initial_loc_x == 1) initial_loc_x++;
            if (initial_loc_x == m_width - 1) initial_loc_x--;
            if (initial_loc_y == 1) initial_loc_y++;
            if (initial_loc_y == m_height - 1) initial_loc_y--;
          }
        }
        else
        {
          int PredictPositionMode = acodec.decode(aPredictPositionMode);
          if (PredictPositionMode == 1)
          {
            if (initial_loc_y == 0) initial_loc_x++;
            else initial_loc_y++;
          }
          if (PredictPositionMode == 2)
          {
            if (initial_loc_y == 0) initial_loc_x--;
            else initial_loc_y--;
          }
        }
			}
		}
		if (!predictTrue)
		{
			int sideDirection = 0;
			if (m_height == 4 && num0 == 3 && num1 != 3) sideDirection = acodec.decode(aSideMode, 0);
			else if (m_height == 4 && num0 != 3 && num1 == 3) sideDirection = acodec.decode(aSideMode, 1);
			else if (m_height == 4 && num0 == 3 && num1 == 3)
			{
				Adaptive_Data_Model Side(2);
				Side.copy(aSideMode, 0, 1);
				sideDirection = acodec.decode(Side) + 2;
				acodec.nodecode(aSideMode, sideDirection);
			}
			else sideDirection = acodec.decode(aSideMode);
			switch (sideDirection)
			{
			case 0:
				initial_loc_x = 0;
				if ((predict == 0 || predict == 2) && m_height == 4)
				{
					if (initial_loc_y == 1) initial_loc_y = 3;
					else initial_loc_y = 1;
				}
				else
				{
					if (predictMode1 == 1 && y1 >= m_height / 2 && y1 < m_height - 2)
					{
						if (acodec.decode(aPredictPosition))
						{
							int num = m_height - y1 - 2;
							if (num > 1)
							{
								Static_Data_Model Position(num);
								initial_loc_y = acodec.decode(Position) + y1 + 2;
							}
							else initial_loc_y = m_height - 1;
						}
						else
						{
							int num = y1 - 2;
							if (num > 1)
							{
								Static_Data_Model Position(num);
								initial_loc_y = -int(acodec.decode(Position)) + y1 - 2;
							}
							else initial_loc_y = 1;
						}
					}
					else if (predictMode1 == -1 && y1 <= m_height / 2 && y1 > 2)
					{
						if (acodec.decode(aPredictPosition))
						{
							int num = y1 - 2;
							if (num > 1)
							{
								Static_Data_Model Position(num);
								initial_loc_y = -int(acodec.decode(Position)) + y1 - 2;
							}
							else initial_loc_y = 1;
						}
						else
						{
							int num = m_height - y1 - 2;
							if (num > 1)
							{
								Static_Data_Model Position(num);
								initial_loc_y = acodec.decode(Position) + y1 + 2;
							}
							else initial_loc_y = m_height - 1;
						}
					}
					else
					{
						Static_Data_Model Position(m_height - 1 - num0);
						int index = acodec.decode(Position);
						initial_loc_y = get_position_dec(index, y1, y2, y3, m_height);
					}
				}
				break;
			case 1:
				if ((predict == 1 || predict == 2) && m_width == 4)
				{
					if (initial_loc_x2 == 1) initial_loc_x = 3;
					else initial_loc_x = 1;
				}
				else
				{
					if (predictMode2 == 1 && x1 >= m_height / 2 && x1 < m_height - 2)
					{
						if (acodec.decode(aPredictPosition))
						{
							int num = m_height - x1 - 2;
							if (num > 1)
							{
								Static_Data_Model Position(num);
								initial_loc_x = acodec.decode(Position) + x1 + 2;
							}
							else initial_loc_x = m_height - 1;
						}
						else
						{
							int num = x1 - 2;
							if (num > 1)
							{
								Static_Data_Model Position(num);
								initial_loc_x = -int(acodec.decode(Position)) + x1 - 2;
							}
							else initial_loc_x = 1;
						}
					}
					else if (predictMode2 == -1 && x1 <= m_height / 2 && x1 > 2)
					{
						if (acodec.decode(aPredictPosition))
						{
							int num = x1 - 2;
							if (num > 1)
							{
								Static_Data_Model Position(num);
								initial_loc_x = -int(acodec.decode(Position)) + x1 - 2;
							}
							else initial_loc_x = 1;
						}
						else
						{
							int num = m_height - x1 - 2;
							if (num > 1)
							{
								Static_Data_Model Position(num);
								initial_loc_x = acodec.decode(Position) + x1 + 2;
							}
							else initial_loc_x = m_height - 1;
						}
					}
					else
					{
						Static_Data_Model Position(m_height - 1 - num1);
						int index = acodec.decode(Position);
						initial_loc_x = get_position_dec(index, x1, x2, x3, m_height);
					}
				}
				initial_loc_y = 0;
				break;
			case 2:
			    {
				    Static_Data_Model Position(m_height - 1);
			        initial_loc_x = acodec.decode(Position) + 1;
			        initial_loc_y = m_height - 1; 
			    }
				break;
			case 3:
			    {
				    initial_loc_x = m_width - 1;
					Static_Data_Model Position(m_height - 2);
					initial_loc_y = acodec.decode(Position) + 1;
				}
				break;
			}
		}
	}
  color2 = 1 - color1;
  if (!g_biValue)
  {
	  color2 = predictColor2(this, initial_loc_y, initial_loc_x, color1);
	  if (color2 == -1)
	  {
		  Adaptive_Data_Model Color(numC - 1);
		  Color.copy(aColor, getColorList(color1, colorList));
		  int index = get_index(acodec.decode(Color), false, getColorList(color1, colorList));
		  acodec.nodecode(aColor, index);
		  color2 = colorList[index];
	  }
	  else
	  {
		  int flag = acodec.decode(aPredictColor2Flag);
		  if (!flag)
		  {
			  if (numC == 3)
			  {
				  int index = get_index(0, false, getColorList(color1, colorList), getColorList(color2, colorList));
				  color2 = colorList[index];
			  }
			  else
			  {
				  Adaptive_Data_Model Color(numC - 2);
				  Color.copy(aColor, getColorList(color1, colorList), getColorList(color2, colorList));
				  int index = get_index(acodec.decode(Color), false, getColorList(color1, colorList), getColorList(color2, colorList));
				  acodec.nodecode(aColor, index);
				  color2 = colorList[index];
			  }
		  }
	  }
  }
  adjustColorList(color2, colorList);

  if (m_height != 2)
  {
	  int color3;
	  for (int i = 0; i < 3; i++)
	  {
		  if (i != color1 && i != color2) color3 = i;
	  }
	  for (int i = 0; i < m_height*m_width; i++)
	  {
		  m_value[i] = color3;
	  }

	  int OT3_0 = -1;
	  int OT3_1 = -1;
	  int OT3_2 = -1;
	  int OT3;
	  int mode = 1;
	  if (initial_loc_x == 0 || initial_loc_y == m_height - 1) mode = 0;
	  if ((initial_loc_x == 0 && location_col > 0) || (initial_loc_y == 0 && location_row > 0))
      {
	      int edgeline[3];
     	  get_context(pImg, location_row, location_col, pic_width, m_width, initial_loc_y, initial_loc_x, color2, edgeline, &context, &mode);
	      for (int i = 0; i < context; i++)
	      {
		      update_context(&OT3_0, &OT3_1, &OT3_2, edgeline[i]);
	      }
      }


	  int loc_y = initial_loc_y;
	  int loc_x = initial_loc_x;

	  int direction;
	  if (initial_loc_x == 0)
	  {
		  direction = 0; loc_x++;
		  m_value[loc_y * m_width + loc_x - 1] = color2;
	  }
	  else if (initial_loc_y == 0)
	  {
		  direction = 3; loc_y++;
		  m_value[(loc_y - 1) * m_width + loc_x] = color2;
	  }
	  else if (initial_loc_y == m_height - 1)
	  {
		  direction = 1;
		  m_value[loc_y * m_width + loc_x] = color2;
	  }
	  else
	  {
		  direction = 2;
		  m_value[loc_y * m_width + loc_x] = color2;
	  }

	  int index = context;
	  int forbidden = -1;
	  int forbid_direction = 0;
	  unsigned char* ChainPath = new unsigned char[(m_height + 1) * (m_width + 1)];
	  for (int i = 0; i < (m_height + 1) * (m_width + 1); i++) ChainPath[i] = 0;

	  do
	  {
		  index += 1;
		  forbidden = -1;
		  if (initial_loc_x == 0)
		  {
			  if (loc_x == 1 && loc_y < initial_loc_y)
			  {
				  if (direction == 1) forbidden = 2;
				  if (direction == 2) { forbidden = 4; forbid_direction = 1; }
			  }
		  }
		  else if (initial_loc_y == 0)
		  {
			  if (loc_x == 1)
			  {
				  if (direction == 2) {forbidden = 4; forbid_direction = 3; }
				  if (direction == 3) forbidden = 2;
			  }
			  else if (loc_x < initial_loc_x && loc_y == 1)
			  {
				  if (direction == 1) {forbidden = 4; forbid_direction = 2; }
				  if (direction == 2) forbidden = 1;
			  }
		  }
		  else if (initial_loc_y == m_height - 1)
		  {
			  if (loc_x == 1)
			  {
				  if (direction == 1) forbidden = 2;
				  if (direction == 2) {forbidden = 4; forbid_direction = 1; }
			  }
			  else if (loc_y == 1)
			  {
				  if (direction == 0) forbidden = 1;
				  if (direction == 1) {forbidden = 4; forbid_direction = 0; }
			  }
			  else if (loc_x < initial_loc_x && loc_y == m_height - 1)
			  {
				  if (direction == 2) forbidden = 3;
				  if (direction == 3) {forbidden = 4; forbid_direction = 2; }
			  }
		  }
		  else
		  {
			  if (loc_x == 1)
			  {
				  if (direction == 2) {forbidden = 4; forbid_direction = 3; }
				  if (direction == 3) forbidden = 2;
			  }
			  else if (loc_y == 1)
			  {
				  if (direction == 1) {forbidden = 4; forbid_direction = 2; }
				  if (direction == 2) forbidden = 1;
			  }
			  else if (loc_y == m_height - 1)
			  {
				  if (direction == 0) forbidden = 3;
				  if (direction == 3) {forbidden = 4; forbid_direction = 0; }
			  }
			  else if (loc_x == m_width - 1 && loc_y < initial_loc_y)
			  {
				  if (direction == 0) {forbidden = 4; forbid_direction = 1; }
				  if (direction == 1) forbidden = 0;
			  }
		  }

		  ChainPath[loc_y * (m_width + 1) + loc_x] = 1;
		  int forbidden1, forbidden2;
		  for (int i = 0; i < 4; i++)
		  {
			  if (i == 2) continue;
			  else if (i == 0) { forbidden1 = (direction + 1) % 4; forbidden2 = (direction + 3) % 4;}
			  else if (i == 1) { forbidden1 = direction; forbidden2 = (direction + 3) % 4; }
			  else { forbidden1 = direction; forbidden2 = (direction + 1) % 4; }
			  switch ((direction + i) % 4)
			  {
			  case 0: if (ChainPath[loc_y * (m_width + 1) + loc_x + 1] == 1) { if (forbidden == forbidden1 || forbidden == forbidden2) { if (forbidden == forbidden1) forbid_direction = forbidden2; else forbid_direction = forbidden1; forbidden = 4; } else if (forbidden != 4) forbidden = 0; }break;
			  case 1: if (ChainPath[(loc_y - 1) * (m_width + 1) + loc_x] == 1) { if (forbidden == forbidden1 || forbidden == forbidden2) { if (forbidden == forbidden1) forbid_direction = forbidden2; else forbid_direction = forbidden1; forbidden = 4; } else if (forbidden != 4) forbidden = 1; }break;
			  case 2: if (ChainPath[loc_y * (m_width + 1) + loc_x - 1] == 1) { if (forbidden == forbidden1 || forbidden == forbidden2) { if (forbidden == forbidden1) forbid_direction = forbidden2; else forbid_direction = forbidden1; forbidden = 4; } else if (forbidden != 4) forbidden = 2; }break;
			  case 3: if (ChainPath[(loc_y + 1) * (m_width + 1) + loc_x] == 1) { if (forbidden == forbidden1 || forbidden == forbidden2) { if (forbidden == forbidden1) forbid_direction = forbidden2; else forbid_direction = forbidden1; forbidden = 4; } else if (forbidden != 4) forbidden = 3; }
			  }
		  }

		  if (forbidden == 4) 
		  { 
			  if (forbid_direction - direction == 1 || forbid_direction - direction == -3)  //left
			  {
				  if (mode == 0) OT3 = 2;
				  if (mode == 1) OT3 = 1;
			  }
			  else if (forbid_direction - direction == -1 || forbid_direction - direction == 3) //right
			  {
				  if (mode == 1) OT3 = 2;
				  if (mode == 0) OT3 = 1;
			  }
			  else
			  {
				  OT3 = 0;
			  }
			  if (OT3 == 1) mode = 1 - mode;
			  direction = forbid_direction; 
			  switch (direction)
			  {
			  case 0:loc_x++; if (initial_loc_x == 0 || initial_loc_y == m_height - 1) m_value[loc_y * m_width + loc_x - 1] = color2; else m_value[(loc_y - 1) * m_width + loc_x - 1] = color2; break;
			  case 1:loc_y--; if (initial_loc_x == 0 || initial_loc_y == m_height - 1) m_value[loc_y * m_width + loc_x] = color2; else m_value[loc_y * m_width + loc_x - 1] = color2; break;
			  case 2:loc_x--; if (initial_loc_x == 0 || initial_loc_y == m_height - 1) m_value[(loc_y - 1) * m_width + loc_x] = color2; else m_value[loc_y * m_width + loc_x] = color2; break;
			  case 3:loc_y++; if (initial_loc_x == 0 || initial_loc_y == m_height - 1) m_value[(loc_y - 1) * m_width + loc_x - 1] = color2; else m_value[(loc_y - 1) * m_width + loc_x] = color2;
			  }
		  }
		  else
		  {
			  if (forbidden >= 0)
			  {
				  if (forbidden - direction == 1 || forbidden - direction == -3)  //left
				  {
					  if (mode == 0) forbidden = 2;
					  if (mode == 1) forbidden = 1;
				  }
				  else if (forbidden - direction == -1 || forbidden - direction == 3) //right
				  {
					  if (mode == 1) forbidden = 2;
					  if (mode == 0) forbidden = 1;
				  }
				  else
				  {
					  forbidden = 0;
				  }
			  }

			  if (index == 1)
			  {
				  if (initial_loc_x == 0) OT3 = acodec.decode(a3OT_L, forbidden);
				  else if (initial_loc_y == 0) OT3 = acodec.decode(a3OT_A, forbidden);
				  else if (initial_loc_y == m_height - 1) OT3 = acodec.decode(a3OT_B, forbidden);
				  else OT3 = acodec.decode(a3OT_R, forbidden);
			  }
			  else if (index == 2)
			  {
				  switch (OT3_0)
				  {
				  case 0:OT3 = acodec.decode(a3OT_0, forbidden); break;
				  case 1:OT3 = acodec.decode(a3OT_1, forbidden); break;
				  case 2:OT3 = acodec.decode(a3OT_2, forbidden);
				  }
			  }
			  else if (index == 3)
			  {
				  switch (OT3_0 * 3 + OT3_1)
				  {
				  case 0:OT3 = acodec.decode(a3OT_00, forbidden); break;
				  case 1:OT3 = acodec.decode(a3OT_01, forbidden); break;
				  case 2:OT3 = acodec.decode(a3OT_02, forbidden); break;
				  case 3:OT3 = acodec.decode(a3OT_10, forbidden); break;
				  case 4:OT3 = acodec.decode(a3OT_11, forbidden); break;
				  case 5:OT3 = acodec.decode(a3OT_12, forbidden); break;
				  case 6:OT3 = acodec.decode(a3OT_20, forbidden); break;
				  case 7:OT3 = acodec.decode(a3OT_21, forbidden); break;
				  case 8:OT3 = acodec.decode(a3OT_22, forbidden);
				  }
			  }
			  else
			  {
				  switch (OT3_0 * 9 + OT3_1 * 3 + OT3_2)
				  {
				  case 0:OT3 = acodec.decode(a3OT_000, forbidden); break;
				  case 1:OT3 = acodec.decode(a3OT_001, forbidden); break;
				  case 2:OT3 = acodec.decode(a3OT_002, forbidden); break;
				  case 3:OT3 = acodec.decode(a3OT_010, forbidden); break;
				  case 4:OT3 = acodec.decode(a3OT_011, forbidden); break;
				  case 5:OT3 = acodec.decode(a3OT_012, forbidden); break;
				  case 6:OT3 = acodec.decode(a3OT_020, forbidden); break;
				  case 7:OT3 = acodec.decode(a3OT_021, forbidden); break;
				  case 8:OT3 = acodec.decode(a3OT_022, forbidden); break;
				  case 9:OT3 = acodec.decode(a3OT_100, forbidden); break;
				  case 10:OT3 = acodec.decode(a3OT_101, forbidden); break;
				  case 11:OT3 = acodec.decode(a3OT_102, forbidden); break;
				  case 12:OT3 = acodec.decode(a3OT_110, forbidden); break;
				  case 13:OT3 = acodec.decode(a3OT_111, forbidden); break;
				  case 14:OT3 = acodec.decode(a3OT_112, forbidden); break;
				  case 15:OT3 = acodec.decode(a3OT_120, forbidden); break;
				  case 16:OT3 = acodec.decode(a3OT_121, forbidden); break;
				  case 17:OT3 = acodec.decode(a3OT_122, forbidden); break;
				  case 18:OT3 = acodec.decode(a3OT_200, forbidden); break;
				  case 19:OT3 = acodec.decode(a3OT_201, forbidden); break;
				  case 20:OT3 = acodec.decode(a3OT_202, forbidden); break;
				  case 21:OT3 = acodec.decode(a3OT_210, forbidden); break;
				  case 22:OT3 = acodec.decode(a3OT_211, forbidden); break;
				  case 23:OT3 = acodec.decode(a3OT_212, forbidden); break;
				  case 24:OT3 = acodec.decode(a3OT_220, forbidden); break;
				  case 25:OT3 = acodec.decode(a3OT_221, forbidden); break;
				  case 26:OT3 = acodec.decode(a3OT_222, forbidden);
				  }
			  }

			  if (OT3 == 0)
			  {
				  switch (direction)
				  {
				  case 0:loc_x++; if (initial_loc_x == 0 || initial_loc_y == m_height - 1) m_value[loc_y * m_width + loc_x - 1] = color2; else m_value[(loc_y - 1) * m_width + loc_x - 1] = color2; break;
				  case 1:loc_y--; if (initial_loc_x == 0 || initial_loc_y == m_height - 1) m_value[loc_y * m_width + loc_x] = color2; else m_value[loc_y * m_width + loc_x - 1] = color2; break;
				  case 2:loc_x--; if (initial_loc_x == 0 || initial_loc_y == m_height - 1) m_value[(loc_y - 1) * m_width + loc_x] = color2; else m_value[loc_y * m_width + loc_x] = color2; break;
				  case 3:loc_y++; if (initial_loc_x == 0 || initial_loc_y == m_height - 1) m_value[(loc_y - 1) * m_width + loc_x - 1] = color2; else m_value[(loc_y - 1) * m_width + loc_x] = color2;
				  }
			  }
			  else
			  {
				  if (OT3 == 1) mode = 1 - mode;
				  if (mode == 0)
				  {
					  switch (direction)
					  {
					  case 0:direction = 1; loc_y--; if (initial_loc_x == 0 || initial_loc_y == m_height - 1) m_value[loc_y * m_width + loc_x] = color2; else m_value[loc_y * m_width + loc_x - 1] = color2; break;
					  case 1:direction = 2; loc_x--; if (initial_loc_x == 0 || initial_loc_y == m_height - 1) m_value[(loc_y - 1) * m_width + loc_x] = color2; else m_value[loc_y * m_width + loc_x] = color2; break;
					  case 2:direction = 3; loc_y++; if (initial_loc_x == 0 || initial_loc_y == m_height - 1) m_value[(loc_y - 1) * m_width + loc_x - 1] = color2; else m_value[(loc_y - 1) * m_width + loc_x] = color2; break;
					  case 3:direction = 0; loc_x++; if (initial_loc_x == 0 || initial_loc_y == m_height - 1) m_value[loc_y * m_width + loc_x - 1] = color2; else m_value[(loc_y - 1) * m_width + loc_x - 1] = color2;
					  }
				  }
				  else
				  {
					  switch (direction)
					  {
					  case 0:direction = 3; loc_y++; if (initial_loc_x == 0 || initial_loc_y == m_height - 1) m_value[(loc_y - 1) * m_width + loc_x - 1] = color2; else m_value[(loc_y - 1) * m_width + loc_x] = color2; break;
					  case 1:direction = 0; loc_x++; if (initial_loc_x == 0 || initial_loc_y == m_height - 1) m_value[loc_y * m_width + loc_x - 1] = color2; else m_value[(loc_y - 1) * m_width + loc_x - 1] = color2; break;
					  case 2:direction = 1; loc_y--; if (initial_loc_x == 0 || initial_loc_y == m_height - 1) m_value[loc_y * m_width + loc_x] = color2; else m_value[loc_y * m_width + loc_x - 1] = color2; break;
					  case 3:direction = 2; loc_x--; if (initial_loc_x == 0 || initial_loc_y == m_height - 1) m_value[(loc_y - 1) * m_width + loc_x] = color2; else m_value[loc_y * m_width + loc_x] = color2;
					  }
				  }
			  }
		  }
		  update_context(&OT3_0, &OT3_1, &OT3_2, OT3);
	  } while (!(loc_x == m_width || loc_y == 0 || loc_x == 0 || loc_y == m_height));

	  advancedExpand(m_value, m_height, m_width, color1, 0, 0);

	  for (int i = 0; i < m_height*m_width; i++)
	  {
		  if (m_value[i] == color3) m_value[i] = color2;
	  }
	  delete[] ChainPath;
  }
  else
  {
    switch (chainPositionMode2x2)
    {
    case 0:m_value[0] = color1; m_value[1] = color2; m_value[2] = color1; m_value[3] = color1; break;
    case 1:m_value[0] = color1; m_value[1] = color1; m_value[2] = color2; m_value[3] = color1; break;
    case 2:m_value[0] = color1; m_value[1] = color1; m_value[2] = color1; m_value[3] = color2; break;
    case 3:m_value[0] = color1; m_value[1] = color1; m_value[2] = color2; m_value[3] = color2; break;
    case 4:m_value[0] = color1; m_value[1] = color2; m_value[2] = color1; m_value[3] = color2; break;
    case 5:m_value[0] = color1; m_value[1] = color2; m_value[2] = color2; m_value[3] = color2; break;
    }
  }
	for (int i = 0; i < m_height; i++)
	{
		for (int j = 0; j < m_width; j++)
		{
			pImg[(i + location_row)*pic_width + j + location_col] = m_value[i * m_width + j];
		}
	}
}

void CodingUnit::deRunLength(unsigned char*pImg)
{
	int widthBitNum = getBitSize(m_width);
	int heightBitNum = getBitSize(m_height);
	int loc_size = widthBitNum + heightBitNum;
	int area_size = m_height*m_width;
	m_value = new unsigned char[m_height*m_width];
	int color1, color2;
	int predict = predictColor1(this, &color1, &color2);
	if (predict == -1) color1 = colorList[acodec.decode(aColor)];
	else if (predict == 0)
	{
		color1 = 0;
		if (ref_frame) color1 = colorList[acodec.decode(aColor)];
	}
	else
	{
		if (!acodec.decode(aPredictColor1Flag))
		{
			if (g_biValue) color1 = 1 - color1;
			else
			{
				if (predict == 2)
				{
					if (acodec.decode(aPredictColor1Flag2)) color1 = color2;
					else
					{
						if (numC == 3)
						{
							int index = get_index(0, false, getColorList(color1, colorList), getColorList(color2, colorList));
							color1 = colorList[index];
						}
						else
						{
							Adaptive_Data_Model Color(numC - 2);
							Color.copy(aColor, getColorList(color1, colorList), getColorList(color2, colorList));
							int index = get_index(acodec.decode(Color), false, getColorList(color1, colorList), getColorList(color2, colorList));
							acodec.nodecode(aColor, index);
							color1 = colorList[index];
						}
					}
				}
				else
				{
					Adaptive_Data_Model Color(numC - 1);
					Color.copy(aColor, getColorList(color1, colorList));
					int index = get_index(acodec.decode(Color), false, getColorList(color1, colorList));
					acodec.nodecode(aColor, index);
					color1 = colorList[index];
				}
			}
		}
	}
	adjustColorList(color1, colorList);

	if (m_height == 2)
	{
		m_value[0] = color1;
    if (!g_biValue)
    {
      m_value[1] = colorList[acodec.decode(aColor)];
	  adjustColorList(m_value[1], colorList);
      m_value[2] = colorList[acodec.decode(aColor)];
	  adjustColorList(m_value[2], colorList);
      m_value[3] = colorList[acodec.decode(aColor)];
	  adjustColorList(m_value[3], colorList);
    }
    else
    {
      m_value[1] = 1 - color1;
      m_value[2] = 1 - color1;
      m_value[3] = color1;
    }
	}
	for (int i = 0; i < m_height; i++)
	{
		for (int j = 0; j < m_width; j++)
		{
			pImg[(i + location_row)*pic_width + j + location_col] = m_value[i * m_width + j];
		}
	}
}

void CodingUnit::deMV(unsigned char * pImg)
{
	int absV1 = 0;
	int absV2 = 0;
	int k = 1;
	while (acodec.decode(aGolomb))
	{
		absV1 += (1 << k);
		k++;
	}
	for (int i = 0; i < k; i++)
	{
		absV2 = 2 * absV2 + acodec.decode(aGolomb);
	}
	absV1 += absV2;
	if (absV1 % 2 == 0) absV1 = -absV1 / 2;
	else absV1 = (absV1 + 1) / 2;
	mvx = absV1;

	absV1 = 0;
	absV2 = 0;
	k = 1;
	while (acodec.decode(aGolomb))
	{
		absV1 += (1 << k);
		k++;
	}
	for (int i = 0; i < k; i++)
	{
		absV2 = 2 * absV2 + acodec.decode(aGolomb);
	}
	absV1 += absV2;
	if (absV1 % 2 == 0) absV1 = -absV1 / 2;
	else absV1 = (absV1 + 1) / 2;
	mvy = absV1;

	if (acodec.decode(aNoResidue))
	{
		for (int i = 0; i < m_height; i++)
		{
			for (int j = 0; j < m_width; j++)
			{
				pImg[(i + location_row)*pic_width + j + location_col] = ref_frame[(i + location_row + mvy)*pic_width + j + location_col + mvx];
			}
		}
	}
	else
	{
		unsigned char* value = new unsigned char[m_height*m_width];
		for (int i = 0; i < m_height; i++)
		{
			for (int j = 0; j < m_width; j++)
			{
				value[i * m_width + j] = ref_frame[(i + location_row + mvy)*pic_width + j + location_col + mvx];
			}
		}
		m_value = new unsigned char[m_height*m_width];

		bool two_color = true;
		int color1 = value[0];
		int color2 = -1;
		for (int i = 1; i < m_height; i++)
		{
			if (value[i*m_width] != color1 && color2 < 0) color2 = value[i*m_width];
			if (value[i*m_width] != color1 && value[i*m_width] != color2) two_color = false;
		}
		for (int j = 1; j < m_width; j++)
		{
			if (value[j] != color1 && color2 < 0) color2 = value[j];
			if (value[j] != color1 && value[j] != color2) two_color = false;
		}
		for (int j = 1; j < m_width; j++)
		{
			if (value[(m_height - 1)*m_width + j] != color1 && color2 < 0) color2 = value[(m_height - 1)*m_width + j];
			if (value[(m_height - 1)*m_width + j] != color1 && value[(m_height - 1)*m_width + j] != color2) two_color = false;
		}
		for (int i = 1; i < m_height - 1; i++)
		{
			if (value[i*m_width + m_width - 1] != color1 && color2 < 0) color2 = value[i*m_width + m_width - 1];
			if (value[i*m_width + m_width - 1] != color1 && value[i*m_width + m_width - 1] != color2) two_color = false;
		}

		int change_num = 0;
		int change_index = 0;
		if (!two_color)
		{
			for (int i = 1; i < m_height; i++)
			{
				if (value[i*m_width] != value[(i-1)*m_width]) change_num++;
			}
			for (int j = 1; j < m_width; j++)
			{
				if (value[j] != value[j-1]) change_num++;
			}
			for (int j = 1; j < m_width; j++)
			{
				if (value[(m_height - 1)*m_width + j] != value[(m_height - 1)*m_width + j - 1]) change_num++;
			}
			for (int i = 1; i < m_height - 1; i++)
			{
				if (value[i*m_width + m_width - 1] != value[(i-1)*m_width + m_width - 1]) change_num++;
			}
			Static_Data_Model Index(change_num);
			change_index = acodec.decode(Index);
		}

		int x = 0, y = 0;
		change_num = 0;
		bool find_change = false;
		for (int i = 1; i < m_height; i++)
		{
			if (value[i*m_width] != value[(i - 1)*m_width])
			{
				if (change_num == change_index)
				{
					color2 = value[i*m_width];
					color1 = value[(i - 1)*m_width];
					x = 0; y = i; find_change = true;
				}
				change_num++;
			}
		}
		if(!find_change)
		for (int j = 1; j < m_width; j++)
		{
			if (value[j] != value[j - 1])
			{
				if (change_num == change_index)
				{
					color2 = value[j];
					color1 = value[j - 1];
					x = j; y = 0; find_change = true;
				}
				change_num++;
			}
		}
		if (!find_change)
		for (int j = 1; j < m_width; j++)
		{
			if (value[(m_height - 1)*m_width + j] != value[(m_height - 1)*m_width + j - 1])
			{
				if (change_num == change_index)
				{
					color2 = value[(m_height - 1)*m_width + j];
					color1 = value[(m_height - 1)*m_width + j - 1];
					x = j; y = m_height - 1; find_change = true;
				}
				change_num++;
			}
		}
		if (!find_change)
		for (int i = 1; i < m_height - 1; i++)
		{
			if (value[i*m_width + m_width - 1] != value[(i - 1)*m_width + m_width - 1])
			{
				if (change_num == change_index)
				{
					color2 = value[i*m_width + m_width - 1];
					color1 = value[(i - 1)*m_width + m_width - 1];
					x = m_width - 1; y = i; find_change = true;
				}
				change_num++;
			}
		}

		int color3;
		for (int i = 0; i < 3; i++)
		{
			if (i != color1 && i != color2) color3 = i;
		}
		for (int i = 0; i < m_height*m_width; i++)
		{
			m_value[i] = color3;
		}

		int initial_loc_y = y, initial_loc_x = x;

		int* ChainRefPath = new int[(m_height + 1) * (m_width + 1)];
		for (int i = 0; i < (m_height + 1) * (m_width + 1); i++) ChainRefPath[i] = -1;
		int direction;
		if (initial_loc_x == 0)
		{
			direction = 0; x++;
		}
		else if (initial_loc_y == 0)
		{
			direction = 3; y++;
		}
		else if (initial_loc_y == m_height - 1) direction = 1;
		else direction = 2;
		while (!((x == m_width) || (y == 0) || (x == 0) || (y == m_height)))
		{
			switch (direction)
			{
			case 0:
				if (initial_loc_x == 0 || initial_loc_y == m_height - 1)
				{
					if (value[y*m_width +x] != color2) direction = 3;
					else if (value[(y - 1)*m_width + x] != color2) direction = 0;
					else direction = 1;
				}
				else
				{
					if (value[(y - 1)*m_width + x] != color2) direction = 1;
					else if (value[y*m_width + x] != color2) direction = 0;
					else direction = 3;
				}
				break;
			case 1:
				if (initial_loc_x == 0 || initial_loc_y == m_height - 1)
				{
					if (value[(y - 1)*m_width + x] != color2) direction = 0;
					else if (value[(y - 1)*m_width + x - 1] != color2) direction = 1;
					else direction = 2;
				}
				else
				{
					if (value[(y - 1)*m_width + x - 1] != color2) direction = 2;
					else if (value[(y - 1)*m_width + x] != color2) direction = 1;
					else direction = 0;
				}
				break;
			case 2:
				if (initial_loc_x == 0 || initial_loc_y == m_height - 1)
				{
					if (value[(y - 1)*m_width + x - 1] != color2) direction = 1;
					else if (value[y*m_width + x - 1] != color2) direction = 2;
					else direction = 3;
				}
				else
				{
					if (value[y*m_width + x - 1] != color2) direction = 3;
					else if (value[(y - 1)*m_width + x - 1] != color2) direction = 2;
					else direction = 1;
				}
				break;
			case 3:
				if (initial_loc_x == 0 || initial_loc_y == m_height - 1)
				{
					if (value[y*m_width + x - 1] != color2) direction = 2;
					else if (value[y*m_width + x] != color2) direction = 3;
					else direction = 0;
				}
				else
				{
					if (value[y*m_width + x] != color2) direction = 0;
					else if (value[y*m_width + x - 1] != color2) direction = 3;
					else direction = 2;
				}
			}
			ChainRefPath[y * (m_width + 1) + x] = direction;
			switch (direction)
			{
			case 0: x++; break;
			case 1: y--; break;
			case 2: x--; break;
			case 3: y++;
			}
		}

		int OT3_0 = -1;
		int OT3_1 = -1;
		int OT3_2 = -1;
		int OT3;
		int mode = 1;
		if (initial_loc_x == 0 || initial_loc_y == m_height - 1) mode = 0;
		if ((initial_loc_x == 0 && location_col > 0) || (initial_loc_y == 0 && location_row > 0))
		{
			int edgeline[3];
			get_context(pImg, location_row, location_col, pic_width, m_width, initial_loc_y, initial_loc_x, color2, edgeline, &context, &mode);
			for (int i = 0; i < context; i++)
			{
				update_context(&OT3_0, &OT3_1, &OT3_2, edgeline[i]);
			}
		}


		y = initial_loc_y; x = initial_loc_x;

		if (initial_loc_x == 0)
		{
			direction = 0; x++;
			m_value[y * m_width + x - 1] = color2;
		}
		else if (initial_loc_y == 0)
		{
			direction = 3; y++;
			m_value[(y - 1) * m_width + x] = color2;
		}
		else if (initial_loc_y == m_height - 1)
		{
			direction = 1;
			m_value[y * m_width + x] = color2;
		}
		else
		{
			direction = 2;
			m_value[y * m_width + x] = color2;
		}

		int index = context;
		int forbidden = -1;
		int forbid_direction = 0;
		unsigned char* ChainPath = new unsigned char[(m_height + 1) * (m_width + 1)];
		for (int i = 0; i < (m_height + 1) * (m_width + 1); i++) ChainPath[i] = 0;
		bool reverse = false;
		do
		{
			index += 1;
			forbidden = -1;
			if (initial_loc_x == 0)
			{
				if (x == 1 && y < initial_loc_y)
				{
					if (direction == 1) forbidden = 2;
					if (direction == 2) { forbidden = 4; forbid_direction = 1; }
				}
			}
			else if (initial_loc_y == 0)
			{
				if (x == 1)
				{
					if (direction == 2) { forbidden = 4; forbid_direction = 3; }
					if (direction == 3) forbidden = 2;
				}
				else if (x < initial_loc_x && y == 1)
				{
					if (direction == 1) { forbidden = 4; forbid_direction = 2; }
					if (direction == 2) forbidden = 1;
				}
			}
			else if (initial_loc_y == m_height - 1)
			{
				if (x == 1)
				{
					if (direction == 1) forbidden = 2;
					if (direction == 2) { forbidden = 4; forbid_direction = 1; }
				}
				else if (y == 1)
				{
					if (direction == 0) forbidden = 1;
					if (direction == 1) { forbidden = 4; forbid_direction = 0; }
				}
				else if (x < initial_loc_x && y == m_height - 1)
				{
					if (direction == 2) forbidden = 3;
					if (direction == 3) { forbidden = 4; forbid_direction = 2; }
				}
			}
			else
			{
				if (x == 1)
				{
					if (direction == 2) { forbidden = 4; forbid_direction = 3; }
					if (direction == 3) forbidden = 2;
				}
				else if (y == 1)
				{
					if (direction == 1) { forbidden = 4; forbid_direction = 2; }
					if (direction == 2) forbidden = 1;
				}
				else if (y == m_height - 1)
				{
					if (direction == 0) forbidden = 3;
					if (direction == 3) { forbidden = 4; forbid_direction = 0; }
				}
				else if (x == m_width - 1 && y < initial_loc_y)
				{
					if (direction == 0) { forbidden = 4; forbid_direction = 1; }
					if (direction == 1) forbidden = 0;
				}
			}

			ChainPath[y * (m_width + 1) + x] = 1;
			int forbidden1, forbidden2;
			for (int i = 0; i < 4; i++)
			{
				if (i == 2) continue;
				else if (i == 0) { forbidden1 = (direction + 1) % 4; forbidden2 = (direction + 3) % 4; }
				else if (i == 1) { forbidden1 = direction; forbidden2 = (direction + 3) % 4; }
				else { forbidden1 = direction; forbidden2 = (direction + 1) % 4; }
				switch ((direction + i) % 4)
				{
				case 0: if (ChainPath[y * (m_width + 1) + x + 1] == 1) { if (forbidden == forbidden1 || forbidden == forbidden2) { if (forbidden == forbidden1) forbid_direction = forbidden2; else forbid_direction = forbidden1; forbidden = 4; } else if (forbidden != 4) forbidden = 0; }break;
				case 1: if (ChainPath[(y - 1) * (m_width + 1) + x] == 1) { if (forbidden == forbidden1 || forbidden == forbidden2) { if (forbidden == forbidden1) forbid_direction = forbidden2; else forbid_direction = forbidden1; forbidden = 4; } else if (forbidden != 4) forbidden = 1; }break;
				case 2: if (ChainPath[y * (m_width + 1) + x - 1] == 1) { if (forbidden == forbidden1 || forbidden == forbidden2) { if (forbidden == forbidden1) forbid_direction = forbidden2; else forbid_direction = forbidden1; forbidden = 4; } else if (forbidden != 4) forbidden = 2; }break;
				case 3: if (ChainPath[(y + 1) * (m_width + 1) + x] == 1) { if (forbidden == forbidden1 || forbidden == forbidden2) { if (forbidden == forbidden1) forbid_direction = forbidden2; else forbid_direction = forbidden1; forbidden = 4; } else if (forbidden != 4) forbidden = 3; }
				}
			}

			if (forbidden == 4)
			{
				if (forbid_direction - direction == 1 || forbid_direction - direction == -3)  //left
				{
					if (mode == 0) OT3 = 2;
					if (mode == 1) OT3 = 1;
				}
				else if (forbid_direction - direction == -1 || forbid_direction - direction == 3) //right
				{
					if (mode == 1) OT3 = 2;
					if (mode == 0) OT3 = 1;
				}
				else
				{
					OT3 = 0;
				}
				if (OT3 == 1) mode = 1 - mode;
				direction = forbid_direction;
				switch (direction)
				{
				case 0:x++; if (initial_loc_x == 0 || initial_loc_y == m_height - 1) m_value[y * m_width + x - 1] = color2; else m_value[(y - 1) * m_width + x - 1] = color2; break;
				case 1:y--; if (initial_loc_x == 0 || initial_loc_y == m_height - 1) m_value[y * m_width + x] = color2; else m_value[y * m_width + x - 1] = color2; break;
				case 2:x--; if (initial_loc_x == 0 || initial_loc_y == m_height - 1) m_value[(y - 1) * m_width + x] = color2; else m_value[y * m_width + x] = color2; break;
				case 3:y++; if (initial_loc_x == 0 || initial_loc_y == m_height - 1) m_value[(y - 1) * m_width + x - 1] = color2; else m_value[(y - 1) * m_width + x] = color2;
				}
			}
			else
			{
				if (forbidden >= 0)
				{
					if (forbidden - direction == 1 || forbidden - direction == -3)  //left
					{
						if (mode == 0) forbidden = 2;
						if (mode == 1) forbidden = 1;
					}
					else if (forbidden - direction == -1 || forbidden - direction == 3) //right
					{
						if (mode == 1) forbidden = 2;
						if (mode == 0) forbidden = 1;
					}
					else
					{
						forbidden = 0;
					}
				}

				bool NoNeedDecode = false;
				if (ChainRefPath[y * (m_width + 1) + x] >= 0) 
				{
					NoNeedDecode = acodec.decode(aSameFlag);
					int newdirection = ChainRefPath[y * (m_width + 1) + x];
					if (newdirection - direction == 1 || newdirection - direction == -3)  //left
					{
						if (mode == 0) OT3 = 2;
						if (mode == 1) OT3 = 1;
					}
					else if (newdirection - direction == -1 || newdirection - direction == 3) //right
					{
						if (mode == 1) OT3 = 2;
						if (mode == 0) OT3 = 1;
					}
					else if(newdirection == direction) OT3 = 0;
					else OT3 = -1;
					if (!NoNeedDecode)
					{
						if (forbidden != -1)
						{
							if (OT3 != -1)
							{
								if (forbidden == OT3) forbidden = OT3;
								else
								{
									switch (forbidden)
									{
									case 0: if (OT3 == 1) OT3 = 2; else OT3 = 1; break;
									case 1: if (OT3 == 0) OT3 = 2; else OT3 = 0; break;
									case 2: if (OT3 == 0) OT3 = 1; else OT3 = 0;
									}
									forbidden = 4;
								}
							}
						}
						else forbidden = OT3;
					}
				}
				if(!NoNeedDecode && forbidden!=4)
				{

					if (index == 1)
					{
						if (initial_loc_x == 0) OT3 = acodec.decode(a3OT_L, forbidden);
						else if (initial_loc_y == 0) OT3 = acodec.decode(a3OT_A, forbidden);
						else if (initial_loc_y == m_height - 1) OT3 = acodec.decode(a3OT_B, forbidden);
						else OT3 = acodec.decode(a3OT_R, forbidden);
					}
					else if (index == 2)
					{
						switch (OT3_0)
						{
						case 0:OT3 = acodec.decode(a3OT_0, forbidden); break;
						case 1:OT3 = acodec.decode(a3OT_1, forbidden); break;
						case 2:OT3 = acodec.decode(a3OT_2, forbidden);
						}
					}
					else if (index == 3)
					{
						switch (OT3_0 * 3 + OT3_1)
						{
						case 0:OT3 = acodec.decode(a3OT_00, forbidden); break;
						case 1:OT3 = acodec.decode(a3OT_01, forbidden); break;
						case 2:OT3 = acodec.decode(a3OT_02, forbidden); break;
						case 3:OT3 = acodec.decode(a3OT_10, forbidden); break;
						case 4:OT3 = acodec.decode(a3OT_11, forbidden); break;
						case 5:OT3 = acodec.decode(a3OT_12, forbidden); break;
						case 6:OT3 = acodec.decode(a3OT_20, forbidden); break;
						case 7:OT3 = acodec.decode(a3OT_21, forbidden); break;
						case 8:OT3 = acodec.decode(a3OT_22, forbidden);
						}
					}
					else
					{
						switch (OT3_0 * 9 + OT3_1 * 3 + OT3_2)
						{
						case 0:OT3 = acodec.decode(a3OT_000, forbidden); break;
						case 1:OT3 = acodec.decode(a3OT_001, forbidden); break;
						case 2:OT3 = acodec.decode(a3OT_002, forbidden); break;
						case 3:OT3 = acodec.decode(a3OT_010, forbidden); break;
						case 4:OT3 = acodec.decode(a3OT_011, forbidden); break;
						case 5:OT3 = acodec.decode(a3OT_012, forbidden); break;
						case 6:OT3 = acodec.decode(a3OT_020, forbidden); break;
						case 7:OT3 = acodec.decode(a3OT_021, forbidden); break;
						case 8:OT3 = acodec.decode(a3OT_022, forbidden); break;
						case 9:OT3 = acodec.decode(a3OT_100, forbidden); break;
						case 10:OT3 = acodec.decode(a3OT_101, forbidden); break;
						case 11:OT3 = acodec.decode(a3OT_102, forbidden); break;
						case 12:OT3 = acodec.decode(a3OT_110, forbidden); break;
						case 13:OT3 = acodec.decode(a3OT_111, forbidden); break;
						case 14:OT3 = acodec.decode(a3OT_112, forbidden); break;
						case 15:OT3 = acodec.decode(a3OT_120, forbidden); break;
						case 16:OT3 = acodec.decode(a3OT_121, forbidden); break;
						case 17:OT3 = acodec.decode(a3OT_122, forbidden); break;
						case 18:OT3 = acodec.decode(a3OT_200, forbidden); break;
						case 19:OT3 = acodec.decode(a3OT_201, forbidden); break;
						case 20:OT3 = acodec.decode(a3OT_202, forbidden); break;
						case 21:OT3 = acodec.decode(a3OT_210, forbidden); break;
						case 22:OT3 = acodec.decode(a3OT_211, forbidden); break;
						case 23:OT3 = acodec.decode(a3OT_212, forbidden); break;
						case 24:OT3 = acodec.decode(a3OT_220, forbidden); break;
						case 25:OT3 = acodec.decode(a3OT_221, forbidden); break;
						case 26:OT3 = acodec.decode(a3OT_222, forbidden);
						}
					}
				}
				if (OT3 == 0)
				{
					switch (direction)
					{
					case 0:x++; if (initial_loc_x == 0 || initial_loc_y == m_height - 1) m_value[y * m_width + x - 1] = color2; else m_value[(y - 1) * m_width + x - 1] = color2; break;
					case 1:y--; if (initial_loc_x == 0 || initial_loc_y == m_height - 1) m_value[y * m_width + x] = color2; else m_value[y * m_width + x - 1] = color2; break;
					case 2:x--; if (initial_loc_x == 0 || initial_loc_y == m_height - 1) m_value[(y - 1) * m_width + x] = color2; else m_value[y * m_width + x] = color2; break;
					case 3:y++; if (initial_loc_x == 0 || initial_loc_y == m_height - 1) m_value[(y - 1) * m_width + x - 1] = color2; else m_value[(y - 1) * m_width + x] = color2;
					}
				}
				else
				{
					if (OT3 == 1) mode = 1 - mode;
					if (mode == 0)
					{
						switch (direction)
						{
						case 0:direction = 1; y--; if (initial_loc_x == 0 || initial_loc_y == m_height - 1) m_value[y * m_width + x] = color2; else m_value[y * m_width + x - 1] = color2; break;
						case 1:direction = 2; x--; if (initial_loc_x == 0 || initial_loc_y == m_height - 1) m_value[(y - 1) * m_width + x] = color2; else m_value[y * m_width + x] = color2; break;
						case 2:direction = 3; y++; if (initial_loc_x == 0 || initial_loc_y == m_height - 1) m_value[(y - 1) * m_width + x - 1] = color2; else m_value[(y - 1) * m_width + x] = color2; break;
						case 3:direction = 0; x++; if (initial_loc_x == 0 || initial_loc_y == m_height - 1) m_value[y * m_width + x - 1] = color2; else m_value[(y - 1) * m_width + x - 1] = color2;
						}
					}
					else
					{
						switch (direction)
						{
						case 0:direction = 3; y++; if (initial_loc_x == 0 || initial_loc_y == m_height - 1) m_value[(y - 1) * m_width + x - 1] = color2; else m_value[(y - 1) * m_width + x] = color2; break;
						case 1:direction = 0; x++; if (initial_loc_x == 0 || initial_loc_y == m_height - 1) m_value[y * m_width + x - 1] = color2; else m_value[(y - 1) * m_width + x - 1] = color2; break;
						case 2:direction = 1; y--; if (initial_loc_x == 0 || initial_loc_y == m_height - 1) m_value[y * m_width + x] = color2; else m_value[y * m_width + x - 1] = color2; break;
						case 3:direction = 2; x--; if (initial_loc_x == 0 || initial_loc_y == m_height - 1) m_value[(y - 1) * m_width + x] = color2; else m_value[y * m_width + x] = color2;
						}
					}
				}
			}
			update_context(&OT3_0, &OT3_1, &OT3_2, OT3);
			//printf("%d ",OT3);
			
		} while (!(x == m_width || y == 0 || x == 0 || y == m_height));
		//printf("\n");
		advancedExpand(m_value, m_height, m_width, color1, 0, 0);
		for (int i = 0; i < m_height*m_width; i++)
		{
			if (m_value[i] == color3)
				m_value[i] = color2;
		}
		for (int i = 0; i < m_height; i++)
		{
			for (int j = 0; j < m_width; j++)
			{
				pImg[(i + location_row)*pic_width + j + location_col] = m_value[i * m_width + j];
			}
		}

		delete[] value;
		delete[] ChainPath;
		delete[] ChainRefPath;
	}
}

void CodingUnit::update(int * element, int index, int forbidden, int forbidden2, int forbidden3)
{
	if (forbidden >= 0)
	{
		if (forbidden2 >= 0)
		{
			if (forbidden3 >= 0)
			{
				float p = float(element[index + 1]) / (element[0] - element[forbidden + 1] - element[forbidden2 + 1] - element[forbidden3 + 1]);
				rate -= log2(p);
				element[0]++;
				element[index + 1]++;
			}
			else
			{
				float p = float(element[index + 1]) / (element[0] - element[forbidden + 1] - element[forbidden2 + 1]);
				rate -= log2(p);
				element[0]++;
				element[index + 1]++;
			}
		}
		else
		{
			float p = float(element[index + 1]) / (element[0] - element[forbidden + 1]);
			rate -= log2(p);
			element[0]++;
			element[index + 1]++;
		}
	}
	else
	{
		float p = float(element[index + 1]) / element[0];
		rate -= log2(p);
		element[0]++;
		element[index + 1]++;
	}
}

