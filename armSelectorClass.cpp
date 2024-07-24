#include <iostream>
#include <vector>
#include <random>

template <typename ArmTemplate, typename RewardTemplate>
class ArmSelector{
protected: 
    std::mt19937* generatorptr;
public:    
    ArmSelector(std::mt19937& generator){
        this->generatorptr = &generator;
    }
    virtual ArmTemplate selectArm(std::vector<ArmTemplate>& arms, std::vector<ArmTemplate>& pastArms, std::vector<RewardTemplate>& pastRewards) = 0;
};

template <typename ArmTemplate, typename RewardTemplate>
class RandomArmSelector : public ArmSelector<ArmTemplate, RewardTemplate>{
public:
    RandomArmSelector(std::mt19937& generator):ArmSelector<ArmTemplate, RewardTemplate>(generator){};
    
    ArmTemplate selectArm(std::vector<ArmTemplate>& arms, std::vector<ArmTemplate>& pastArms, std::vector<RewardTemplate>& pastRewards) override {
        std::cout<<"RandomArmSelector selectArm"<<"\t";
        std::uniform_int_distribution<int> uniform(0,arms.size()-1);
        int selectedIndex = uniform(*(this->generatorptr));
        std::cout<<"Selected Arm: "<<selectedIndex<<"\n";
        return arms[selectedIndex];
    }
};
