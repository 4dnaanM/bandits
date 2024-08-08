#include <iostream>
#include <vector>
#include <random>
#include <map> 

template <typename ArmTemplate, typename RewardTemplate, typename AllocatorTemplate, typename EvaluatorTemplate>
class fixedBudget{
protected:
    int ARMS; 
    int BUDGET; 
    std::map<int,ArmTemplate> arms;
    std::mt19937* generatorptr;
    AllocatorTemplate armSelectorInstance;
    EvaluatorTemplate evaluatorInstance;
public:

    fixedBudget(int ARMS, int BUDGET, std::mt19937& generator):
        ARMS(ARMS),
        BUDGET(BUDGET),
        generatorptr(&generator),
        arms(createArms(ARMS, &generator)),
        armSelectorInstance(arms,generator),
        evaluatorInstance(arms)
    {}
    
    virtual std::map<int,ArmTemplate> createArms(int ARMS, std::mt19937* generatorptr){
        std::map<int,ArmTemplate> localArms;
        for(int i = 0; i<ARMS; i++){
            std::cout<<i<<" | ";
            localArms.emplace(i,ArmTemplate(*(generatorptr),i));
        }
        std::cout<<"\n";
        return localArms;
    }
    
    virtual void run(){
        std::cout<<"\n";
        for(int i = 0; i< this->BUDGET; i++){
            std::cout<<"ROUND: "<<i<<" | ";
            ArmTemplate selectedArm = this->armSelectorInstance.selectArm();
            RewardTemplate reward = this->armSelectorInstance.playArm(selectedArm);
            selectedArm.printReward(reward);
        }
        std::cout<<"\n";
    }

