#include <iostream>  
#include <assert.h>
#include <fstream>
#include <iomanip>
#include "../../source/commonlib/global_arithmetic.h"
#include "../../source/commonlib/Picture.h"
#include "../../source/commonlib/CodingUnit.h"
#include "../../source/commonlib/utility.h"
#include "encOptions.h"
#include <time.h>
using namespace std;

AC* compressCU(CodingUnit* pcCU, AC* element = NULL)
{
	int height = pcCU->getHeight();
	int width = pcCU->getWidth();
	unsigned char* value = pcCU->getValue();
	unsigned char* img = pcCU->getImg();
	int depth = pcCU->getDepth();
	int pic_width = pcCU->pic_width;
	int pic_height = pcCU->pic_height;
	int loc_row = pcCU->location_row;
	int loc_col = pcCU->location_col;
	if(element) pcCU->element->copy(element);
	else element = new AC;

	/*is bound -- split*/
	int side_length = g_max_CU / (2 << depth);
	if (side_length == g_min_CU / 2)  
	{
		assert(height == g_min_CU && width == g_min_CU);
	}
	else if(height != 2 * side_length || width != 2 * side_length)   //out of bound
	{
		if (side_length >= height && side_length >= width)
		{
			pcCU->split_flag = true;
			CodingUnit *CU = new CodingUnit;
			CU->create(img, pic_height, pic_width, height, width, loc_row, loc_col, depth + 1);
			pcCU->child[0] = CU;
			AC* element1 = compressCU(CU, pcCU->element);
			pcCU->rate = CU->rate;
			pcCU->element->copy(element1);
			return pcCU->element;
		}
		else if (side_length >= height && side_length < width)
		{
			pcCU->split_flag = true;
			CodingUnit *CU1 = new CodingUnit;
			CU1->create(img, pic_height, pic_width, height, side_length, loc_row, loc_col, depth + 1);
			pcCU->child[0] = CU1;
			AC* element1 = compressCU(CU1, pcCU->element);
			CodingUnit *CU2 = new CodingUnit;
			CU2->create(img, pic_height, pic_width, height, width - side_length, loc_row, loc_col + side_length, depth + 1);
			pcCU->child[1] = CU2;
			AC* element2 = compressCU(CU2, element1);
			pcCU->rate = CU1->rate + CU2->rate;
			pcCU->element->copy(element2);
			return pcCU->element;
		}
		else if (side_length < height && side_length >= width)
		{
			pcCU->split_flag = true;
			CodingUnit *CU1 = new CodingUnit;
			CU1->create(img, pic_height, pic_width, side_length, width, loc_row, loc_col, depth + 1);
			pcCU->child[0] = CU1;
			AC* element1 = compressCU(CU1, pcCU->element);
			CodingUnit *CU2 = new CodingUnit;
			CU2->create(img, pic_height, pic_width, height - side_length, width, loc_row + side_length, loc_col, depth + 1);
			pcCU->child[2] = CU2;
			AC* element2 = compressCU(CU2, element1);
			pcCU->rate = CU1->rate + CU2->rate;
			pcCU->element->copy(element2);
			return pcCU->element;
		}
		else
		{
			pcCU->split_flag = true;
			CodingUnit *CU1 = new CodingUnit;
			CU1->create(img, pic_height, pic_width, side_length, side_length, loc_row, loc_col, depth + 1);
			pcCU->child[0] = CU1;
			AC* element1 = compressCU(CU1, pcCU->element);
			CodingUnit *CU2 = new CodingUnit;
			CU2->create(img, pic_height, pic_width, side_length, width - side_length, loc_row, loc_col + side_length, depth + 1);
			pcCU->child[1] = CU2;
			AC* element2 = compressCU(CU2, element1);
			CodingUnit *CU3 = new CodingUnit;
			CU3->create(img, pic_height, pic_width, height - side_length, side_length, loc_row + side_length, loc_col, depth + 1);
			pcCU->child[2] = CU3;
			AC* element3 = compressCU(CU3, element2);
			CodingUnit *CU4 = new CodingUnit;
			CU4->create(img, pic_height, pic_width, height - side_length, width - side_length, loc_row + side_length, loc_col + side_length, depth + 1);
			pcCU->child[3] = CU4;
			AC* element4 = compressCU(CU4, element3);
			pcCU->rate = CU1->rate + CU2->rate + CU3->rate + CU4->rate;
			pcCU->element->copy(element4);
			return pcCU->element;
		}
	}

	/*coding or split*/

	bool OneColor = isOneColor(value, height, width);
	bool ContiEdge = false;
	bool Inter = false;
	if (!OneColor)
	{
		ContiEdge = isContiEdge(value, height, width, width);
		if (ContiEdge) pcCU->setEdge();
		if (InterMode && ref_frame && height <= g_max_inter && height >= g_min_inter)
		{
			Inter = isInter(value, height, width, loc_row, loc_col, pic_height, pic_width, pcCU, ContiEdge);
			pcCU->element->copy(element);
			pcCU->rate = 0;
		}

	}

	/*coding or split*/
	if (OneColor)
	{
		// encode one_color_flag
		pcCU->split_flag = false;
		if (side_length > g_min_CU / 2) pcCU->enSplitFlag(false);
		pcCU->oneColor = true;
		pcCU->setColorValue(value[0]);
		pcCU->update(pcCU->element->aOneColorFlag, 1);
		pcCU->enOneColor(false);
		return pcCU->element;
	}
	/*else if the current CU has two colors and only one continous edge*/
	else if (ContiEdge || Inter)
	{
		//encode the edge with chain code
		if (ContiEdge)
		{
			pcCU->split_flag = false;
			if (side_length > g_min_CU / 2) pcCU->enSplitFlag(false);
			pcCU->contiEdge = true;
			pcCU->update(pcCU->element->aOneColorFlag, 0);
			if (side_length == g_min_CU / 2 || (InterMode && ref_frame && height <= g_max_inter && height >= g_min_inter))	pcCU->update(pcCU->element->aContinueEdgeFlag, 1);
			pcCU->enChainCoding(false);
		}
		if (Inter)
		{
			if (ContiEdge)
			{
				float rate = pcCU->rate;
				pcCU->rate = 0;
				AC* element_origin = new AC;
				element_origin->copy(pcCU->element);
				pcCU->element->copy(element);
				pcCU->split_flag = false;
				pcCU->enSplitFlag(false);
				pcCU->update(pcCU->element->aOneColorFlag, 0);
				pcCU->update(pcCU->element->aContinueEdgeFlag, 0);
				pcCU->enMV(false);
				if (rate > pcCU->rate + g_CTU_scale * g_offset)
				{
					pcCU->contiEdge = false;
				}
				else
				{
					pcCU->rate = rate;
					pcCU->element->copy(element_origin);
					delete element_origin;
				}
			}
			else
			{
				pcCU->split_flag = false;
				pcCU->enSplitFlag(false);
				pcCU->update(pcCU->element->aOneColorFlag, 0);
				pcCU->update(pcCU->element->aContinueEdgeFlag, 0);
				pcCU->enMV(false);
			}
		}

		if (side_length > g_RO_size / 2)
		{
			float rate = pcCU->rate;
			pcCU->rate = 0;
			AC* element_origin = new AC;
			element_origin->copy(pcCU->element);
			pcCU->element->copy(element);
			pcCU->split_flag = true;
			pcCU->enSplitFlag(false);
			CodingUnit *CU1 = new CodingUnit;
			CU1->create(img, pic_height, pic_width, side_length, side_length, loc_row, loc_col, depth + 1);
			pcCU->child[0] = CU1;
			AC* element1 = compressCU(CU1, pcCU->element);
			CodingUnit *CU2 = new CodingUnit;
			CU2->create(img, pic_height, pic_width, side_length, side_length, loc_row, loc_col + side_length, depth + 1);
			pcCU->child[1] = CU2;
			AC* element2 = compressCU(CU2, element1);
			CodingUnit *CU3 = new CodingUnit;
			CU3->create(img, pic_height, pic_width, side_length, side_length, loc_row + side_length, loc_col, depth + 1);
			pcCU->child[2] = CU3;
			AC* element3 = compressCU(CU3, element2);
			CodingUnit *CU4 = new CodingUnit;
			CU4->create(img, pic_height, pic_width, side_length, side_length, loc_row + side_length, loc_col + side_length, depth + 1);
			pcCU->child[3] = CU4;
			AC* element4 = compressCU(CU4, element3);

			if (rate > pcCU->rate + CU1->rate + CU2->rate + CU3->rate + CU4->rate + g_CTU_scale * g_offset)
			{
				pcCU->element->copy(element4);
				pcCU->rate = pcCU->rate + CU1->rate + CU2->rate + CU3->rate + CU4->rate;
			}
			else
			{
				pcCU->split_flag = false;
				pcCU->element->copy(element_origin);
				delete element_origin;
				pcCU->rate = rate;
				for (int i = loc_row; i < loc_row + height; i++)
				{
					for (int j = loc_col; j < loc_col + width; j++)
					{
						depthMap[i * pic_width + j] = depth;
					}
				}
			}
		}

		return pcCU->element;
	}
	else if (side_length == g_min_CU / 2)
	{
		//encode the edge with runlength
		pcCU->setRunLength();
		pcCU->update(pcCU->element->aOneColorFlag, 0);
		pcCU->update(pcCU->element->aContinueEdgeFlag, 0);
		pcCU->enRunLength(false);
		return pcCU->element;
	}
	else
	{
		/*else quad-tree partitioning*/
		pcCU->split_flag = true;
		pcCU->enSplitFlag(false);
		CodingUnit *CU1 = new CodingUnit;
		CU1->create(img, pic_height, pic_width, side_length, side_length, loc_row, loc_col, depth + 1);
		pcCU->child[0] = CU1;
		AC* element1 = compressCU(CU1, pcCU->element);
		CodingUnit *CU2 = new CodingUnit;
		CU2->create(img, pic_height, pic_width, side_length, side_length, loc_row, loc_col + side_length, depth + 1);
		pcCU->child[1] = CU2;
		AC* element2 = compressCU(CU2, element1);
		CodingUnit *CU3 = new CodingUnit;
		CU3->create(img, pic_height, pic_width, side_length, side_length, loc_row + side_length, loc_col, depth + 1);
		pcCU->child[2] = CU3;
		AC* element3 = compressCU(CU3, element2);
		CodingUnit *CU4 = new CodingUnit;
		CU4->create(img, pic_height, pic_width, side_length, side_length, loc_row + side_length, loc_col + side_length, depth + 1);
		pcCU->child[3] = CU4;
		AC* element4 = compressCU(CU4, element3);
		pcCU->rate = pcCU->rate + CU1->rate + CU2->rate + CU3->rate + CU4->rate;
		pcCU->element->copy(element4);
		return pcCU->element;
	}
}

