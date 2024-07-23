#ifndef ARM_HPP
#define ARM_HPP

#include <random>
#include <iostream>
#include <vector> 

template <typename RewardTemplate>
class Arm{
protected: 
    std::mt19937* generatorptr;
public: 
    Arm(std::mt19937& generator);
    virtual ~Arm() = default;
    virtual RewardTemplate getReward() = 0;
};

class RandomGaussianArm: public Arm<double>{
    double mean;
    double stdDev;
public:
    RandomGaussianArm(std::mt19937& generator);
    double getReward() override;
};

#endif