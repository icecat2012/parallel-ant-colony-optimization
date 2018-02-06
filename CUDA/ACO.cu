#include<iostream>
#include<cstdlib>
#include<pthread.h>
#include<semaphore.h>
#include<time.h>
#include "ant.h"
#include "map3d.h"

#define MAX_STEP 10000
#define THREAD_COUNT 4
#define TOTAL_ANTS 60
#define MAX_HORM_LEFT 100
#define MAP_X 50
#define MAP_Y 50
#define MAP_Z 3
#define HOME_X 11
#define HOME_Y 11
#define HOME_Z 1
#define FOOD_X 39
#define FOOD_Y 39
#define FOOD_Z 1
#define BLOCK_SIZE 512

using namespace std;
int tt=0, ff=0, fh=0;
int counter1,counter2;
sem_t update_barrier;
sem_t barrier1,barrier2;
sem_t mutex1,mutex2;
ant ants[TOTAL_ANTS];
map3d mmap;
double *Md1, *Md2, blue_horm[MAP_X*MAP_Y*MAP_Z], red_horm[MAP_X*MAP_Y*MAP_Z];
int total_block, tpoints;


__global__ void horm_update(double *Md, double decline)
{
	int index = blockIdx.x * blockDim.x + threadIdx.x;
	if(index < MAP_X*MAP_Y*MAP_Z)
        Md[index] = Md[index]*decline;
}

int matrix_3d_to_1d(int x,int y,int z)
{
    return z * MAP_X * MAP_Y + y * MAP_X + x;
}

void layout(const char *file_name, double *horm_array)
{
    FILE *fp;
    fp = fopen(file_name,"a+");
    if(fp!=NULL)
    {
        fprintf(fp, "\n%d*%d*%d\n",MAP_X, MAP_Y, MAP_Z);
        for(int i=0;i<MAP_Z;i++)
        {
            for(int j=0;j<MAP_Y;j++)
            {
                for(int k=0;k<MAP_X;k++)
                {
                    fprintf(fp, "%f",horm_array[matrix_3d_to_1d(k,j,i)]);
                    if(k!=MAP_X-1)
                        fprintf(fp, ",");
                    else
                        fprintf(fp, "\n");
                }
            }
        }
    }
}

void* run_ants(void* data)
{
	double horm0, horm1, horm2, horm3, horm4, horm5, horm6, horm7, horm8, horm9;
	unsigned int i, j, pos_x, pos_y, pos_z;
	unsigned int thread_id = *(unsigned int*) data;
	unsigned int istart = thread_id * TOTAL_ANTS / THREAD_COUNT;
	unsigned int iend = (thread_id + 1) * TOTAL_ANTS / THREAD_COUNT;
	double left_horm;
	if (thread_id == THREAD_COUNT - 1) 
		iend = TOTAL_ANTS;

	while(true){
		// cpu: update horm map
		if(thread_id == 0)
		{
			for(j = 0; j < TOTAL_ANTS; j++)
            {
				pos_x = ants[j].get_x(); 
				pos_y = ants[j].get_y(); 
				pos_z = ants[j].get_z(); 
				left_horm = ants[j].get_horm();
				if(ants[j].get_state() == 0/*blue*/) 
					blue_horm[matrix_3d_to_1d(pos_x,pos_y,pos_z)] += left_horm;
				else if(ants[j].get_state() == 1/*red*/) 
					red_horm[matrix_3d_to_1d(pos_x,pos_y,pos_z)] += left_horm;
				else if(ants[j].get_state() == -1/*init blue*/) 
					blue_horm[matrix_3d_to_1d(pos_x,pos_y,pos_z)] += left_horm;
			}
			
			// release barrier
			for(j = 0; j < THREAD_COUNT - 1; j++){
				sem_post(&update_barrier);
			}
		}
		else{
			sem_wait(&update_barrier);
		}
		
		// cpu: get & compute horm & decide direction
		for(int i=istart; i<iend; i++)
        {
            pos_x = ants[i].get_x(); 
			pos_y = ants[i].get_y(); 
			pos_z = ants[i].get_z();
            if(ants[i].get_state() == 1){ // the ant is in red horm
				horm0 = blue_horm[matrix_3d_to_1d(pos_x,pos_y+1,pos_z)];
				horm1 = blue_horm[matrix_3d_to_1d(pos_x-1,pos_y,pos_z)];
				horm2 = blue_horm[matrix_3d_to_1d(pos_x+1,pos_y,pos_z)];
				horm3 = blue_horm[matrix_3d_to_1d(pos_x,pos_y-1,pos_z)];
				horm4 = blue_horm[matrix_3d_to_1d(pos_x,pos_y,pos_z+1)];
				horm5 = blue_horm[matrix_3d_to_1d(pos_x,pos_y,pos_z-1)];
            }
            else if (ants[i].get_state() == 0){ // the ant is in blue horm
				horm0 = red_horm[matrix_3d_to_1d(pos_x,pos_y+1,pos_z)];
				horm1 = red_horm[matrix_3d_to_1d(pos_x-1,pos_y,pos_z)];
				horm2 = red_horm[matrix_3d_to_1d(pos_x+1,pos_y,pos_z)];
				horm3 = red_horm[matrix_3d_to_1d(pos_x,pos_y-1,pos_z)];
				horm4 = red_horm[matrix_3d_to_1d(pos_x,pos_y,pos_z+1)];
				horm5 = red_horm[matrix_3d_to_1d(pos_x,pos_y,pos_z-1)];
            }
            else if (ants[i].get_state() == -1){ // the ant is in blue horm
				horm0 = -blue_horm[matrix_3d_to_1d(pos_x,pos_y+1,pos_z)]+red_horm[matrix_3d_to_1d(pos_x,pos_y+1,pos_z)];
				horm1 = -blue_horm[matrix_3d_to_1d(pos_x-1,pos_y,pos_z)]+red_horm[matrix_3d_to_1d(pos_x-1,pos_y,pos_z)];
				horm2 = -blue_horm[matrix_3d_to_1d(pos_x+1,pos_y,pos_z)]+red_horm[matrix_3d_to_1d(pos_x+1,pos_y,pos_z)];
				horm3 = -blue_horm[matrix_3d_to_1d(pos_x,pos_y-1,pos_z)]+red_horm[matrix_3d_to_1d(pos_x,pos_y-1,pos_z)];
				horm4 = -blue_horm[matrix_3d_to_1d(pos_x,pos_y,pos_z+1)]+red_horm[matrix_3d_to_1d(pos_x,pos_y,pos_z+1)];
				horm5 = -blue_horm[matrix_3d_to_1d(pos_x,pos_y,pos_z-1)]+red_horm[matrix_3d_to_1d(pos_x,pos_y,pos_z-1)];
            }
            ants[i].set_sight(horm0, horm1, horm2, horm3, horm4, horm5);
        	ants[i].decide_direction(mmap);
		}
		
		// barrier
		sem_wait(&mutex1);
		++counter1;
		if (counter1 == THREAD_COUNT)
		{
			counter1 = 0;
			for(j = 0; j < THREAD_COUNT; j++)
				sem_post(&barrier1);
		}
		sem_post(&mutex1);
		sem_wait(&barrier1);
		
		// cpu: decide direction, gpu: decline horm
		if(thread_id == 0)
        {
            cudaMemcpy(blue_horm, Md1, tpoints*sizeof(double), cudaMemcpyHostToDevice);
            cudaMemcpy(red_horm, Md2, tpoints*sizeof(double), cudaMemcpyHostToDevice);
            horm_update<<<total_block, BLOCK_SIZE>>>(Md1, 0.99);
            horm_update<<<total_block, BLOCK_SIZE>>>(Md2, 0.99);
            cudaMemcpy(red_horm, Md2, tpoints*sizeof(double), cudaMemcpyDeviceToHost);
            cudaMemcpy(blue_horm, Md1, tpoints*sizeof(double), cudaMemcpyDeviceToHost);
		}

		// barrier
		sem_wait(&mutex2);
		++counter2;
		if (counter2 == THREAD_COUNT)
		{
			counter2 = 0;
			if (++tt%100 == 0)
			{
				ff = 0;
				fh = 0;
				for(j = 0; j < TOTAL_ANTS; j++)
                {
					if(ants[j].get_state()==1) 
						++ff;
					else if(ants[j].get_state()==0) 
						++fh;
				}
				cout << tt << "\t" << TOTAL_ANTS-ff-fh << " : " << ff << " : " << fh <<endl;
				if(tt>=MAX_STEP)
				{
				    remove( "blue.txt" );
				    layout("blue.txt", blue_horm);
					remove( "red.txt" );
				    layout("red.txt", red_horm);
					exit(0);
				}
			}
			for(j = 0; j < THREAD_COUNT; j++)
				sem_post(&barrier2);
		}
		sem_post(&mutex2);
		sem_wait(&barrier2);
	}
}

