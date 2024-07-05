#pragma once
#include "global_arithmetic.h"
#include <iostream> 
using namespace std;

class CodingUnit
{
public:
	CodingUnit();
	~CodingUnit();
	void create(unsigned char* pImg, int h, int w, int CU_h, int CU_w, int loc_row, int loc_col, int dep);
	void create(unsigned char* pImg, int h, int w, int CU_h, int CU_w, int loc_row, int loc_col, int dep, int max_cu);
	void destroy(CodingUnit* CU);
	int getWidth();
	int getHeight();
	int getDepth();
	unsigned char* getValue();
	unsigned char* getImg();
	int getColorValue();
	void setColorValue(int color);
	int* getEdge();
	int getEdgeLength();
	void setEdge();
	void setRunLength();
	int* getRunLength();

  void enSplitFlag(bool encode = true);
	void enOneColor(bool encode = true);
	void enChainCoding(bool encode = true);
	void enRunLength(bool encode = true);
	void enMV(bool encode = true);

  int deSplitFlag();
	void deOneColor(unsigned char*pImg);
	void deChainCoding(unsigned char*pImg);
	void deRunLength(unsigned char*pImg);
	void deMV(unsigned char*pImg);

	void update(int * element, int index, int forbidden = -1, int forbidden2 = -1, int forbidden3 = -1);

	int pic_width;
	int pic_height;
	bool split_flag;
	bool oneColor;
	bool contiEdge;
	int location_row;
	int location_col;
	int context;
	int mode;   // left:0  right:1
	CodingUnit* child[4];
	int* ForbiddenLine;
	float rate;
	AC* element;

	int mvx;
	int mvy;
	bool noResidue;
	int* difEdge;

private:
	unsigned char *m_value;
	unsigned char *pImage;
	int m_height;
	int m_width;
	int m_depth;
	int colorValue;
	int* Edge;        //color1  color2_y  color2_x  color2  chaincode
	int edgeLength;
	int runLengthMode; //removed
					   
	int* runLength;  

};


