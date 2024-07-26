#include <iostream>
#include <vector>
#include <random>
#include <unordered_map> 

template <typename ArmTemplate, typename RewardTemplate, typename AllocatorTemplate, typename EvaluatorTemplate>
class banditAlgorithm{
protected:
    int ARMS; 
    int BUDGET; 
    std::unordered_map<int,ArmTemplate> arms;
    std::mt19937* generatorptr;
    AllocatorTemplate armSelectorInstance;
    EvaluatorTemplate evaluatorInstance;
public:

    banditAlgorithm(int ARMS, int BUDGET, std::mt19937& generator):
        ARMS(ARMS),
        BUDGET(BUDGET),
        generatorptr(&generator),
        arms(createArms(ARMS, &generator)),
        armSelectorInstance(arms,generator),
        evaluatorInstance(arms)
    {}
    
    virtual std::unordered_map<int,ArmTemplate> createArms(int ARMS, std::mt19937* generatorptr){
        std::unordered_map<int,ArmTemplate> localArms;
        for(int i = 0; i<ARMS; i++){
            localArms.emplace(i,ArmTemplate(*(generatorptr),i));
        }
        std::cout<<"\n";
        return localArms;
    }
    
    virtual void run(){
        std::cout<<"\n";
        for(int i = 0; i< this->BUDGET; i++){
            std::cout<<"ROUND: "<<i<<"\n";
            ArmTemplate selectedArm = this->armSelectorInstance.selectArm();
            RewardTemplate reward = this->armSelectorInstance.playArm(selectedArm);
            std::cout<<"\n";
        }
    }

    virtual ArmTemplate findBestArm(){
        std::unordered_map<int,RewardTemplate> rewardsforArm;
        std::unordered_map<int,int> timesArmPulled;
        for(int i = 0; i<this->BUDGET; i++){
            rewardsforArm[this->armSelectorInstance.pastArms[i].id]+=this->armSelectorInstance.pastRewards[i];
            timesArmPulled[this->armSelectorInstance.pastArms[i].id]+=1;
        }
        std::unordered_map<int,RewardTemplate> averageRewards;
        for(int i = 0; i<this->ARMS; i++){
            averageRewards[i] = timesArmPulled[i]>0? rewardsforArm[i]/timesArmPulled[i]: INT_MIN;
        }
        int bestArmIndex = std::max_element(averageRewards.begin(),averageRewards.end(),[](std::pair<const int,RewardTemplate>& a, std::pair<const int,RewardTemplate>& b){return a.second < b.second;})->first;
        return this->arms.find(bestArmIndex)->second;
    }

    virtual RewardTemplate evaluate(){
        ArmTemplate bestArm = findBestArm();
        std::cout<<"Empirical Best Arm: "<<bestArm.id<<"\n";
        return this->evaluatorInstance.evaluateRegret(bestArm);
    }
};

template <typename ArmTemplate, typename RewardTemplate, typename AllocatorTemplate, typename EvaluatorTemplate>
class SuccessiveRejectsAlgorithm: public banditAlgorithm<ArmTemplate,RewardTemplate,AllocatorTemplate,EvaluatorTemplate>{
    double C;
public: 
    SuccessiveRejectsAlgorithm(int ARMS, int BUDGET, std::mt19937& generator): banditAlgorithm<ArmTemplate,RewardTemplate,AllocatorTemplate,EvaluatorTemplate>(ARMS,BUDGET,generator) {
        double logK = 0.5; 
        for(double i = 2; i<=this->ARMS; i+=1){
            logK+=1/i;
        }
        this->C = (this->BUDGET-this->ARMS)/logK;
        std::cout<<"C: "<<this->C<<"\n";
    }
    ArmTemplate findWorstArm(int endIndex, int phaseLength){
        std::unordered_map<int,RewardTemplate> rewardsforArm;
        std::unordered_map<int,int> timesArmPulled;
        for(int i = endIndex-phaseLength+1; i<=endIndex; i++){
            rewardsforArm[this->armSelectorInstance.pastArms[i].id]+=this->armSelectorInstance.pastRewards[i];
            timesArmPulled[this->armSelectorInstance.pastArms[i].id]+=1;
        }
        std::unordered_map<int,RewardTemplate> averageRewards;
        for(auto p : rewardsforArm){
            int id = p.first;
            averageRewards[id] = timesArmPulled[id]>0? rewardsforArm[id]/timesArmPulled[id]: INT_MIN;
        }
        int worstArmIndex = std::min_element(averageRewards.begin(),averageRewards.end(),[](std::pair<const int,RewardTemplate>& a, std::pair<const int,RewardTemplate>& b){return a.second < b.second;})->first;
        return this->arms.find(worstArmIndex)->second;
    }
    void run(){
        for(int PHASE = 1; PHASE <= this->ARMS -1; PHASE++){
            int length = ceil(this->C/this->arms.size());
            std::cout<<"Phase "<<PHASE<<" with length "<<length<<"\n";
            for(int i = 0; i< length; i++){
                std::cout<<"ROUND: "<<i<<"\n";
                int run_length = ceil(this->C/this->arms.size());
                ArmTemplate selectedArm = this->armSelectorInstance.selectArm();
                RewardTemplate reward = this->armSelectorInstance.playArm(selectedArm);
            }
            std::cout<<"Finding Worst Arm:\n";
            ArmTemplate worstArm = findWorstArm(this->armSelectorInstance.pastRewards.size()-1,length);
            this->arms.erase(worstArm.id);
            std::cout<<"Removed Worst Arm: "<<worstArm.id<<"\n";
            std::cout<<"\n";
        }
    }
    RewardTemplate evaluate(){
        if(this->arms.size()>1){
            ArmTemplate bestArm = this->findBestArm();
            std::cout<<"There are multiple Arms left! Empirical best Arm: "<<bestArm.id<<"\n";
            return this->evaluatorInstance.evaluateRegret(bestArm);
        }
        ArmTemplate bestArm = this->arms.begin()->second;
        std::cout<<"Final Remaining Arm: "<<bestArm.id<<"\n";
        return this->evaluatorInstance.evaluateRegret(bestArm);
    }
};
// has k-1 phases
// each arm is pulled ceil[(n-K/logK)*1/(remaining arms)] times
// sum is less than N anyways
// first phase: p1 = ceil[n-K/KlogK]
// second phase: p2 = ceil[n-K/logK(K-1)] - p1
// ...
// last phase: pK-1 = ceil[n-K/2logK] - p {K-2}