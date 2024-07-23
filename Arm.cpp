#include <random>
#include <iostream>

template <typename Reward>
class Arm{
protected: 
    std::mt19937 generator;
public: 
    Arm(std::mt19937& generator){
        this->generator = generator;
    }
    virtual Reward getReward() = 0;
};

class RandomGaussianArm: public Arm<double>{
    double Mean;
    double StdDev;
public:
    RandomGaussianArm(std::mt19937& generator):Arm<double>(generator){
        std::cout<<"RandomGaussianArm constructor"<<"\t";
        std::uniform_real_distribution<double> uniformGen(0,1);
        this->Mean = uniformGen(this->generator);
        this->StdDev = uniformGen(this->generator);
        
        std::cout<<"Mean: "<<this->Mean<<"\t";
        std::cout<<"StdDev: "<<this->StdDev<<"\n";
    };
    double getReward() override {
        std::cout<<"RandomGaussianArm getReward"<<"\t";
        std::normal_distribution<double> gaussian(this->Mean,this->StdDev);
        double reward = gaussian(this->generator);
        std::cout<<"reward: "<<reward<<std::endl;
        return reward;
    }
};

int main(){
    auto seed = static_cast<unsigned long>(std::time(0));
    std::mt19937 generator(seed);
    int ARMS = 3;
    int ROUNDS = 3;
    std::vector<RandomGaussianArm> arms; 
    for(int i = 0; i<ARMS; i++){
        std::cout<<"Arm: "<<i<<"\n";
        arms.push_back(RandomGaussianArm(generator));
        for (int j = 0; j<ROUNDS; j++){
            arms[i].getReward();
        }
        std::cout<<std::endl;
    }
    return 0;
}