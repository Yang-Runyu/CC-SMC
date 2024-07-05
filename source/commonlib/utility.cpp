#include "utility.h"
#include <iostream>  
#include "CodingUnit.h"
using namespace std;

bool isOneColor(unsigned char* value, int height, int width)      //whether is one color. Input a vector.
{
	int pixel_value = value[0];
	for (int i = 0; i < height*width; i++)
	{
		if (value[i] != pixel_value)
		{
			return false;
		}
	}
	return true;
}
//flooding algorithm
void advancedExpand2(unsigned char* img, int height, int width, int color1, int color2_y, int color2_x)    //based on queue, can deal with g_max_CU =256, NO SUPPOERT 512
{
	int color2 = img[color2_y*width + color2_x];
	img[color2_y*width + color2_x] = color1;
	unsigned short indexStack[100000];
	int p = 0, q = 0;
	if (color2_y - 1 >= 0 && img[(color2_y - 1)*width + color2_x] == color2)
	{
		indexStack[p++] = color2_y - 1; indexStack[p++] = color2_x;
	}
	if (color2_y + 1 < height && img[(color2_y + 1)*width + color2_x] == color2)
	{
		indexStack[p++] = color2_y + 1; indexStack[p++] = color2_x;
	}
	if (color2_x - 1 >= 0 && img[color2_y*width + color2_x - 1] == color2)
	{
		indexStack[p++] = color2_y; indexStack[p++] = color2_x - 1;
	}
	if (color2_x + 1 < width && img[color2_y*width + color2_x + 1] == color2)
	{
		indexStack[p++] = color2_y; indexStack[p++] = color2_x + 1;
	}
	while (p != q)
	{
		color2_y = indexStack[q++];
		color2_x = indexStack[q++];
		if (q == 100000) q = 0;
		if (img[color2_y*width + color2_x] == color2)
		{
			img[color2_y*width + color2_x] = color1;
			if (color2_y - 1 >= 0 && img[(color2_y - 1)*width + color2_x] == color2)
			{
				indexStack[p++] = color2_y - 1;
				indexStack[p++] = color2_x;
				if (p == 100000) p = 0;
			}
			if (color2_y + 1 < height && img[(color2_y + 1)*width + color2_x] == color2)
			{
				indexStack[p++] = color2_y + 1;
				indexStack[p++] = color2_x;
				if (p == 100000) p = 0;
			}
			if (color2_x - 1 >= 0 && img[color2_y*width + color2_x - 1] == color2)
			{
				indexStack[p++] = color2_y;
				indexStack[p++] = color2_x - 1;
				if (p == 100000) p = 0;
			}
			if (color2_x + 1 < width && img[color2_y*width + color2_x + 1] == color2)
			{
				indexStack[p++] = color2_y;
				indexStack[p++] = color2_x + 1;
				if (p == 100000) p = 0;
			}
		}
	}
}

void advancedExpand(unsigned char* img, int height, int width, int color1, int color2_y, int color2_x) 
{
	unsigned short indexStack[1000];
	int color2 = img[color2_y*width + color2_x];
	bool reachLeft = false;
	bool reachRight = false;
	int p = 0;
	while (color2_y >=0 && img[color2_y*width + color2_x] == color2)
	{
		color2_y--;
	}
	indexStack[p++] = color2_y + 1; indexStack[p++] = color2_x;
	while (p > 0)
	{
		color2_x = indexStack[--p];
		color2_y = indexStack[--p];
		while (color2_y >= 0 && img[color2_y*width + color2_x] == color2)
		{
			color2_y--;
		}
		color2_y++;
		reachLeft = false;
		reachRight = false;
		for (int y = color2_y; y < height; y++)
		{
			if (img[y*width + color2_x] != color2) break;
			img[y*width + color2_x] = color1;
			if (color2_x > 0)
			{
				if (!reachLeft && img[y*width + color2_x - 1] == color2)
				{
					reachLeft = true;
					indexStack[p++] = y; indexStack[p++] = color2_x - 1;
				}
				else if (reachLeft && img[y*width + color2_x - 1] != color2)
				{
					reachLeft = false;
				}
			}
			if (color2_x < width - 1)
			{
				if (!reachRight && img[y*width + color2_x + 1] == color2)
				{
					reachRight = true;
					indexStack[p++] = y; indexStack[p++] = color2_x + 1;
				}
				else if (reachRight && img[y*width + color2_x + 1] != color2)
				{
					reachRight = false;
				}
			}
		}
	}
}


bool isContiEdge(unsigned char * value, int height, int width, int pic_width)
{  /*Whether CU only have two colors and a continue edge*/
	int color1 = value[0];
	int color2 = -1;
	int i = 0;
	unsigned char* img = new unsigned char[height*width];

	for (i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			img[i*width + j] = value[i*pic_width + j];
		}
	}

	for (i = 0; i < height*width; i++)  //get color2
	{
		if (color1 != img[i])
		{
			color2 = img[i];
			break;
		}
	}
	if (color2 == -1) return false;
	int y = i / width;
	int x = i % width;
	for (i = y; i < height; i++)  //whether have color3
	{
		for (int j = 0; j < width; j++)
		{
			if (img[i*width + j] != color1 && img[i*width + j] != color2)
				return false;
		}
	}
	//have two colors	 

	bool contained = true;
	for (i = 0; i < height; i++)                    //whether color2 is totally contained in color1 
	{
		for (int j = 0; j < width; j++)
		{
			if ((i == 0 || j == 0 || i == height - 1 || j == width - 1) && img[i*width + j] == color2)
			{
				contained = false;
			}
		}
	}
	if (contained == true) return false;


	advancedExpand(img, height, width, color1, y, x);      //fill color2 with color1
	bool expandColor1 = true;
	for (i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			if (img[i*width + j] == color2)
			{
				expandColor1 = false;
			}
		}
	}
	for (i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			img[i*width + j] = value[i*pic_width + j];
		}
	}
	advancedExpand(img, height, width, color2, 0, 0);     //fill color1 with color2
	bool expandColor2 = true;
	for (i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			if (img[i*width + j] == color1)
			{
				expandColor2 = false;
			}
		}
	}
	delete[] img;
	return expandColor1 && expandColor2;
}

void update_context(int* OT3_0, int* OT3_1, int* OT3_2, int OT3)
{
	bool full_context = true;
	if (*OT3_2 == -1) full_context = false;
	if (full_context)
	{
		*OT3_0 = *OT3_1;
		*OT3_1 = *OT3_2;
		*OT3_2 = OT3;
	}
	else
	{
		if (*OT3_0 == -1) *OT3_0 = OT3;
		else if (*OT3_1 == -1) *OT3_1 = OT3;
		else *OT3_2 = OT3;
	}
}

