#include <bits/stdc++.h>

template <typename RewardTemplate = double>
struct Metrics{
    std::string algorithm;
    std::unordered_map<std::string,RewardTemplate> algorithmRunMetadata; 
    std::unordered_map<std::string,RewardTemplate> algorithmMetadata; 
    int pulls; 
    Metrics(std::string algorithm, std::unordered_map<std::string,RewardTemplate> algorithmMetadata, std::unordered_map<std::string,RewardTemplate> algorithmRunMetadata, int pulls){
        this->algorithm = algorithm;
        this->algorithmMetadata = algorithmMetadata;
        this->algorithmRunMetadata = algorithmRunMetadata;
        this->pulls = pulls;
    }
    void display(){
        std::cout<<"Algorithm: "<<this->algorithm<<"\n";
        for(std::pair<std::string,RewardTemplate> i: this->algorithmMetadata){
            std::cout<<i.first<<": "<<i.second<<"\n";
        }
        for(std::pair<std::string,RewardTemplate> i: this->algorithmRunMetadata){
            std::cout<<i.first<<": "<<i.second<<"\n";
        }
        std::cout<<"Pulls: "<<this->pulls<<"\n";
    }
};