void encodeCU(CodingUnit* pcCU)
{
	// encode split_flag
	int depth = pcCU->getDepth();
	if (pcCU->split_flag)
	{
		if (pcCU->getHeight() == (g_max_CU >> depth) && (pcCU->getWidth() == g_max_CU >> depth))
		{
			pcCU->enSplitFlag();               //if the CU is not the minimum size CU, encode split_flag.
		}
		for (int childIndex = 0; childIndex < 4; childIndex++)
			if (NULL != pcCU->child[childIndex]) encodeCU(pcCU->child[childIndex]);
	}
	else
	{
		if (pcCU->getWidth() != g_min_CU || pcCU->getHeight() != g_min_CU)
		{
			pcCU->enSplitFlag();               //if the CU is not the minimum size CU, encode split_flag.
		}
		// encode one_color_flag
		if (pcCU->oneColor) {
			acodec.encode(1, aOneColorFlag);
			pcCU->enOneColor();
		}
		else if (pcCU->contiEdge) {
			// encode Chain coding
			acodec.encode(0, aOneColorFlag);
			if ((pcCU->getWidth() == g_min_CU && pcCU->getHeight() == g_min_CU) || (InterMode && ref_frame && pcCU->getWidth() <= g_max_inter && pcCU->getWidth() >= g_min_inter))
				acodec.encode(1, aContinueEdgeFlag);
			pcCU->enChainCoding();
		}
		else if (pcCU->getWidth() == g_min_CU && pcCU->getHeight() == g_min_CU)
		{
			// encode runlength and encode color value directly if g_min_CU == 2
			acodec.encode(0, aOneColorFlag);
			acodec.encode(0, aContinueEdgeFlag);
			pcCU->enRunLength();
		}
		else
		{
			//cout << "mvy: " << pcCU->mvy << endl;
			//cout << "mvx: " << pcCU->mvx << endl;
			//cout << "col: " << pcCU->location_col << endl;
			//cout << "row: " << pcCU->location_row << endl;
			//cout << "side: " << pcCU->getHeight() << endl;
			//cout << " " << endl;

			//if (!pcCU->noResidue)
			//{
			//	int* edge = pcCU->getEdge();
			//	int length = edge[4] - pcCU->context;
			//	int* dif = pcCU->difEdge;
			//	printf("%d %d\n", dif[0], dif[1]);
			//	for (int i = 0; i < length; i++)
			//		printf("%d ", dif[2 + 3 * i]);
			//	printf("\n");
			//	for (int i = 0; i < length; i++)
			//		printf("%d ", dif[2 + 3 * i + 1]);
			//	printf("\n");
			//	for (int i = 0; i < length; i++)
			//		printf("%d ", dif[2 + 3 * i + 2]);
			//	printf("\n");
			//	printf("\n");
			//}

			acodec.encode(0, aOneColorFlag);
			acodec.encode(0, aContinueEdgeFlag);
			pcCU->enMV();
		}
	}
}


