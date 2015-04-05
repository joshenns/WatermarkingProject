#include <iostream>
#include <cmath>
using namespace std;


/** The 1D Haar Transform **/
void haar1d(float *vec, int n)
{
	int i=0;
	int w=n;
	float *vecp = new float[n];
	for(i=0;i<n;i++)
		vecp[i] = 0;

	while(w>1)
	{
		w/=2;
		for(i=0;i<w;i++)
		{
			vecp[i] = (vec[2*i] + vec[2*i+1])/sqrt(2.0);
			vecp[i+w] = (vec[2*i] - vec[2*i+1])/sqrt(2.0);
		}
		
		for(i=0;i<(w*2);i++)
			vec[i] = vecp[i]; 
	}

	delete [] vecp;
}


/** A Modified version of 1D Haar Transform, used by the 2D Haar Transform function **/
void haar1(float *vec, int n, int w)
{
	int i=0;
	float *vecp = new float[n];
	for(i=0;i<n;i++)
		vecp[i] = 0;

		w/=2;
		for(i=0;i<w;i++)
		{
			vecp[i] = (vec[2*i] + vec[2*i+1])/sqrt(2.0);
			vecp[i+w] = (vec[2*i] - vec[2*i+1])/sqrt(2.0);
		}
		
		for(i=0;i<(w*2);i++)
			vec[i] = vecp[i];

		delete [] vecp;
}


/** The 2D Haar Transform **/
void haar2(float **matrix, int rows, int cols)
{
	float *temp_row = new float[cols];
	float *temp_col = new float[rows];

	int i=0,j=0;
	int w = cols, h=rows;
	while(w>1 || h>1)
	{
		if(w>1)
		{
			for(i=0;i<h;i++)
			{
				for(j=0;j<cols;j++)
					temp_row[j] = matrix[i][j];

				haar1(temp_row,cols,w);
				
				for(j=0;j<cols;j++)
					matrix[i][j] = temp_row[j];
			}
		}

		if(h>1)
		{
			for(i=0;i<w;i++)
			{
				for(j=0;j<rows;j++)
					temp_col[j] = matrix[j][i];
				haar1(temp_col, rows, h);
				for(j=0;j<rows;j++)
					matrix[j][i] = temp_col[j];
			}
		}

		if(w>1)
			w/=2;
		if(h>1)
			h/=2;
	}

	delete [] temp_row;
	delete [] temp_col;
}

/** Here's an example on how to use these functions **/
int main(int argc, char **argv)
{
	int i=0;
	float vec3[4] = {4,2,5,5};

	haar1d(vec3,4);

	cout << "The 1D Haar Transform: " << endl;
	for(i=0;i<4;i++)
		cout << vec3[i] << " ";
	cout << endl;
	
	cout << "\n\nThe 2D Haar Transform: " << endl;
	float **mat = new float*[4];
	for(int m=0;m<4;m++)
		mat[m] = new float[4];
	mat[0][0] = 5; mat[0][1] = 6; mat[0][2] = 1; mat[0][3] = 2;
	mat[1][0] = 4; mat[1][1] = 2; mat[1][2] = 5; mat[1][3] = 5;
	mat[2][0] = 3; mat[2][1] = 1; mat[2][2] = 7; mat[2][3] = 1;
	mat[3][0] = 6; mat[3][1] = 3; mat[3][2] = 5; mat[3][3] = 1;


	haar2(mat,4,4);
	for(i=0;i<4;i++)
	{
		for(int j=0;j<4;j++)
			cout << mat[i][j] << " ";
		cout << endl;
	}

	cout << endl;
	return 0;
}
