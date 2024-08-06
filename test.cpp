#include <bits/stdc++.h>
#include "Arm.cpp"
#include "armSelectorClass.cpp"
#include "evaluatorClass.cpp"
#include "bandit.cpp"
int main(){
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::mt19937 generator(seed);
    int ARMS = 8;
    int BUDGET = 123;
    double a = 100;
    double epsilon = 0.2;
    double delta = 0.2;
    // std::cout<<"Testing fixed budget with "<<ARMS<<" arms and "<<BUDGET<<" budget\n\n";
    // fixedBudget<RandomGaussianArm<double>,double,UniformAllocator<RandomGaussianArm<double>,double>,DeltaEvaluator<RandomGaussianArm<double>,double>> algorithmInstance(ARMS,BUDGET,generator);
    // fixedBudget<RandomGaussianArm<double>,double,RandomArmSelector<RandomGaussianArm<double>,double>,DeltaEvaluator<RandomGaussianArm<double>,double>> algorithmInstance(ARMS,BUDGET,generator);
    // SuccessiveRejectsAlgorithm<RandomGaussianArm<double>,double,UniformAllocator<RandomGaussianArm<double>,double>,DeltaEvaluator<RandomGaussianArm<double>,double>> algorithmInstance(ARMS,BUDGET,generator);
    // UCB_2<RandomGaussianArm<double>,double,UniformAllocator<RandomGaussianArm<double>,double>,DeltaEvaluator<RandomGaussianArm<double>,double>> algorithmInstance(ARMS,BUDGET,generator,a);
    // fixedBudget<BernoulliArm<double>,double,UniformAllocator<BernoulliArm<double>,double>,DeltaEvaluator<BernoulliArm<double>,double>> algorithmInstance(ARMS,BUDGET,generator);
    // fixedBudget<BernoulliArm<double>,double,RandomArmSelector<BernoulliArm<double>,double>,DeltaEvaluator<BernoulliArm<double>,double>> algorithmInstance(ARMS,BUDGET,generator);
    // SuccessiveRejectsAlgorithm<BernoulliArm<double>,double,UniformAllocator<BernoulliArm<double>,double>,DeltaEvaluator<BernoulliArm<double>,double>> algorithmInstance(ARMS,BUDGET,generator);
    // UCB_2<BernoulliArm<double>,double,UniformAllocator<BernoulliArm<double>,double>,DeltaEvaluator<BernoulliArm<double>,double>> algorithmInstance(ARMS,BUDGET,generator,a);
    
    std::cout<<"Testing fixed confidence with "<<ARMS<<" arms and "<<delta<<" delta\n\n";
    
    MedianElimination<BernoulliArm<double>,double,UniformAllocator<BernoulliArm<double>, double>, DeltaEvaluator<BernoulliArm<double>,double>> algorithmInstance(ARMS,generator,epsilon,delta);
    // MedianElimination<RandomGaussianArm<double>,double,UniformAllocator<RandomGaussianArm<double>, double>, DeltaEvaluator<RandomGaussianArm<double>,double>> algorithmInstance(ARMS,generator,epsilon,delta);
    algorithmInstance.run();
    algorithmInstance.evaluate();

    return 0;
}
// make skeleton 