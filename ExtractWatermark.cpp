#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <cmath>
#include "Haar.h"

const double PI = 3.14159;

using namespace cv;
using namespace std;

double ***initialize_hsi(Mat image);
void to_hsi(double ***hsi, Mat image);
void to_rgb(double ***hsi, Mat &image);
void embed(double ***hsi, double ***watermarkHsi, Mat image);
void toArray(Mat image, double ***array);
void toImage(Mat image, double ***array);
double to_rad(int deg);
void brighten_results(double***hsi, Mat &image);

int main(int argc, char** argv )
{
    if ( argc != 3 )
    {
        cout << "usage: DisplayImage.out <Image_Path> <Image_Path>\n";
        return -1;
    }
    //create image container to hold input image
    Mat image;
    Mat watermark;
    //read image to image container
    image = imread( argv[1], 1 );
    watermark = imread( argv[2], 1 );
    // Mat tempImage = Mat::zeros(image.size()*2,image.type());

    if ( !image.data )
    {
        cout << "No image data \n";
        return -1;
    }

    if ( !watermark.data )
    {
        cout << "No watermark data \n";
        return -1;
    }

    /* initialize an array of doubles to hold hsi values */
    double ***hsi = initialize_hsi(image); 
    double ***watermarkHsi = initialize_hsi(watermark); 

    /* convert image to hsi */
    to_hsi(hsi, image);
    to_hsi(watermarkHsi, watermark);

    //create 2D Haar wavelets
    haar2D(hsi,image.rows,image.cols);
    // haar2(watermarkHsi,watermark.rows,watermark.cols);

    // brighten_results(hsi,image);
    //embed image
    // embed(hsi, watermarkHsi, image);

    //convert from 2D Haar wavelets to pixels
    // invHaar2D(hsi, image.rows, image.cols);

    /* convert image to rgb */
    to_rgb(hsi, image);

    // namedWindow("Display Image", WINDOW_AUTOSIZE );
    //namedWindow("New Image", WINDOW_AUTOSIZE );
    // imshow("Display Image", image);
    imwrite("extract.jpg",image);
    //imshow("New Image", temp);
    waitKey(0);

    return 0;
}


/* initializes hsi array to zeros */
double ***initialize_hsi(Mat image)
{
  double ***hsi;
  int row, col;
  //create 3d array to store histogram
  hsi = new double **[image.rows];
  //initialize array to zeros
  for (row = 0; row < image.rows; row++){
    hsi[row] = new double*[image.cols];
    for (col = 0; col < image.cols; col++){
      hsi[row][col] = new double[3];
      hsi[row][col][0] = 0;
      hsi[row][col][1] = 0;
      hsi[row][col][2] = 0;

    }
  }
  return hsi;
}

/* convert image to hsi */
void to_hsi(double ***hsi, Mat image)
{
  int row, col;
  double h, s, i, r, g, b;
  for (row = 0; row < image.rows; row++) {
    for (col = 0; col < image.cols; col++) {
      //save rgb values into temp variables
      r = (double)image.at<Vec3b>(row,col)[0];
      g = (double)image.at<Vec3b>(row,col)[1];
      b = (double)image.at<Vec3b>(row,col)[2];

      //calculate i
      i = (r+g+b);

      //calculate s
      if (r==0&&g==0&&b==0){
        s = 1;
      } else {
        s = 1 - (3*min(r,min(g,b))/(r+g+b));
      }
      //check for undefined h
      if (r==g && r==b){
        h = 0.0;
      } else {
        //calculate h
        if (b <= g){
          h = acos(((r-g/2-b/2))/sqrt(pow((r),2) + pow((g),2) + pow((b),2) - r*g - r*b - g*b));
        }
        else if (b > g){
              h = 2*PI - acos(((r-g/2-b/2))/sqrt(pow((r),2) + pow((g),2) + pow((b),2) - r*g - r*b - g*b));
        }
      }
      h = h/(2*PI);

      //save values to hsi array
      hsi[row][col][0] = h;
      hsi[row][col][1] = s;
      hsi[row][col][2] = i;
    }
  }
}

