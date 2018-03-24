#ifndef CONFIG_HPP
#define CONFIG_HPP
#include <string>

#include "Coordinates.hpp"

namespace Config{
    enum type{EUC_2D,GEO,ATT};

    extern unsigned GENERATION_LIMIT;
    const unsigned N_BEST{3};
    const unsigned AFTER_BEST{1};

    extern type TYPE;
    extern Coordinates map;
    extern unsigned BEST_FITNESS;

    //argumentos do algoritmo
    extern unsigned ID;
    extern std::string NAME;
    extern std::string LIB_PATH;
    extern unsigned NEW_POP_TYPE;
    extern double LK_PERCENTAGE;
    extern unsigned POP_SIZE;
}


#endif