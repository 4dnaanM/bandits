#include "selectArm.hpp"

template <typename ArmTemplate, typename RewardTemplate>
ArmSelector<ArmTemplate,RewardTemplate>::ArmSelector(std::mt19937& generator){
    this->generatorptr = &generator;
}

template <typename ArmTemplate, typename RewardTemplate>
RandomArmSelector<ArmTemplate,RewardTemplate>::RandomArmSelector(std::mt19937& generator):ArmSelector<ArmTemplate, RewardTemplate>(generator){
    std::cout<<"RandomArmSelector constructor"<<"\t";
};
    
template <typename ArmTemplate, typename RewardTemplate>
ArmTemplate RandomArmSelector<ArmTemplate,RewardTemplate>::selectArm(std::vector<ArmTemplate>& arms, std::vector<ArmTemplate>& pastArms, std::vector<RewardTemplate>& pastRewards){
    std::cout<<"RandomArmSelector selectArm"<<"\t";
    std::uniform_int_distribution<int> uniform(0,arms.size()-1);
    int selectedIndex = uniform(*(this->generatorptr));
    std::cout<<"Selected Arm: "<<selectedIndex<<"\n";
    return arms[selectedIndex];
}

int main(){
    std::cout<<"Testing Arm\n";
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::mt19937 generator(seed);    
    int ARMS = 3;
    int ROUNDS = 3;
    std::vector<RandomGaussianArm> firstarms; 
    for(int i = 0; i<ARMS; i++){

        std::cout<<"Arm: "<<i+1<<"\n";
        firstarms.push_back(RandomGaussianArm(generator));
        for (int j = 0; j<ROUNDS; j++){
            firstarms[i].getReward();
        }
        std::cout<<std::endl;
    }
    std::cout<<"Testing RandomArmSelector\n";
    ARMS = 5;
    ROUNDS = 5;
    std::vector<RandomGaussianArm> secondarms; 
    for(int i = 0; i<ARMS; i++){
        secondarms.push_back(RandomGaussianArm(generator));
    }
    std::vector<RandomGaussianArm> pickedArms;
    std::vector<double> pickedRewards;
    for(int i = 0; i<ROUNDS; i++){
        RandomArmSelector<RandomGaussianArm,double> randomArmSelectorInstance(generator);
        RandomGaussianArm selectedArm = randomArmSelectorInstance.selectArm(secondarms,pickedArms,pickedRewards);
        double reward = selectedArm.getReward();
        pickedArms.push_back(selectedArm);
        pickedRewards.push_back(reward);
    }
    return 0;
}