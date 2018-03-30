/**
 *  \file mandelbrot_ms.cc
 *
 *  \brief Implement your parallel mandelbrot set in this file.
 */

#include <iostream>
#include <cstdlib>
#include <mpi.h>
#include "render.hh"
using namespace std;

//#define WIDTH 1000
//#define HEIGHT 1000
#define tag 1 
#define Fin_tag 0

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


void master(int height,int width,double minX, double jt, double minY,int size){
  
  double start_time = MPI_Wtime(); 
  int *store_buffer= new int[height*width];
  MPI_Status status;
  int rec_buffer[width];
  int next_row = 0;
  for (int i = 1; i < size; i++) {
	MPI_Send(&next_row, 1, MPI_INT, i,tag, MPI_COMM_WORLD);
	next_row++;
  }
  while (next_row < height) {
		MPI_Recv(rec_buffer, width + 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
		MPI_Send(&next_row, 1, MPI_INT, status.MPI_SOURCE, tag, MPI_COMM_WORLD);
 		memcpy(store_buffer + (rec_buffer[width] * width), rec_buffer, width * sizeof(int));
		next_row++;
  } 		
  for (int i = 1; i < size; i++) {
 		MPI_Recv(rec_buffer, width + 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
                MPI_Send(0, 0, MPI_INT, status.MPI_SOURCE, Fin_tag, MPI_COMM_WORLD);
		memcpy(store_buffer+ (rec_buffer[width] * width), rec_buffer, width * sizeof(int));
		 																		}
  
  printf("problem is not here");
  printf("the runtime for mandelbrot_ms is %f", MPI_Wtime()-start_time);
  
 
  gil::rgb8_image_t img(height, width);
  auto img_view = gil::view(img);
  for(int k =0;k<height;++k){
     for(int p=0;p<width;p++){
          img_view(p,k) = render(store_buffer[(k*width)+p]/512.0);
     }
  }
  gil::png_write_view("mandelbrot_ms_for_2000.png", const_view(img));
}

void slave(int rank,int width,double minX,double it,double minY,double jt){
  int buffer_send[width+1];
  MPI_Status status;
  int compute_row;
  double tmp_x, tmp_y;
  while (true) {
     MPI_Recv(&compute_row, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
     if (status.MPI_TAG != Fin_tag) {
           tmp_y = minY + (compute_row * it);
           tmp_x = minX;
           int index=0;
           while(index<width){
                 buffer_send[index++] = mandelbrot(tmp_x, tmp_y);
                 tmp_x += jt;
                 index++;
           }
           buffer_send[width] = compute_row; 
           MPI_Send(buffer_send, width + 1, MPI_INT, 0, tag, MPI_COMM_WORLD);
     }
     else
          continue ;
   }
 printf("the problem is not here");
}


int
main (int argc, char* argv[])
{
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

  double it = (maxY - minY)/height;
  double jt = (maxX - minX)/width;
  double x, y;
  int rank,size;
  MPI_Init(&argc,&argv);
  
  MPI_Comm_rank(MPI_COMM_WORLD,&rank);
  MPI_Comm_size(MPI_COMM_WORLD,&size);
  
  if(rank == 0){
     master(height,width,minX,jt,minY,size);
  }else {
     slave(rank,width,minX,it,minY,jt);
  }                                                                                                                                                            MPI_Finalize();
  return 0;
}

/* eof */
