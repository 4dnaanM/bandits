#include <random>
#include <iostream>
#include <vector> 

template <typename RewardTemplate>
class Arm{
protected: 
    std::mt19937* generatorptr;
public: 
    int id; 
    Arm(std::mt19937& generator, int id){
        this->id=id;
        this->generatorptr = &generator;
    }
    virtual RewardTemplate getReward() = 0;
};

class RandomGaussianArm: public Arm<double>{
    double mean; 
    double stdDev;
public:
    RandomGaussianArm(std::mt19937& generator, int id):Arm<double>(generator,id){
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
    double getMean(){
        return this->mean;
    }
    double getStdDev(){
        return this->stdDev;
    }
    void setMean(){
        std::uniform_real_distribution<double> uniform(0,1);
        this->mean = uniform(*(this->generatorptr));
    }
    void setStdDev(){
        std::uniform_real_distribution<double> uniform(0,1);
        this->stdDev = uniform(*(this->generatorptr));
    }
};