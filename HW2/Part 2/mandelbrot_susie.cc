/**
 *  \file mandelbrot_serial.cc
 *  \brief Lab 2: Mandelbrot set serial code
 */


#include <iostream>
#include <cstdlib>
#include <mpi.h>
#include "render.hh"

using namespace std;

#define WIDTH 1000
#define HEIGHT 1000

int
mandelbrot(double x, double y) {
  int maxit = 511;
  double cx = x;
  double cy = y;
  double newx, newy;

  int it = 0;
  for (it = 0; it < maxit && (x*x + y*y) < 4; ++it) {
    newx = x*x - y*y + cx;
    newy = 2*x*y + cy;
    x = newx;
    y = newy;
  }
  return it;
}

int
main(int argc, char* argv[]) {
/* Lucky you, you get to write MPI code */

 
  double minX = -2.1;
  double maxX = 0.7;
  double minY = -1.25;
  double maxY = 1.25;

  int height, width;
  if (argc == 3) {
    height = atoi (argv[1]);
    width = atoi (argv[2]);
    assert (height > 0 && width > 0);
  } else {
    fprintf (stderr, "usage: %s <height> <width>\n", argv[0]);
    fprintf (stderr, "where <height> and <width> are the dimensions of the image.\n");
    return -1;
  }
  
  double time_start;
  double it = (maxY - minY)/height;
  double jt = (maxX - minX)/width;
  double x, y;
  int rank,total_process;
  MPI_Init(&argc,&argv);
  MPI_Comm_rank(MPI_COMM_WORLD,&rank);
  MPI_Comm_size(MPI_COMM_WORLD,&total_process);

  gil::rgb8_image_t img(height, width);
  auto img_view = gil::view(img);


  int *buffer_receive = new int[width*height];
  int buffer_send[(height*width)/total_process];

  time_start = MPI_Wtime();
 
  y = minY + rank*it;
  for(int i = 0; i < (height/total_process); i++)
  {
    x = minX;
    for(int j = 0; j < width; ++j)
    {
      buffer_send[(i*width)+j] = mandelbrot(x,y);
      x += jt;
    }
    y += it*total_process;
  }


  MPI_Gather(buffer_send, (height/total_process)*width, MPI_INT, buffer_receive, (height/total_process)*width, MPI_INT, 0, MPI_COMM_WORLD);
  //printf("Run time for susie is %f\n",MPI_Wtime()-time_start);  
  if(rank == 0)
  {
    int chunk = 0,row=0,col=0;

    while(row<height)
    {

    chunk = row%total_process==0?row/total_process:chunk;
    col=0;
      while(col < width)
      {
        img_view(col++,row) = render(buffer_receive[(chunk*width)+col]/512.0);
      }
      chunk += height/total_process;
      row++;
    }


  gil::png_write_view("mandelbrot_susie_for_1000.png", const_view(img));
  }
  
  printf("run time is %f\n",MPI_Wtime()-time_start);
  MPI_Finalize();

  return 0;
}

/* eof */
