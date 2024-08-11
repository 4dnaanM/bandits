#include <bits/stdc++.h>

template <typename RewardTemplate = double>
struct Metrics{
    RewardTemplate optimalMean; 
    RewardTemplate regret; 
    RewardTemplate H1; 
    int pulls; 
    Metrics(RewardTemplate optimalMean, RewardTemplate regret, RewardTemplate H1, int pulls){
        this->optimalMean = optimalMean;
        this->regret = regret;
        this->H1 = H1;
        this->pulls = pulls;
    }
    void display(){
        std::cout<<this->optimalMean<<","<<this->regret<<","<<this->H1<<","<<this->pulls<<"\n";
    }
};