#include <bits/stdc++.h>
#include "Arm.cpp"
#include "armSelectorClass.cpp"
#include "evaluatorClass.cpp"
#include "bandit.cpp"
// want to: 
    // check accuracy 
    // check time
    // make csv files for graphs
    // have modes defined by strings
class Wrapper{
private:
    std::unique_ptr<BanditAlgorithm<>> algorithmInstance;
    std::mt19937 generator;
public: 
    Wrapper(){
        unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
        std::mt19937 generator(seed);
        this->generator = generator;
        construct(); 
        run();
    }
    void construct(){
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
                this->algorithmInstance = std::make_unique<SuccessiveRejectsAlgorithm<>>(ARMS, BUDGET, this->generator);
            }
            else if(mode=="UA"){
                this->algorithmInstance = std::make_unique<fixedBudget<>>(ARMS, BUDGET, this->generator);
            }
            else if(mode=="UCB"){
                double a;
                std::cout<<"Enter a: ";
                std::cin>>a;
                this->algorithmInstance = std::make_unique<UCB_2<>>(ARMS, BUDGET, this->generator, a);
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
                this->algorithmInstance = std::make_unique<MedianElimination<>>(ARMS, this->generator, epsilon, delta);
            }
            else{
                double delta;
                std::cout<<"Enter confidence: ";
                std::cin>>delta;
                this->algorithmInstance = std::make_unique<ExponentialGapElimination<>>(ARMS, this->generator, delta);
            }
        }
        else{
            std::cout<<"Invalid mode\n";
            return;
        }
    }
    void run(){
        this->algorithmInstance->run();
        this->algorithmInstance->evaluate();
    }
};

int main(){
    Wrapper wrapper;
    return 0;
}