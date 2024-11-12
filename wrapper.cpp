#include <bits/stdc++.h>
#include <fstream>
#include <iostream>
#include "bandit.cpp"
// want to: 
    // check accuracy 
    // check time
    // make csv files for graphs
template <typename RewardTemplate = double>
class Wrapper{
private:
    std::unique_ptr<BanditAlgorithm<RewardTemplate>> algorithmInstance;
    std::mt19937 generator;
public: 
    Wrapper(){
        unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
        std::mt19937 generator(seed);
        this->generator = generator;
        std::cout<<"\n";
        this->algorithmInstance = constructAlgorithm(); 
        int ROUNDS = 1000;
        for(int i = 0; i<ROUNDS; i++){
            std::cout<<"Running iteration "<<i+1<<"\n";
            run();
            std::cout<<"\n";
            if(i!=ROUNDS-1){
                this->algorithmInstance->refresh();
            }
        }
    }
    std::unique_ptr<BanditAlgorithm<RewardTemplate>> constructAlgorithm(){
        std::string mode;
        std::cout<<"Enter mode: ";
        std::cin>>mode;
        int ARMS; 
        std::cout<<"Enter number of arms: ";
        std::cin>>ARMS; 
        if(mode == "UA"||mode == "SR"||mode == "UCB"||mode=="SH"){    
            int BUDGET;
            std::cout<<"Enter budget: ";
            std::cin>>BUDGET;
            if(mode=="SR"){
                std::cout<<"\n";
                return std::make_unique<SuccessiveRejectsAlgorithm<>>(ARMS, BUDGET, this->generator,true);
            }
            else if(mode=="UA"){
                std::cout<<"\n";
                return std::make_unique<fixedBudget<>>(ARMS, BUDGET, this->generator, true);
            }
            else if(mode=="UCB"){
                double a;
                std::cout<<"Enter a: ";
                std::cin>>a;
                std::cout<<"\n";
                return std::make_unique<UCB_2<>>(ARMS, BUDGET, this->generator, a, true);
            }
            else if(mode=="SH"){
                std::cout<<"\n";
                return std::make_unique<SequentialHalving<>>(ARMS, BUDGET, this->generator, true);
            }
        }
        else if(mode == "ME"||mode == "EGE"){
            if(mode=="ME"){
                double epsilon;
                double delta;
                std::cout<<"Enter approximation: ";
                std::cin>>epsilon;
                std::cout<<"Enter confidence: ";
                std::cin>>delta;
                std::cout<<"\n";
                return std::make_unique<MedianElimination<>>(ARMS, this->generator, epsilon, delta, true);
            }
            else{
                double delta;
                std::cout<<"Enter confidence: ";
                std::cin>>delta;
                std::cout<<"\n";
                return std::make_unique<ExponentialGapElimination<>>(ARMS, this->generator, delta, true);
            }
        }
        std::cout<<"Invalid mode! Returning EGE with 10 arms, delta 0.1\n";
        std::cout<<"\n";
        return std::make_unique<ExponentialGapElimination<>>(10, this->generator, 0.1, true);
    }
    void run(){
        int error = this->algorithmInstance->run();
        if(!error){
            Metrics<RewardTemplate> metrics = this->algorithmInstance->evaluate();
            writeMetric(metrics, "metrics.csv");
        }
    }
    void writeMetric(Metrics<RewardTemplate> metrics, std::string filename){
        bool isEmpty = false;
        std::ifstream infile(filename);
        if (infile.peek() == std::ifstream::traits_type::eof()) {
            isEmpty = true;
        }
        infile.close();

        std::ofstream file(filename,std::ios::app);
        if(!file.is_open()){
            std::cout<<"Failed to open file\n";
            return;
        }
        if(isEmpty)file<<"Algorithm,Metadata,Rundata,Pulls\n";
        
        file<<"\""<<metrics.algorithm<<"\",";
        
        int count = metrics.algorithmMetadata.size();
        int c = 0; 
        file<<"\"{";
        for(std::pair<std::string,RewardTemplate> i: metrics.algorithmMetadata){
            file<<"\\\""<<i.first<<"\\\""<<":"<<i.second;
            if(c!=count-1)file<<",";
            c++;
        }
        file<<"}\",";
        
        count = metrics.algorithmRunMetadata.size();
        c = 0; 
        file<<"\"{";
        for(std::pair<std::string,RewardTemplate> i: metrics.algorithmRunMetadata){
            file<<"\\\""<<i.first<<"\\\""<<":"<<i.second;
            if(c!=count-1)file<<",";
            c++;
        }
        file<<"}\",";

        file<<metrics.pulls<<"\n";

        file.close();
    }
};

int main(){
    Wrapper<> wrapper;
    return 0;
}