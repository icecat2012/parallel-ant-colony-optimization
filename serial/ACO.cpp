#include<iostream>
#include<cstdlib>
#include<time.h>
#include "ant.h"
#include "map3d.h"
#include "horm.h"

#define MAX_STEP 10000
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

using namespace std;
ant ants[TOTAL_ANTS];
map3d mmap;
horm red_horm, blue_horm;
int tt=0,ff=0,fh=0;

inline void update_horm_map()
{
	double left_horm;
	int pos_x, pos_y, pos_z, ant_state;
	for(int i = 0; i < TOTAL_ANTS; i++)
    {
        pos_x = ants[i].get_x(); 
		pos_y = ants[i].get_y(); 
		pos_z = ants[i].get_z(); 
		left_horm = ants[i].get_horm();
		ant_state = ants[i].get_state();
        if(ant_state == 0)/*blue*/ 
			blue_horm.add(pos_x,pos_y,pos_z, left_horm);
        else if(ant_state == 1)/*red*/
        	red_horm.add(pos_x,pos_y,pos_z, left_horm);
        else if(ant_state == -1)/*init blue*/ 
			blue_horm.add(pos_x,pos_y,pos_z, left_horm);
    }
}

inline void ant_move()
{
	int i, pos_x, pos_y, pos_z, ant_state;
	double horm0, horm1, horm2, horm3, horm4, horm5;
	for(i=0; i<TOTAL_ANTS; i++)
	{
		pos_x = ants[i].get_x(); 
		pos_y = ants[i].get_y(); 
		pos_z = ants[i].get_z();
		ant_state = ants[i].get_state();
		if(ant_state == 1)
		{ // the ant is in red horm
			horm0 = blue_horm.get(pos_x,pos_y+1,pos_z);
			horm1 = blue_horm.get(pos_x-1,pos_y,pos_z);
			horm2 = blue_horm.get(pos_x+1,pos_y,pos_z);
			horm3 = blue_horm.get(pos_x,pos_y-1,pos_z);
			horm4 = blue_horm.get(pos_x,pos_y,pos_z+1);
			horm5 = blue_horm.get(pos_x,pos_y,pos_z-1);
		}
		else if(ant_state == 0)
		{ // the ant is in blue horm
			horm0 = red_horm.get(pos_x,pos_y+1,pos_z);
			horm1 = red_horm.get(pos_x-1,pos_y,pos_z);
			horm2 = red_horm.get(pos_x+1,pos_y,pos_z);
			horm3 = red_horm.get(pos_x,pos_y-1,pos_z);
			horm4 = red_horm.get(pos_x,pos_y,pos_z+1);
			horm5 = red_horm.get(pos_x,pos_y,pos_z-1);
		}
		else if(ant_state == -1)
		{ // the ant is in blue horm
			horm0 = -blue_horm.get(pos_x,pos_y+1,pos_z)+red_horm.get(pos_x,pos_y+1,pos_z);
			horm1 = -blue_horm.get(pos_x-1,pos_y,pos_z)+red_horm.get(pos_x,pos_y+1,pos_z);
			horm2 = -blue_horm.get(pos_x+1,pos_y,pos_z)+red_horm.get(pos_x,pos_y+1,pos_z);
			horm3 = -blue_horm.get(pos_x,pos_y-1,pos_z)+red_horm.get(pos_x,pos_y+1,pos_z);
			horm4 = -blue_horm.get(pos_x,pos_y,pos_z+1)+red_horm.get(pos_x,pos_y+1,pos_z);
			horm5 = -blue_horm.get(pos_x,pos_y,pos_z-1)+red_horm.get(pos_x,pos_y+1,pos_z);
		}
		ants[i].set_sight(horm0, horm1, horm2, horm3, horm4, horm5);
	}
	for(i=0; i<TOTAL_ANTS; i++)
	{
		ants[i].decide_direction(mmap);
	}
}

inline void horm_map_decline()
{
	blue_horm.decline(0.99);
	red_horm.decline(0.99);
}

inline void print_ant_info(int step_gap)
{
	int ant_state;
	if (++tt%step_gap == 0)
	{
		ff = 0;
		fh = 0;
		for(int j=0; j<TOTAL_ANTS; j++)
        {
        	ant_state = ants[j].get_state();
			if(ant_state==1) ++ff;
			else if(ant_state==0) ++fh;
		}
		cout << tt << "\t" << TOTAL_ANTS-ff-fh << " : " << ff << " : " << fh <<endl;
	}
}

inline void run_ants()
{
	while(true)
	{
		// cpu: update horm map
        update_horm_map();
        
		// cpu: get & compute horm & decide direction
		ant_move();
		
		// gpu: decline horm
		horm_map_decline();
		
		//print info (can be removed)
		print_ant_info(100);
		
		if(tt>=MAX_STEP)
		{
			cout << "END\n";
			//blue_horm.layout("blue.txt");
			//red_horm.layout("red.txt");
			break;
		}
	}
}

inline void init()
{	
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
	
	//initial horm maps 
	red_horm.create(MAP_X,MAP_Y,MAP_Z);
	blue_horm.create(MAP_X,MAP_Y,MAP_Z);
}

int main(int argc, char* argv[])
{
	init();
    run_ants();
	return 0;
}
