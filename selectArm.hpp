#ifndef SELECTARM_HPP
#define SELECTARM_HPP

#include "Arm.hpp"

template <typename ArmTemplate, typename RewardTemplate>
class ArmSelector{
protected: 
    std::mt19937* generatorptr;
public:    
    ArmSelector(std::mt19937& generator);
    virtual ~ArmSelector() = default;
    virtual ArmTemplate selectArm(std::vector<ArmTemplate>& arms, std::vector<ArmTemplate>& pastArms, std::vector<RewardTemplate>& pastRewards) = 0;
};

template <typename ArmTemplate, typename RewardTemplate>
class RandomArmSelector : public ArmSelector<ArmTemplate, RewardTemplate>{
public:
    RandomArmSelector(std::mt19937& generator);
    ArmTemplate selectArm(std::vector<ArmTemplate>& arms, std::vector<ArmTemplate>& pastArms, std::vector<RewardTemplate>& pastRewards) override ;
};

#endif