#include <iostream>  
#include <assert.h>
#include <fstream>
#include "../../source/commonlib/CodingUnit.h"
#include "../../source/commonlib/Picture.h"
#include "decOptions.h"
#include "../../source/commonlib/utility.h"
#include <time.h>
using namespace std;

void decodeCU(CodingUnit* pcCU, unsigned char* pImg, int max_cu, int min_cu)
{
	int loc_row = pcCU->location_row;
	int loc_col = pcCU->location_col;
	int h = pcCU->pic_height;
	int w = pcCU->pic_width;
	int CU_h = pcCU->getHeight();
	int CU_w = pcCU->getWidth();
	int depth = pcCU->getDepth();
	int side_length = max_cu >> (depth + 1);
	if (loc_row + 2 * side_length > h || loc_col + 2 * side_length > w)
	{
		pcCU->split_flag = true;
		if (loc_row + side_length >= h && loc_col + side_length >= w)
		{
			CodingUnit *CU = new CodingUnit;
			CU->create(pImg, h, w, CU_h, CU_w, loc_row, loc_col, depth + 1, max_cu);
			pcCU->child[0] = CU;
			decodeCU(pcCU->child[0], pImg, max_cu, min_cu);
		}
		else if (loc_row + side_length >= h && loc_col + side_length < w)
		{
			CodingUnit *CU1 = new CodingUnit;
			CU1->create(pImg, h, w, CU_h, side_length, loc_row, loc_col, depth + 1, max_cu);
			pcCU->child[0] = CU1;
			decodeCU(pcCU->child[0], pImg, max_cu, min_cu);
			CodingUnit *CU2 = new CodingUnit;
			CU2->create(pImg, h, w, CU_h, CU_w - side_length, loc_row, loc_col + side_length, depth + 1, max_cu);
			pcCU->child[1] = CU2;
			decodeCU(pcCU->child[1], pImg, max_cu, min_cu);
		}
		else if (loc_row + side_length < h && loc_col + side_length >= w)
		{
			CodingUnit *CU1 = new CodingUnit;
			CU1->create(pImg, h, w, side_length, CU_w, loc_row, loc_col, depth + 1, max_cu);
			pcCU->child[0] = CU1;
			decodeCU(pcCU->child[0], pImg, max_cu, min_cu);
			CodingUnit *CU2 = new CodingUnit;
			CU2->create(pImg, h, w, CU_h - side_length, CU_w, loc_row + side_length, loc_col, depth + 1, max_cu);
			pcCU->child[2] = CU2;
			decodeCU(pcCU->child[2], pImg, max_cu, min_cu);
		}
		else
		{
			CodingUnit *CU1 = new CodingUnit;
			CU1->create(pImg, h, w, side_length, side_length, loc_row, loc_col, depth + 1, max_cu);
			pcCU->child[0] = CU1;
			decodeCU(pcCU->child[0], pImg, max_cu, min_cu);
			CodingUnit *CU2 = new CodingUnit;
			CU2->create(pImg, h, w, side_length, CU_w - side_length, loc_row, loc_col + side_length, depth + 1, max_cu);
			pcCU->child[1] = CU2;
			decodeCU(pcCU->child[1], pImg, max_cu, min_cu);
			CodingUnit *CU3 = new CodingUnit;
			CU3->create(pImg, h, w, CU_h - side_length, side_length, loc_row + side_length, loc_col, depth + 1, max_cu);
			pcCU->child[2] = CU3;
			decodeCU(pcCU->child[2], pImg, max_cu, min_cu);
			CodingUnit *CU4 = new CodingUnit;
			CU4->create(pImg, h, w, CU_h - side_length, CU_w - side_length, loc_row + side_length, loc_col + side_length, depth + 1, max_cu);
			pcCU->child[2] = CU4;
			decodeCU(pcCU->child[2], pImg, max_cu, min_cu);
		}
	}
	else if (CU_h == min_cu && CU_w == min_cu)
	{
		pcCU->oneColor = acodec.decode(aOneColorFlag);
		if (pcCU->oneColor)
		{
			pcCU->deOneColor(pImg);
			pcCU->contiEdge = 0;
		}
		else 
		{
			pcCU->contiEdge = acodec.decode(aContinueEdgeFlag);
			if (pcCU->contiEdge)
			{
				pcCU->deChainCoding(pImg);
			}
			else
			{
				pcCU->deRunLength(pImg);
			}
		}
	}
	else
	{
		if (!pcCU->deSplitFlag())
		{
			pcCU->oneColor = acodec.decode(aOneColorFlag);
			if (pcCU->oneColor)
			{
				pcCU->deOneColor(pImg);
				pcCU->contiEdge = 0;
			}
			else
			{
				pcCU->contiEdge = 1;
				if (InterMode&&ref_frame&&CU_h <= g_max_inter && CU_h >= g_min_inter) pcCU->contiEdge = acodec.decode(aContinueEdgeFlag);
				if (pcCU->contiEdge)	pcCU->deChainCoding(pImg);
				else pcCU->deMV(pImg);
			}
		}
		else
		{
			pcCU->split_flag = true;
			int h = pcCU->pic_height;
			int w = pcCU->pic_width;
			int CU_h = pcCU->getHeight();
			int CU_w = pcCU->getWidth();
			int m_depth = pcCU->getDepth();
			int loc_row = pcCU->location_row;
			int loc_col = pcCU->location_col;
			CodingUnit *CU1 = new CodingUnit;
			CU1->create(pImg, h, w, CU_h / 2, CU_w / 2, loc_row, loc_col, m_depth + 1, max_cu);
			pcCU->child[0] = CU1;
			CodingUnit *CU2 = new CodingUnit;
			CU2->create(pImg, h, w, CU_h / 2, CU_w / 2, loc_row, loc_col + CU_w / 2, m_depth + 1, max_cu);
			pcCU->child[1] = CU2;
			CodingUnit *CU3 = new CodingUnit;
			CU3->create(pImg, h, w, CU_h / 2, CU_w / 2, loc_row + CU_h / 2, loc_col, m_depth + 1, max_cu);
			pcCU->child[2] = CU3;
			CodingUnit *CU4 = new CodingUnit;
			CU4->create(pImg, h, w, CU_h / 2, CU_w / 2, loc_row + CU_h / 2, loc_col + CU_w / 2, m_depth + 1, max_cu);
			pcCU->child[3] = CU4;
			decodeCU(pcCU->child[0], pImg, max_cu, min_cu);
			decodeCU(pcCU->child[1], pImg, max_cu, min_cu);
			decodeCU(pcCU->child[2], pImg, max_cu, min_cu);
			decodeCU(pcCU->child[3], pImg, max_cu, min_cu);
		}
	}
}


