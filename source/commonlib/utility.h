#pragma once
#include "CodingUnit.h"
#include "global_arithmetic.h"

bool isOneColor(unsigned char* value, int height, int width);
bool isContiEdge(unsigned char* value, int height, int width, int pic_width);
bool isInter(unsigned char * value, int height, int width, int loc_row, int loc_col, int pic_height, int pic_width, CodingUnit* pcCU, bool ContiEdge);
void draw_mask(unsigned char *mask, int picWidth, CodingUnit* pcCU, int ctuIdx);
void write2yuv(unsigned char* p, int cols, int rols, char* file_name);
void advancedExpand(unsigned char* img, int height, int width, int color1, int color2_y, int color2_x);
void update_context(int* OT3_0, int* OT3_1, int* OT3_2, int OT3);
int readBin(unsigned char* buffer, int length, int* readBit);
void writeByArithmetic(Arithmetic_Codec* ace, Adaptive_Bit_Model* bit, int output, int num);
int readByArithmetic(Arithmetic_Codec* acd, Adaptive_Bit_Model* bit, int num);
void write2bin(unsigned char* p, int size, char* file_name);
void write2binary(unsigned char* stream, int input, int length, int*g_codeBit);
int getFSize(FILE* pFile);
int getBitSize(int num);
int predictColor1(CodingUnit* CU, int*predict1, int*predict2);
int predictColor2(CodingUnit* CU, int loc_y, int loc_x, int color1);
int predictInitialPosition(CodingUnit* CU, int* ini_y, int* ini_x,int* ini_y2,int* ini_x2,int color, int* predictMode1, int* predictMode2);
void encodeEdgeLength(CodingUnit* CU, bool encode);
void encodeDifEdgeLength(CodingUnit* CU, bool encode);
int getColorList(int color, int* colorlist);
void adjustColorList(int color, int* colorlist);
int get_index(int index, bool encoder, int forbidden1 = 10000, int forbidden2 = 10000, int forbidden3 = 10000);

