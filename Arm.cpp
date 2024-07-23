#include "Arm.hpp"

template <typename RewardTemplate>
Arm<RewardTemplate>::Arm(std::mt19937& generator){
    this->generatorptr = &generator;
}

RandomGaussianArm::RandomGaussianArm(std::mt19937& generator):Arm<double>(generator){
    
    std::cout<<"RandomGaussianArm constructor"<<"\t";

    std::uniform_real_distribution<double> uniform(0,1);
    this->mean = uniform(*(this->generatorptr));
    this->stdDev = uniform(*(this->generatorptr));

    std::cout<<"Mean: "<<this->mean<<"\t"<<"StdDev: "<<this->stdDev<<"\n";
};
double RandomGaussianArm::getReward(){
    
    std::cout<<"RandomGaussianArm getReward"<<"\t";

    std::normal_distribution<double> gaussian(this->mean,this->stdDev);
    double reward = gaussian(*(this->generatorptr));

    std::cout<<"reward: "<<reward<<std::endl;

    return reward;
}
