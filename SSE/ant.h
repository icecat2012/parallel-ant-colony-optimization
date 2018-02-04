#ifndef ANT_H
#define ANT_H

#include <exception>
#include<iostream>
#include<stdlib.h>
#include<vector>
#include<time.h>
#include<math.h>
#include "map3d.h"

#define DEFAULT_HORM 10
#define HORM_DECLINE 0.9
#define HOME_ALPHA 1.0
#define HOME_BETA 1.5
#define FOOD_ALPHA 1.0
#define FOOD_BETA 1.5
#define PREFER_DECLINE 0.995

using namespace std;

class ant
{
	private:
		int x,y,z,state,step,last_x,last_y,last_z;
		double horm, max_horm;
		int sight_size, home_xyz[3], food_xyz[3];
		double sight[6], prefer_direction[6];
		unsigned int seed;
		inline void sight2positive()
		{
			double min_value = sight[0];
			for(int i=1; i<sight_size; ++i)
				min_value = (min_value>sight[i])?sight[i]:min_value;

			if(min_value<0)
			{
				min_value *= -1;
				for(int i=0; i<sight_size; ++i)
					sight[i] += min_value;
			}
		}
		inline void reverse_sight()
		{
			for(int i=0; i<sight_size; ++i)
				sight[i] *= -1;
		}
		inline void inverse_prefer_direction()
		{
		    double tmp_sight;
		    tmp_sight = prefer_direction[0];
		    prefer_direction[0] = prefer_direction[3];
		    prefer_direction[3] = tmp_sight;

		    tmp_sight = prefer_direction[1];
		    prefer_direction[1] = prefer_direction[2];
		    prefer_direction[2] = tmp_sight;

		    tmp_sight = prefer_direction[4];
		    prefer_direction[4] = prefer_direction[5];
		    prefer_direction[5] = tmp_sight;
		}
		inline void prefer_direction_decline()
		{
		    prefer_direction[0] = pow(prefer_direction[0], PREFER_DECLINE);
			prefer_direction[1] = pow(prefer_direction[1], PREFER_DECLINE);
			prefer_direction[2] = pow(prefer_direction[2], PREFER_DECLINE);
			prefer_direction[3] = pow(prefer_direction[3], PREFER_DECLINE);
			prefer_direction[4] = pow(prefer_direction[4], PREFER_DECLINE);
			prefer_direction[5] = pow(prefer_direction[5], PREFER_DECLINE);
		}
		inline void initial_home_xyz()
		{
		    home_xyz[0] = 0;
		    home_xyz[1] = 0;
		    home_xyz[2] = 0;
		}
		inline void initial_food_xyz()
		{
		    food_xyz[0] = 0;
		    food_xyz[1] = 0;
		    food_xyz[2] = 0;
		}
	public:
		ant()
		{
			x = 0;
			y = 0;
			z = 0;
			last_x=0;
			last_y=0;
			last_z=0;
			initial_home_xyz();
			initial_food_xyz();
			horm = 0.0;
			max_horm = 0;
			sight_size = 6;
			state = 0;
			step = 0;
			seed = time(NULL);
			for(int i=0; i<sight_size; ++i)
                prefer_direction[i] = 0;
		}
		ant(int ini_x,int ini_y,int ini_z,int ini_state,double ini_horm)
		{
			x = ini_x;
			y = ini_y;
			z = ini_z;
			last_x=ini_x;
			last_y=ini_y;
			last_z=ini_z;
			initial_home_xyz();
			initial_food_xyz();
			sight_size = 6;
			step = 0;
			horm = ini_horm;
			max_horm = ini_horm;
			state = ini_state;
			seed = time(NULL);
			for(int i=0; i<sight_size; ++i)
                prefer_direction[i] = 0;
		}
		ant(ant &in)
		{
			x = in.get_x();
			y = in.get_y();
			z = in.get_z();
			last_x=x;
			last_y=y;
			last_z=z;
			state = in.get_state();
			horm = in.get_horm();
			sight_size = 6;
			max_horm = in.get_max_horm();
			step = in.get_step();
			for(int i=0; i<sight_size; ++i)
                prefer_direction[i] = in.get_prefer_direction(i);
		}
		void set_state(int set_state)
		{
			state = set_state;
		}
		void set_seed(unsigned int set_seed)
		{
			seed = set_seed;
		}
		void set_position(int set_x,int set_y,int set_z)
		{
			x = set_x;
			y = set_y;
			z = set_z;
			last_x=set_x;
			last_y=set_y;
			last_z=set_z;
		}
		void set_step(int set_step)
		{
			step = set_step;
		}
		void set_horm(double set_horm)
		{
			horm = set_horm;
		}
		void set_max_horm(double set_horm)
		{
			max_horm = set_horm;
		}
		void set_sight(double front_, double left, double right, double back_, double up, double down)
		{
			sight[0] = front_;
			sight[1] = left;
			sight[2] = right;
			sight[3] = back_;
			sight[4] = up;
			sight[5] = down;
		}
		void set_prefer_direction(double front_, double left, double right, double back_, double up, double down)
		{
			prefer_direction[0] = front_;
			prefer_direction[1] = left;
			prefer_direction[2] = right;
			prefer_direction[3] = back_;
			prefer_direction[4] = up;
			prefer_direction[5] = down;
		}
		void set_home_xyz(int home_x, int home_y, int home_z)
		{
			home_xyz[0] = home_x;
			home_xyz[1] = home_y;
			home_xyz[2] = home_z;
		}
		void set_food_xyz(int food_x, int food_y, int food_z)
		{
			food_xyz[0] = food_x;
			food_xyz[1] = food_y;
			food_xyz[2] = food_z;
		}
		void ini_prefer_direction()
		{
			for(int i=0;i<sight_size;i++){
				prefer_direction[i] = 0;
			}
		}
		void rand_prefer_direction()
		{
		    int uper_bound = max_horm;
		    prefer_direction[0] = (double)(rand_r(&seed) % uper_bound);
			prefer_direction[1] = (double)(rand_r(&seed) % uper_bound);
			prefer_direction[2] = (double)(rand_r(&seed) % uper_bound);
			prefer_direction[3] = (double)(rand_r(&seed) % uper_bound);
			prefer_direction[4] = (double)(rand_r(&seed) % uper_bound);
			prefer_direction[5] = (double)(rand_r(&seed) % uper_bound);
		}
		int get_x()
		{
			return x;
		}
		int get_y()
		{
			return y;
		}
		int get_z()
		{
			return z;
		}
		int get_state()
		{
			return state;
		}
		int get_step()
		{
		    return step;
		}
		double get_horm()
		{
			return horm;
		}
		double get_max_horm()
		{
			return max_horm;
		}
		double get_prefer_direction(int i)
		{
		    return prefer_direction[i];
		}
		vector<double> get_sight()
		{
			vector<double> s(sight_size);
			for(int i=0; i<sight_size ;++i)
				s[i] = sight[i];
			return s;
		}
		vector<int> get_position()
		{
			vector<int> pos(3);
			pos[0] = x;
			pos[1] = y;
			pos[2] = z;
			return pos;
		}
		void print_xyz()
		{
			cout << "[x,y,z] = [" << x << "," << y  << "," << z << "]" << endl;
		}
		void print_state()
		{
			cout << "state : " << state <<endl;
		}
		void print_prefer_direction()
		{
			cout << "seed : " << seed  <<endl;
			cout << "prefer_direction = [" << prefer_direction[0] << "," << prefer_direction[1] << "," << prefer_direction[2] << "," << prefer_direction[3]<< "," << prefer_direction[4] << "," << prefer_direction[5] << "]" << endl;
		}

