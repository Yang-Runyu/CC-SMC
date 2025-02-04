# CC-SMC
Chain Coding-Based Segmentation Map Lossless Compression (CC-SMC) is a software for segmentation map lossless compression. The platform is VS2017.

  There are 9 kinds of commands.  
  -h(help information)  
  -i(input file which only support .yuv now)  
  -o(output file)  
  -r(row number of the graph)  
  -c(col number of the graph)  
  -f(number of frames)  
  -s(skip number of frames)  
  -t(type of video include 400 and 420)  
  -m(whether use inter prediction)  
  -M(whether print quadtree partition)  
  for example:  
  encoder.exe -i D:/input.yuv -r 1024 -c 2048 -f 1 -t 400 -o output.bin  
  decoder.exe -i output.bin -o recover.yuv  