bool checkTwoColor(unsigned char * value, int height, int width, int pic_width, int initial_y, int initial_x, int color1, int color2, bool*two_color)
{
	*two_color = true;
	for (int i = 1; i < height; i++)
	{
		if (value[i*pic_width] != color1 && value[i*pic_width] != color2) { *two_color = false;  return true; }
	}
	for (int j = 1; j < width; j++)
	{
		if (value[j] != color1 && value[j] != color2) { *two_color = false;  return true; }
	}
	for (int j = 1; j < width; j++)
	{
		if (value[(height - 1)*pic_width + j] != color1 && value[(height - 1)*pic_width + j] != color2) { *two_color = false;  return true; }
	}
	for (int i = 1; i < height - 1; i++)
	{
		if (value[i*pic_width + width - 1] != color1 && value[i*pic_width + width - 1] != color2) { *two_color = false;  return true; }
	}

	for (int i = 1; i < height; i++)
	{
		if (value[i*pic_width] != value[(i - 1)*pic_width])
		{
			if(i == initial_y && initial_x == 0) return true;
			else return false;
		}
	}
	for (int j = 1; j < width; j++)
	{
		if (value[j] != value[j-1]) 
		{
			if (initial_y == 0 && initial_x == j) return true;
			else return false;
		}
	}
	for (int j = 1; j < width; j++)
	{
		if (value[(height - 1)*pic_width + j] != value[(height - 1)*pic_width + j - 1]) 
		{
			if (initial_y == height - 1 && initial_x == j) return true;
			else return false;
		}
	}
	for (int i = 1; i < height - 1; i++)
	{
		if (value[i*pic_width + width - 1] != value[(i-1)*pic_width + width - 1]) 
		{
			if (i == initial_y && initial_x == width - 1) return true;
			else return false;
		}
	}
}

void checkChange(unsigned char * value, int height, int width, int pic_width, int initial_y, int initial_x, int color1, int color2, int*change_num, int*index)
{
	*change_num = 0;
	*index = 0;
	for (int i = 1; i < height; i++)
	{
		if (value[i*pic_width] != value[(i - 1)*pic_width])
		{
			if (i == initial_y && initial_x == 0) *index = *change_num;
			*change_num += 1;
		}
	}
	for (int j = 1; j < width; j++)
	{
		if (value[j] != value[j - 1])
		{
			if (initial_y == 0 && initial_x == j) *index = *change_num;
			*change_num += 1;
		}
	}
	for (int j = 1; j < width; j++)
	{
		if (value[(height - 1)*pic_width + j] != value[(height - 1)*pic_width + j - 1])
		{
			if (initial_y == height - 1 && initial_x == j) *index = *change_num;
			*change_num += 1;
		}
	}
	for (int i = 1; i < height - 1; i++)
	{
		if (value[i*pic_width + width - 1] != value[(i - 1)*pic_width + width - 1])
		{
			if (i == initial_y && initial_x == width - 1) *index = *change_num;
			*change_num += 1;
		}
	}
}

bool isSame(unsigned char * value, int height, int width, int loc_row, int loc_col, int pic_width)
{
	for (int h = 0; h < height; h++)
	{
		for (int w = 0; w < width; w++)
		{
			if (value[h*width + w] != ref_frame[(loc_row + h)*pic_width + loc_col + w]) return false;
		}
	}
	return true;
}

bool canPredict(unsigned char * value, int height, int width, int loc_row, int loc_col, int pic_width, CodingUnit* pcCU)
{
	int* edge = pcCU->getEdge();
	int color1 = edge[0];
	int initial_y = edge[1];
	int initial_x = edge[2];
	int color2 = edge[3];
	int edgeLength = edge[4] - pcCU->context;

	if (initial_x == 0)
	{
		if (ref_frame[(loc_row + initial_y) *pic_width + loc_col + initial_x] != color2 || ref_frame[(loc_row + initial_y - 1) *pic_width + loc_col + initial_x] != color1) return false;
	}
	else if (initial_y == 0)
	{
		if (ref_frame[(loc_row + initial_y) *pic_width + loc_col + initial_x] != color2 || ref_frame[(loc_row + initial_y) *pic_width + loc_col + initial_x - 1] != color1) return false;
	}
	else if (initial_y == height - 1)
	{
		if (ref_frame[(loc_row + initial_y) *pic_width + loc_col + initial_x] != color2 || ref_frame[(loc_row + initial_y) *pic_width + loc_col + initial_x - 1] != color1) return false;
	}
	else
	{
		if (ref_frame[(loc_row + initial_y) *pic_width + loc_col + initial_x] != color2 || ref_frame[(loc_row + initial_y - 1) *pic_width + loc_col + initial_x] != color1) return false;
	}
	bool two_color;
	if(!checkTwoColor(ref_frame + loc_row * pic_width + loc_col, height, width, pic_width, initial_y, initial_x, color1, color2, &two_color)) return false;

	pcCU->difEdge = new int[2 + 3 * edgeLength];
	int* ChainPath = new int[(height + 1) * (width + 1)];
	for (int i = 0; i < (height + 1) * (width + 1); i++) ChainPath[i] = -1;

	if (two_color) pcCU->difEdge[0] = 0;
	else checkChange(ref_frame + loc_row * pic_width + loc_col, height, width, pic_width, initial_y, initial_x, color1, color2, pcCU->difEdge, pcCU->difEdge + 1);

	int x = initial_x, y = initial_y;
	int direction;
	if (initial_x == 0)
	{
		direction = 0; x++;
	}
	else if (initial_y == 0)
	{
		direction = 3; y++;
	}
	else if (initial_y == height - 1) direction = 1;
	else direction = 2;
	while (!((x == width) || (y == 0) || (x == 0) || (y == height)))
	{
		switch (direction)
		{
		case 0:
			if (initial_x == 0 || initial_y == height - 1)
			{
				if (ref_frame[(y + loc_row)*pic_width + loc_col + x] != color2) direction = 3;
				else if (ref_frame[(y - 1 + loc_row)*pic_width + loc_col + x] != color2) direction = 0;
				else direction = 1;
			}
			else
			{
				if (ref_frame[(y - 1 + loc_row)*pic_width + loc_col + x] != color2) direction = 1;
				else if (ref_frame[(y + loc_row)*pic_width + loc_col + x] != color2) direction = 0;
				else direction = 3;
			}
			break;
		case 1:
			if (initial_x == 0 || initial_y == height - 1)
			{
				if (ref_frame[(y - 1 + loc_row)*pic_width + loc_col + x] != color2) direction = 0;
				else if (ref_frame[(y - 1 + loc_row)*pic_width + loc_col + x - 1] != color2) direction = 1;
				else direction = 2;
			}
			else
			{
				if (ref_frame[(y - 1 + loc_row)*pic_width + loc_col + x - 1] != color2) direction = 2;
				else if (ref_frame[(y - 1 + loc_row)*pic_width + loc_col + x] != color2) direction = 1;
				else direction = 0;
			}
			break;
		case 2:
			if (initial_x == 0 || initial_y == height - 1)
			{
				if (ref_frame[(y - 1 + loc_row)*pic_width + loc_col + x - 1] != color2) direction = 1;
				else if (ref_frame[(y + loc_row)*pic_width + loc_col + x - 1] != color2) direction = 2;
				else direction = 3;
			}
			else
			{
				if (ref_frame[(y + loc_row)*pic_width + loc_col + x - 1] != color2) direction = 3;
				else if (ref_frame[(y - 1 + loc_row)*pic_width + loc_col + x - 1] != color2) direction = 2;
				else direction = 1;
			}
			break;
		case 3:
			if (initial_x == 0 || initial_y == height - 1)
			{
				if (ref_frame[(y + loc_row)*pic_width + loc_col + x - 1] != color2) direction = 2;
				else if (ref_frame[(y + loc_row)*pic_width + loc_col + x] != color2) direction = 3;
				else direction = 0;
			}
			else
			{
				if (ref_frame[(y + loc_row)*pic_width + loc_col + x] != color2) direction = 0;
				else if (ref_frame[(y + loc_row)*pic_width + loc_col + x - 1] != color2) direction = 3;
				else direction = 2;
			}
		}
		ChainPath[y * (width + 1) + x] = direction;
		switch (direction)
		{
		case 0: x++; break;
		case 1: y--; break;
		case 2: x--; break;
		case 3: y++;
		}
	}

	x = initial_x; y = initial_y;
	if (initial_x == 0)
	{
		direction = 0; x++;
	}
	else if (initial_y == 0)
	{
		direction = 3; y++;
	}
	else if (initial_y == height - 1) direction = 1;
	else direction = 2;
	int newdirection;
	int forbidden;
	int OT3mode = pcCU->mode;
	for (int i = 0; i < edgeLength; i++)
	{
		forbidden = -1;
		switch (edge[i + 5 + pcCU->context])
		{
		case 0: newdirection = direction; break;
		case 1: 
			if (OT3mode == 0) newdirection = (direction + 3) % 4;
			else newdirection = (direction + 1) % 4;
			break;
		case 2:
			if (OT3mode == 0) newdirection = (direction + 1) % 4;
			else newdirection = (direction + 3) % 4;
			break;
		}
		if (newdirection == ChainPath[y * (width + 1) + x])
		{
			pcCU->difEdge[2 + 3*i + 1] = 1;
		}
		else
		{
			if (ChainPath[y * (width + 1) + x] == -1)
			{
				pcCU->difEdge[2 + 3 * i + 1] = -1;
			}
			else
			{
				pcCU->difEdge[2 + 3 * i + 1] = 0;
				if (OT3mode)
				{
					if (ChainPath[y * (width + 1) + x] - direction == 1 || ChainPath[y * (width + 1) + x] - direction == -3) forbidden = 1;
					if (ChainPath[y * (width + 1) + x] - direction == -1 || ChainPath[y * (width + 1) + x] - direction == 3) forbidden = 2;
				}
				else
				{
					if (ChainPath[y * (width + 1) + x] - direction == 1 || ChainPath[y * (width + 1) + x] - direction == -3) forbidden = 2;
					if (ChainPath[y * (width + 1) + x] - direction == -1 || ChainPath[y * (width + 1) + x] - direction == 3) forbidden = 1;
				}
				if (ChainPath[y * (width + 1) + x] == direction) forbidden = 0;
			}
		}
		pcCU->difEdge[2 + 3 * i] = edge[i + 5 + pcCU->context];
		if (forbidden >= 0)
		{
			if (pcCU->ForbiddenLine[i + pcCU->context] == -1 || forbidden == pcCU->ForbiddenLine[i + pcCU->context]) pcCU->difEdge[2 + 3 * i + 2] = forbidden;
			else pcCU->difEdge[2 + 3 * i + 2] = 4;
		}
		else pcCU->difEdge[2 + 3 * i + 2] = pcCU->ForbiddenLine[i + pcCU->context];


		switch (newdirection)
		{
		case 0: x++; break;
		case 1: y--; break;
		case 2: x--; break;
		case 3: y++;
		}
		if(edge[i + 5 + pcCU->context] == 1) OT3mode = 1 - OT3mode;
		direction = newdirection;
	}

	delete[] ChainPath;
	return true;
}

