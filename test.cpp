#include <bits/stdc++.h>
#include "Arm.cpp"
#include "armSelectorClass.cpp"
#include "evaluatorClass.cpp"
#include "bandit.cpp"
int main(){
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::mt19937 generator(seed);
    int ARMS = 10;
    int BUDGET = 100;
    
    std::cout<<"Testing bandit with "<<ARMS<<" arms and "<<BUDGET<<" budget\n";
    RandomExploreAlgorithm<RandomGaussianArm,double> exploreInstance(ARMS,BUDGET,generator);
    exploreInstance.run();
    
    // std::cout<<"Testing RandomGaussianArm by making "<<ARMS<<" arms\n";
    // std::vector<RandomGaussianArm> arms; 
    // for(int i = 0; i<ARMS; i++){
    //     arms.push_back(RandomGaussianArm(generator));
    // }
    // std::cout<<"\nTesting UniformAllocator and reward by selecting arm "<<BUDGET<<" times and sampling a reward from it\n";
    // RandomArmSelector<RandomGaussianArm,double> allocatorInstance(arms,generator);
    // // UniformAllocator<RandomGaussianArm,double> allocatorInstance(arms);
    // for(int i = 0; i<BUDGET; i++){
    //     RandomGaussianArm selectedArm = allocatorInstance.selectArm();
    //     double reward = selectedArm.getReward();
    // }
    // std::cout<<"\nTesting DeltaEvaluator by evaluating regret for each Arm\n";
    // DeltaEvaluator<RandomGaussianArm,double> deltaEvaluatorInstance(arms);
    // for(int i = 0; i<ARMS; i++){
    //     RandomGaussianArm arm = arms[i];
    //     std::cout<<"Mean: "<<arm.getMean()<<"\t";
    //     double regret = deltaEvaluatorInstance.evaluateRegret(arms,arm);
    //     std::cout<<"Regret: "<<regret<<"\n";
    // }

    return 0;
}