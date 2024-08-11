#include <bits/stdc++.h>
#include <fstream>
#include <iostream>
#include "bandit.cpp"
// want to: 
    // check accuracy 
    // check time
    // make csv files for graphs
    // have modes defined by strings
    // implement silent mode

template <typename RewardTemplate = double>
class Wrapper{
private:
    std::unique_ptr<BanditAlgorithm<RewardTemplate>> algorithmInstance;
    std::mt19937 generator;
    std::vector<Metrics<RewardTemplate>> metricsList; 
public: 
    Wrapper(){
        unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
        std::mt19937 generator(seed);
        this->generator = generator;
        metricsList = std::vector<Metrics<RewardTemplate>>();
        std::cout<<"\n";
        this->algorithmInstance = constructAlgorithm(); 
        int ROUNDS = 1000;
        for(int i = 0; i<ROUNDS; i++){
            // std::cout<<"\n";
            std::cout<<"Running iteration "<<i+1<<"\n";
            run();
            std::cout<<"\n";
            if(i!=ROUNDS-1){
                this->algorithmInstance->refresh();
            }
        }
        displayMetrics();
        writeMetrics("metrics.csv");
    }
    std::unique_ptr<BanditAlgorithm<RewardTemplate>> constructAlgorithm(){
        std::string mode;
        std::cout<<"Enter mode: ";
        std::cin>>mode;
        int ARMS; 
        std::cout<<"Enter number of arms: ";
        std::cin>>ARMS; 
        if(mode == "UA"||mode == "SR"||mode == "UCB"){    
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
        this->algorithmInstance->run();
        Metrics<RewardTemplate> metrics = this->algorithmInstance->evaluate();

        this->metricsList.push_back(metrics);
    }
    void displayMetrics(){
        for(Metrics<RewardTemplate> metrics: this->metricsList){
            metrics.display();
        }
    }

    void writeMetrics(std::string filename){
        
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
        if(isEmpty)file<<"Optimal Mean,Regret,H1,Pulls\n";
        for(Metrics<RewardTemplate> metrics: this->metricsList){
            file<<metrics.optimalMean<<","<<metrics.regret<<","<<metrics.H1<<","<<metrics.pulls<<"\n";
        }
        file.close();
    }
};

int main(){
    Wrapper<> wrapper;
    return 0;
}