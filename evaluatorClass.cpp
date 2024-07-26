#include <iostream>
#include <unordered_map> 
#include <vector>

template <typename ArmTemplate>
class OptimalArmComp{
public:
    bool operator()(std::pair<const int,ArmTemplate>& a, std::pair<const int,ArmTemplate>& b){
        return a.second.getMean() < b.second.getMean();
    }
};

template <typename ArmTemplate,typename RewardTemplate>
class Evaluator{
protected: 
    std::unordered_map<int,ArmTemplate>* armsptr;
public:    
    Evaluator(std::unordered_map<int,ArmTemplate>& arms){
        this->armsptr = &arms;
    }
    virtual RewardTemplate evaluateRegret(ArmTemplate& selectedArm) = 0;
};

template <typename ArmTemplate, typename RewardTemplate>
class DeltaEvaluator : public Evaluator<ArmTemplate,RewardTemplate>{
    ArmTemplate optimalArm; 
public:
    DeltaEvaluator(std::unordered_map<int,ArmTemplate>& arms):
        Evaluator<ArmTemplate,RewardTemplate>(arms),
        optimalArm(std::max_element((*(this->armsptr)).begin(),(*(this->armsptr)).end(),OptimalArmComp<ArmTemplate>())->second){
        std::cout<<"DeltaEvaluator constructor"<<"\t";
        std::cout<<"Optimal Arm is Arm "<<optimalArm.id<<" with Mean: "<<this->optimalArm.getMean()<<"\n";
    };
    
    RewardTemplate evaluateRegret(ArmTemplate& selectedArm) override {
        std::cout<<"DeltaEvaluator evaluateRegret"<<"\t";
        RewardTemplate regret = this->optimalArm.getMean() - selectedArm.getMean();
        std::cout<<"Regret: "<<regret<<"\n";
        return regret;
    }
};