#include <random>
#include <iostream>
#include <vector> 

template <typename RewardTemplate>
class Arm{
protected: 
    std::mt19937* generatorptr;
public: 
    Arm(std::mt19937& generator){
        this->generatorptr = &generator;
    }
    virtual RewardTemplate getReward() = 0;
};

class RandomGaussianArm: public Arm<double>{
    double mean;
    double stdDev;
public:
    RandomGaussianArm(std::mt19937& generator):Arm<double>(generator){
        std::uniform_real_distribution<double> uniform(0,1);
        this->mean = uniform(*(this->generatorptr));
        this->stdDev = uniform(*(this->generatorptr));
    };
    double getReward() override {;
        std::normal_distribution<double> gaussian(this->mean,this->stdDev);
        double reward = gaussian(*(this->generatorptr));
        return reward;
    }
};