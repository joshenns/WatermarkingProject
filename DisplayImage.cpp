#include <stdio.h>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

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