    virtual ArmTemplate findBestArm(){
        std::map<int,RewardTemplate> averageRewards;
        for(int i = 0; i<this->ARMS; i++){
            averageRewards[i] = this->armSelectorInstance.totalPulls[i]>0? this->armSelectorInstance.totalRewards[i]/this->armSelectorInstance.totalPulls[i]: INT_MIN;
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
class SuccessiveRejectsAlgorithm: public fixedBudget<ArmTemplate,RewardTemplate,AllocatorTemplate,EvaluatorTemplate>{
    double C;
public: 
    SuccessiveRejectsAlgorithm(int ARMS, int BUDGET, std::mt19937& generator): fixedBudget<ArmTemplate,RewardTemplate,AllocatorTemplate,EvaluatorTemplate>(ARMS,BUDGET,generator) {
        double logK = 0.5; 
        for(double i = 2; i<=this->ARMS; i+=1){
            logK+=1/i;
        }
        this->C = (this->BUDGET-this->ARMS)/logK;
    }
    ArmTemplate findWorstArm(){
        std::map<int,RewardTemplate> averageRewards;
        for(auto p : this->arms){
            int id = p.first;
            averageRewards[id] = this->armSelectorInstance.totalPulls[id]>0? this->armSelectorInstance.totalRewards[id]/this->armSelectorInstance.totalPulls[id]: INT_MIN;
        }
        int worstArmIndex = std::min_element(averageRewards.begin(),averageRewards.end(),[](std::pair<const int,RewardTemplate>& a, std::pair<const int,RewardTemplate>& b){return a.second < b.second;})->first;
        return this->arms.find(worstArmIndex)->second;
    }
    void run() override {
        std::cout<<"\n";
        for(int PHASE = 1; PHASE <= this->ARMS -1; PHASE++){
            int length = ceil(this->C/this->arms.size())-((PHASE>1)?ceil(this->C/(this->arms.size()+1)):0);
            std::cout<<"Phase "<<PHASE<<" with length "<<length<<"*"<<this->arms.size()<<" = "<<length*this->arms.size()<<"\n";
            if(length==0) continue; 
            for(auto p: this->arms){
                int id = p.first; 
                ArmTemplate arm = p.second;
                std::cout<<"Playing Arm "<<id<<std::endl;
                for(int i = 0; i< length; i++){
                    std::cout<<"ROUND: "<<i<<" | ";
                    RewardTemplate reward = this->armSelectorInstance.playArm(arm);
                    arm.printReward(reward);
                }
            }
            ArmTemplate worstArm = findWorstArm();
            this->arms.erase(worstArm.id);
            std::cout<<"Removed Worst Arm: "<<worstArm.id<<"\n";
            std::cout<<"\n";
        }
    }
    RewardTemplate evaluate() override {
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

template <typename ArmTemplate, typename RewardTemplate, typename AllocatorTemplate, typename EvaluatorTemplate>
class UCB_2: public fixedBudget<ArmTemplate,RewardTemplate,AllocatorTemplate,EvaluatorTemplate>{
    double a;
public: 
    UCB_2(int ARMS, int BUDGET, std::mt19937& generator, double a): fixedBudget<ArmTemplate,RewardTemplate,AllocatorTemplate,EvaluatorTemplate>(ARMS,BUDGET,generator) {
        this->a = a;
    }
    ArmTemplate getNextArm(){
        std::map<int,RewardTemplate> comparator;
        for(int i = 0; i<this->ARMS; i++){
            comparator[i] = this->armSelectorInstance.totalPulls[i]>0? (this->armSelectorInstance.totalRewards[i]/this->armSelectorInstance.totalPulls[i]+sqrt(a/this->armSelectorInstance.totalPulls[i])): INT_MAX;
        }
        int bestArmIndex = std::max_element(comparator.begin(),comparator.end(),[](std::pair<const int,RewardTemplate>& a, std::pair<const int,RewardTemplate>& b){return a.second < b.second;})->first;
        std::cout<<"Selected Arm: "<<bestArmIndex<<" | ";
        return this->arms.find(bestArmIndex)->second;
    }
    void run() override {
        std::cout<<"\n";
        for(int i = 0; i< this->BUDGET; i++){
            std::cout<<"ROUND: "<<i<<" | ";
            ArmTemplate selectedArm = getNextArm();
            RewardTemplate reward = this->armSelectorInstance.playArm(selectedArm);
            selectedArm.printReward(reward);
        }
        std::cout<<"\n";
    }
};

template <typename ArmTemplate, typename RewardTemplate, typename AllocatorTemplate, typename EvaluatorTemplate>
class fixedConfidence{
protected:
    int ARMS; 
    double delta;
    std::map<int,ArmTemplate> arms;
    std::mt19937* generatorptr;
    AllocatorTemplate armSelectorInstance;
    EvaluatorTemplate evaluatorInstance;
    int totalPulls; 
public:

    fixedConfidence(int ARMS, double delta, std::mt19937& generator):
        ARMS(ARMS),
        delta(delta),
        generatorptr(&generator),
        arms(createArms(ARMS, &generator)),
        armSelectorInstance(arms,generator),
        evaluatorInstance(arms)
    {}
    
    virtual std::map<int,ArmTemplate> createArms(int ARMS, std::mt19937* generatorptr){
        std::map<int,ArmTemplate> localArms;
        for(int i = 0; i<ARMS; i++){
            std::cout<<i<<" | ";
            localArms.emplace(i,ArmTemplate(*(generatorptr),i));
        }
        std::cout<<"\n";
        return localArms;
    }
    
    virtual void run() = 0;

    virtual ArmTemplate findBestArm(){
        std::map<int,RewardTemplate> averageRewards;
        for(int i = 0; i<this->ARMS; i++){
            averageRewards[i] = this->armSelectorInstance.totalPulls[i]>0? this->armSelectorInstance.totalRewards[i]/this->armSelectorInstance.totalPulls[i]: INT_MIN;
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
class MedianElimination: public fixedConfidence<ArmTemplate,RewardTemplate,AllocatorTemplate,EvaluatorTemplate>{
    RewardTemplate epsilon; 
public: 
    MedianElimination(int ARMS, std::mt19937& generator, RewardTemplate epsilon, double delta): fixedConfidence<ArmTemplate,RewardTemplate,AllocatorTemplate,EvaluatorTemplate>(ARMS,delta,generator) {
        this->epsilon = epsilon;
    }
    void sampleArms(int length){
        for(auto p: this->arms){
            int id = p.first; 
            ArmTemplate arm = p.second;
            // std::cout<<"Playing Arm "<<id<<std::endl;
            for(int i = 0; i< length; i++){
                // std::cout<<"ROUND: "<<i<<" | ";
                RewardTemplate reward = this->armSelectorInstance.playArm(arm);
            }
        }
    }
    RewardTemplate getMedian(){
        std::vector<RewardTemplate> averageRewards;
        for(auto p : this->arms){
            int id = p.first;
            averageRewards.push_back(this->armSelectorInstance.totalPulls[id]>0? this->armSelectorInstance.totalRewards[id]/this->armSelectorInstance.totalPulls[id]: INT_MIN);
        }
        std::sort(averageRewards.begin(),averageRewards.end());
        RewardTemplate median = averageRewards[averageRewards.size()/2];
        if(averageRewards.size()%2==0){
            median = (median+averageRewards[averageRewards.size()/2-1])/2;
        }
        return median;
    }
    void removeArms(RewardTemplate median){
        std::vector<int> toRemove; 
        for(auto p: this->arms){
            int id = p.first;
            RewardTemplate averageReward = this->armSelectorInstance.totalPulls[id]>0? this->armSelectorInstance.totalRewards[id]/this->armSelectorInstance.totalPulls[id]: INT_MAX;
            std::cout<<"Arm "<<id<<" has average reward "<<averageReward<<"\n";
            if(averageReward < median){
                toRemove.push_back(id);
            }
        }
        std::cout<<"Removing Arms: ";
        for(int id: toRemove){
            std::cout<<id<<" ";
            this->arms.erase(id);
        }
        std::cout<<"\n";
    }        
    void run() override {
        RewardTemplate e = this->epsilon/4; 
        double d = this->delta/2;
        
        while(this->arms.size()>1){
            int length = ceil(4*log(3/d)/(e*e));
            std::cout<<"Phase with length "<<length<<"*"<<this->arms.size()<<" = "<<length*this->arms.size()<<"\n";
            this->totalPulls+=length*this->arms.size();
            
            this->armSelectorInstance.totalRewards.clear();
            this->armSelectorInstance.totalPulls.clear();
            if(length==0)break; 
            sampleArms(length);
            RewardTemplate median = getMedian();
            removeArms(median);
            e = 3*e/4; 
            d = d/2;
            std::cout<<"\n";
        }
        std::cout<<"Total Pulls: "<<this->totalPulls<<"\n";
    }
    RewardTemplate evaluate() override {
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

template <typename ArmTemplate, typename RewardTemplate, typename AllocatorTemplate, typename EvaluatorTemplate>
class ExponentialGapElimination: public fixedConfidence<ArmTemplate,RewardTemplate,AllocatorTemplate,EvaluatorTemplate>{
    int medianPulls; 
public: 
    ExponentialGapElimination(int ARMS, std::mt19937& generator, double delta)
        :fixedConfidence<ArmTemplate,RewardTemplate,AllocatorTemplate,EvaluatorTemplate>(ARMS,delta,generator) {
            this->medianPulls = 0;
        }
    void sampleArms(int length){
        for(auto p: this->arms){
            int id = p.first; 
            ArmTemplate arm = p.second;
            for(int i = 0; i< length; i++){
                RewardTemplate reward = this->armSelectorInstance.playArm(arm);
            }
        }
    }
    void removeArms(RewardTemplate threshold){
        std::vector<int> toRemove; 
        for(auto p: this->arms){
            int id = p.first;
            RewardTemplate averageReward = this->armSelectorInstance.totalPulls[id]>0? this->armSelectorInstance.totalRewards[id]/this->armSelectorInstance.totalPulls[id]: INT_MAX;
            std::cout<<"Arm "<<id<<" has average reward "<<averageReward<<"\n";
            if(averageReward < threshold){
                toRemove.push_back(id);
            }
        }
        std::cout<<"Removing Arms: ";
        for(int id: toRemove){
            std::cout<<id<<" ";
            this->arms.erase(id);
        }
        std::cout<<"\n";
    }        
    int medianElimination(RewardTemplate epsilon, double delta){
        std::map<int,ArmTemplate> arms = this->arms; 
        RewardTemplate e = epsilon/4; 
        double d = delta/2;
        
        while(arms.size()>1){
            int length = ceil(4*log(3/d)/(e*e));
            this->totalPulls+=length*arms.size();
            this->medianPulls+=length*arms.size();
            if(length==0)break;
            std::map<int,RewardTemplate> armRewards;
            std::map<int,int> armPulls; 
            for(auto p: arms){for(int i = 0; i< length; i++){armRewards[p.first]+=p.second.getReward();armPulls[p.first]+=1;}}
            std::vector<RewardTemplate> averageRewards;
            for(auto p : arms){
                int id = p.first;
                averageRewards.push_back(armPulls[id]>0? armRewards[id]/armPulls[id]: INT_MIN);
            }
            std::sort(averageRewards.begin(),averageRewards.end());
            RewardTemplate median = averageRewards[averageRewards.size()/2];
            if(averageRewards.size()%2==0){median = (median+averageRewards[averageRewards.size()/2-1])/2;}
            std::vector<int> toRemove; 
            for(auto p: arms){
                int id = p.first;
                RewardTemplate averageReward = armPulls[id]>0? armRewards[id]/armPulls[id]: INT_MAX;
                if(averageReward < median){toRemove.push_back(id);}
            }
            for(int id: toRemove){arms.erase(id);}
            e = 3*e/4; 
            d = d/2;
        }
        return arms.begin()->first;
    }
    void run() override {
        int round = 1;
        while(this->arms.size()>1){
            RewardTemplate e = pow(2,-1*round)/4; 
            double d = this->delta/(50*round*round*round);
            int length = ceil(2*log(2/d)/(e*e));
            this->totalPulls+=length*this->arms.size();
            this->armSelectorInstance.totalRewards.clear();
            this->armSelectorInstance.totalPulls.clear();
            if(length==0)break; 
            std::cout<<"Sampling for "<<length<<"*"<<this->arms.size()<<" = "<<length*this->arms.size()<<" rounds each \n";
            sampleArms(length);
            int optArm = medianElimination(e/2,d);
            RewardTemplate optReward = this->armSelectorInstance.totalPulls[optArm]>0? this->armSelectorInstance.totalRewards[optArm]/this->armSelectorInstance.totalPulls[optArm]: INT_MAX;
            std::cout<<"Optimal Arm: "<<optArm<<" with Average Reward: "<<optReward<<"\n";
            std::cout<<"Threshold: "<<optReward<<"-"<<e<<" = "<<optReward-e<<"\n";
            removeArms(optReward-e);
            round+=1; 
            std::cout<<"\n";
        }
        std::cout<<"Total Pulls: "<<this->totalPulls<<" | ";
        std::cout<<"MedianElimination Pulls: "<<this->medianPulls<<" | ";
        std::cout<<"EGElimination Pulls: "<<this->totalPulls-this->medianPulls<<"\n";
    }
    RewardTemplate evaluate() override {
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