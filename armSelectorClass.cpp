#include <iostream>
#include <vector>
#include <random>
#include <map> 

template <typename ArmTemplate, typename RewardTemplate>
class ArmSelector{
protected: 
    std::map<int,ArmTemplate>* armsptr;
public:    
    std::vector<int> pastArms;
    std::vector<RewardTemplate> pastRewards;
    
    ArmSelector(std::map<int,ArmTemplate>& arms){
        this->armsptr = &arms;
        this->pastArms = std::vector<int>();
        this->pastRewards = std::vector<RewardTemplate>();
    };
    virtual ArmTemplate selectArm() = 0;
    virtual RewardTemplate playArm(ArmTemplate& arm){
        RewardTemplate reward = arm.getReward();
        updateMemory(arm,reward);
        return reward;
    };
    void updateMemory(ArmTemplate& selectedArm, RewardTemplate& reward){
        this->pastArms.push_back(selectedArm.id);
        this->pastRewards.push_back(reward);
    }
    ArmTemplate getArm(int position){
        auto it = (*(this->armsptr)).begin();
        std::advance(it,position);
        return it->second; 
    }
};

template <typename ArmTemplate, typename RewardTemplate>
class RandomArmSelector : public ArmSelector<ArmTemplate, RewardTemplate>{
private: 
    std::mt19937* generatorptr;
public:
    RandomArmSelector(std::map<int,ArmTemplate>& arms,std::mt19937& generator):ArmSelector<ArmTemplate, RewardTemplate>(arms){
        this->generatorptr = &generator;
    };
    
    ArmTemplate selectArm() override {
        std::uniform_int_distribution<int> uniform(0,(*(this->armsptr)).size()-1);
        ArmTemplate selectedArm = this->getArm(uniform(*(this->generatorptr)));
        std::cout<<"Random ArmSelector selected Arm: "<<selectedArm.id<<" | ";
        return selectedArm;
    }
};

template <typename ArmTemplate, typename RewardTemplate> 
class UniformAllocator : public ArmSelector<ArmTemplate, RewardTemplate>{
public: 
    UniformAllocator(std::map<int,ArmTemplate>& arms, std::mt19937& generator):ArmSelector<ArmTemplate, RewardTemplate>(arms){};
    ArmTemplate selectArm() override {
        int selectedIndex = this->pastArms.size()%(*(this->armsptr)).size();
        ArmTemplate selectedArm = this->getArm(selectedIndex);
        std::cout<<"UniformAllocator selected Arm: "<<selectedArm.id<<" | ";
        return selectedArm;
    }
};