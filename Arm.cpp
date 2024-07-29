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

template <typename RewardTemplate>
class RandomGaussianArm: public Arm<RewardTemplate>{
    RewardTemplate mean; 
    RewardTemplate stdDev;
public:
    RandomGaussianArm(std::mt19937& generator, int id):Arm<RewardTemplate>(generator,id){
        setMean();
        setStdDev();
        std::cout<<"RandomGaussianArm Mean: "<<this->mean<<" "<<"StdDev: "<<this->stdDev<<"\n";
    };
    RewardTemplate getReward() override {;
        std::normal_distribution<RewardTemplate> gaussian(this->mean,this->stdDev);
        RewardTemplate reward = gaussian(*(this->generatorptr));
        std::cout<<"RandomGaussianArm reward: "<<reward<<"\n";
        return reward;
    }
    RewardTemplate getMean(){
        return this->mean;
    }
    RewardTemplate getStdDev(){
        return this->stdDev;
    }
    void setMean(){
        std::uniform_real_distribution<RewardTemplate> uniform(0,1);
        this->mean = uniform(*(this->generatorptr));
    }
    void setStdDev(){
        std::uniform_real_distribution<RewardTemplate> uniform(0,1);
        this->stdDev = uniform(*(this->generatorptr));
    }
};

template <typename RewardTemplate>
class BernoulliArm: public Arm<RewardTemplate>{
    RewardTemplate p; 
public:
    BernoulliArm(std::mt19937& generator, int id):Arm<RewardTemplate>(generator,id){
        setMean();
        std::cout<<"BernoulliArm p: "<<this->p<<"\n";
    };
    RewardTemplate getReward() override {;
        std::uniform_real_distribution<RewardTemplate> uniform(0,1);
        RewardTemplate sample = uniform(*(this->generatorptr));
        RewardTemplate reward = sample<=this->p? 1: 0;
        std::cout<<"BernoulliArm reward: "<<reward<<"\n";
        return reward;
    }
    RewardTemplate getMean(){
        return this->p;
    }
    void setMean(){
        std::uniform_real_distribution<RewardTemplate> uniform(0,1);
        this->p = uniform(*(this->generatorptr));
    }
};