#include <iostream>
#include <vector>
#include <random>
#include <unordered_map> 

template <typename ArmTemplate, typename RewardTemplate>
class ArmSelector{
protected: 
    std::unordered_map<int,ArmTemplate>* armsptr;
public:    
    std::vector<ArmTemplate> pastArms;
    std::vector<RewardTemplate> pastRewards;
    ArmSelector(std::unordered_map<int,ArmTemplate>& arms){
        this->armsptr = &arms;
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
    RandomArmSelector(std::unordered_map<int,ArmTemplate>& arms,std::mt19937& generator):ArmSelector<ArmTemplate, RewardTemplate>(arms){
        this->generatorptr = &generator;
    };
    
    ArmTemplate selectArm() override {
        std::cout<<"RandomArmSelector selectArm"<<"\t";
        std::uniform_int_distribution<int> uniform(0,(*(this->armsptr)).size()-1);
        ArmTemplate selectedArm = this->getArm(uniform(*(this->generatorptr)));
        std::cout<<"Selected Arm: "<<selectedArm.id<<"\n";
        return selectedArm;
    }
};

template <typename ArmTemplate, typename RewardTemplate> 
class UniformAllocator : public ArmSelector<ArmTemplate, RewardTemplate>{
public: 
    UniformAllocator(std::unordered_map<int,ArmTemplate>& arms, std::mt19937& generator):ArmSelector<ArmTemplate, RewardTemplate>(arms){};
    ArmTemplate selectArm() override {
        std::cout<<"UniformAllocator selectArm"<<"\t";
        int selectedIndex = this->pastArms.size()%(*(this->armsptr)).size();
        ArmTemplate selectedArm = this->getArm(selectedIndex);
        std::cout<<"Selected Arm: "<<selectedArm.id<<"\n";
        return selectedArm;
    }
};