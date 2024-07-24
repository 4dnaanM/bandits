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
    virtual RewardTemplate getMean() = 0;
    virtual RewardTemplate getStdDev() = 0;
    virtual void setMean() = 0;
    virtual void setStdDev() = 0;
};

class RandomGaussianArm: public Arm<double>{
    double mean; 
    double stdDev;
public:
    RandomGaussianArm(std::mt19937& generator):Arm<double>(generator){
        std::cout<<"RandomGaussianArm Constructor"<<"\t";
        setMean();
        setStdDev();
        std::cout<<"Mean: "<<this->mean<<"\t"<<"StdDev: "<<this->stdDev<<"\n";

    };
    double getReward() override {;
        std::cout<<"RandomGaussianArm getReward"<<"\t";
        std::normal_distribution<double> gaussian(this->mean,this->stdDev);
        double reward = gaussian(*(this->generatorptr));
        std::cout<<"Reward: "<<reward<<"\n";
        return reward;
    }
    double getMean() override {
        return this->mean;
    }
    double getStdDev() override {
        return this->stdDev;
    }
    void setMean() override {
        std::uniform_real_distribution<double> uniform(0,1);
        this->mean = uniform(*(this->generatorptr));
    }
    void setStdDev() override {
        std::uniform_real_distribution<double> uniform(0,1);
        this->stdDev = uniform(*(this->generatorptr));
    }
};