bool isInter(unsigned char * value, int height, int width, int loc_row, int loc_col, int pic_height, int pic_width, CodingUnit* pcCU, bool ContiEdge)
{
	float rate = 1000000;
	int x_best = 0;
	int y_best = 0;
	int* difEdge_best = NULL;
	bool find = false;
	AC* element = new AC;
	element->copy(pcCU->element);

	for (int i = 0; i <= g_search_range; i++)
	{
		for (int x = i; x >= -i; x--)
		{
			int y = i - abs(x);
			if (loc_row + y >= 0 && loc_col + x >= 0 && loc_row + y + height <= pic_height && loc_col + x + width <= pic_width)
			{
				if (isSame(value, height, width, loc_row + y, loc_col + x, pic_width))
				{
					pcCU->mvx = x;
					pcCU->mvy = y;
					pcCU->noResidue = true;
					pcCU->enMV(false);
					if (pcCU->rate > rate)
					{
						pcCU->mvx = x_best;
						pcCU->mvy = y_best;
						pcCU->noResidue = false;
						pcCU->difEdge = difEdge_best;
					}
					delete element;
					return true;
				}
				if (ContiEdge && canPredict(value, height, width, loc_row + y, loc_col + x, pic_width, pcCU))
				{
					pcCU->mvx = x;
					pcCU->mvy = y;
					pcCU->noResidue = false;
					pcCU->enMV(false);
					if (pcCU->rate > rate)
					{
						pcCU->mvx = x_best;
						pcCU->mvy = y_best;
						pcCU->noResidue = false;
						delete[] pcCU->difEdge;
						pcCU->difEdge = NULL;
						pcCU->difEdge = difEdge_best;
					}
					else
					{
						rate = pcCU->rate;
						x_best = x;
						y_best = y;
						difEdge_best = pcCU->difEdge;
					}
					pcCU->rate = 0;
					pcCU->element->copy(element);
					find = true;
				}
			}
			if (y > 0)
			{
				y = -y;
				if (loc_row + y >= 0 && loc_col + x >= 0 && loc_row + y + height <= pic_height && loc_col + x + width <= pic_width)
				{
					if (isSame(value, height, width, loc_row + y, loc_col + x, pic_width))
					{
						pcCU->mvx = x;
						pcCU->mvy = y;
						pcCU->noResidue = true;
						pcCU->enMV(false);
						if (pcCU->rate > rate)
						{
							pcCU->mvx = x_best;
							pcCU->mvy = y_best;
							pcCU->noResidue = false;
							pcCU->difEdge = difEdge_best;
						}
					    delete element;
						return true;
					}
					if (ContiEdge && canPredict(value, height, width, loc_row + y, loc_col + x, pic_width, pcCU))
					{
						pcCU->mvx = x;
						pcCU->mvy = y;
						pcCU->noResidue = false;
						pcCU->enMV(false);
						if (pcCU->rate > rate)
						{
							pcCU->mvx = x_best;
							pcCU->mvy = y_best;
							pcCU->noResidue = false;
							delete[] pcCU->difEdge;
							pcCU->difEdge = NULL;
							pcCU->difEdge = difEdge_best;
						}
						else
						{
							rate = pcCU->rate;
							x_best = x;
							y_best = y;
							difEdge_best = pcCU->difEdge;
						}
						pcCU->rate = 0;
						pcCU->element->copy(element);
						find = true;
					}
				}
			}
		}
	}
	delete element;
	return find;
}


void draw_mask(unsigned char *mask, int picWidth, CodingUnit* pcCU, int ctuIdx)
{
	if (pcCU->split_flag)
	{
		for (int childIndex = 0; childIndex < 4; childIndex++)
			if (NULL != pcCU->child[childIndex]) draw_mask(mask, picWidth, pcCU->child[childIndex], ctuIdx);
	}
	else
	{
		int height = pcCU->getHeight();
		int width = pcCU->getWidth();
		int loc_y = pcCU->location_row;
		int loc_x = pcCU->location_col;
		for (int y = 0; y < height; y++)
		{
			for (int x = 0; x < width; x++)
			{
				if (y < 2 || x < 2 || y > height - 3 || x > width - 3)
					mask[(y + loc_y)*picWidth + x + loc_x] = 255;
			}
		}
		if (pcCU->contiEdge == 0 && pcCU->oneColor == 0 && height <= g_max_inter && height >= g_min_inter)
		{
			for (int y = 0; y < height; y++)
			{
				for (int x = 0; x < width; x++)
				{
					if (y == 0 || x == 0 || y == height - 1 || x == width - 1)
						mask[(y + loc_y)*picWidth + x + loc_x] = 160;
				}
			}

		}

	}
}

