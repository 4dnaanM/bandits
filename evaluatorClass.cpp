#include <iostream>
#include <map> 
#include <vector>

template <typename RewardTemplate = double, typename ArmTemplate = BernoulliArm<RewardTemplate>>
class Evaluator{
protected: 
    std::map<int,ArmTemplate>* armsptr;
public:    
    Evaluator(std::map<int,ArmTemplate>& arms){
        this->armsptr = &arms;
    }
    virtual RewardTemplate evaluateRegret(ArmTemplate& selectedArm) = 0;
};

template <typename RewardTemplate = double, typename ArmTemplate = BernoulliArm<RewardTemplate>>
class DeltaEvaluator : public Evaluator<RewardTemplate, ArmTemplate>{
public:
    ArmTemplate optimalArm; 
    RewardTemplate H1;
    DeltaEvaluator(std::map<int,ArmTemplate>& arms):
        Evaluator<RewardTemplate, ArmTemplate>(arms),
        optimalArm(std::max_element((*(this->armsptr)).begin(),(*(this->armsptr)).end(),[](std::pair<const int,ArmTemplate>& a, std::pair<const int,ArmTemplate>& b){return a.second.getMean() < b.second.getMean();})->second){
        std::cout<<"Optimal Arm is Arm "<<optimalArm.id<<" with Mean: "<<this->optimalArm.getMean()<<"\n";
        this->H1 = 0; 
        for(std::pair<int,ArmTemplate> p: arms){
            ArmTemplate arm = p.second;
            if(arm.getMean()==optimalArm.getMean()) continue;
            this->H1+=1/((optimalArm.getMean()-arm.getMean())*(optimalArm.getMean()-arm.getMean()));
        }
        std::cout<<"DeltaEvaluator H1: "<<this->H1<<"\n";
    };
    
    RewardTemplate evaluateRegret(ArmTemplate& selectedArm) override {
        RewardTemplate regret = this->optimalArm.getMean() - selectedArm.getMean();
        std::cout<<"DeltaEvaluator regret: "<<regret<<"\n";
        return regret;
    }
};