#pragma once
#include "CodingUnit.h"

class Picture
{
public:
	Picture();
	~Picture();
	unsigned char* value;
	int m_height;
	int m_width;
	int m_numCtuInRow;
	int m_numCtuInCol;
	int m_numColor;
	int* m_colorMap;
	CodingUnit** m_Ctu;

	void createPicture(unsigned char*p, int h, int w, int* cMap, int numC);
	void createPicture(unsigned char*p, int h, int w, int max_cu, int min_cu, int* cMap, int numC);
	void destroyPicture();
	CodingUnit* getCtu(int i);

};