void write2yuv(unsigned char* p, int cols, int rols ,char* file_name)
{
	FILE* fid;
	fopen_s(&fid,file_name, "wb");
	for (int i = 0; i < rols; i++) 
  {
	  fwrite(p, sizeof(unsigned char), cols, fid);
    p += cols;
	}
	fclose(fid);
}

void writeByArithmetic(Arithmetic_Codec* ace, Adaptive_Bit_Model* bit, int output, int num)
{
	for (int i = num; i > 0; i--)
	{
		int outBit =( output >> (i - 1)) & 1;
		ace->encode(outBit, *bit);
	}
}

int readByArithmetic(Arithmetic_Codec* acd, Adaptive_Bit_Model* bit, int num)
{
	int output = 0;
	for (int i = 0; i < num; i++)
	{
		output = (output << 1) + acd->decode(*bit);
	}
	return output;
}

int readBin(unsigned char* buffer, int length, int* readBit)
{
	int value = 0;
	int byteNum = *readBit / 8;
	int bitNum = *readBit % 8;
	if (length <= 8 - bitNum)
	{
		value = buffer[byteNum] >> (8 - bitNum);
		value = value << (8 - bitNum);
		value = buffer[byteNum] - value;
		value = value >> (8 - bitNum - length);
		*readBit += length;
	}
	else
	{
		int i;
		value = buffer[byteNum] >> (8 - bitNum);
		value = value << (8 - bitNum);
		value = buffer[byteNum] - value;
		for (i = 0; i < (length - (8 - bitNum)) / 8; i++)
		{
			value = (value << 8) + buffer[byteNum + i + 1];
		}
		int bitRest = (length - (8 - bitNum)) % 8;
		value = (value << bitRest) + (buffer[byteNum + i + 1] >> (8 - bitRest));
		*readBit += length;
	}
	return value;
}

void write2bin(unsigned char* p, int size, char* file_name)
{
	FILE* fid;
	fopen_s(&fid, file_name, "wb");
	fwrite(p, sizeof(unsigned char), size, fid);
	fclose(fid);
}

void write2binary(unsigned char* stream, int input, int length, int*g_codeBit)
{
	int byteNum = *g_codeBit / 8;
	int bitNum = *g_codeBit % 8;
	if (length <= 8 - bitNum)
	{
		stream[byteNum] += input << (8 - bitNum - length);
		*g_codeBit += length;
	}
	else
	{
		int writein = input >> (length - (8 - bitNum));
		stream[byteNum] += writein;
		*g_codeBit += 8 - bitNum;
		input -= writein << (length - (8 - bitNum));
		write2binary(stream, input, length - (8 - bitNum), g_codeBit);
	}
}

int getFSize(FILE* pFile)
{
	int size;
	fseek(pFile, 0, SEEK_END);
	size = ftell(pFile);
	fseek(pFile, 0, SEEK_SET);
	return size;
}

int getBitSize(int num)
{
	for (int i = 10; i > 0; i--)
	{
		if ((num >> i) & 1)
			return i;
	}
}

int predictColor1(CodingUnit* CU, int*predict1, int*predict2)
{
	unsigned char* pImg = CU->getImg();
	int h = CU->pic_height;
	int w = CU->pic_width;
	int locCUy = CU->location_row;
	int locCUx = CU->location_col;
	int color1, color2, color3;
	if (locCUx == 0 && locCUy == 0) return 0;  //without reference ,no need to transmit the flag
	else if (locCUy == 0)
	{
		*predict1 = pImg[w*locCUy + locCUx - 1];
		return 1;
	}
	else if (locCUx == 0)
	{
		*predict1 = pImg[w*(locCUy - 1) + locCUx];
		return 1;
	}
	else
	{
		color1 = pImg[w*(locCUy - 1) + locCUx];
		color2 = pImg[w*locCUy + locCUx - 1];
		color3 = pImg[w*(locCUy - 1) + locCUx - 1];
		if (color1 == color2)
		{
			*predict1 = color1;
			return 1;
		}
		else if (color1 == color3 && color1 != color2)
		{
			*predict1 = color2;
			*predict2 = color1;
			return 2;
		}
		else if (color2 == color3 && color2 != color1)
		{
			*predict1 = color1;
			*predict2 = color2;
			return 2;
		}
		else return -1;
	}
}

int predictColor2(CodingUnit* CU, int loc_y, int loc_x, int color1)
{
	unsigned char* pImg = CU->getImg();
	int h = CU->pic_height;
	int w = CU->pic_width;
	int locCUy = CU->location_row;
	int locCUx = CU->location_col;
	int CUh = CU->getHeight();
	int CUw = CU->getWidth();
	int color;
	if (loc_x == 0 && locCUx)
	{
		int maxRange = ((CUh - 1 - loc_y) > loc_y) ? (CUh - 1 - loc_y) : loc_y;
		for (int i = 0; i <= maxRange; i++)
		{
			if (loc_y + i < CUh)
			{
				color = pImg[w * (locCUy + loc_y + i) + locCUx - 1];
				if (color != color1) return color;
			}
			if (loc_y - i >= 0)
			{
				color = pImg[w * (locCUy + loc_y - i) + locCUx - 1];
				if (color != color1) return color;
			}
		}
	}
	if (loc_y == 0 && locCUy)
	{
		int maxRange = ((CUw - 1 - loc_x) > loc_x) ? (CUw - 1 - loc_x) : loc_x;
		for (int i = 0; i <= maxRange; i++)
		{
			if (loc_x + i < CUw)
			{
				color = pImg[w * (locCUy - 1) + locCUx + loc_x + i];
				if (color != color1) return color;
			}
			if (loc_x - i >= 0)
			{
				color = pImg[w * (locCUy - 1) + locCUx + loc_x - i];
				if (color != color1) return color;
			}
		}
	}
	return -1;
}

int predictInitialPosition(CodingUnit* CU, int* ini_y, int* ini_x, int* ini_y2, int* ini_x2, int color, int* predictMode1, int* predictMode2)
{
	unsigned char* pImg = CU->getImg();
	int w = CU->pic_width;
	int loc_y = CU->location_row;
	int loc_x = CU->location_col;
	int CUh = CU->getHeight();
	int CUw = CU->getWidth();
	int predict = 0;
	bool predict1 = false;
	bool predict2 = false;
	if (loc_x)
	{
		for (int i = 1; i < CUh; i++)
		{
			if (pImg[w*(loc_y + i) + loc_x - 1] != color && !predict1)
			{
				if (pImg[w*(loc_y + i - 1) + loc_x - 2] != color && i < CUh - 1)
				{
					*ini_y = i + 1;
					*ini_x = 0;
					*predictMode1 = 1;
				}
				else if (pImg[w*(loc_y + i - 1) + loc_x - 2] != color && i == CUh - 1)
				{
					*ini_y = i;
					*ini_x = 0;
					*predictMode1 = 1;
				}
				else if (pImg[w*(loc_y + i) + loc_x - 2] != color)
				{
					*ini_y = i;
					*ini_x = 0;
					*predictMode1 = 0;
				}
				else if (i == 1)
				{
					*ini_y = i;
					*ini_x = 0;
					*predictMode1 = -1;
				}
				else
				{
					*ini_y = i - 1;
					*ini_x = 0;
					*predictMode1 = -1;
				}
				predict1 = true;
			}
		}
	}
	if (loc_y)
	{
		for (int i = 1; i < CUw; i++)
		{
			if (pImg[w*(loc_y - 1) + loc_x + i] != color && !predict2)
			{
				if (pImg[w*(loc_y - 2) + loc_x + i - 1] != color && i < CUw - 1)
				{
					*ini_y2 = 0;
					*ini_x2 = i + 1;
					*predictMode2 = 1;
				}
				else if (pImg[w*(loc_y - 2) + loc_x + i - 1] != color && i == CUw - 1)
				{
					*ini_y2 = 0;
					*ini_x2 = i;
					*predictMode2 = 1;
				}
				else if (pImg[w*(loc_y - 2) + loc_x + i] != color)
				{
					*ini_y2 = 0;
					*ini_x2 = i;
					*predictMode2 = 0;
				}
				else if (i == 1)
				{
					*ini_y2 = 0;
					*ini_x2 = i;
					*predictMode2 = -1;
				}
				else
				{
					*ini_y2 = 0;
					*ini_x2 = i - 1;
					*predictMode2 = -1;
				}
				predict2 = true;
			}
		}
	}
	if (!(predict1 || predict2))
	{
		return -1;
	}
	else
	{
		if (predict1 && predict2)
		{
			return 2;
		}
		if (predict1)
		{
			return 0;
		}
		if (predict2)
		{
			return 1;
		}
	}
}

