#include <iostream>
#include <vector>
#include <random>

template <typename ArmTemplate, typename RewardTemplate>
class ArmSelector{
protected: 
    std::vector<ArmTemplate> arms;
public:    
    std::vector<ArmTemplate> pastArms;
    std::vector<RewardTemplate> pastRewards;
    ArmSelector(std::vector<ArmTemplate>& arms){
        this->arms = arms;
        this->pastArms = std::vector<ArmTemplate>();
        this->pastRewards = std::vector<RewardTemplate>();
    };
    virtual ArmTemplate selectArm() = 0;
    virtual RewardTemplate playArm(ArmTemplate& arm){
        RewardTemplate reward = arm.getReward();
        updateMemory(arm,reward);
        return reward;
    };
    void updateMemory(ArmTemplate& selectedArm, RewardTemplate& reward){
        this->pastArms.push_back(selectedArm);
        this->pastRewards.push_back(reward);
    }

};

template <typename ArmTemplate, typename RewardTemplate>
class RandomArmSelector : public ArmSelector<ArmTemplate, RewardTemplate>{
private: 
    std::mt19937* generatorptr;
public:
    RandomArmSelector(std::vector<ArmTemplate>& arms,std::mt19937& generator):ArmSelector<ArmTemplate, RewardTemplate>(arms){
        this->generatorptr = &generator;
    };
    
    ArmTemplate selectArm() override {
        std::cout<<"RandomArmSelector selectArm"<<"\t";
        std::uniform_int_distribution<int> uniform(0,this->arms.size()-1);
        int selectedIndex = uniform(*(this->generatorptr));
        std::cout<<"Selected Arm: "<<selectedIndex<<"\n";
        return this->arms[selectedIndex];
    }

};

template <typename ArmTemplate, typename RewardTemplate> 
class UniformAllocator : public ArmSelector<ArmTemplate, RewardTemplate>{
public: 
    UniformAllocator(std::vector<ArmTemplate>& arms, std::mt19937& generator):ArmSelector<ArmTemplate, RewardTemplate>(arms){};
    ArmTemplate selectArm() override {
        std::cout<<"UniformAllocator selectArm"<<"\t";
        int selectedIndex = this->pastArms.size()%this->arms.size();
        std::cout<<"Selected Arm: "<<selectedIndex<<"\n";
        return this->arms[selectedIndex];
    }
    
};