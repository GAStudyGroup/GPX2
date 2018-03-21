#ifndef CONFIG_HPP
#define CONFIG_HPP

namespace Config{
    enum type{EUC_2D,GEO,ATT};

    const unsigned GENERATION_LIMIT{10};
    const double LK_PERCENTAGE{0};
    const unsigned N_BEST{3};
    extern type TYPE;
}


#endif