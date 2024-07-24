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
    virtual RewardTemplate evaluateRegret(std::vector<ArmTemplate>& arms, ArmTemplate& selectedArm) = 0;
};

template <typename ArmTemplate, typename RewardTemplate>
class DeltaEvaluator : public Evaluator<ArmTemplate,RewardTemplate>{
    ArmTemplate optimalArm; 
public:
    DeltaEvaluator(std::vector<ArmTemplate>& arms):
        Evaluator<ArmTemplate,RewardTemplate>(arms),
        optimalArm(*std::max_element(arms.begin(),arms.end(),[](ArmTemplate& a, ArmTemplate& b){return a.getMean() < b.getMean();})){
        std::cout<<"DeltaEvaluator constructor"<<"\n";
    };
    
    RewardTemplate evaluateRegret(std::vector<ArmTemplate>& arms, ArmTemplate& selectedArm) override {
        std::cout<<"DeltaEvaluator evaluateRegret"<<"\t";
        RewardTemplate regret = this->optimalArm.getMean() - selectedArm.getMean();
        return regret;
    }
};