void encodeEdgeLength(CodingUnit* CU, bool encode)
{
	int* edge = CU->getEdge();
	int loc_y = edge[1];
	int loc_x = edge[2];
	int CUh = CU->getHeight();
	int CUw = CU->getWidth();

	if (encode)
	{
		for (int i = 5 + CU->context; i < edge[4] + 5; i++)
		{
			if (CU->ForbiddenLine[i - 5] != 4)
			{
				if (i == 5)
				{
					if (loc_x == 0) acodec.encode(edge[i], a3OT_L, CU->ForbiddenLine[i - 5]);
					else if (loc_y == 0) acodec.encode(edge[i], a3OT_A, CU->ForbiddenLine[i - 5]);
					else if (loc_y == CUh - 1) acodec.encode(edge[i], a3OT_B, CU->ForbiddenLine[i - 5]);
					else acodec.encode(edge[i], a3OT_R, CU->ForbiddenLine[i - 5]);
				}
				else if (i == 6)
				{
					switch (edge[i - 1])
					{
					case 0:acodec.encode(edge[i], a3OT_0, CU->ForbiddenLine[i - 5]); break;
					case 1:acodec.encode(edge[i], a3OT_1, CU->ForbiddenLine[i - 5]); break;
					case 2:acodec.encode(edge[i], a3OT_2, CU->ForbiddenLine[i - 5]);
					}
				}
				else if (i == 7)
				{
					switch (edge[i - 2] * 3 + edge[i - 1])
					{
					case 0:acodec.encode(edge[i], a3OT_00, CU->ForbiddenLine[i - 5]); break;
					case 1:acodec.encode(edge[i], a3OT_01, CU->ForbiddenLine[i - 5]); break;
					case 2:acodec.encode(edge[i], a3OT_02, CU->ForbiddenLine[i - 5]); break;
					case 3:acodec.encode(edge[i], a3OT_10, CU->ForbiddenLine[i - 5]); break;
					case 4:acodec.encode(edge[i], a3OT_11, CU->ForbiddenLine[i - 5]); break;
					case 5:acodec.encode(edge[i], a3OT_12, CU->ForbiddenLine[i - 5]); break;
					case 6:acodec.encode(edge[i], a3OT_20, CU->ForbiddenLine[i - 5]); break;
					case 7:acodec.encode(edge[i], a3OT_21, CU->ForbiddenLine[i - 5]); break;
					case 8:acodec.encode(edge[i], a3OT_22, CU->ForbiddenLine[i - 5]);
					}
				}
				else
				{
					switch (edge[i - 3] * 9 + edge[i - 2] * 3 + edge[i - 1])
					{
					case 0:acodec.encode(edge[i], a3OT_000, CU->ForbiddenLine[i - 5]); break;
					case 1:acodec.encode(edge[i], a3OT_001, CU->ForbiddenLine[i - 5]); break;
					case 2:acodec.encode(edge[i], a3OT_002, CU->ForbiddenLine[i - 5]); break;
					case 3:acodec.encode(edge[i], a3OT_010, CU->ForbiddenLine[i - 5]); break;
					case 4:acodec.encode(edge[i], a3OT_011, CU->ForbiddenLine[i - 5]); break;
					case 5:acodec.encode(edge[i], a3OT_012, CU->ForbiddenLine[i - 5]); break;
					case 6:acodec.encode(edge[i], a3OT_020, CU->ForbiddenLine[i - 5]); break;
					case 7:acodec.encode(edge[i], a3OT_021, CU->ForbiddenLine[i - 5]); break;
					case 8:acodec.encode(edge[i], a3OT_022, CU->ForbiddenLine[i - 5]); break;
					case 9:acodec.encode(edge[i], a3OT_100, CU->ForbiddenLine[i - 5]); break;
					case 10:acodec.encode(edge[i], a3OT_101, CU->ForbiddenLine[i - 5]); break;
					case 11:acodec.encode(edge[i], a3OT_102, CU->ForbiddenLine[i - 5]); break;
					case 12:acodec.encode(edge[i], a3OT_110, CU->ForbiddenLine[i - 5]); break;
					case 13:acodec.encode(edge[i], a3OT_111, CU->ForbiddenLine[i - 5]); break;
					case 14:acodec.encode(edge[i], a3OT_112, CU->ForbiddenLine[i - 5]); break;
					case 15:acodec.encode(edge[i], a3OT_120, CU->ForbiddenLine[i - 5]); break;
					case 16:acodec.encode(edge[i], a3OT_121, CU->ForbiddenLine[i - 5]); break;
					case 17:acodec.encode(edge[i], a3OT_122, CU->ForbiddenLine[i - 5]); break;
					case 18:acodec.encode(edge[i], a3OT_200, CU->ForbiddenLine[i - 5]); break;
					case 19:acodec.encode(edge[i], a3OT_201, CU->ForbiddenLine[i - 5]); break;
					case 20:acodec.encode(edge[i], a3OT_202, CU->ForbiddenLine[i - 5]); break;
					case 21:acodec.encode(edge[i], a3OT_210, CU->ForbiddenLine[i - 5]); break;
					case 22:acodec.encode(edge[i], a3OT_211, CU->ForbiddenLine[i - 5]); break;
					case 23:acodec.encode(edge[i], a3OT_212, CU->ForbiddenLine[i - 5]); break;
					case 24:acodec.encode(edge[i], a3OT_220, CU->ForbiddenLine[i - 5]); break;
					case 25:acodec.encode(edge[i], a3OT_221, CU->ForbiddenLine[i - 5]); break;
					case 26:acodec.encode(edge[i], a3OT_222, CU->ForbiddenLine[i - 5]);
					}
				}
			}
		}
	//printf("\n");
	}
	else
	{
		for (int i = 5 + CU->context; i < edge[4] + 5; i++)
		{
			if (CU->ForbiddenLine[i-5] != 4)
			{
				if (i == 5)
				{
					if (loc_x == 0) CU->update(CU->element->a3OT_L, edge[i], CU->ForbiddenLine[i - 5]);
					else if (loc_y == 0) CU->update(CU->element->a3OT_A, edge[i], CU->ForbiddenLine[i - 5]);
					else if (loc_y == CUh - 1) CU->update(CU->element->a3OT_B, edge[i], CU->ForbiddenLine[i - 5]);
					else CU->update(CU->element->a3OT_R, edge[i], CU->ForbiddenLine[i - 5]);
				}
				else if (i == 6)
				{
					switch (edge[i - 1])
					{
					case 0:CU->update(CU->element->a3OT_0, edge[i], CU->ForbiddenLine[i - 5]); break;
					case 1:CU->update(CU->element->a3OT_1, edge[i], CU->ForbiddenLine[i - 5]); break;
					case 2:CU->update(CU->element->a3OT_2, edge[i], CU->ForbiddenLine[i - 5]);
					}
				}
				else if (i == 7)
				{
					switch (edge[i - 2] * 3 + edge[i - 1])
					{
					case 0:CU->update(CU->element->a3OT_00, edge[i], CU->ForbiddenLine[i - 5]); break;
					case 1:CU->update(CU->element->a3OT_01, edge[i], CU->ForbiddenLine[i - 5]); break;
					case 2:CU->update(CU->element->a3OT_02, edge[i], CU->ForbiddenLine[i - 5]); break;
					case 3:CU->update(CU->element->a3OT_10, edge[i], CU->ForbiddenLine[i - 5]); break;
					case 4:CU->update(CU->element->a3OT_11, edge[i], CU->ForbiddenLine[i - 5]); break;
					case 5:CU->update(CU->element->a3OT_12, edge[i], CU->ForbiddenLine[i - 5]); break;
					case 6:CU->update(CU->element->a3OT_20, edge[i], CU->ForbiddenLine[i - 5]); break;
					case 7:CU->update(CU->element->a3OT_21, edge[i], CU->ForbiddenLine[i - 5]); break;
					case 8:CU->update(CU->element->a3OT_22, edge[i], CU->ForbiddenLine[i - 5]);
					}
				}
				else
				{
					switch (edge[i - 3] * 9 + edge[i - 2] * 3 + edge[i - 1])
					{
					case 0:CU->update(CU->element->a3OT_000, edge[i], CU->ForbiddenLine[i - 5]); break;
					case 1:CU->update(CU->element->a3OT_001, edge[i], CU->ForbiddenLine[i - 5]); break;
					case 2:CU->update(CU->element->a3OT_002, edge[i], CU->ForbiddenLine[i - 5]); break;
					case 3:CU->update(CU->element->a3OT_010, edge[i], CU->ForbiddenLine[i - 5]); break;
					case 4:CU->update(CU->element->a3OT_011, edge[i], CU->ForbiddenLine[i - 5]); break;
					case 5:CU->update(CU->element->a3OT_012, edge[i], CU->ForbiddenLine[i - 5]); break;
					case 6:CU->update(CU->element->a3OT_020, edge[i], CU->ForbiddenLine[i - 5]); break;
					case 7:CU->update(CU->element->a3OT_021, edge[i], CU->ForbiddenLine[i - 5]); break;
					case 8:CU->update(CU->element->a3OT_022, edge[i], CU->ForbiddenLine[i - 5]); break;
					case 9:CU->update(CU->element->a3OT_100, edge[i], CU->ForbiddenLine[i - 5]); break;
					case 10:CU->update(CU->element->a3OT_101, edge[i], CU->ForbiddenLine[i - 5]); break;
					case 11:CU->update(CU->element->a3OT_102, edge[i], CU->ForbiddenLine[i - 5]); break;
					case 12:CU->update(CU->element->a3OT_110, edge[i], CU->ForbiddenLine[i - 5]); break;
					case 13:CU->update(CU->element->a3OT_111, edge[i], CU->ForbiddenLine[i - 5]); break;
					case 14:CU->update(CU->element->a3OT_112, edge[i], CU->ForbiddenLine[i - 5]); break;
					case 15:CU->update(CU->element->a3OT_120, edge[i], CU->ForbiddenLine[i - 5]); break;
					case 16:CU->update(CU->element->a3OT_121, edge[i], CU->ForbiddenLine[i - 5]); break;
					case 17:CU->update(CU->element->a3OT_122, edge[i], CU->ForbiddenLine[i - 5]); break;
					case 18:CU->update(CU->element->a3OT_200, edge[i], CU->ForbiddenLine[i - 5]); break;
					case 19:CU->update(CU->element->a3OT_201, edge[i], CU->ForbiddenLine[i - 5]); break;
					case 20:CU->update(CU->element->a3OT_202, edge[i], CU->ForbiddenLine[i - 5]); break;
					case 21:CU->update(CU->element->a3OT_210, edge[i], CU->ForbiddenLine[i - 5]); break;
					case 22:CU->update(CU->element->a3OT_211, edge[i], CU->ForbiddenLine[i - 5]); break;
					case 23:CU->update(CU->element->a3OT_212, edge[i], CU->ForbiddenLine[i - 5]); break;
					case 24:CU->update(CU->element->a3OT_220, edge[i], CU->ForbiddenLine[i - 5]); break;
					case 25:CU->update(CU->element->a3OT_221, edge[i], CU->ForbiddenLine[i - 5]); break;
					case 26:CU->update(CU->element->a3OT_222, edge[i], CU->ForbiddenLine[i - 5]);
					}
				}
			}
		}

	}
}