		void decide_direction(map3d &mmap)
		{
			int direction;
			sight2positive();
			
			sight[0] = mmap.reachable(x,y+1,z)? (sight[0]+DEFAULT_HORM) : 0;
			sight[1] = mmap.reachable(x-1,y,z)? (sight[1]+DEFAULT_HORM) : 0;
			sight[2] = mmap.reachable(x+1,y,z)? (sight[2]+DEFAULT_HORM) : 0;
			sight[3] = mmap.reachable(x,y-1,z)? (sight[3]+DEFAULT_HORM) : 0;
			sight[4] = mmap.reachable(x,y,z+1)? (sight[4]+DEFAULT_HORM) : 0;
			sight[5] = mmap.reachable(x,y,z-1)? (sight[5]+DEFAULT_HORM) : 0;

			//dont like to go backward
			sight[0] = ((sight[0]>0) && ((y+1)==last_y))? 1 : (sight[0]);
			sight[1] = ((sight[1]>0) && ((x-1)==last_x))? 1 : (sight[1]);
			sight[2] = ((sight[2]>0) && ((x+1)==last_x))? 1 : (sight[2]);
			sight[3] = ((sight[3]>0) && ((y-1)==last_y))? 1 : (sight[3]);
			sight[4] = ((sight[4]>0) && ((z+1)==last_z))? 1 : (sight[4]);
			sight[5] = ((sight[5]>0) && ((z-1)==last_z))? 1 : (sight[5]);
			last_x = x;
			last_y = y;
			last_z = z;
			
			double rand_high_bound = 0;
			if(state == 0)
            {
                for(direction=0; direction<sight_size; ++direction)
                {
                    if(sight[direction]==0)
                        continue;
                    sight[direction] =  pow(sight[direction], FOOD_ALPHA);
                    sight[direction] += pow(prefer_direction[direction], FOOD_BETA);
                    rand_high_bound += sight[direction];
                }
            }
            else if(state == 1)
            {
                for(direction=0; direction<sight_size; ++direction)
                {
                    if(sight[direction]==0)
                        continue;
                    sight[direction] =  pow(sight[direction], HOME_ALPHA);
                    sight[direction] += pow(prefer_direction[direction], HOME_BETA);
                    rand_high_bound += sight[direction];
                }
            }
            else if(state == -1)
            {
                for(direction=0; direction<sight_size; ++direction)
                {
                    if(sight[direction]==0)
                        continue;
                    sight[direction] =  pow(sight[direction], FOOD_ALPHA);
                    sight[direction] += pow(prefer_direction[direction], FOOD_BETA);
                    rand_high_bound += sight[direction];
                }
            }
            else
            {
                cout << "Exception" << endl;
            }

			double rand_value = (double)rand_r(&seed) / (double)RAND_MAX;
			double rand_dir = rand_value * rand_high_bound;
			for(direction=0; direction<sight_size; ++direction)
			{
				rand_dir -= sight[direction];
				if(rand_dir<0)
					break;
			}
			switch(direction)
			{
				case 0:
				    ++y;
				    break;
				case 1:
				    --x;
				    break;
				case 2:
				    ++x;
				    break;
				case 3:
				    --y;
				    break;
				case 4:
				    ++z;
				    break;
				case 5:
				    --z;
				    break;
				default:
					break;
			}

            if(mmap.find_food(x,y,z))
            {
            	last_x = x;
            	last_y = y;
            	last_z = z;
        	    horm = max_horm;
                food_xyz[0] = x;
                food_xyz[1] = y;
                food_xyz[2] = z;
                state = 1;
                step = 0;
            }
            else if(mmap.find_home(x,y,z))
            {
            	if(state==1)
                {
                	last_x = x;
                	last_y = y;
                	last_z = z;
                    state = 0;
                    step = 0;
                }
			    horm = max_horm;
                home_xyz[0] = x;
                home_xyz[1] = y;
                home_xyz[2] = z;
            
            }

            ++step;
            horm = pow(horm, HORM_DECLINE);
            //prefer_direction_decline();

            if(state==0) // find food now
            {	
                prefer_direction[0] = (food_xyz[1]>y)? max_horm : 0;
                prefer_direction[1] = (food_xyz[0]<x)? max_horm : 0;
                prefer_direction[2] = (food_xyz[0]>x)? max_horm : 0;
                prefer_direction[3] = (food_xyz[1]<y)? max_horm : 0;
                prefer_direction[4] = (food_xyz[2]>z)? max_horm : 0;
                prefer_direction[5] = (food_xyz[2]<z)? max_horm : 0;
            }
            else if (state==1) // find home now
            {
                prefer_direction[0] = (home_xyz[1]>y)? max_horm : 0;
                prefer_direction[1] = (home_xyz[0]<x)? max_horm : 0;
                prefer_direction[2] = (home_xyz[0]>x)? max_horm : 0;
                prefer_direction[3] = (home_xyz[1]<y)? max_horm : 0;
                prefer_direction[4] = (home_xyz[2]>z)? max_horm : 0;
                prefer_direction[5] = (home_xyz[2]<z)? max_horm : 0;
            }
		}
};

#endif
