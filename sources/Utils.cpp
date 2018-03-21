#include "Utils.hpp"
#include "Config.hpp"

extern Config::type TYPE;

/* 
int distance(const CityNode& c1, const CityNode& c2)
{
    return (round(sqrt(pow((c1.getX() - c2.getX()), 2) + pow((c1.getY() - c2.getY()), 2))));
}
 */ 


int distance(std::pair<double,double> p1,std::pair<double,double> p2){
    switch(TYPE){
        case Config::type::EUC_2D:{
            return(EUC_2D(p1,p2));
        }
        case Config::type::GEO:{
            return(GEO(p1,p2));
        }
        case Config::type::ATT:{
            return(ATT(p1,p2));
        }
    }
}

int EUC_2D(std::pair<double,double> p1,std::pair<double,double> p2){
    return (round(
    sqrt(pow((p1.first - p2.first), 2)+pow((p1.second - p2.second), 2))));
}

int GEO(std::pair<double,double> p1,std::pair<double,double> p2){
    double PI{3.131592};
    double RRR{6378.388};
    double lat_1,lng_1,lat_2,lng_2;

    {
        auto conv = [PI](int deg, double min){
            return(PI * (deg + (5.0*(min/3.0)))/180.0);
        };

        int deg_x1{round(p1.first)},deg_y1{round(p1.second)};
        double min_x1{deg_x1-p1.first},min_y1{deg_y1-p1.second}; 
        int deg_x2{round(p2.first)},deg_y2{round(p2.second)};
        double min_x2{deg_x2-p2.first},min_y2{deg_y2-p2.second};

        lat_1 = conv(deg_x1,min_x1);
        lng_1 = conv(deg_y1,min_y1);

        lat_2 = conv(deg_x2,min_x2);
        lng_2 = conv(deg_y2,min_y2);
    }

    double q1 = cos(lng_1 - lng_2) q2 = cos(lat_1 - lat_2) q3 = cos(lat_1 + lat_2);

    return((RRR*acos(0.5*((1.0+q2)*q2 - (1.0-q2)*q3)))+1.0);
}

int ATT(std::pair<double,double> p1,std::pair<double,double> p2){
    double dX{p1.first - p2.first},dY{p1.second-p2.second},rij,tij;
    rij = sqrt((dX*dX + dY*dY)/10.0);
    tij = round(rij);

    return(tij<rij?(tij+1):tij);
}