void encodeDifEdgeLength(CodingUnit* CU, bool encode)
{
	int* edge = CU->getEdge();
	int loc_y = edge[1];
	int loc_x = edge[2];
	int CUh = CU->getHeight();
	int CUw = CU->getWidth();
	int OT3_0 = -1;
	int OT3_1 = -1;
	int OT3_2 = -1;
	for (int i = 5; i < 5 + CU->context; i++)
	{
		update_context(&OT3_0, &OT3_1, &OT3_2, edge[i]);
	}

	if (encode)
	{
		for (int i = 0; i < edge[4] - CU->context; i++)
		{
			if (CU->ForbiddenLine[i + CU->context] != 4)
			{
				if (CU->difEdge[2 + 3 * i + 1] == 1)
				{
					acodec.encode(1, aSameFlag);
				}
				else
				{
					if(CU->difEdge[2 + 3 * i + 1] == 0) acodec.encode(0, aSameFlag);
					if (CU->difEdge[2 + 3 * i + 2] != 4)
					{
						if (i + CU->context == 0)
						{
							if (loc_x == 0) acodec.encode(CU->difEdge[2 + 3 * i], a3OT_L, CU->difEdge[2 + 3 * i + 2]);
							else if (loc_y == 0) acodec.encode(CU->difEdge[2 + 3 * i], a3OT_A, CU->difEdge[2 + 3 * i + 2]);
							else if (loc_y == CUh - 1) acodec.encode(CU->difEdge[2 + 3 * i], a3OT_B, CU->difEdge[2 + 3 * i + 2]);
							else acodec.encode(CU->difEdge[2 + 3 * i], a3OT_R, CU->difEdge[2 + 3 * i + 2]);
						}
						else if (i + CU->context == 1)
						{
							switch (OT3_0)
							{
							case 0:acodec.encode(CU->difEdge[2 + 3 * i], a3OT_0, CU->difEdge[2 + 3 * i + 2]); break;
							case 1:acodec.encode(CU->difEdge[2 + 3 * i], a3OT_1, CU->difEdge[2 + 3 * i + 2]); break;
							case 2:acodec.encode(CU->difEdge[2 + 3 * i], a3OT_2, CU->difEdge[2 + 3 * i + 2]);
							}
						}
						else if (i + CU->context == 2)
						{
							switch (OT3_0 * 3 + OT3_1)
							{
							case 0:acodec.encode(CU->difEdge[2 + 3 * i], a3OT_00, CU->difEdge[2 + 3 * i + 2]); break;
							case 1:acodec.encode(CU->difEdge[2 + 3 * i], a3OT_01, CU->difEdge[2 + 3 * i + 2]); break;
							case 2:acodec.encode(CU->difEdge[2 + 3 * i], a3OT_02, CU->difEdge[2 + 3 * i + 2]); break;
							case 3:acodec.encode(CU->difEdge[2 + 3 * i], a3OT_10, CU->difEdge[2 + 3 * i + 2]); break;
							case 4:acodec.encode(CU->difEdge[2 + 3 * i], a3OT_11, CU->difEdge[2 + 3 * i + 2]); break;
							case 5:acodec.encode(CU->difEdge[2 + 3 * i], a3OT_12, CU->difEdge[2 + 3 * i + 2]); break;
							case 6:acodec.encode(CU->difEdge[2 + 3 * i], a3OT_20, CU->difEdge[2 + 3 * i + 2]); break;
							case 7:acodec.encode(CU->difEdge[2 + 3 * i], a3OT_21, CU->difEdge[2 + 3 * i + 2]); break;
							case 8:acodec.encode(CU->difEdge[2 + 3 * i], a3OT_22, CU->difEdge[2 + 3 * i + 2]);
							}
						}
						else
						{
							switch (OT3_0 * 9 + OT3_1 * 3 + OT3_2)
							{
							case 0:acodec.encode(CU->difEdge[2 + 3 * i], a3OT_000, CU->difEdge[2 + 3 * i + 2]); break;
							case 1:acodec.encode(CU->difEdge[2 + 3 * i], a3OT_001, CU->difEdge[2 + 3 * i + 2]); break;
							case 2:acodec.encode(CU->difEdge[2 + 3 * i], a3OT_002, CU->difEdge[2 + 3 * i + 2]); break;
							case 3:acodec.encode(CU->difEdge[2 + 3 * i], a3OT_010, CU->difEdge[2 + 3 * i + 2]); break;
							case 4:acodec.encode(CU->difEdge[2 + 3 * i], a3OT_011, CU->difEdge[2 + 3 * i + 2]); break;
							case 5:acodec.encode(CU->difEdge[2 + 3 * i], a3OT_012, CU->difEdge[2 + 3 * i + 2]); break;
							case 6:acodec.encode(CU->difEdge[2 + 3 * i], a3OT_020, CU->difEdge[2 + 3 * i + 2]); break;
							case 7:acodec.encode(CU->difEdge[2 + 3 * i], a3OT_021, CU->difEdge[2 + 3 * i + 2]); break;
							case 8:acodec.encode(CU->difEdge[2 + 3 * i], a3OT_022, CU->difEdge[2 + 3 * i + 2]); break;
							case 9:acodec.encode(CU->difEdge[2 + 3 * i], a3OT_100, CU->difEdge[2 + 3 * i + 2]); break;
							case 10:acodec.encode(CU->difEdge[2 + 3 * i], a3OT_101, CU->difEdge[2 + 3 * i + 2]); break;
							case 11:acodec.encode(CU->difEdge[2 + 3 * i], a3OT_102, CU->difEdge[2 + 3 * i + 2]); break;
							case 12:acodec.encode(CU->difEdge[2 + 3 * i], a3OT_110, CU->difEdge[2 + 3 * i + 2]); break;
							case 13:acodec.encode(CU->difEdge[2 + 3 * i], a3OT_111, CU->difEdge[2 + 3 * i + 2]); break;
							case 14:acodec.encode(CU->difEdge[2 + 3 * i], a3OT_112, CU->difEdge[2 + 3 * i + 2]); break;
							case 15:acodec.encode(CU->difEdge[2 + 3 * i], a3OT_120, CU->difEdge[2 + 3 * i + 2]); break;
							case 16:acodec.encode(CU->difEdge[2 + 3 * i], a3OT_121, CU->difEdge[2 + 3 * i + 2]); break;
							case 17:acodec.encode(CU->difEdge[2 + 3 * i], a3OT_122, CU->difEdge[2 + 3 * i + 2]); break;
							case 18:acodec.encode(CU->difEdge[2 + 3 * i], a3OT_200, CU->difEdge[2 + 3 * i + 2]); break;
							case 19:acodec.encode(CU->difEdge[2 + 3 * i], a3OT_201, CU->difEdge[2 + 3 * i + 2]); break;
							case 20:acodec.encode(CU->difEdge[2 + 3 * i], a3OT_202, CU->difEdge[2 + 3 * i + 2]); break;
							case 21:acodec.encode(CU->difEdge[2 + 3 * i], a3OT_210, CU->difEdge[2 + 3 * i + 2]); break;
							case 22:acodec.encode(CU->difEdge[2 + 3 * i], a3OT_211, CU->difEdge[2 + 3 * i + 2]); break;
							case 23:acodec.encode(CU->difEdge[2 + 3 * i], a3OT_212, CU->difEdge[2 + 3 * i + 2]); break;
							case 24:acodec.encode(CU->difEdge[2 + 3 * i], a3OT_220, CU->difEdge[2 + 3 * i + 2]); break;
							case 25:acodec.encode(CU->difEdge[2 + 3 * i], a3OT_221, CU->difEdge[2 + 3 * i + 2]); break;
							case 26:acodec.encode(CU->difEdge[2 + 3 * i], a3OT_222, CU->difEdge[2 + 3 * i + 2]);
							}
						}
					}
				}
			}
			update_context(&OT3_0, &OT3_1, &OT3_2, CU->difEdge[2 + 3 * i]);
		}
	}
	else
	{
		for (int i = 0; i < edge[4] - CU->context; i++)
		{
			if (CU->ForbiddenLine[i + CU->context] != 4)
			{
				if (CU->difEdge[2 + 3 * i + 1] == 1)
				{
					CU->update(CU->element->aSameFlag, 1);
				}
				else
				{
					if (CU->difEdge[2 + 3 * i + 1] == 0) CU->update(CU->element->aSameFlag, 0);
					if (CU->difEdge[2 + 3 * i + 2] != 4)
					{
						if (i + CU->context == 0)
						{
							if (loc_x == 0) CU->update(CU->element->a3OT_L, CU->difEdge[2 + 3 * i], CU->difEdge[2 + 3 * i + 2]);
							else if (loc_y == 0) CU->update(CU->element->a3OT_A, CU->difEdge[2 + 3 * i], CU->difEdge[2 + 3 * i + 2]);
							else if (loc_y == CUh - 1) CU->update(CU->element->a3OT_B, CU->difEdge[2 + 3 * i], CU->difEdge[2 + 3 * i + 2]);
							else CU->update(CU->element->a3OT_R, CU->difEdge[2 + 3 * i], CU->difEdge[2 + 3 * i + 2]);
						}
						else if (i + CU->context == 1)
						{
							switch (OT3_0)
							{
							case 0:CU->update(CU->element->a3OT_0, CU->difEdge[2 + 3 * i], CU->difEdge[2 + 3 * i + 2]); break;
							case 1:CU->update(CU->element->a3OT_1, CU->difEdge[2 + 3 * i], CU->difEdge[2 + 3 * i + 2]); break;
							case 2:CU->update(CU->element->a3OT_2, CU->difEdge[2 + 3 * i], CU->difEdge[2 + 3 * i + 2]);
							}
						}
						else if (i + CU->context == 2)
						{
							switch (OT3_0 * 3 + OT3_1)
							{
							case 0:CU->update(CU->element->a3OT_00, CU->difEdge[2 + 3 * i], CU->difEdge[2 + 3 * i + 2]); break;
							case 1:CU->update(CU->element->a3OT_01, CU->difEdge[2 + 3 * i], CU->difEdge[2 + 3 * i + 2]); break;
							case 2:CU->update(CU->element->a3OT_02, CU->difEdge[2 + 3 * i], CU->difEdge[2 + 3 * i + 2]); break;
							case 3:CU->update(CU->element->a3OT_10, CU->difEdge[2 + 3 * i], CU->difEdge[2 + 3 * i + 2]); break;
							case 4:CU->update(CU->element->a3OT_11, CU->difEdge[2 + 3 * i], CU->difEdge[2 + 3 * i + 2]); break;
							case 5:CU->update(CU->element->a3OT_12, CU->difEdge[2 + 3 * i], CU->difEdge[2 + 3 * i + 2]); break;
							case 6:CU->update(CU->element->a3OT_20, CU->difEdge[2 + 3 * i], CU->difEdge[2 + 3 * i + 2]); break;
							case 7:CU->update(CU->element->a3OT_21, CU->difEdge[2 + 3 * i], CU->difEdge[2 + 3 * i + 2]); break;
							case 8:CU->update(CU->element->a3OT_22, CU->difEdge[2 + 3 * i], CU->difEdge[2 + 3 * i + 2]);
							}
						}
						else
						{
							switch (OT3_0 * 9 + OT3_1 * 3 + OT3_2)
							{
							case 0:CU->update(CU->element->a3OT_000, CU->difEdge[2 + 3 * i], CU->difEdge[2 + 3 * i + 2]); break;
							case 1:CU->update(CU->element->a3OT_001, CU->difEdge[2 + 3 * i], CU->difEdge[2 + 3 * i + 2]); break;
							case 2:CU->update(CU->element->a3OT_002, CU->difEdge[2 + 3 * i], CU->difEdge[2 + 3 * i + 2]); break;
							case 3:CU->update(CU->element->a3OT_010, CU->difEdge[2 + 3 * i], CU->difEdge[2 + 3 * i + 2]); break;
							case 4:CU->update(CU->element->a3OT_011, CU->difEdge[2 + 3 * i], CU->difEdge[2 + 3 * i + 2]); break;
							case 5:CU->update(CU->element->a3OT_012, CU->difEdge[2 + 3 * i], CU->difEdge[2 + 3 * i + 2]); break;
							case 6:CU->update(CU->element->a3OT_020, CU->difEdge[2 + 3 * i], CU->difEdge[2 + 3 * i + 2]); break;
							case 7:CU->update(CU->element->a3OT_021, CU->difEdge[2 + 3 * i], CU->difEdge[2 + 3 * i + 2]); break;
							case 8:CU->update(CU->element->a3OT_022, CU->difEdge[2 + 3 * i], CU->difEdge[2 + 3 * i + 2]); break;
							case 9:CU->update(CU->element->a3OT_100, CU->difEdge[2 + 3 * i], CU->difEdge[2 + 3 * i + 2]); break;
							case 10:CU->update(CU->element->a3OT_101, CU->difEdge[2 + 3 * i], CU->difEdge[2 + 3 * i + 2]); break;
							case 11:CU->update(CU->element->a3OT_102, CU->difEdge[2 + 3 * i], CU->difEdge[2 + 3 * i + 2]); break;
							case 12:CU->update(CU->element->a3OT_110, CU->difEdge[2 + 3 * i], CU->difEdge[2 + 3 * i + 2]); break;
							case 13:CU->update(CU->element->a3OT_111, CU->difEdge[2 + 3 * i], CU->difEdge[2 + 3 * i + 2]); break;
							case 14:CU->update(CU->element->a3OT_112, CU->difEdge[2 + 3 * i], CU->difEdge[2 + 3 * i + 2]); break;
							case 15:CU->update(CU->element->a3OT_120, CU->difEdge[2 + 3 * i], CU->difEdge[2 + 3 * i + 2]); break;
							case 16:CU->update(CU->element->a3OT_121, CU->difEdge[2 + 3 * i], CU->difEdge[2 + 3 * i + 2]); break;
							case 17:CU->update(CU->element->a3OT_122, CU->difEdge[2 + 3 * i], CU->difEdge[2 + 3 * i + 2]); break;
							case 18:CU->update(CU->element->a3OT_200, CU->difEdge[2 + 3 * i], CU->difEdge[2 + 3 * i + 2]); break;
							case 19:CU->update(CU->element->a3OT_201, CU->difEdge[2 + 3 * i], CU->difEdge[2 + 3 * i + 2]); break;
							case 20:CU->update(CU->element->a3OT_202, CU->difEdge[2 + 3 * i], CU->difEdge[2 + 3 * i + 2]); break;
							case 21:CU->update(CU->element->a3OT_210, CU->difEdge[2 + 3 * i], CU->difEdge[2 + 3 * i + 2]); break;
							case 22:CU->update(CU->element->a3OT_211, CU->difEdge[2 + 3 * i], CU->difEdge[2 + 3 * i + 2]); break;
							case 23:CU->update(CU->element->a3OT_212, CU->difEdge[2 + 3 * i], CU->difEdge[2 + 3 * i + 2]); break;
							case 24:CU->update(CU->element->a3OT_220, CU->difEdge[2 + 3 * i], CU->difEdge[2 + 3 * i + 2]); break;
							case 25:CU->update(CU->element->a3OT_221, CU->difEdge[2 + 3 * i], CU->difEdge[2 + 3 * i + 2]); break;
							case 26:CU->update(CU->element->a3OT_222, CU->difEdge[2 + 3 * i], CU->difEdge[2 + 3 * i + 2]);
							}
						}
					}
				}
			}
			update_context(&OT3_0, &OT3_1, &OT3_2, CU->difEdge[2 + 3 * i]);
		}
	}
}

