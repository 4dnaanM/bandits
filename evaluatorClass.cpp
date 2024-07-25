#include <iostream>
#include <vector>

template <typename ArmTemplate,typename RewardTemplate>
class Evaluator{
protected: 
    std::vector<ArmTemplate> arms;
public:    
    Evaluator(std::vector<ArmTemplate>& arms){
        this->arms = arms;
    }
    virtual RewardTemplate evaluateRegret(ArmTemplate& selectedArm) = 0;
};

template <typename ArmTemplate, typename RewardTemplate>
class DeltaEvaluator : public Evaluator<ArmTemplate,RewardTemplate>{
    ArmTemplate optimalArm; 
public:
    DeltaEvaluator(std::vector<ArmTemplate>& arms):
        Evaluator<ArmTemplate,RewardTemplate>(arms),
        optimalArm(*std::max_element(this->arms.begin(),this->arms.end(),[](ArmTemplate& a, ArmTemplate& b){return a.getMean() < b.getMean();})){
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