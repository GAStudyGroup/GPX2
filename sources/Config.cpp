#include "Config.hpp"

Config::type    Config::TYPE{Config::type::EUC_2D};
unsigned        Config::AFTER_BEST{0};
unsigned        Config::N_BEST{3};
unsigned        Config::ID{0};
string          Config::NAME{""};
string          Config::LIB_PATH{"./"};
unsigned        Config::NEW_POP_TYPE{1};
double          Config::LK_PERCENTAGE{0};
unsigned        Config::POP_SIZE{0};
Coordinates     Config::map;
unsigned        Config::BEST_FITNESS{0};
unsigned        Config::GENERATION_LIMIT{50};