int getColorList(int color, int* colorlist)
{
	for (int i = 0; i < numC; i++) 
	{
		if (colorlist[i] == color) return i;
	}
}

void adjustColorList(int color, int* colorlist)
{
	int index = getColorList(color, colorlist);
	for (int i = index - 1; i >= 0; i--)
	{
		colorlist[i + 1] = colorlist[i];
	}
	colorlist[0] = color;
}

int get_index(int index, bool encoder, int forbidden1, int forbidden2, int forbidden3)
{
	if (encoder)
	{
		if (forbidden1 < index && forbidden2 < index && forbidden3 < index) return index - 3;
		if (forbidden1 > index && forbidden2 > index && forbidden3 > index) return index;
		if ((forbidden1 > index && forbidden2 > index && forbidden3 < index) || (forbidden1 > index && forbidden2 < index && forbidden3 > index) || (forbidden1 < index && forbidden2 > index && forbidden3 > index)) return index - 1;
		return index - 2;
	}
	else
	{
		if (forbidden2 < forbidden1) swap(forbidden1, forbidden2);
		if (forbidden3 < forbidden1) { swap(forbidden1, forbidden3); swap(forbidden2, forbidden3);}
		else if(forbidden3 < forbidden2) swap(forbidden2, forbidden3);

		if (forbidden1 <= index) index++;
		if (forbidden2 <= index) index++;
		if (forbidden3 <= index) index++;
		return index;
	}
}
