#include <random>
#include <iostream>
#include <vector> 

// FIX: THIS MAKES MANY ARMS WITH THE SAME MEAN AND STDDEV, BUT REWARD WORKS PROPERLY 

template <typename Reward>
class Arm{
protected: 
    std::mt19937* generatorptr;
public: 
    Arm(std::mt19937& generator){
        this->generatorptr = &generator;
    }
    virtual Reward getReward() = 0;
};

class RandomGaussianArm: public Arm<double>{
    double mean;
    double stdDev;
public:
    RandomGaussianArm(std::mt19937& generator):Arm<double>(generator){
        std::cout<<"RandomGaussianArm constructor"<<"\t";
        std::uniform_real_distribution<double> uniform(0,1);
        this->mean = uniform(*(this->generatorptr));
        this->stdDev = uniform(*(this->generatorptr));
        
        std::cout<<"Mean: "<<this->mean<<"\t";
        std::cout<<"StdDev: "<<this->stdDev<<"\n";
    };
    double getReward() override {
        std::cout<<"RandomGaussianArm getReward"<<"\t";
        std::normal_distribution<double> gaussian(this->mean,this->stdDev);
        double reward = gaussian(*(this->generatorptr));
        std::cout<<"reward: "<<reward<<std::endl;
        return reward;
    }
};

int main(){
    
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::mt19937 generator(seed);    
    int ARMS = 3;
    int ROUNDS = 3;
    std::vector<RandomGaussianArm> arms; 
    for(int i = 0; i<ARMS; i++){

        std::cout<<"Arm: "<<i+1<<"\n";
        arms.push_back(RandomGaussianArm(generator));
        for (int j = 0; j<ROUNDS; j++){
            arms[i].getReward();
        }
        std::cout<<std::endl;
    }
    return 0;
}