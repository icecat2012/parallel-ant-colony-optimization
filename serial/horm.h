#ifndef HORM_H
#define HORM_H

#include<iostream>
#include<stdio.h>
using namespace std;

class horm
{
	private:
		double ***ptr3D;
		int x_width, y_width, z_width;
	public:
		horm()
		{
			ptr3D = NULL;
			x_width = 0;
			y_width = 0;
			z_width = 0;
		}
		horm(int ini_x,int ini_y,int ini_z)
		{
			x_width = ini_x;
			y_width = ini_y;
			z_width = ini_z;
			ptr3D = new double**[x_width];
			for(int i=0;i<x_width;i++)
			{
			    ptr3D[i] = new double*[y_width];

			    for(int j=0;j<y_width;j++)
			    {
			        ptr3D[i][j]=new double[z_width];
			        for(int k=0;k<z_width;k++)
			        {
			            ptr3D[i][j][k]=0;
			        }
			    }
			}
		}
		horm(horm &in)
		{
			if (x_width>0 || y_width>0 || z_width>0)
			{
				free3d();
			}
			x_width = in.get_x_width();
			y_width = in.get_y_width();
			z_width = in.get_z_width();
			ptr3D = new double**[x_width];
			for(int i=0;i<x_width;i++)
			{
			    ptr3D[i] = new double*[y_width];

			    for(int j=0;j<y_width;j++)
			    {
			        ptr3D[i][j]=new double[z_width];
			        for(int k=0;k<z_width;k++)
			        {
			            ptr3D[i][j][k]=in.get(i,j,k);
			        }
			    }
			}
		}
		~horm()
		{
			for(int i=0;i<x_width;i++)
			{
			    for(int j=0;j<y_width;j++)
			    {
			        delete[] ptr3D[i][j];
			    }
			    delete[] ptr3D[i];
			}
			delete[] ptr3D;
		}
		int get_x_width()
		{
			return x_width;
		}
		int get_y_width()
		{
			return y_width;
		}
		int get_z_width()
		{
			return z_width;
		}
		void free3d()
		{
			for(int i=0;i<x_width;i++)
			{
			    for(int j=0;j<y_width;j++)
			    {
			        delete[] ptr3D[i][j];
			    }
			    delete[] ptr3D[i];
			}
			delete[] ptr3D;
			ptr3D = NULL;
			x_width = 0;
			y_width = 0;
			z_width = 0;
		}
		void create(int x,int y,int z)
		{
			if (x_width>0 || y_width>0 || z_width>0)
			{
				free3d();
			}
			x_width = x;
			y_width = y;
			z_width = z;
			ptr3D = new double**[x_width];
			for(int i=0;i<x_width;i++)
			{
			    ptr3D[i] = new double*[y_width];

			    for(int j=0;j<y_width;j++)
			    {
			        ptr3D[i][j]=new double[z_width];
			        for(int k=0;k<z_width;k++)
			        {
			            ptr3D[i][j][k]=0;
			        }
			    }
			}
		}

		void layout(const char *name)
		{
			FILE *fp;
			fp = fopen(name,"a+");
			if(fp!=NULL)
			{
				fprintf(fp, "\n%d*%d*%d\n",x_width, y_width, z_width);
				for(int i=0;i<z_width;i++)
				{
					for(int j=0;j<y_width;j++)
					{
						for(int k=0;k<x_width;k++)
						{
							fprintf(fp, "%f",ptr3D[k][j][i]);
							if(k!=x_width-1)
								fprintf(fp, ",");
							else
								fprintf(fp, "\n");
						}
					}
				}
			}
		}

		int load(const char *name)
		{
			FILE *fp;
			fp = fopen(name,"r");
			float tmp;
			if(fp!=NULL)
			{
				int x_limit, y_limit, z_limit;
				fscanf(fp, "\n%d*%d*%d\n",&x_limit, &y_limit, &z_limit);
				if(x_limit!=x_width || y_limit!=y_width || z_limit!=z_width)
					return -1;
				for(int i=0;i<z_width;i++)
				{
					for(int j=0;j<y_width;j++)
					{
						for(int k=0;k<x_width;k++)
						{
							fscanf(fp, "%f", &tmp);
							ptr3D[k][j][i] = (double)tmp;
							if(k!=x_width-1)
								fscanf(fp, ",");
							else
								fscanf(fp, "\n");
						}
					}
				}
			}
			return 1;
		}

		inline void edit(int x,int y,int z, double value)
		{
			ptr3D[x][y][z]=value;
		}
		inline void add(int x,int y,int z, double value)
		{
			ptr3D[x][y][z] += value;
		}
		inline double get(int x,int y,int z)
		{
			return ptr3D[x][y][z];
		}
		void decline(double mul)
		{
			for(int i=0;i<x_width;i++)
			{
				for(int j=0;j<y_width;j++)
				{
					for(int k=0;k<z_width;k++)
					{
						ptr3D[i][j][k] *= mul;
					}
				}
			}
		}
		double find_max()
		{
			double max_val = 0;
			for(int i=0;i<x_width;i++)
			{
				for(int j=0;j<y_width;j++)
				{
					for(int k=0;k<z_width;k++)
					{
						if(ptr3D[i][j][k] > max_val)
							max_val = ptr3D[i][j][k];
					}
				}
			}
			return max_val;
		}
};

#endif
