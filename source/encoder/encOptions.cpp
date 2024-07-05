#include "encOptions.h"
#include <iostream>  
using namespace std;

int readOptions(int argc, char* argv[], char* filein, char*fileout, int* rows, int* cols, int* frameNum, int* skip, int* type, int* mode, int* mask_flag, int* speed)
{
	int i;
	// get input para
	for (i = 1; i < argc; i++)
	{
		if (argv[i][0] == '-')
		{
			switch (argv[i][1])
			{
			case 'h':
				cout << " There are 9 kinds of commands.\n -h(help information) -i(input file which only support .yuv now) -o(output file) -r(row number of the graph) -c(col number of the graph) \
					-f(number of frames) -s(skip number of frames) -t(type of video include 400 and 420) -m(whether use inter prediction) -M(whether print quadtree partition)\
           \nfor example: -i D:/input.yuv -r 1024 -c 2048 -f 1 -t 400 -o output.bin " << endl;
				return -1;
				break;
			case 'i':
				i++;
				strcpy_s(filein, strlen(argv[i]) + 1, argv[i]);
				if (filein[strlen(argv[i]) - 1] != 'v' || filein[strlen(argv[i]) - 2] != 'u' || filein[strlen(argv[i]) - 3] != 'y' || filein[strlen(argv[i]) - 4] != '.')
				{
					cout << "only support .yuv now" << endl;
					return -1;
				}
				break;
			case 'o':
				i++;
				strcpy_s(fileout, strlen(argv[i]) + 1, argv[i]);
				break;
			case 'r':
				sscanf_s(argv[++i], "%d", rows);
				break;
			case 'c':
				sscanf_s(argv[++i], "%d", cols);
				break;
			case 'f':
				sscanf_s(argv[++i], "%d", frameNum);
				break;
			case 's':
				sscanf_s(argv[++i], "%d", skip);
				break;
			case 't':
				sscanf_s(argv[++i], "%d", type);
				break;
			case 'm':
				sscanf_s(argv[++i], "%d", mode);
				break;
			case 'M':
				sscanf_s(argv[++i], "%d", mask_flag);
				break;
			case 'S':
				sscanf_s(argv[++i], "%d", speed);
				break;
			default:
				cout << "unsupport command exist" << endl;
				cout << " There are 9 kinds of commands.\n -h(help information) -i(input file which only support .yuv now) -o(output file) \
					-r(row number of the graph) -c(col number of the graph) -f(number of frames) -t(type of video include 400 and 420) -m(whether use inter prediction) -M(whether print quadtree partition)\
          \nfor example: -i D:/input.yuv -r 1024 -c 2048 -f 1 -t 400 -o output.bin " << endl;
				return -1;
				break;
			}
		}
	}
	return 0;
}
