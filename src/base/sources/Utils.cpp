#include "Utils.hpp"
#include "Globals.hpp" 
#include "City.hpp"

#include <cmath>

int getFitness(const vector<int> &t)
{ // Calculate fitness of tour
    int sum{0};
    unsigned size{(unsigned)t.size()};
    for(unsigned i=0; i<size; i++){
        sum += distance(t[i], t[(i+1)%size]);
    }
    return(sum);
}

ostream& printTour(const vector<int> &t, ostream &output){
    for (int cityId : t) {
        output << cityId << " " <<Globals::map.getCityById(cityId).getX()<<" "<<Globals::map.getCityById(cityId).getY()<<"\n";
    }
    // output << "\nFitness: " << getFitness(t) << "\n";
    output.flush();
    return (output);
}

int distance(string p1, string p2){
    int tmp1{stoi(p1)},tmp2{stoi(p2)};
    return(distance(tmp1,tmp2));
}

int distance(int p1,int p2){
    switch(Config::TYPE){
        case Config::type::EUC_2D:{
            return(EUC_2D(p1,p2));
        }
        case Config::type::GEO:{
            return(GEO(p1,p2));
        }
        case Config::type::ATT:{
            return(ATT(p1,p2));
        }
        default:
            exit(1);
            return -1;
    }
}

int EUC_2D(int p1,int p2){
    City c1{Globals::map.getCityById(p1)},c2{Globals::map.getCityById(p2)};
    return (round(
    sqrt(pow((c1.getX() - c2.getX()), 2)+pow((c1.getY() - c2.getY()), 2))));
}

int GEO(int p1,int p2){
    // double PI{3.141592};
    City c1{Globals::map.getCityById(p1)},c2{Globals::map.getCityById(p2)};
    double PI{M_PI};
    double RRR{6378.388};
    double lat_1,lng_1,lat_2,lng_2;

    {
        auto conv = [PI](int deg, double min){
            return(PI * (deg + (5.0*(min/3.0)))/180.0);
        };

        int deg_x1{(int)round(c1.getX())},deg_y1{(int)round(c1.getY())};
        double min_x1{deg_x1-c1.getX()},min_y1{deg_y1-c1.getY()}; 
        int deg_x2{(int)round(c2.getX())},deg_y2{(int)round(c2.getY())};
        double min_x2{deg_x2-c2.getX()},min_y2{deg_y2-c2.getY()};

        lat_1 = conv(deg_x1,min_x1);
        lng_1 = conv(deg_y1,min_y1);

        lat_2 = conv(deg_x2,min_x2);
        lng_2 = conv(deg_y2,min_y2);
    }

    double q1 = cos(lng_1 - lng_2), q2 = cos(lat_1 - lat_2), q3 = cos(lat_1 + lat_2);

    return(round((RRR*acos(0.5*((1.0+q1)*q2 - (1.0-q1)*q3)))+1.0));
}

int ATT(int p1,int p2){
    City c1{Globals::map.getCityById(p1)},c2{Globals::map.getCityById(p2)};
    double dX{c1.getX() - c2.getX()},dY{c1.getY()-c2.getY()},rij,tij;
    rij = sqrt((dX*dX + dY*dY)/10.0);
    tij = round(rij);

    return(tij<rij?(tij+1):tij);
}