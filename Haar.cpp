#include "Haar.h"
#include <cmath>
#include <iostream>
using namespace std;
//Haar 1D transform
void haar1D(double *vec, int n, int w)
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
        }
        
        for(i=0;i<(w*2);i++)
            vec[i] = vecp[i];

        delete [] vecp;
}

/** The 2D Haar Transform **/
void haar2D(double ***array, int rows, int cols)
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
                    temp_row[j] = array[i][j][2];

                haar1D(temp_row,cols,w);
                
                for(j=0;j<cols;j++)
                    array[i][j][2] = temp_row[j];
            }
        }

        if(h>1)
        {
            for(i=0;i<w;i++)
            {
                for(j=0;j<rows;j++)
                    temp_col[j] = array[j][i][2];

                haar1D(temp_col, rows, h);

                for(j=0;j<rows;j++)
                    array[j][i][2] = temp_col[j];
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

void invHaar1D(double *vec, int n, int w)
{
    int i=0;
    double *vecp = new double[n];
    for(i=0;i<n;i++)
        vecp[i] = 0;

    for(i=0;i<n;i+=2)
    {
        // cout << vec[i/2+w/2] << endl;
        vecp[i] = (vec[i/2] + vec[i/2+w/2])/sqrt(2.0);
        vecp[i+1] = (vec[i/2] - vec[i/2+w/2])/sqrt(2.0);

    }
    
    for(i=0;i<(w);i++)
        vec[i] = vecp[i];

    delete [] vecp;
}

/** The 2D Haar Transform **/
void invHaar2D(double ***array, int rows, int cols)
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
                    temp_row[j] = array[i][j][2];

                invHaar1D(temp_row,cols,w);
                
                for(j=0;j<cols;j++)
                    array[i][j][2] = temp_row[j];
            }
        }

        if(h>1)
        {
            for(i=0;i<w;i++)
            {
                for(j=0;j<rows;j++)
                    temp_col[j] = array[j][i][2];
                invHaar1D(temp_col, rows, h);
                for(j=0;j<rows;j++)
                    array[j][i][2] = temp_col[j];
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