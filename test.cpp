#include <bits/stdc++.h>
#include "Arm.cpp"
#include "armSelectorClass.cpp"
#include "evaluatorClass.cpp"
int main(){
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::mt19937 generator(seed);
    int ARMS = 5;
    int ROUNDS = 5;
    std::cout<<"Testing RandomGaussianArm by making "<<ARMS<<" arms\n";
    std::vector<RandomGaussianArm> arms; 
    for(int i = 0; i<ARMS; i++){
        arms.push_back(RandomGaussianArm(generator));
    }
    std::vector<RandomGaussianArm> pickedArms;
    std::vector<double> pickedRewards;
    std::cout<<"\nTesting RandomArmSelector and reward by selecting arm "<<ROUNDS<<" times and sampling a reward from it\n";
    for(int i = 0; i<ROUNDS; i++){
        RandomArmSelector<RandomGaussianArm,double> randomArmSelectorInstance(generator);
        RandomGaussianArm selectedArm = randomArmSelectorInstance.selectArm(arms,pickedArms,pickedRewards);
        double reward = selectedArm.getReward();
        pickedArms.push_back(selectedArm);
        pickedRewards.push_back(reward);
    }
    std::cout<<"\nTesting DeltaEvaluator by evaluating regret for each Arm\n";
    DeltaEvaluator<RandomGaussianArm,double> deltaEvaluatorInstance(arms);
    for(int i = 0; i<ARMS; i++){
        RandomGaussianArm arm = arms[i];
        std::cout<<"Mean: "<<arm.getMean()<<"\t";
        double regret = deltaEvaluatorInstance.evaluateRegret(arms,arm);
        std::cout<<"Regret: "<<regret<<"\n";
    }
    return 0;
}