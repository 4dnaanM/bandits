#include <bits/stdc++.h>
#include "Arm.cpp"
#include "armSelectorClass.cpp"
#include "evaluatorClass.cpp"
#include "bandit.cpp"
int main(){
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::mt19937 generator(seed);
    int ARMS = 5;
    int BUDGET = 100;
    
    std::cout<<"Testing bandit with "<<ARMS<<" arms and "<<BUDGET<<" budget\n\n";
    // banditAlgorithm<RandomGaussianArm,double,UniformAllocator<RandomGaussianArm,double>,DeltaEvaluator<RandomGaussianArm,double>> algorithmInstance(ARMS,BUDGET,generator);
    // banditAlgorithm<RandomGaussianArm,double,RandomArmSelector<RandomGaussianArm,double>,DeltaEvaluator<RandomGaussianArm,double>> algorithmInstance(ARMS,BUDGET,generator);
    SuccessiveRejectsAlgorithm<RandomGaussianArm,double,RandomArmSelector<RandomGaussianArm,double>,DeltaEvaluator<RandomGaussianArm,double>> algorithmInstance(ARMS,BUDGET,generator);
    algorithmInstance.run();
    algorithmInstance.evaluate();

    return 0;
}

// DO: UCB