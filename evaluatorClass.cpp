#include <iostream>
#include <map> 
#include <vector>

template <typename ArmTemplate,typename RewardTemplate>
class Evaluator{
protected: 
    std::map<int,ArmTemplate>* armsptr;
public:    
    Evaluator(std::map<int,ArmTemplate>& arms){
        this->armsptr = &arms;
    }
    virtual RewardTemplate evaluateRegret(ArmTemplate& selectedArm) = 0;
};

template <typename ArmTemplate, typename RewardTemplate>
class DeltaEvaluator : public Evaluator<ArmTemplate,RewardTemplate>{
    ArmTemplate optimalArm; 
public:
    DeltaEvaluator(std::map<int,ArmTemplate>& arms):
        Evaluator<ArmTemplate,RewardTemplate>(arms),
        optimalArm(std::max_element((*(this->armsptr)).begin(),(*(this->armsptr)).end(),[](std::pair<const int,ArmTemplate>& a, std::pair<const int,ArmTemplate>& b){return a.second.getMean() < b.second.getMean();})->second){
        std::cout<<"Optimal Arm is Arm "<<optimalArm.id<<" with Mean: "<<this->optimalArm.getMean()<<"\n";
    };
    
    RewardTemplate evaluateRegret(ArmTemplate& selectedArm) override {
        RewardTemplate regret = this->optimalArm.getMean() - selectedArm.getMean();
        std::cout<<"DeltaEvaluator regret: "<<regret<<"\n";
        return regret;
    }
};