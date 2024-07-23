#include <iostream>
#include <vector>
#include <random>

#include "Arm.cpp"

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
    RandomArmSelector(std::mt19937& generator):ArmSelector<ArmTemplate, RewardTemplate>(generator){
        std::cout<<"RandomArmSelector constructor"<<"\t";
    };
    
    ArmTemplate selectArm(std::vector<ArmTemplate>& arms, std::vector<ArmTemplate>& pastArms, std::vector<RewardTemplate>& pastRewards) override {
        std::cout<<"RandomArmSelector selectArm"<<"\t";
        std::uniform_int_distribution<int> uniform(0,arms.size()-1);
        int selectedIndex = uniform(*(this->generatorptr));
        std::cout<<"Selected Arm: "<<selectedIndex<<"\n";
        return arms[selectedIndex];
    }
};

int main(){
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::mt19937 generator(seed);
    int ARMS = 5;
    int ROUNDS = 5;
    std::vector<RandomGaussianArm> arms; 
    for(int i = 0; i<ARMS; i++){
        arms.push_back(RandomGaussianArm(generator));
    }
    std::vector<RandomGaussianArm> pickedArms;
    std::vector<double> pickedRewards;
    for(int i = 0; i<ROUNDS; i++){
        RandomArmSelector<RandomGaussianArm,double> randomArmSelectorInstance(generator);
        RandomGaussianArm selectedArm = randomArmSelectorInstance.selectArm(arms,pickedArms,pickedRewards);
        double reward = selectedArm.getReward();
        pickedArms.push_back(selectedArm);
        pickedRewards.push_back(reward);
    }
    return 0;
}