void compressPicture(Picture* pcPic)
{
	int total_num_ctu = pcPic->m_numCtuInRow * pcPic->m_numCtuInCol;
	AC* element1 = NULL;
	AC* element2 = NULL;

	for (int ctuIdx = 0; ctuIdx < total_num_ctu; ctuIdx++)      //compress one CTU by one CTU, can precess in parallel.
	{
		CodingUnit* pcCtu = pcPic->getCtu(ctuIdx);
		element2 = compressCU(pcCtu, element1);
		element1 = element2;
		g_rate += pcCtu->rate/8;
		//g_CTU_scale = pow(0.5, g_rate / g_scale);
	}
	//cout << g_rate << endl;

}

void encodePicture(Picture* pcPic)
{
	int total_num_ctu = pcPic->m_numCtuInRow * pcPic->m_numCtuInCol;
	for (int ctuIdx = 0; ctuIdx < total_num_ctu; ctuIdx++)
	{
		CodingUnit* pcCtu = pcPic->getCtu(ctuIdx);
		encodeCU(pcCtu);
		if (mask_flag) draw_mask(mask, pcPic->m_width, pcCtu, ctuIdx);
	}
}

void encodeHead(unsigned char* buffer, int rows, int cols, int frameNum, int type, char* file_name, unsigned char* encodeStream, int* cMap, int* numC)
{
	acodec.set_buffer(10000000, encodeStream);
	acodec.start_encoder();

	writeByArithmetic(&acodec, &ahead, rows, 16);
	writeByArithmetic(&acodec, &ahead, cols, 16);
  writeByArithmetic(&acodec, &ahead, frameNum, 16);
  if (type == 400)
  {
    acodec.encode(0, ahead);
  }
  else
  {
    acodec.encode(1, ahead);
  }
  if (frameNum > 1) acodec.encode(InterMode, ahead);
  int m_numColor = 0;
  int offset = rows*cols;
  if (type == 420)
  {
    offset = (offset * 3) >> 1;
  }
  for (int k = 0; k < frameNum; k++)
  {
    for (int i = 0; i < rows; i++)
    {
      for (int j = 0; j < cols; j++)
      {
        int index = buffer[offset*k + i*cols + j];
        if (cMap[index] > 255)
        {
          cMap[index] = m_numColor;
          buffer[offset*k + i*cols + j] = m_numColor;
          m_numColor++;
        }
        else
        {
          buffer[offset*k + i*cols + j] = cMap[index];
        }
      }
    }
  }
  writeByArithmetic(&acodec, &ahead, m_numColor, 8);
  *numC = m_numColor;

	for (int i = 0; i < m_numColor; i++)
	{
		for (int j = 0; j < 256; j++)
		{
			if (cMap[j] == i)
			{
				writeByArithmetic(&acodec, &ahead, j, 8);
				break;
			}
		}
	}
  if (m_numColor > 1)
  {
    aColor.set_alphabet(m_numColor);
	colorList = new int[m_numColor];
  }
  if (m_numColor == 2)
  {
    g_biValue = true;
  }
  else
  {
    g_biValue = false;
  }
}