void decodePicture(Picture* pcPic,unsigned char* pImg, int max_cu, int min_cu)
{
	int total_num_ctu = pcPic->m_numCtuInRow * pcPic->m_numCtuInCol;
	for (int ctuIdx = 0; ctuIdx < total_num_ctu; ctuIdx++)
	{
		CodingUnit* pcCtu = pcPic->getCtu(ctuIdx);
		decodeCU(pcCtu, pImg, max_cu, min_cu);
	}
}

int main(int argc, char** argv)
{
	int begin, end;
	begin = clock();

	char filein[1000];
	char fileout[1000];
	readOptions(argc, argv, filein, fileout);
	FILE * pFile;
	int size;
	fopen_s(&pFile, filein, "rb");
	size = getFSize(pFile);
	unsigned char* buffer = new unsigned char[size];
	fread(buffer, 1, size, pFile);
	fclose(pFile);

	acodec.set_buffer(10000000, buffer);
	acodec.start_decoder();

	int pic_height = readByArithmetic(&acodec, &ahead, 16);
	int pic_width = readByArithmetic(&acodec, &ahead, 16);
    int frameNum = readByArithmetic(&acodec, &ahead, 16);
    int type = acodec.decode(ahead);
	InterMode = 0;
	if (frameNum > 1) InterMode = acodec.decode(ahead);
    numC = readByArithmetic(&acodec, &ahead, 8);

    depthMap = new int[pic_height*pic_width];
	int* m_colorMap = new int[numC];

	for (int i = 0; i < numC; i++)
	{
		m_colorMap[i] = readByArithmetic(&acodec, &ahead, 8);
	}

  unsigned char* pic = new unsigned char[(pic_height*pic_width*frameNum*(type+2))>>1];
  for (int i = 0; i < (pic_height*pic_width*frameNum*(type+2))>>1; i++) 
    pic[i] = 0;

	if (numC == 1)
	{
		int color = m_colorMap[0];
    for (int k = 0; k < frameNum; k++)
    {
      int offset = (pic_height*pic_width*k*(type+2))>>1;
      for (int i = 0; i < pic_height; i++)
      {
        for (int j = 0; j < pic_width; j++)
        {
          pic[offset + i*pic_width + j] = color;
        }
      }
    }
		acodec.stop_decoder();

		write2bin(pic, pic_width*(pic_height*frameNum*(type+2))>>1, fileout);
		end = clock();
		cout << "time: " << (end - begin) / 1000.0 << " sec" << endl;
		delete[] buffer;
		delete[]depthMap;
		return 0;
	}
  if (numC == 2)
  {
    g_biValue = true;
  }
  else
  {
    g_biValue = false;
  }
	aColor.set_alphabet(numC);
	colorList = new int[numC];
	Picture* pcPic = new Picture;
  unsigned char* slice = pic;
  for (int i = 0; i < frameNum; i++)
  {
    reset_arithmetic();
	for (int c = 0; c < numC; c++)
		colorList[c] = c;

    pcPic->createPicture(slice, pic_height, pic_width, g_max_CU, g_min_CU, m_colorMap, numC);
    decodePicture(pcPic, slice, g_max_CU, g_min_CU);
    pcPic->destroyPicture();
	ref_frame = slice;
    slice += (pic_height*pic_width*(type+2))>>1;
  }
	acodec.stop_decoder();

  int offset = (pic_height*pic_width*(type + 2)) >> 1;
  for (int k = 0; k < frameNum; k++)
  {
    for (int i = 0; i < pic_height; i++)
    {
      for (int j = 0; j < pic_width; j++)
      {
        pic[k*offset + i*pic_width + j] = m_colorMap[pic[k*offset + i*pic_width + j]];
      }
    }
  }

  end = clock();
  cout << "time: " << (end - begin) / 1000.0 << " sec" << endl;

  write2bin(pic, pic_width*(pic_height*frameNum*(type+2))>>1, fileout);

	delete[] buffer;
	delete[]depthMap;
	delete[]colorList;
	return 0;
}