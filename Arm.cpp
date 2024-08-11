#include <random>
#include <iostream> 

template <typename RewardTemplate = double>
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
    virtual void printReward(RewardTemplate reward) = 0; 
};

template <typename RewardTemplate = double>
class RandomGaussianArm: public Arm<RewardTemplate>{
    RewardTemplate mean; 
    RewardTemplate stdDev;
    bool silent; 
public:
    RandomGaussianArm(std::mt19937& generator, int id, bool silent = false):Arm<RewardTemplate>(generator,id){
        setMean();
        setStdDev();
        if(!this->silent)std::cout<<"RandomGaussianArm Mean: "<<this->mean<<" "<<"StdDev: "<<this->stdDev<<"\n";
    };
    void printReward(RewardTemplate reward) override {
        std::cout<<"RandomGaussianArm reward: "<<reward<<"\n";
    }
    RewardTemplate getReward() override {;
        std::normal_distribution<RewardTemplate> gaussian(this->mean,this->stdDev);
        RewardTemplate reward = gaussian(*(this->generatorptr));
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

template <typename RewardTemplate = double>
class BernoulliArm: public Arm<RewardTemplate>{
    RewardTemplate p; 
    bool silent;
public:
    BernoulliArm(std::mt19937& generator, int id, bool silent = false):Arm<RewardTemplate>(generator,id){
        setMean();
        if(!silent)std::cout<<"BernoulliArm p: "<<this->p<<"\n";
    };
    void printReward(RewardTemplate reward) override {
        std::cout<<"BernoulliArm reward: "<<reward<<"\n";
    }
    RewardTemplate getReward() override {;
        std::uniform_real_distribution<RewardTemplate> uniform(0,1);
        RewardTemplate sample = uniform(*(this->generatorptr));
        RewardTemplate reward = sample<=this->p? 1: 0;
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