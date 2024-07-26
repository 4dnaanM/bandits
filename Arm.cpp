#include <random>
#include <iostream> 

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
        setMean();
        setStdDev();
        std::cout<<"RandomGaussianArm Mean: "<<this->mean<<" "<<"StdDev: "<<this->stdDev<<"\n";
    };
    double getReward() override {;
        std::normal_distribution<double> gaussian(this->mean,this->stdDev);
        double reward = gaussian(*(this->generatorptr));
        std::cout<<"RandomGaussianArm reward: "<<reward<<"\n";
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