int main(int argc, char** argv)
{
	int begin, end;
	begin = clock();
	int rows;
	int cols;
  int frameNum = 1;
  int skip = 0;
  int speed = 0;
  int type = 400;
	char filein[1000];
	char fileout[1000];
	unsigned char* mask_point = NULL;
	int error = readOptions(argc, argv, filein, fileout, &rows, &cols, &frameNum, &skip, &type, &InterMode, &mask_flag, &speed);
	if (error < 0) { getchar(); return 0; }
	if (frameNum == 1)
	{
		InterMode = 0;
		if (speed) g_RO_size = g_max_CU;
		else g_RO_size = g_min_CU;
	}
	else
	{
		if (speed) g_search_range = 8;
		g_RO_size = g_min_CU;
		if(speed && InterMode == 0) g_RO_size = g_max_CU;
	}
	int size;
	int real_size;
	int skip_size;
	FILE * pFile;
	fopen_s(&pFile, filein, "rb");
	size = getFSize(pFile);
	if (type == 400)
	{
		real_size = rows * cols * frameNum;
		skip_size = rows * cols * skip;
		assert(size >= real_size + skip_size);
	}
	if (type == 420)
	{
		real_size = ((rows*cols* frameNum * 3) >> 1);
		skip_size = ((rows*cols* skip * 3) >> 1);
		assert(size >= real_size + skip_size);
	}
	unsigned char* buffer = new unsigned char[real_size];
	fseek(pFile, skip_size, SEEK_SET);
	fread(buffer, 1, real_size, pFile);

	if (mask_flag)
	{
		mask = new unsigned char[real_size];
		fseek(pFile, 0, SEEK_SET);
		fread(mask, 1, real_size, pFile);
		mask_point = mask;
	}
	fclose(pFile);
  unsigned char* graph = buffer;
  unsigned char* encodeStream = new unsigned char[10000000];
	for (int i = 0; i < 10000000; i++)
		encodeStream[i] = 0;

  depthMap = new int[rows*cols];
  int* cMap = new int[256];
  for (int i = 0; i < 256; i++) 
    cMap[i] = 256;
  encodeHead(buffer, rows, cols, frameNum, type, fileout, encodeStream, cMap, &numC);
  Picture* pcPic = new Picture;
  if (numC > 1)
  {
    for (int i = 0; i < frameNum; i++)
    {
      reset_arithmetic();
	  for (int c = 0; c < numC; c++)
		  colorList[c] = c;

      pcPic->createPicture(graph, rows, cols, cMap, numC);
      compressPicture(pcPic);
      encodePicture(pcPic);
      pcPic->destroyPicture();
	  ref_frame = graph;
	  if (type == 400)
	  {
		  graph += rows * cols;
		  if (mask_flag) mask += rows * cols;
	  }
	  if (type == 420)
	  {
		  graph += ((rows*cols * 3) >> 1);
		  if (mask_flag) mask += ((rows*cols * 3) >> 1);
	  }
    }
  }

  int bytenum = acodec.stop_encoder();
	write2bin(encodeStream, bytenum, fileout);
	cout << "byteNum: " << bytenum << endl;
	end = clock();
	cout << "time: " << (end - begin) / 1000.0 << " sec" << endl;

	if (mask_flag)
	{
		write2bin(mask_point, real_size, "mask.yuv");
		delete[]mask;
	}

	delete[]encodeStream;
	delete[]buffer;
  delete[]depthMap;
  delete[]cMap;
  if (numC > 1)   delete[]colorList;

	return 0;
}
