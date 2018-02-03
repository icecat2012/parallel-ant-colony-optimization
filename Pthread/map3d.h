#ifndef MAP3D_H
#define MAP3D_H

#include<iostream>
using namespace std;

class map3d
{
	private:
		char ***ptr3D;
		int x_width, y_width, z_width;
		inline bool is_barrier(int x,int y,int z)
		{
			return (ptr3D[x][y][z]<0);
		}
	public:
		map3d()
		{
			ptr3D = NULL;
			x_width = 0;
			y_width = 0;
			z_width = 0;
		}
		map3d(map3d &in)
		{
			if (x_width>0 || y_width>0 || z_width>0)
			{
				free3d();
			}
			x_width = in.get_x_width();
			y_width = in.get_y_width();
			z_width = in.get_z_width();
			ptr3D = new char**[x_width];
			for(int i=0;i<x_width;i++)
			{
			    ptr3D[i] = new char*[y_width];  
			
			    for(int j=0;j<y_width;j++)
			    {
			        ptr3D[i][j]=new char[z_width];
			        for(int k=0;k<z_width;k++)
			        {
			            ptr3D[i][j][k]=in.get(i,j,k); 
			        }
			    }
			}
		}
		~map3d()
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
		void load_file(char *file_name)
		{
			if (x_width>0 || y_width>0 || z_width>0)
			{
				free3d();
			}
			FILE *fp;
			
			fp = fopen(file_name, "r");
			int read_x,read_y,read_z;
			fscanf(fp, "\n%d*%d*%d\n", &read_x, &read_y, &read_z);
			x_width = read_x;
			y_width = read_y;
			z_width = read_z;
			
			ptr3D = new char**[x_width];
			for(int i=0;i<x_width;i++)
			{
			    ptr3D[i] = new char*[y_width];  
			
			    for(int j=0;j<y_width;j++)
			    {
			        ptr3D[i][j]=new char[z_width];
			        for(int k=0;k<z_width;k++)
			        {
		        		ptr3D[i][j][k]=-1;
		        	}
			    }
			}
			
			int read_tmp;
			
			for(int i=0;i<z_width;i++)
			{
				for(int j = 0;j<y_width;j++)
				{
					for(int k = 0;k<x_width;k++)
					{
						fscanf(fp, "%d ", &read_tmp);
						if (read_tmp == 1)
							ptr3D[k][j][i] = -1;
						else
							ptr3D[k][j][i] = 1;
					}
					fscanf(fp, "\n");
				}
			}
			fclose(fp);
		}
		void load_sample(int x,int y,int z)
		{
			if (x_width>0 || y_width>0 || z_width>0)
			{
				free3d();
			}
			x_width = x;
			y_width = y;
			z_width = z;
			ptr3D = new char**[x_width];
			for(int i=0;i<x_width;i++)
			{
			    ptr3D[i] = new char*[y_width];  
			
			    for(int j=0;j<y_width;j++)
			    {
			        ptr3D[i][j]=new char[z_width];
			        for(int k=0;k<z_width;k++)
			        {
			        		ptr3D[i][j][k]=-1;
			        }
			    }
			}
			for(int i=1;i<x_width-1;i++)
				for(int j = 1;j<y_width-1;j++)
					ptr3D[i][j][1] = 1;
		}
		void reachable_analysis()
		{
			for(int i=1;i < x_width-1;++i)
			{
				for(int j=1;j < y_width-1;++j)
				{
					for(int k=1;k < z_width-1;++k)
					{
						//if((i==0&&j==0) || (j==0&&k==0) || (i==0&&k==0) || (i==x_width-1&&j==y_width-1) || (j==y_width-1&&k==z_width-1) || (i==x_width-1&&k==z_width-1))
						//	continue;
						if(is_barrier(i,j,k))
						{
							continue;
						}
						else if (is_barrier(i-1,j,k) && is_barrier(i+1,j,k)
								&& is_barrier(i,j-1,k) && is_barrier(i,j+1,k)
								&& is_barrier(i,j,k-1) && is_barrier(i,j,k+1))
						{
							ptr3D[i][j][k] = 0;
						}
						else if(is_barrier(i-1,j-1,k-1) || is_barrier(i-1,j-1,k) || is_barrier(i-1,j-1,k+1)
							|| is_barrier(i,j-1,k-1) || is_barrier(i,j-1,k) || is_barrier(i,j-1,k+1)
							|| is_barrier(i+1,j-1,k-1) || is_barrier(i+1,j-1,k) || is_barrier(i+1,j-1,k+1)
							|| is_barrier(i+1,j,k-1) || is_barrier(i+1,j,k) || is_barrier(i+1,j,k+1)
							|| is_barrier(i+1,j+1,k-1) || is_barrier(i+1,j+1,k) || is_barrier(i+1,j+1,k+1)
							|| is_barrier(i,j+1,k-1) || is_barrier(i,j+1,k) || is_barrier(i,j+1,k+1)
							|| is_barrier(i-1,j+1,k-1) || is_barrier(i-1,j+1,k) || is_barrier(i-1,j+1,k+1)
							|| is_barrier(i-1,j,k-1) || is_barrier(i-1,j,k) || is_barrier(i-1,j,k+1)
							|| is_barrier(i,j,k-1) || is_barrier(i,j,k+1))
						{
							continue;
						}
						else
						{
							ptr3D[i][j][k] = 0;
						}
					}
				}
			}
		}
		void edit(int x,int y,int z, char value)
		{
			ptr3D[x][y][z]=value;
		}
		bool reachable(int x,int y,int z)
		{
			return (ptr3D[x][y][z]>0);
		}
		bool find_food(int x,int y,int z)
		{
			return (ptr3D[x][y][z]==100);
		}
		bool find_home(int x,int y,int z)
		{
			return (ptr3D[x][y][z]==101);
		}
		char get(int x,int y,int z)
		{
			return ptr3D[x][y][z];
		}
};

#endif
