#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <cmath>
#include "Haar.h"

const double PI = 3.14159;

using namespace cv;
using namespace std;

double ***initialize_hsi(Mat image);
double ***storeA(double ***hsi, Mat image);
double ***calculateB(double ***hsi, Mat image);
double ***calculateC(double ***A, double ***B, Mat image);
void applyArnoldTransformation(double ***C, Mat image);
void embedWatermark(double ***C, double ***hsi, Mat image);
void to_hsi(double ***hsi, Mat image);
void to_rgb(double ***hsi, Mat &image);
void toArray(Mat image, double ***array);
void toImage(Mat image, double ***array);
double to_rad(int deg);

int main(int argc, char** argv )
{
    if ( argc != 2 )
    {
        cout << "usage: DisplayImage.out <Image_Path> " << endl;
        return -1;
    }
    //create image container to hold input image
    Mat image;
    //read image to image container
    image = imread( argv[1], 1 );
    // Mat tempImage = Mat::zeros(image.size()*2,image.type());

    if ( !image.data )
    {
        cout << "No image data \n";
        return -1;
    }

    /* initialize an array of doubles to hold hsi values */
    double ***hsi = initialize_hsi(image); 

    /* convert image to hsi */
    to_hsi(hsi, image);

    //create 2D Haar wavelets
    haar2D(hsi,image.rows,image.cols);
    // haar2(watermarkHsi,watermark.rows,watermark.cols);

    //store LL portion of decomposed image in separate array 'A'
    double ***A = storeA(hsi,image);

    // calculate mean 2x2 of image and story in array 'B'
    double ***B = calculateB(hsi, image);

    // calculate difference between A and B and convert to binary
    double ***C = calculateC(A, B, image);

    // apply arnold transformation to create watermark by disordering pixels
    applyArnoldTransformation(C, image);

    // replace HH of image with watermark
    embedWatermark(C, hsi, image);

    //convert from 2D Haar wavelets to pixels
    invHaar2D(hsi, image.rows, image.cols);

    to_rgb(hsi, image);

    imwrite("blind.jpg",image);
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

double ***storeA(double ***hsi, Mat image)
{
  double ***LL;
  int row, col;
  //create 3d array to store histogram
  LL = new double **[image.rows/2];
  //initialize array to zeros
  for (row = 0; row < image.rows/2; row++){
    LL[row] = new double*[image.cols/2];
    for (col = 0; col < image.cols/2; col++){
      LL[row][col] = new double[3];
      LL[row][col][0] = hsi[row][col][0];
      LL[row][col][1] = hsi[row][col][1];
      LL[row][col][2] = hsi[row][col][2];

    }
  }
  return LL;
}

double ***calculateB(double ***hsi, Mat image)
{
  
  int row, col;
  //create 3d array to store B
  double ***B;
  B = new double **[image.rows/2];
  //initialize array to zeros
  for (row = 0; row < image.rows/2; row++){
    B[row] = new double*[image.cols/2];
    for (col = 0; col < image.cols/2; col++){
      B[row][col] = new double[3];
      B[row][col][0] = 0;
      B[row][col][1] = 0;
      B[row][col][2] = 0;
    }
  }
  //calculate mean 2x2's of image
  for (row=0; row < image.rows; row+=2)
  {
    for(col=0; col< image.cols; col+=2)
    {
      B[row/2][col/2][0] = (hsi[row][col][0] + hsi[row+1][col][0] +
        hsi[row][col+1][0] + hsi[row+1][col+1][0]) / 4;
      B[row/2][col/2][1] = (hsi[row][col][1] + hsi[row+1][col][1] +
        hsi[row][col+1][1] + hsi[row+1][col+1][1]) / 4;
      B[row/2][col/2][2] = (hsi[row][col][2] + hsi[row+1][col][2] +
        hsi[row][col+1][2] + hsi[row+1][col+1][2]) / 4;
    }
  }
  return B;
}

double ***calculateC(double ***A, double ***B, Mat image)
{
  int row, col;
  //create 3d array to store C
  double ***C;
  C = new double **[image.rows/2];
  //initialize array to zeros
  for (row = 0; row < image.rows/2; row++)
  {
    C[row] = new double*[image.cols/2];
    for (col = 0; col < image.cols/2; col++)
    {
      C[row][col] = new double[3];
      C[row][col][0] = 0;
      C[row][col][1] = 0;
      C[row][col][2] = 0;
    }
  }
  for (row = 0; row < image.rows/2; row++)
  {
    for (col = 0; col < image.cols/2; col++)
    {
      // cout << abs(A[row][col][2]-B[row][col][2]) << endl;
      // cout << A[row][col][2] << endl;
      //set C value to 0 if A-B is even, 1 if A-B is odd
      if(int(abs(A[row][col][2]-B[row][col][2]))%2 == 0)
        C[row][col][2] = 0;
      else
        C[row][col][2] = 1;
    }
  }
  return C;
}

void applyArnoldTransformation(double ***C, Mat image)
{
  double ***temp;
  int row, col;
  //create 3d array to store histogram
  temp = new double **[image.rows/2];
  //initialize array to zeros
  for (row = 0; row < image.rows/2; row++){
    temp[row] = new double*[image.cols/2];
    for (col = 0; col < image.cols/2; col++){
      temp[row][col] = new double[3];
      temp[row][col][0] = 0;
      temp[row][col][1] = 0;
      temp[row][col][2] = 0;

    }
  }
  int newx, newy;
  for(int x = 0; x < image.rows/2; x++)
  {
    for(int y = 0; y < image.cols/2; y++)
    {
      newx = (2*x+y)%image.rows/2;
      newy = (x+y)%image.cols/2;
      temp[x][y][2] += C[newx][newy][2];
    }
  }

  for(int x = 0; x < image.rows/2; x++)
  {
    for(int y = 0; y < image.cols/2; y++)
    {
      // cout << temp[x][y][2] << endl;
      C[x][y][2] = temp[x][y][2];
    }
  }
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

void embedWatermark(double ***C, double ***hsi, Mat image)
{
  for(int y = image.rows/2; y < image.rows; y++)
  {
     for (int x = image.cols/2; x < image.rows; x++)
     {
        hsi[x][y][2] = C[x-image.rows/2][y-image.cols/2][2];
     }
   }
}

/* convert degrees to radians */
double to_rad(int deg){
  return deg * PI/180; 
}