#include "Picture.h"
#include "CodingUnit.h"
#include "../../source/commonlib/global_arithmetic.h"
#include <iostream> 

Picture::Picture()
{
  m_numCtuInRow = 0;
	m_numCtuInCol = 0;
  m_colorMap = NULL;
	m_Ctu = NULL;
}

Picture::~Picture()
{

}

CodingUnit* Picture::getCtu(int i)
{
	return m_Ctu[i];
}

void Picture::createPicture(unsigned char*p, int h, int w, int* cMap, int numC)
{
	value = p;
	m_height = h;
	m_width = w;
  m_numCtuInRow = m_height / g_max_CU + (m_height % g_max_CU > 0);
  m_numCtuInCol = m_width / g_max_CU  + (m_width  % g_max_CU > 0);

	m_colorMap = new int[256];
	m_numColor = numC;
	for (int i = 0; i < 256; i++)
	{
    m_colorMap[i] = cMap[i];
	}

  m_Ctu = new CodingUnit*[m_numCtuInRow * m_numCtuInCol];
	for (int i = 0; i < m_numCtuInRow; i++)
	{
		for (int j = 0; j < m_numCtuInCol; j++)
		{
			int loc_row = i*g_max_CU;
			int loc_col = j*g_max_CU;
			int ctuWidth;
			int ctuHeight;
			if (j == m_numCtuInCol - 1)
			{
        ctuWidth = m_width - g_max_CU*j;
			}
			else
			{
        ctuWidth = g_max_CU;
			}
			if (i == m_numCtuInRow - 1)
			{
        ctuHeight = m_height - g_max_CU*i;
			}
			else
			{
        ctuHeight = g_max_CU;
			}
			CodingUnit *Ctu = new CodingUnit;
			Ctu->create(p, h, w, ctuHeight, ctuWidth, loc_row, loc_col, 0);
			m_Ctu[i*m_numCtuInCol + j] = Ctu;
		}
	}
}

void Picture::createPicture(unsigned char*p, int h, int w, int max_cu, int min_cu, int* cMap, int numC)
{
	m_height = h;
	m_width = w;
	m_numCtuInRow = m_height / max_cu + (m_height%max_cu>0);
	m_numCtuInCol = m_width / max_cu + (m_width%max_cu>0);
	void createPicture(int h, int w, int max_cu, int min_cu, int cMap[256], int numC);
	m_numColor = numC;
	m_colorMap = new int[m_numColor];
	for (int i = 0; i < m_numColor; i++)
	{
		m_colorMap[i] = cMap[i];
	}
	m_Ctu = new CodingUnit*[m_numCtuInRow*m_numCtuInCol];
	for (int i = 0; i < m_numCtuInRow; i++)
	{
		for (int j = 0; j < m_numCtuInCol; j++)
		{
			int loc_row = i*max_cu;
			int loc_col = j*max_cu;
			int CU_w;
			int CU_h;
			if (j == m_numCtuInCol - 1)
			{
				CU_w = m_width - max_cu*j;
			}
			else
			{
				CU_w = max_cu;
			}
			if (i == m_numCtuInRow - 1)
			{
				CU_h = m_height - max_cu*i;
			}
			else
			{
				CU_h = max_cu;
			}
			CodingUnit *Ctu = new CodingUnit;
			Ctu->create(p, h, w, CU_h, CU_w, loc_row, loc_col, 0, max_cu);
			m_Ctu[i*m_numCtuInCol + j] = Ctu;
		}
	}
}

void Picture::destroyPicture()
{
	int total_num_ctu = m_numCtuInRow * m_numCtuInCol;
	for (int ctuIdx = 0; ctuIdx < total_num_ctu; ctuIdx++)
	{
		CodingUnit* pcCtu = getCtu(ctuIdx);
		pcCtu->destroy(pcCtu);
	}
	m_Ctu = NULL;
}
