#include <iostream>
#include <vector>
#include <random>
#include <map> 

template <typename RewardTemplate = double, typename ArmTemplate = BernoulliArm<RewardTemplate>>
class ArmSelector{
protected: 
    bool silent;
    std::map<int,ArmTemplate>* armsptr;
public:    
    std::vector<int> pastArms;
    std::vector<RewardTemplate> pastRewards;
    std::map<int,RewardTemplate> totalRewards;
    std::map<int,int> totalPulls;
    
    ArmSelector(std::map<int,ArmTemplate>& arms, bool silent = false){
        this->armsptr = &arms;
        this->pastArms = std::vector<int>();
        this->pastRewards = std::vector<RewardTemplate>();
        this->silent = silent;
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
        this->totalRewards[selectedArm.id]+=reward;
        this->totalPulls[selectedArm.id]+=1;
    }
    ArmTemplate getArm(int position){
        auto it = (*(this->armsptr)).begin();
        std::advance(it,position);
        return it->second; 
    }
};

template <typename RewardTemplate = double, typename ArmTemplate = BernoulliArm<RewardTemplate>>
class RandomArmSelector : public ArmSelector<RewardTemplate, ArmTemplate>{
private: 
    std::mt19937* generatorptr;
public:
    RandomArmSelector(std::map<int,ArmTemplate>& arms,std::mt19937& generator,bool silent = false):ArmSelector<RewardTemplate, ArmTemplate>(arms,silent){
        this->generatorptr = &generator;
    };
    
    ArmTemplate selectArm() override {
        std::uniform_int_distribution<int> uniform(0,(*(this->armsptr)).size()-1);
        ArmTemplate selectedArm = this->getArm(uniform(*(this->generatorptr)));
        if(!this->silent)std::cout<<"Random ArmSelector selected Arm: "<<selectedArm.id<<" | ";
        return selectedArm;
    }
};

template <typename RewardTemplate = double, typename ArmTemplate = BernoulliArm<RewardTemplate>>
class UniformAllocator : public ArmSelector<RewardTemplate, ArmTemplate>{
public: 
    UniformAllocator(std::map<int,ArmTemplate>& arms, std::mt19937& generator,bool silent = false):ArmSelector<RewardTemplate, ArmTemplate>(arms,silent){};
    ArmTemplate selectArm() override {
        int selectedIndex = this->pastArms.size()%(*(this->armsptr)).size();
        ArmTemplate selectedArm = this->getArm(selectedIndex);
        if(!this->silent)std::cout<<"UniformAllocator selected Arm: "<<selectedArm.id<<" | ";
        return selectedArm;
    }
};