/* convert image from hsi to rgb */
void to_rgb(double ***hsi, Mat &image)
{
  int row, col, r, g, b;
  double h, s, i;
  for (row = 0; row < image.rows; row++) {
    for (col = 0; col < image.cols; col++) {
      //save hsi to temp variables
      h = hsi[row][col][0];
      s = hsi[row][col][1];
      i = hsi[row][col][2]/3;
      h = h*(2*PI);
      
      //calculate rgb values
      if (h==0){
        r = static_cast<int>(i + (2 * i * s));
        g = static_cast<int>(i - (i * s));
        b = static_cast<int>(i - (i * s));

      } else if (h>=0 && h<to_rad(120)){
        r = static_cast<int>(i + (i * s) * cos(h) / cos(to_rad(60)-h));
        g = static_cast<int>(i+ (i * s) * (1 - cos(h) / cos(to_rad(60)-h)));
        b = static_cast<int>(i - (i * s));

      } else if (h>=to_rad(120) && h<to_rad(240)) {
        r = static_cast<int>(i - (i * s));
        g = static_cast<int>(i + (i * s) * cos(h-to_rad(120)) / cos(to_rad(180)-h));
        b = static_cast<int>(i + (i * s) * (1 - cos(h-to_rad(120)) / cos(to_rad(180)-h)));

      } else if (h>=to_rad(240) && h <to_rad(360)){
        r = static_cast<int>(i + (i * s) * (1 - cos(h-to_rad(240)) / cos(to_rad(300)-h)));
        g = static_cast<int>(i - (i * s));
        b = static_cast<int>(i + (i * s) * cos(h-to_rad(240)) / cos(to_rad(300)-h));
      }
      //set rgb to i if saturation = 0
      if (s == 0){
        r = i;
        g = i;
        b = i;
      }
      //stop rgb from going out of bounds [0-255]
      if (r<0){
        r = 0;
      } else if (r>255){
        r = 255;
      } 
      if (g<0){
        g = 0;
      } else if (g>255){
        g=255;
      }
      if (b<0){
        b=0;
      } else if (b>255){
        b=255;
      }
      //save values to original image
      image.at<Vec3b>(row,col)[0] = r;
      image.at<Vec3b>(row,col)[1] = g;
      image.at<Vec3b>(row,col)[2] = b;

    }
  }
}



void embed(double ***hsi, double ***watermarkHsi, Mat image)
{
    int horizCenter = image.rows/2;
    int vertCenter = image.cols/2;
    // double max = 0;
    // for(int x=horizCenter; x < image.rows;x++)
    // {
    //     for(int y=vertCenter; y < image.cols;y++)
    //     {
    //         if (hsi[x][y][2] > max)
    //             max = hsi[x][y][2];
    //     }
    // }
    for(int row=0;row<image.rows/2;row++)
    {
        for(int col=0;col<image.cols/2;col++)
        {
            hsi[row+horizCenter][col+vertCenter][2] = hsi[row+horizCenter][col+vertCenter][2] + watermarkHsi[row*2][col*2][2]; 
        }
    }

}

void toArray(Mat image, double ***array)
{
    for(int y = 0; y < image.rows; y++){
     for (int x = 0; x < image.cols; x++)
     {
         array[x][y][0] = (double)image.at<Vec3b>(y,x)[0];
         array[x][y][1] = (double)image.at<Vec3b>(y,x)[1];
         array[x][y][2] = (double)image.at<Vec3b>(y,x)[2];
     }
    }
}

void brighten_results(double***hsi, Mat &image)
{
  for(int x=image.rows/2;x<image.rows;x++)
  {
    for(int y=image.cols/2;y<image.cols;y++)
    {
      hsi[x][y][2] = hsi[x][y][2]*20;
    }
  }
}
void toImage(Mat image, double ***array)
{
    for(int y = 0; y < image.rows; y++){
     for (int x = 0; x < image.rows; x++)
     {
        if(array[x][y][0]<0)
            array[x][y][0]=0;
        if(array[x][y][1]>255)
            array[x][y][1]=255;
        if(array[x][y][2]<0)
            array[x][y][2]=0;
        if(array[x][y][2]>255)
            array[x][y][2]=255;
         image.at<Vec3b>(y,x)[0] = array[x][y][0];
         image.at<Vec3b>(y,x)[1] = array[x][y][1];
         image.at<Vec3b>(y,x)[2] = array[x][y][2];
     }
    }
}

/* convert degrees to radians */
double to_rad(int deg){
  return deg * PI/180; 
}