#if !defined(ANT_HPP)
#define ANT_HPP

#include <vector>
using std::vector;

#include <utility>
using std::pair;

class Ant{
    private:
        vector<int> path;

        double calcChance(int nextCity);

        using Choice = pair<int,double>;

        int nextStep();
    public:
        Ant(int init);

        Ant();
        
        void run(); //process entire path of ant

        vector<int>& getPath();

        void setPath(vector<int> path);
};

#endif // ANT_HPP
