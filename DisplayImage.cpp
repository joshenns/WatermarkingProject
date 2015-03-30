#include <stdio.h>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

void Haar1D(float *A, int width);

int main(int argc, char** argv )
{
    if ( argc != 2 )
    {
        cout << "usage: DisplayImage.out <Image_Path>\n";
        return -1;
    }

    Mat image;
    image = imread( argv[1], 1 );

    if ( !image.data )
    {
        cout << "No image data \n";
        return -1;
    }
    
    Mat temp = Mat::zeros(image.size(),image.type());


    for(int y = 0; y < temp.rows; y++){
    	for (int x = 0; x < temp.rows; x++)
    	{
    		temp.at<Vec3b>(y,x)[0] = image.at<Vec3b>(y,x)[0]/2;
    		temp.at<Vec3b>(y,x)[1] = image.at<Vec3b>(y,x)[1]/2;
    		temp.at<Vec3b>(y,x)[2] = image.at<Vec3b>(y,x)[2]/2;
    	}
    }
    
    namedWindow("Display Image", WINDOW_AUTOSIZE );
    namedWindow("New Image", WINDOW_AUTOSIZE );
    imshow("Display Image", image);
    imshow("New Image", temp);
    waitKey(0);

    return 0;
}

// Unormalized version
// assumes width is a power of 2
void Haar1D(float *A, int width)
{
    float tmp[width]; // some compilers might not like this

    while(width > 1) {
        width /= 2;

        for(int i=0; i < width; i++) {
            tmp[i] = (A[2*i] + A[2*i+1]) * 0.5;
            tmp[width + i] = (A[2*i] - A[2*i+1]) * 0.5;
        }

        memcpy(A, tmp, width*2*sizeof(float));
    }
}