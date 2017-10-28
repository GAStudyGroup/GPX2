#include "City.hpp"
#include "GPX2.hpp"
#include "ListOfCities.hpp"
#include "Tour.hpp"
#include <algorithm>
#include <deque>
#include <iostream>
#include <iterator>
#include <vector>

using namespace std;

// Tours já criados no arquivo "Tours.TSP"

void run(vector<City>&, vector<City>&);

int main()
{
    {
        cout<<"\nTour1 - Sem Ghost\n" << endl;
        vector<City> cities, cities2;

        cities.push_back(City(1, 0.0, 1.0));
        cities.push_back(City(4, 1.0, 1.0));
        cities.push_back(City(5, 1.0, 2.0));
        cities.push_back(City(2, 0.0, 2.0));
        cities.push_back(City(6, 1.0, 3.0));
        cities.push_back(City(8, 2.0, 3.0)); 
        cities.push_back(City(12, 2.0, 3.0));  
        cities.push_back(City(11, 3.0, 2.0));
        cities.push_back(City(14, 4.0, 2.0));  
        cities.push_back(City(13, 4.0, 1.0)); 
        cities.push_back(City(10, 3.0, 1.0));
        cities.push_back(City(9, 3.0, 0.0));  
        cities.push_back(City(7, 2.0, 0.0)); 
        cities.push_back(City(3, 1.0, 0.0));
    
        cities2.push_back(City(1, 0.0, 1.0));
        cities2.push_back(City(2, 0.0, 2.0));
        cities2.push_back(City(5, 1.0, 2.0));
        cities2.push_back(City(6, 1.0, 3.0));
        cities2.push_back(City(8, 2.0, 3.0)); 
        cities2.push_back(City(12, 2.0, 3.0));  
        cities2.push_back(City(14, 4.0, 2.0));  
        cities2.push_back(City(11, 3.0, 2.0));
        cities2.push_back(City(10, 3.0, 1.0));
        cities2.push_back(City(13, 4.0, 1.0)); 
        cities2.push_back(City(9, 3.0, 0.0));  
        cities2.push_back(City(7, 2.0, 0.0)); 
        cities2.push_back(City(3, 1.0, 0.0));
        cities2.push_back(City(4, 1.0, 1.0));

        run(cities, cities2);
    }

    {
        cout<<"\nTour1 - Com Ghost\n" << endl;
        vector<City> cities, cities2;

        cities.push_back(City(1, 0.0, 1.0));
        cities.push_back(City(4, 1.0, 1.0));
        cities.push_back(City(5, 1.0, 2.0));
        cities.push_back(City(2, 0.0, 2.0));
        cities.push_back(City(8, 2.0, 3.0));  
        cities.push_back(City(11, 3.0, 2.0));
        cities.push_back(City(14, 4.0, 2.0));  
        cities.push_back(City(13, 4.0, 1.0)); 
        cities.push_back(City(10, 3.0, 1.0));
        cities.push_back(City(9, 3.0, 0.0));  
        cities.push_back(City(7, 2.0, 0.0)); 
        cities.push_back(City(3, 1.0, 0.0));
    
        cities2.push_back(City(1, 0.0, 1.0));
        cities2.push_back(City(2, 0.0, 2.0));
        cities2.push_back(City(5, 1.0, 2.0));
        cities2.push_back(City(8, 2.0, 3.0));  
        cities2.push_back(City(14, 4.0, 2.0));  
        cities2.push_back(City(11, 3.0, 2.0));
        cities2.push_back(City(10, 3.0, 1.0));
        cities2.push_back(City(13, 4.0, 1.0)); 
        cities2.push_back(City(9, 3.0, 0.0));  
        cities2.push_back(City(7, 2.0, 0.0)); 
        cities2.push_back(City(3, 1.0, 0.0));
        cities2.push_back(City(4, 1.0, 1.0));

        run(cities, cities2);
    }

    {
        cout<<"\nKapriel Tour - Partição não recombinante\n" << endl;
        vector<City> cities, cities2;


        cities.push_back(City(1, 0.0, 0.0));
        cities.push_back(City(2, 0.0, -1.0));
        cities.push_back(City(4, 1.0, -1.0)); 
        cities.push_back(City(6, 2.0, 1.0));
        cities.push_back(City(7, 3.0, 0.0));  
        cities.push_back(City(9, 4.0, 0.0));
        cities.push_back(City(12, 5.0, -1.0));
        cities.push_back(City(11, 5.0, 0.0)); 
        cities.push_back(City(10, 4.0, -1.0)); 
        cities.push_back(City(8, 3.0, -1.0));  
        cities.push_back(City(5, 2.0, 0.0)); 
        cities.push_back(City(3, 1.0, 0.0));
    
        cities2.push_back(City(1, 0.0, 0.0));
        cities2.push_back(City(4, 1.0, -1.0)); 
        cities2.push_back(City(6, 2.0, 1.0));
        cities2.push_back(City(8, 3.0, -1.0)); 
        cities2.push_back(City(10, 4.0, -1.0));  
        cities2.push_back(City(12, 5.0, -1.0));
        cities2.push_back(City(11, 5.0, 0.0)); 
        cities2.push_back(City(9, 4.0, 0.0));
        cities2.push_back(City(7, 3.0, 0.0));  
        cities2.push_back(City(5, 2.0, 0.0)); 
        cities2.push_back(City(3, 1.0, 0.0));
        cities2.push_back(City(2, 0.0, -1.0));

        run(cities, cities2);
    }

    {
        cout<<"\nTour Noda Feliz\n" << endl;
        vector<City> cities, cities2;

        cities.push_back(City(1, 0.0, 4.0));
        cities.push_back(City(2, 3.0, 4.0));
        cities.push_back(City(3, 4.0, 3.0)); 
        cities.push_back(City(4, 3.0, 3.0));
        cities.push_back(City(5, 2.0, 3.0));  
        cities.push_back(City(6, 1.0, 2.0));
        cities.push_back(City(7, 2.0, 1.0));
        cities.push_back(City(8, 3.0, 1.0)); 
        cities.push_back(City(9, 3.0, 0.0)); 
        cities.push_back(City(10, 0.0, 0.0));
    
        cities2.push_back(City(1, 0.0, 4.0));
        cities2.push_back(City(2, 3.0, 4.0));
        cities2.push_back(City(4, 3.0, 3.0));
        cities2.push_back(City(5, 2.0, 3.0)); 
        cities2.push_back(City(7, 2.0, 1.0));
        cities2.push_back(City(8, 3.0, 1.0));  
        cities2.push_back(City(3, 4.0, 3.0)); 
        cities2.push_back(City(9, 3.0, 0.0)); 
        cities2.push_back(City(10, 0.0, 0.0));
        cities2.push_back(City(6, 1.0, 2.0));

        run(cities, cities2);
    }

    cout << "THE END" << endl;
}

void run(vector<City>& c1, vector<City>& c2)
{
    Tour t1(c1), t2(c2);
    GPX2 cross;

    cout << "red ";
    for (City c : t1.getTour()) {
        cout << c.getId() << " ";
    }
    cout << endl;

    cout << "blue ";
    for (City c : t2.getTour()) {
        cout << c.getId() << " ";
    }
    cout << endl;

    Tour offspring = cross.crossover(t1, t2);

    cout << "offspring ";
    for (City c : offspring.getTour()) {
        cout << c.getId() << " ";
    }
    cout << endl;

    cout << "dist t " << t1.getFitness() << "\n";
    cout << "dist t2 " << t2.getFitness() << "\n";
    cout << "dist offspring " << offspring.getFitness() << endl;

    cout<<"\n=============================================================="<<endl;
}