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
        cities.push_back(City(6, 2.0, -1.0));
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
        cities2.push_back(City(6, 2.0, -1.0));
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

    {
        cout<<"\nTour One Partition\n" << endl;
        vector<City> cities, cities2;

        cities.push_back(City(1, 2.0, 0.0));
        cities.push_back(City(2, 2.0, 2.0));
        cities.push_back(City(3, 1.0, 1.0));
        cities.push_back(City(4, 0.0, 2.0));
        cities.push_back(City(5, 0.0, 0.0));
    
        cities2.push_back(City(1, 2.0, 0.0));
        cities2.push_back(City(2, 2.0, 2.0));
        cities2.push_back(City(4, 0.0, 2.0));
        cities2.push_back(City(3, 1.0, 1.0));
        cities2.push_back(City(5, 0.0, 0.0));

        run(cities, cities2);
    }

    {
        cout<<"\nTour unfeasable\n" << endl;
        vector<City> cities, cities2;

        cities.push_back(City(1, 0.0, 1.0));
        cities.push_back(City(2, 0.0, 0.0));
        cities.push_back(City(3, 1.0, 1.0)); 
        cities.push_back(City(4, 1.0, 2.0));
        cities.push_back(City(5, 3.0, 0.0));  
        cities.push_back(City(6, 4.0, 0.0));
        cities.push_back(City(7, 1.0, 5.0));
        cities.push_back(City(8, 1.0, 6.0)); 
        cities.push_back(City(9, 1.0, 7.0)); 
        cities.push_back(City(10, 7.0, 0.0));
        cities.push_back(City(11, 6.0, 0.0));
        cities.push_back(City(12, 5.0, 0.0));
        cities.push_back(City(13, 1.0, 4.0));
        cities.push_back(City(14, 1.0, 3.0));
        cities.push_back(City(15, 2.0, 0.0));
        cities.push_back(City(16, 1.0, 0.0));
    

        cities2.push_back(City(1, 0.0, 1.0));
        cities2.push_back(City(3, 1.0, 1.0)); 
        cities2.push_back(City(4, 1.0, 2.0));
        cities2.push_back(City(14, 1.0, 3.0));
        cities2.push_back(City(13, 1.0, 4.0));
        cities2.push_back(City(7, 1.0, 5.0));
        cities2.push_back(City(8, 1.0, 6.0)); 
        cities2.push_back(City(10, 7.0, 0.0));
        cities2.push_back(City(9, 1.0, 7.0)); 
        cities2.push_back(City(11, 6.0, 0.0));
        cities2.push_back(City(12, 5.0, 0.0));
        cities2.push_back(City(6, 4.0, 0.0));
        cities2.push_back(City(5, 3.0, 0.0));  
        cities2.push_back(City(15, 2.0, 0.0));
        cities2.push_back(City(16, 1.0, 0.0));
        cities2.push_back(City(2, 0.0, 0.0));

        run(cities, cities2);
    }

    {
        cout<<"\nTour exemplo gpx2_s\n" << endl;
        vector<City> cities, cities2;

        cities.push_back(City(1, 0.0, 5.0));
        cities.push_back(City(2, 4.0, 5.0));
        cities.push_back(City(3, 4.0, 4.0)); 
        cities.push_back(City(4, 2.0, 4.0));
        cities.push_back(City(5, 1.0, 3.0));  
        cities.push_back(City(6, 3.0, 3.0));
        cities.push_back(City(7, 3.0, 2.0));
        cities.push_back(City(8, 1.0, 2.0)); 
        cities.push_back(City(9, 2.0, 1.0)); 
        cities.push_back(City(10, 4.0, 1.0));
        cities.push_back(City(11, 5.0, 0.0));
        cities.push_back(City(12, 6.0, 0.0));
        cities.push_back(City(13, 7.0, 1.0));
        cities.push_back(City(14, 9.0, 1.0));
        cities.push_back(City(15, 8.0, 2.0));
        cities.push_back(City(16, 10.0, 2.0));
        cities.push_back(City(17, 10.0, 3.0));
        cities.push_back(City(18, 8.0, 3.0));
        cities.push_back(City(19, 9.0, 4.0)); 
        cities.push_back(City(20, 7.0, 4.0));
        cities.push_back(City(21, 6.0, 4.0));  
        cities.push_back(City(22, 5.0, 4.0));
        cities.push_back(City(23, 5.0, 5.0));
        cities.push_back(City(24, 6.0, 5.0)); 
        cities.push_back(City(25, 7.0, 5.0)); 
        cities.push_back(City(26, 11.0, 5.0));
        cities.push_back(City(27, 11.0, 0.0));
        cities.push_back(City(28, 7.0, 0.0));
        cities.push_back(City(29, 6.0, 1.0));
        cities.push_back(City(30, 5.0, 1.0));
        cities.push_back(City(31, 4.0, 0.0));
        cities.push_back(City(32, 0.0, 0.0));
    

        cities2.push_back(City(1, 0.0, 5.0));
        cities2.push_back(City(2, 4.0, 5.0));
        cities2.push_back(City(23, 5.0, 5.0));
        cities2.push_back(City(24, 6.0, 5.0)); 
        cities2.push_back(City(21, 6.0, 4.0)); 
        cities2.push_back(City(22, 5.0, 4.0));
        cities2.push_back(City(3, 4.0, 4.0)); 
        cities2.push_back(City(4, 2.0, 4.0)); 
        cities2.push_back(City(6, 3.0, 3.0));
        cities2.push_back(City(5, 1.0, 3.0)); 
        cities2.push_back(City(8, 1.0, 2.0));
        cities2.push_back(City(7, 3.0, 2.0)); 
        cities2.push_back(City(9, 2.0, 1.0)); 
        cities2.push_back(City(10, 4.0, 1.0));
        cities2.push_back(City(30, 5.0, 1.0));
        cities2.push_back(City(29, 6.0, 1.0));
        cities2.push_back(City(13, 7.0, 1.0));
        cities2.push_back(City(14, 9.0, 1.0));
        cities2.push_back(City(16, 10.0, 2.0));
        cities2.push_back(City(15, 8.0, 2.0));
        cities2.push_back(City(18, 8.0, 3.0));
        cities2.push_back(City(17, 10.0, 3.0));
        cities2.push_back(City(19, 9.0, 4.0)); 
        cities2.push_back(City(20, 7.0, 4.0));
        cities2.push_back(City(25, 7.0, 5.0)); 
        cities2.push_back(City(26, 11.0, 5.0));
        cities2.push_back(City(27, 11.0, 0.0));
        cities2.push_back(City(28, 7.0, 0.0));
        cities2.push_back(City(12, 6.0, 0.0)); 
        cities2.push_back(City(11, 5.0, 0.0));
        cities2.push_back(City(31, 4.0, 0.0));
        cities2.push_back(City(32, 0.0, 0.0));

        run(cities, cities2);
    }

    {
        cout<<"\nTour exemplo tunneling\n" << endl;
        vector<City> cities, cities2;

        cities.push_back(City(1, 1.0, 1.0));
        cities.push_back(City(2, 2.0, 1.0));
        cities.push_back(City(3, 3.01, 0.47)); 
        cities.push_back(City(4, 3.62, 1.6));
        cities.push_back(City(5, 4.0, 1.0));  
        cities.push_back(City(6, 5.0, 1.0));
        cities.push_back(City(7, 4.98, 1.58));
        cities.push_back(City(8, 5.59, 1.52));
        cities.push_back(City(9, 6.0, 1.0)); 
        cities.push_back(City(10, 6.0, 3.0)); 
        cities.push_back(City(11, 4.61, 3.61));
        cities.push_back(City(12, 5.0, 4.0));
        cities.push_back(City(13, 5.79, 3.99));
        cities.push_back(City(14, 5.76, 4.82));
        cities.push_back(City(15, 4.02, 5.55));
        cities.push_back(City(16, 2.99, 4.58));
        cities.push_back(City(17, 2, 5));
        cities.push_back(City(18, 1.38, 5.39));
        cities.push_back(City(19, 0.99, 4.44));
        cities.push_back(City(20, 1, 4.0)); 
        cities.push_back(City(21, 2.02, 3.81));
        cities.push_back(City(22, 3.02, 3.81));  
        cities.push_back(City(23, 2.21, 3.18));
        cities.push_back(City(24, 2.98, 3.19));
        cities.push_back(City(25, 4.08, 3.22)); 
        cities.push_back(City(26, 5.01, 2.87)); 
        cities.push_back(City(27, 4.4, 2.32));
        cities.push_back(City(28, 3.01, 0.47));
        cities.push_back(City(29, 1.42, 2.02));

        cities2.push_back(City(1, 1.0, 1.0));
        cities2.push_back(City(2, 2.0, 1.0));
        cities2.push_back(City(3, 3.01, 0.47));
        cities2.push_back(City(5, 4.0, 1.0));  
        cities2.push_back(City(4, 3.62, 1.6));
        cities2.push_back(City(7, 4.98, 1.58));
        cities2.push_back(City(6, 5.0, 1.0));
        cities2.push_back(City(8, 5.59, 1.52));
        cities2.push_back(City(9, 6.0, 1.0)); 
        cities2.push_back(City(10, 6.0, 3.0)); 
        cities2.push_back(City(13, 5.79, 3.99));
        cities2.push_back(City(14, 5.76, 4.82));
        cities2.push_back(City(15, 4.02, 5.55));
        cities2.push_back(City(17, 2, 5));
        cities2.push_back(City(18, 1.38, 5.39));
        cities2.push_back(City(19, 0.99, 4.44));
        cities2.push_back(City(20, 1, 4.0)); 
        cities2.push_back(City(16, 2.99, 4.58));
        cities2.push_back(City(12, 5.0, 4.0));
        cities2.push_back(City(26, 5.01, 2.87)); 
        cities2.push_back(City(11, 4.61, 3.61));
        cities2.push_back(City(25, 4.08, 3.22)); 
        cities2.push_back(City(27, 4.4, 2.32));
        cities2.push_back(City(28, 3.01, 0.47));
        cities2.push_back(City(24, 2.98, 3.19));
        cities2.push_back(City(22, 3.02, 3.81)); 
        cities2.push_back(City(21, 2.02, 3.81)); 
        cities2.push_back(City(23, 2.21, 3.18));
        cities2.push_back(City(29, 1.42, 2.02));
        

        run(cities, cities2);
    }
    
    cout << "THE END" << endl;
}

void run(vector<City>& c1, vector<City>& c2)
{
    bool legacy {false};
    Tour t1(c1), t2(c2);

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

    Tour offspring = GPX2::crossover(t1, t2,legacy);

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