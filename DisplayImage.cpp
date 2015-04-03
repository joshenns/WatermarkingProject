#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <cmath>

const double PI = 3.14159;

using namespace cv;
using namespace std;

double ***initialize_hsi(Mat image);
void to_hsi(double ***hsi, Mat image);
void to_rgb(double ***hsi, Mat &image);
void haar1(double *vec, int n, int w);
void haar2(double ***matrix, int rows, int cols);
void embed(double ***hsi, double ***watermarkHsi, Mat image);
void invHaar1(double *vec, int n, int w);
void invHaar2(double ***matrix, int rows, int cols);
void toArray(Mat image, double ***array);
void toImage(Mat image, double ***array);
double to_rad(int deg);

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
    haar2(hsi,image.rows,image.cols);
    haar2(watermarkHsi,watermark.rows,watermark.cols);

    //embed image
    embed(hsi, watermarkHsi, image);

    //convert from 2D Haar wavelets to pixels
    invHaar2(hsi, image.rows, image.cols);

    /* convert image to rgb */
    to_rgb(hsi, image);

    

    //Mat temp = Mat::zeros(image.size(),image.type());

    //     for(int y = 0; y < image.rows; y++){
    //  for (int x = 0; x < image.rows; x++)
    //  {
    //      image.at<Vec3b>(y,x)[0] = 5;
    //      image.at<Vec3b>(y,x)[1] = 5;
    //      image.at<Vec3b>(y,x)[2] = image.at<Vec3b>(y,x)[2]/2;
    //  }
    // }

    // namedWindow("Display Image", WINDOW_AUTOSIZE );
    //namedWindow("New Image", WINDOW_AUTOSIZE );
    // imshow("Display Image", image);
    imwrite("image.jpg",image);
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

/** A Modified version of 1D Haar Transform, used by the 2D Haar Transform function **/
void haar1(double *vec, int n, int w)
{
    int i=0;
    double *vecp = new double[n];
    for(i=0;i<n;i++)
        vecp[i] = 0;

        w/=2;
        for(i=0;i<w;i++)
        {
            vecp[i] = (vec[2*i] + vec[2*i+1])/sqrt(2.0);
            vecp[i+w] = (vec[2*i] - vec[2*i+1])/sqrt(2.0);
            // vecp[i] = vec[i];
            // vecp[i+w] = vec[i];
        }
        
        for(i=0;i<(w*2);i++)
            vec[i] = vecp[i];

        delete [] vecp;
}

/** The 2D Haar Transform **/
void haar2(double ***matrix, int rows, int cols)
{
    double *temp_row = new double[cols];
    double *temp_col = new double[rows];

    int i=0,j=0;
    int w = cols, h=rows;

    // while(w>1 || h>1)
    // {
        if(w>1)
        {
            for(i=0;i<h;i++)
            {
                for(j=0;j<cols;j++)
                    temp_row[j] = matrix[i][j][2];

                haar1(temp_row,cols,w);
                
                for(j=0;j<cols;j++)
                    matrix[i][j][2] = temp_row[j];
            }
        }

        if(h>1)
        {
            for(i=0;i<w;i++)
            {
                for(j=0;j<rows;j++)
                    temp_col[j] = matrix[j][i][2];
                haar1(temp_col, rows, h);
                for(j=0;j<rows;j++)
                    matrix[j][i][2] = temp_col[j];
            }
        }

        if(w>1)
            w/=2;
        if(h>1)
            h/=2;
    // }

    delete [] temp_row;
    delete [] temp_col;
}

void invHaar1(double *vec, int n, int w)
{
    int i=0;
    double *vecp = new double[n];
    for(i=0;i<n;i++)
        vecp[i] = 0;

        // w/=2;
        for(i=0;i<n;i+=2)
        {
            vecp[i] = (vec[i/2] - vecp[i/2+w/2])/sqrt(2.0);
            vecp[i+1] = (vec[i/2] + vecp[i/2+w/2])/sqrt(2.0);
        }
        
        for(i=0;i<(w);i++)
            vec[i] = vecp[i];

        delete [] vecp;
}

/** The 2D Haar Transform **/
void invHaar2(double ***matrix, int rows, int cols)
{
    double *temp_row = new double[cols];
    double *temp_col = new double[rows];

    int i=0,j=0;
    int w = cols, h=rows;

    // while(w>1 || h>1)
    // {
        if(w>1)
        {
            for(i=0;i<h;i++)
            {
                for(j=0;j<cols;j++)
                    temp_row[j] = matrix[i][j][2];

                invHaar1(temp_row,cols,w);
                
                for(j=0;j<cols;j++)
                    matrix[i][j][2] = temp_row[j];
            }
        }

        if(h>1)
        {
            for(i=0;i<w;i++)
            {
                for(j=0;j<rows;j++)
                    temp_col[j] = matrix[j][i][2];
                invHaar1(temp_col, rows, h);
                for(j=0;j<rows;j++)
                    matrix[j][i][2] = temp_col[j];
            }
        }

        if(w>1)
            w/=2;
        if(h>1)
            h/=2;
    // }

    delete [] temp_row;
    delete [] temp_col;
}

void embed(double ***hsi, double ***watermarkHsi, Mat image)
{
    int horizCenter = image.rows/2;
    int vertCenter = image.cols/2;
    double max = 0;
    for(int x=horizCenter; x < image.rows;x++)
    {
        for(int y=vertCenter; y < image.cols;y++)
        {
            if (hsi[x][y][2] > max)
                max = hsi[x][y][2];
        }
    }
    for(int x=horizCenter;x < image.rows; x++)
    {
        for(int y=vertCenter;y < image.cols; y++)
        {
            hsi[x][y][2] = max/watermarkHsi[x][y][2]; 
        }
    }
    hsi[image.rows-1][image.cols-1][2] = max;

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

/* convert degrees to radians */
double to_rad(int deg){
  return deg * PI/180; 
}