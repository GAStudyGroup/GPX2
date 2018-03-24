#include "Config.hpp"

Config::type TYPE = Config::type::EUC_2D;
unsigned Config::N_BEST{3};
unsigned Config::ID{0};
std::string Config::NAME{""};
std::string Config::LIB_PATH{""};
unsigned Config::NEW_POP_TYPE{0};
double Config::LK_PERCENTAGE{0};
unsigned Config::POP_SIZE{0};
Coordinates Config::map;
unsigned Config::BEST_FITNESS{0};
unsigned Config::GENERATION_LIMIT{100};