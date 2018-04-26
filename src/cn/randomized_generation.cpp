#include "randomized_generation.h"

#include <random>

CN randomize_cn(std::size_t n, std::size_t n_min_pre_places, std::size_t n_max_pre_places)
{
	std::random_device rd;	


    std::random_device rd;  //Will be used to obtain a seed for the random number engine
    std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
    std::uniform_int_distribution<> dis(1, 6);
 
        std::cout << dis(gen) << ' ';
}

}
