#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <string>
using std::string;

#include <random>
using std::mt19937;

//used for map
#include "Coordinates.hpp"

namespace Config{
    enum class type{EUC_2D,GEO,ATT};

    extern unsigned AFTER_BEST;

    extern unsigned GENERATION_LIMIT;

    //Arguments
    extern unsigned ID;
    extern string NAME;
    extern string LIB_PATH;
    extern unsigned NEW_POP_TYPE;
    extern double LK_PERCENTAGE;
    extern unsigned POP_SIZE;
    extern unsigned BEST_FITNESS;
    extern type TYPE;
    extern unsigned N_BEST;
    extern double RESET_PERCENTAGE;
    extern unsigned NEW_TOUR_MODE;
}

namespace Globals{
    extern mt19937 urng;
    extern Coordinates map;
    extern unsigned TOUR_SIZE;
}


#endif