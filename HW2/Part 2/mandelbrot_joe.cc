/**
 *  \file mandelbrot_joe.cc
 *
 *  \brief Implement your parallel mandelbrot set in this file.
 */

#include <iostream>
#include <cstdlib>
#include <mpi.h>
#include "render.hh"
#include <cmath>

using namespace std;

#define WIDTH 1000
#define HEIGHT 1000
#define Total_No_Of_Rows 1000


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
  int rank,size;
  double it = (maxY - minY)/height;
  double jt = (maxX - minX)/width;
  double x, y;
  double time_start;
  MPI_Init(&argc,&argv);
  MPI_Comm_rank(MPI_COMM_WORLD,&rank);
  MPI_Comm_size(MPI_COMM_WORLD,&size);
  //printf("%d and %d",rank,size);
   
  int Rows_split_per_processes = height/size;
  int data_count = (Rows_split_per_processes*width);
  int send_buffer[data_count];

  time_start= MPI_Wtime();
  y = minY+(rank*Rows_split_per_processes*it);
  for (int i = 0; i < Rows_split_per_processes; ++i) {
    x = minX;
    for (int j = 0; j < width; ++j) {
      send_buffer[(i*width)+j] = mandelbrot(x, y);
      x += jt;
    }
    y += it;
   }
  
  MPI_Barrier(MPI_COMM_WORLD);
  int *rec_buffer =0; 
  if(rank == 0){
      rec_buffer = new int[size*data_count];
         
  }
  
  MPI_Gather(send_buffer,data_count,MPI_INT,rec_buffer,data_count,MPI_INT,0,MPI_COMM_WORLD);
 // printf("run time of mandelbrot_joe is %f\n",MPI_Wtime()-time_start);
  if(rank == 0){
    
     gil::rgb8_image_t img(height, width);
     auto img_view = gil::view(img);
     for(int k =0;k<(Rows_split_per_processes*size);++k){
	for(int p=0;p<width;p++){
	    img_view(p,k) = render(rec_buffer[(k*width)+p]/512.0);
	}  
     }
  gil::png_write_view("mandelbrot_joe_for_1000.png", const_view(img));
  }
  printf("run time for joe is %f\n",MPI_Wtime()-time_start);	 
  MPI_Finalize(); 
  return 0;
}


/* eof */