void init()
{	
	counter1 = 0;
	counter2 = 0;
	sem_init(&update_barrier, 0, 0);
	sem_init(&mutex1, 0, 1);
	sem_init(&barrier1, 0, 0);
	sem_init(&mutex2, 0, 1);
	sem_init(&barrier2, 0, 0);

    tpoints = MAP_X*MAP_Y*MAP_Z;
    cudaMalloc((void**) &Md1, tpoints*sizeof(double));
    cudaMalloc((void**) &Md2, tpoints*sizeof(double));
    total_block = ((tpoints % BLOCK_SIZE) == 0)? (tpoints/BLOCK_SIZE) : (tpoints/BLOCK_SIZE + 1);

	mmap.load_sample(MAP_X,MAP_Y,MAP_Z);
	
	//set home and food point
	mmap.edit(HOME_X,HOME_Y,HOME_Z,101);
	mmap.edit(FOOD_X,FOOD_Y,FOOD_Z,100);
	
	//initial ants
	srand (time(NULL));
	int offset = rand()/2;
	unsigned int set_seed;
	for(int i=0; i<TOTAL_ANTS; ++i)
	{
		ants[i].set_position(HOME_X,HOME_Y,HOME_Z);
		ants[i].set_home_xyz(HOME_X,HOME_Y,HOME_Z);
		
		ants[i].set_horm(MAX_HORM_LEFT);
		ants[i].set_max_horm(MAX_HORM_LEFT);
		ants[i].set_state(-1);
		set_seed = i+offset;
		ants[i].set_seed(set_seed);
		ants[i].ini_prefer_direction();
	}
    for(int i=0;i<tpoints;++i)
    {
        blue_horm[i] = 0;
        red_horm[i] = 0;
    }
}

int main(int argc, char* argv[])
{
	init();

	unsigned int thread_id[THREAD_COUNT];
	unsigned int thread;
	pthread_t* thread_handles;
	thread_handles = (pthread_t*) malloc(THREAD_COUNT*sizeof(pthread_t));

	for(thread = 0; thread < THREAD_COUNT; thread++){
		thread_id[thread] = thread;
		pthread_create(&thread_handles[thread], NULL, run_ants,(void*) &thread_id[thread]);
	}

	char key = 'w';
	while(key != 'x')
	{
		cin >> key;
	}
	return 0;
}
