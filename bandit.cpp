#include <iostream>
#include <vector>
#include <random>
#include <map> 
#include "Arm.cpp"
#include "armSelectorClass.cpp"
#include "evaluatorClass.cpp"
#include "metrics.cpp"

template <typename RewardTemplate = double>
class BanditAlgorithm{
public:
    virtual int run() = 0;
    virtual void refresh() = 0;
    virtual Metrics<RewardTemplate> evaluate() = 0;
    virtual ~BanditAlgorithm() = default;
};

template <typename RewardTemplate = double, typename ArmTemplate = BernoulliArm<RewardTemplate>, typename AllocatorTemplate = UniformAllocator<RewardTemplate,ArmTemplate>, typename EvaluatorTemplate = DeltaEvaluator<RewardTemplate,ArmTemplate>>
class fixedBudget: public BanditAlgorithm<RewardTemplate>{
protected:
    int ARMS; 
    int BUDGET; 
    std::map<int,ArmTemplate> arms;
    std::mt19937* generatorptr;
    AllocatorTemplate armSelectorInstance;
    EvaluatorTemplate evaluatorInstance;
    int totalPulls; 
    bool silent;
public:

    fixedBudget(int ARMS, int BUDGET, std::mt19937& generator, bool silent = false):
        ARMS(ARMS),
        BUDGET(BUDGET),
        generatorptr(&generator),
        arms(createArms(ARMS, &generator,silent)),
        armSelectorInstance(arms,generator,silent),
        evaluatorInstance(arms),
        silent(silent){
            this->totalPulls = 0;
        }
    
    void refresh() override {
        this->arms = createArms(this->ARMS,this->generatorptr,this->silent);
        this->armSelectorInstance = AllocatorTemplate(this->arms,*(this->generatorptr),this->silent);
        this->evaluatorInstance = EvaluatorTemplate(this->arms);
        this->totalPulls = 0; 
    }

    virtual std::map<int,ArmTemplate> createArms(int ARMS, std::mt19937* generatorptr,bool silent = false){
        std::map<int,ArmTemplate> localArms;
        for(int i = 0; i<ARMS; i++){
            if(!silent){std::cout<<i<<" | ";}
            localArms.emplace(i,ArmTemplate(*(generatorptr),i,silent));
        }
        if(!silent){std::cout<<"\n";}
        return localArms;
    }
    
    virtual int run() override {
        if(!this->silent)std::cout<<"\n";
        for(int i = 0; i< this->BUDGET; i++){
            if(!this->silent)std::cout<<"ROUND: "<<i<<" | ";
            this->totalPulls+=1; 
            ArmTemplate selectedArm = this->armSelectorInstance.selectArm();
            RewardTemplate reward = this->armSelectorInstance.playArm(selectedArm);
            if(!this->silent)selectedArm.printReward(reward);
        }
        if(!this->silent)std::cout<<"\n";
        return 0;
    }

    virtual ArmTemplate findBestArm(){
        std::map<int,RewardTemplate> averageRewards;
        for(int i = 0; i<this->ARMS; i++){
            averageRewards[i] = this->armSelectorInstance.totalPulls[i]>0? this->armSelectorInstance.totalRewards[i]/this->armSelectorInstance.totalPulls[i]: INT_MIN;
        }
        int bestArmIndex = std::max_element(averageRewards.begin(),averageRewards.end(),[](std::pair<const int,RewardTemplate>& a, std::pair<const int,RewardTemplate>& b){return a.second < b.second;})->first;
        return this->arms.find(bestArmIndex)->second;
    }

    virtual Metrics<RewardTemplate> evaluate() override {
        ArmTemplate bestArm = findBestArm();
        if(!this->silent)std::cout<<"Empirical Best Arm: "<<bestArm.id<<"\n";
        return Metrics<RewardTemplate>(
            "Fixed Budget",
            std::unordered_map<std::string,RewardTemplate>({
                {"Arms",this->ARMS},
                {"Budget",this->BUDGET}
            }),
            std::unordered_map<std::string,RewardTemplate>({
                {"Optimal Mean",this->evaluatorInstance.optimalArm.getMean()},
                {"Regret",this->evaluatorInstance.evaluateRegret(bestArm)},
                {"H1",this->evaluatorInstance.H1}
            }),
            this->totalPulls
        );
    }
};

template <typename RewardTemplate = double, typename ArmTemplate = BernoulliArm<RewardTemplate>, typename AllocatorTemplate = UniformAllocator<RewardTemplate,ArmTemplate>, typename EvaluatorTemplate = DeltaEvaluator<RewardTemplate,ArmTemplate>>
class SuccessiveRejectsAlgorithm: public fixedBudget<RewardTemplate,ArmTemplate,AllocatorTemplate,EvaluatorTemplate>{
    double C;
public: 
    SuccessiveRejectsAlgorithm(int ARMS, int BUDGET, std::mt19937& generator, bool silent=false): fixedBudget<RewardTemplate,ArmTemplate,AllocatorTemplate,EvaluatorTemplate>(ARMS,BUDGET,generator,silent) {
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
    int run() override {
        if(!this->silent)std::cout<<"\n";
        for(int PHASE = 1; PHASE <= this->ARMS -1; PHASE++){
            int length = ceil(this->C/this->arms.size())-((PHASE>1)?ceil(this->C/(this->arms.size()+1)):0);
            this->totalPulls+=length*this->arms.size();
            if(!this->silent)std::cout<<"Phase "<<PHASE<<" with length "<<length<<"*"<<this->arms.size()<<" = "<<length*this->arms.size()<<"\n";
            if(length==0){
                std::cout<<"Not Enough Budget to continue!\n";    
                continue;
            }
            for(auto p: this->arms){
                int id = p.first; 
                ArmTemplate arm = p.second;
                if(!this->silent)std::cout<<"Playing Arm "<<id<<std::endl;
                for(int i = 0; i< length; i++){
                    if(!this->silent)std::cout<<"ROUND: "<<i<<" | ";
                    RewardTemplate reward = this->armSelectorInstance.playArm(arm);
                    if(!this->silent)arm.printReward(reward);
                }
            }
            ArmTemplate worstArm = findWorstArm();
            this->arms.erase(worstArm.id);
            if(!this->silent)std::cout<<"Removed Worst Arm: "<<worstArm.id<<"\n";
            if(!this->silent)std::cout<<"\n";
        }
        return 0;
    }
    Metrics<RewardTemplate> evaluate() override {
        if(this->arms.size()>1){
            ArmTemplate bestArm = this->findBestArm();
            if(!this->silent)std::cout<<"There are multiple Arms left! Empirical best Arm: "<<bestArm.id<<"\n";
            return Metrics<RewardTemplate>(
                "Successive Rejects",
                std::unordered_map<std::string,RewardTemplate>({
                    {"Arms",this->ARMS},
                    {"Budget",this->BUDGET}
                }),
                std::unordered_map<std::string,RewardTemplate>({
                    {"Optimal Mean",this->evaluatorInstance.optimalArm.getMean()},
                    {"Regret",this->evaluatorInstance.evaluateRegret(bestArm)},
                    {"H1",this->evaluatorInstance.H1}
                }),
            this->totalPulls
            );
        }
        else{
            ArmTemplate bestArm = this->arms.begin()->second;
            if(!this->silent)std::cout<<"Final Remaining Arm: "<<bestArm.id<<"\n";
            return Metrics<RewardTemplate>(
                "Successive Rejects",
                std::unordered_map<std::string,RewardTemplate>({
                    {"Arms",this->ARMS},
                    {"Budget",this->BUDGET}
                }),
                std::unordered_map<std::string,RewardTemplate>({
                    {"Optimal Mean",this->evaluatorInstance.optimalArm.getMean()},
                    {"Regret",this->evaluatorInstance.evaluateRegret(bestArm)},
                    {"H1",this->evaluatorInstance.H1},
                }),
                this->totalPulls
            );
        }
    }
};

template <typename RewardTemplate = double, typename ArmTemplate = BernoulliArm<RewardTemplate>, typename AllocatorTemplate = UniformAllocator<RewardTemplate,ArmTemplate>, typename EvaluatorTemplate = DeltaEvaluator<RewardTemplate,ArmTemplate>>
class UCB_2: public fixedBudget<RewardTemplate,ArmTemplate,AllocatorTemplate,EvaluatorTemplate>{
    double a;
public: 
    UCB_2(int ARMS, int BUDGET, std::mt19937& generator, double a, bool silent = false): fixedBudget<RewardTemplate,ArmTemplate,AllocatorTemplate,EvaluatorTemplate>(ARMS,BUDGET,generator,silent) {
        this->a = a;
    }
    ArmTemplate getNextArm(){
        std::map<int,RewardTemplate> comparator;
        for(int i = 0; i<this->ARMS; i++){
            comparator[i] = this->armSelectorInstance.totalPulls[i]>0? (this->armSelectorInstance.totalRewards[i]/this->armSelectorInstance.totalPulls[i]+sqrt(a/this->armSelectorInstance.totalPulls[i])): INT_MAX;
        }
        int bestArmIndex = std::max_element(comparator.begin(),comparator.end(),[](std::pair<const int,RewardTemplate>& a, std::pair<const int,RewardTemplate>& b){return a.second < b.second;})->first;
        if(!this->silent)std::cout<<"Selected Arm: "<<bestArmIndex<<" | ";
        return this->arms.find(bestArmIndex)->second;
    }
    int run() override {
        if(!this->silent)std::cout<<"\n";
        for(int i = 0; i< this->BUDGET; i++){
            if(!this->silent)std::cout<<"ROUND: "<<i<<" | ";
            ArmTemplate selectedArm = getNextArm();
            RewardTemplate reward = this->armSelectorInstance.playArm(selectedArm);
            if(!this->silent)selectedArm.printReward(reward);
        }
        if(!this->silent)std::cout<<"\n";
        return 0; 
    }
    Metrics<RewardTemplate> evaluate() override {
        ArmTemplate bestArm = this->findBestArm();
        if(!this->silent)std::cout<<"Empirical Best Arm: "<<bestArm.id<<"\n";
        return Metrics<RewardTemplate>(
            "Upper Confidence Bound 2",
            std::unordered_map<std::string,RewardTemplate>({
                {"Arms",this->ARMS},
                {"a",this->a}
            }),
            std::unordered_map<std::string,RewardTemplate>({
                {"Optimal Mean",this->evaluatorInstance.optimalArm.getMean()},
                {"Regret",this->evaluatorInstance.evaluateRegret(bestArm)},
                {"H1",this->evaluatorInstance.H1}
            }),
            this->totalPulls
        );
    }
};

template <typename RewardTemplate = double, typename ArmTemplate = BernoulliArm<RewardTemplate>, typename AllocatorTemplate = UniformAllocator<RewardTemplate,ArmTemplate>, typename EvaluatorTemplate = DeltaEvaluator<RewardTemplate,ArmTemplate>>
class fixedConfidence: public BanditAlgorithm<RewardTemplate>{
protected:
    int ARMS; 
    double delta;
    std::map<int,ArmTemplate> arms;
    std::mt19937* generatorptr;
    AllocatorTemplate armSelectorInstance;
    EvaluatorTemplate evaluatorInstance;
    int totalPulls; 
    bool silent; 
public:

    fixedConfidence(int ARMS, double delta, std::mt19937& generator,bool silent = false):
        ARMS(ARMS),
        delta(delta),
        generatorptr(&generator),
        arms(createArms(ARMS, &generator,silent)),
        armSelectorInstance(arms,generator,silent),
        evaluatorInstance(arms),
        silent(silent)
    {}

    void refresh() override {
        this->arms = createArms(this->ARMS,this->generatorptr,this->silent);
        this->armSelectorInstance = AllocatorTemplate(this->arms,*(this->generatorptr));
        this->evaluatorInstance = EvaluatorTemplate(this->arms);
        this->totalPulls = 0;
    }
    
    virtual std::map<int,ArmTemplate> createArms(int ARMS, std::mt19937* generatorptr,bool silent = false){
        std::map<int,ArmTemplate> localArms;
        for(int i = 0; i<ARMS; i++){
            if(!silent){std::cout<<i<<" | ";}
            localArms.emplace(i,ArmTemplate(*(generatorptr),i,silent));
        }
        if(!silent){std::cout<<"\n";}
        return localArms;
    }
    
    virtual int run() override = 0;

    virtual ArmTemplate findBestArm(){
        std::map<int,RewardTemplate> averageRewards;
        for(int i = 0; i<this->ARMS; i++){
            averageRewards[i] = this->armSelectorInstance.totalPulls[i]>0? this->armSelectorInstance.totalRewards[i]/this->armSelectorInstance.totalPulls[i]: INT_MIN;
        }
        int bestArmIndex = std::max_element(averageRewards.begin(),averageRewards.end(),[](std::pair<const int,RewardTemplate>& a, std::pair<const int,RewardTemplate>& b){return a.second < b.second;})->first;
        return this->arms.find(bestArmIndex)->second;
    }

    virtual Metrics<RewardTemplate> evaluate() override {
        ArmTemplate bestArm = findBestArm();
        if(!this->silent)std::cout<<"Empirical Best Arm: "<<bestArm.id<<"\n";
        return Metrics<RewardTemplate>(
            "Fixed Confidence",
            std::unordered_map<std::string,RewardTemplate>({
                {"Arms",this->ARMS},
                {"Confidence",this->delta}
            }),
            std::unordered_map<std::string,RewardTemplate>({
                {"Optimal Mean",this->evaluatorInstance.optimalArm.getMean()},
                {"Regret",this->evaluatorInstance.evaluateRegret(bestArm)},
                {"H1",this->evaluatorInstance.H1}
            }),
            this->totalPulls
        );
    }
};

template <typename RewardTemplate = double, typename ArmTemplate = BernoulliArm<RewardTemplate>, typename AllocatorTemplate = UniformAllocator<RewardTemplate,ArmTemplate>, typename EvaluatorTemplate = DeltaEvaluator<RewardTemplate,ArmTemplate>>
class MedianElimination: public fixedConfidence<RewardTemplate,ArmTemplate,AllocatorTemplate,EvaluatorTemplate>{
    RewardTemplate epsilon; 
public: 
    MedianElimination(int ARMS, std::mt19937& generator, RewardTemplate epsilon, double delta, bool silent = false): fixedConfidence<RewardTemplate,ArmTemplate,AllocatorTemplate,EvaluatorTemplate>(ARMS,delta,generator,silent) {
        this->epsilon = epsilon;
    }
    void sampleArms(int length){
        for(auto p: this->arms){
            for(int i = 0; i< length; i++){
                RewardTemplate reward = this->armSelectorInstance.playArm(p.second);
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
            if(!this->silent)std::cout<<"Arm "<<id<<" has average reward "<<averageReward<<"\n";
            if(averageReward < median){
                toRemove.push_back(id);
            }
        }
        if(!this->silent)std::cout<<"Removing Arms: ";
        for(int id: toRemove){
            if(!this->silent)std::cout<<id<<" ";
            this->arms.erase(id);
        }
        if(!this->silent)std::cout<<"\n";
    }        
    int run() override {
        RewardTemplate e = this->epsilon/4; 
        double d = this->delta/2;
        
        while(this->arms.size()>1){
            if(!this->silent)std::cout<<"e: "<<e<<" | d: "<<d<<"\n";
            int length = ceil(4/(e*e*log(3/d)));
            // int length = ceil(4*log(3/d)/(e*e));
            if(!this->silent)std::cout<<"Phase with length "<<length<<"*"<<this->arms.size()<<" = "<<length*this->arms.size()<<"\n";
            this->totalPulls+=length*this->arms.size();
            
            this->armSelectorInstance.totalRewards.clear();
            this->armSelectorInstance.totalPulls.clear();
            if(length==0)break; 
            sampleArms(length);
            RewardTemplate median = getMedian();
            removeArms(median);
            e = 3*e/4; 
            d = d/2;
            if(!this->silent)std::cout<<"\n";
        }
        std::cout<<"Total Pulls: "<<this->totalPulls<<"\n";
        return 0;
    }
    Metrics<RewardTemplate> evaluate() override {
        ArmTemplate bestArm = this->findBestArm();
        if(this->arms.size()>1){
            if(!this->silent)std::cout<<"There are multiple Arms left! Empirical best Arm: "<<bestArm.id<<"\n";
        }
        else{
            if(!this->silent)std::cout<<"Final Remaining Arm: "<<bestArm.id<<"\n";
        }
        return Metrics<RewardTemplate>(
            "Median Elimination",
            std::unordered_map<std::string,RewardTemplate>({
                {"Arms",this->ARMS},
                {"Confidence",this->delta},
                {"Approximation",this->epsilon}
            }),
            std::unordered_map<std::string,RewardTemplate>({
                {"Optimal Mean",this->evaluatorInstance.optimalArm.getMean()},
                {"Regret",this->evaluatorInstance.evaluateRegret(bestArm)},
                {"H1",this->evaluatorInstance.H1}
            }),
            this->totalPulls
        );
    }
};

template <typename RewardTemplate = double, typename ArmTemplate = BernoulliArm<RewardTemplate>, typename AllocatorTemplate = UniformAllocator<RewardTemplate,ArmTemplate>, typename EvaluatorTemplate = DeltaEvaluator<RewardTemplate,ArmTemplate>>
class ExponentialGapElimination: public fixedConfidence<RewardTemplate,ArmTemplate,AllocatorTemplate,EvaluatorTemplate>{
    int medianPulls; 
public: 
    ExponentialGapElimination(int ARMS, std::mt19937& generator, double delta, bool silent = false)
        :fixedConfidence<RewardTemplate,ArmTemplate,AllocatorTemplate,EvaluatorTemplate>(ARMS,delta,generator,silent) {
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
            if(!this->silent)std::cout<<"Arm "<<id<<" has average reward "<<averageReward<<"\n";
            if(averageReward < threshold){
                toRemove.push_back(id);
            }
        }
        if(!this->silent)std::cout<<"Removing Arms: ";
        for(int id: toRemove){
            if(!this->silent)std::cout<<id<<" ";
            this->arms.erase(id);
        }
        if(!this->silent)std::cout<<"\n";
    }        
    int medianElimination(RewardTemplate epsilon, double delta){
        std::map<int,ArmTemplate> arms = this->arms; 
        RewardTemplate e = epsilon/4; 
        double d = delta/2;
        
        while(arms.size()>1){
            int length = ceil(4/(e*e*log(3/d)));
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
    int run() override {
        if(this->evaluatorInstance.H1>10000){
            std::cout<<"H1 is too large for Exponential Gap Elimination! Returning\n";
            return 1;
        }
        int round = 1;
        while(this->arms.size()>1){
            RewardTemplate e = pow(2,-1*round)/4; 
            double d = this->delta/(50*round*round*round);
            int length = ceil(2*log(2/d)/(e*e));
            this->totalPulls+=length*this->arms.size();
            this->armSelectorInstance.totalRewards.clear();
            this->armSelectorInstance.totalPulls.clear();
            if(length==0)break; 
            if(!this->silent)std::cout<<"Sampling for "<<length<<"*"<<this->arms.size()<<" = "<<length*this->arms.size()<<" rounds each \n";
            sampleArms(length);
            if(!this->silent)std::cout<<"Running Median Elimination with e: "<<e<<" and d: "<<d<<"\n";
            int optArm = medianElimination(e/2,d);
            RewardTemplate optReward = this->armSelectorInstance.totalPulls[optArm]>0? this->armSelectorInstance.totalRewards[optArm]/this->armSelectorInstance.totalPulls[optArm]: INT_MAX;
            if(!this->silent)std::cout<<"Optimal Arm: "<<optArm<<" with Average Reward: "<<optReward<<"\n";
            if(!this->silent)std::cout<<"Threshold: "<<optReward<<"-"<<e<<" = "<<optReward-e<<"\n";
            removeArms(optReward-e);
            round+=1; 
            if(!this->silent)std::cout<<"\n";
        }
        std::cout<<"Total Pulls: "<<this->totalPulls<<" | ";
        std::cout<<"MedianElimination Pulls: "<<this->medianPulls<<" | ";
        std::cout<<"EGElimination Pulls: "<<this->totalPulls-this->medianPulls<<"\n"; 
        return 0;
    }
    Metrics<RewardTemplate> evaluate() override {
        ArmTemplate bestArm = this->findBestArm();
        if(this->arms.size()>1){
            if(!this->silent)std::cout<<"There are multiple Arms left! Empirical best Arm: "<<bestArm.id<<"\n";
        }
        else{
            if(!this->silent)std::cout<<"Final Remaining Arm: "<<bestArm.id<<"\n";
        }
        Metrics<RewardTemplate> metrics(
            "Exponential Gap Elimination",
            std::unordered_map<std::string,RewardTemplate>({
                {"Arms",this->ARMS},
                {"Confidence",this->delta}
            }),
            std::unordered_map<std::string,RewardTemplate>({
                {"Optimal Mean",this->evaluatorInstance.optimalArm.getMean()},
                {"Regret",this->evaluatorInstance.evaluateRegret(bestArm)},
                {"H1",this->evaluatorInstance.H1}
            }),
            this->totalPulls
        );
        this->totalPulls = 0; 
        this->medianPulls = 0;
        return metrics;
    }
};

template <typename RewardTemplate = double, typename ArmTemplate = BernoulliArm<RewardTemplate>, typename AllocatorTemplate = UniformAllocator<RewardTemplate,ArmTemplate>, typename EvaluatorTemplate = DeltaEvaluator<RewardTemplate,ArmTemplate>>
class SequentialHalving: public fixedBudget<RewardTemplate,ArmTemplate,AllocatorTemplate,EvaluatorTemplate>{
public: 
    SequentialHalving(int ARMS, int BUDGET, std::mt19937& generator, bool silent=false): fixedBudget<RewardTemplate,ArmTemplate,AllocatorTemplate,EvaluatorTemplate>(ARMS,BUDGET,generator,silent) {
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
    std::vector<int> findBottomHalf(){
        std::vector<std::pair<int, RewardTemplate>> averageRewards;
        for (auto p : this->arms) {
            int id = p.first;
            RewardTemplate avgReward = this->armSelectorInstance.totalPulls[id] > 0 ? this->armSelectorInstance.totalRewards[id] / this->armSelectorInstance.totalPulls[id] : INT_MIN;
            averageRewards.push_back({id, avgReward});
        }
        std::sort(averageRewards.begin(), averageRewards.end(), [](const std::pair<int, RewardTemplate>& a, const std::pair<int, RewardTemplate>& b) {
            return a.second < b.second;
        });
        std::vector<int> bottomHalf;
        for(int i = 0; i<this->arms.size()/2; i++){
            bottomHalf.push_back(averageRewards[i].first);
        }
        return bottomHalf;
    }
    int run() override {
        if(!this->silent)std::cout<<"\n";
        int rounds = ceil(log2(this->ARMS));
        for(int round = 0; round< rounds; round++){
            int lenRound = floor(this->BUDGET/(this->arms.size()*rounds));
            if(!this->silent)std::cout<<"Sampling for "<<lenRound<<"="<<this->BUDGET/rounds<<"/"<<this->arms.size()<<" rounds each \n";
            this->sampleArms(lenRound);
            this->totalPulls+=lenRound*this->arms.size();
            std::vector<int> bottomHalf = this->findBottomHalf();

            if(!this->silent)std::cout<<"Removed Arms: ";
            for (int id: bottomHalf){
                this->arms.erase(id);
                if(!this->silent)std::cout<<id<<" ";
            }
            if(!this->silent)std::cout<<"\n";    
        } 
        if(!this->silent)std::cout<<"Total Pulls: "<<this->totalPulls<<"\n";
        if(!this->silent)std::cout<<"Remaining Arms: "<<this->arms.size()<<"\n";
        
        return 0;
    }
    Metrics<RewardTemplate> evaluate() override {
        if(this->arms.size()>1){
            ArmTemplate bestArm = this->findBestArm();
            if(!this->silent)std::cout<<"There are multiple Arms left! Empirical best Arm: "<<bestArm.id<<"\n";
            return Metrics<RewardTemplate>(
                "Sequential Halving",
                std::unordered_map<std::string,RewardTemplate>({
                    {"Arms",this->ARMS},
                    {"Budget",this->BUDGET}
                }),
                std::unordered_map<std::string,RewardTemplate>({
                    {"Optimal Mean",this->evaluatorInstance.optimalArm.getMean()},
                    {"Regret",this->evaluatorInstance.evaluateRegret(bestArm)},
                    {"H1",this->evaluatorInstance.H1}
                }),
            this->totalPulls
            );
        }
        else{
            ArmTemplate bestArm = this->arms.begin()->second;
            if(!this->silent)std::cout<<"Final Remaining Arm: "<<bestArm.id<<"\n";
            return Metrics<RewardTemplate>(
                "Sequential Halving",
                std::unordered_map<std::string,RewardTemplate>({
                    {"Arms",this->ARMS},
                    {"Budget",this->BUDGET}
                }),
                std::unordered_map<std::string,RewardTemplate>({
                    {"Optimal Mean",this->evaluatorInstance.optimalArm.getMean()},
                    {"Regret",this->evaluatorInstance.evaluateRegret(bestArm)},
                    {"H1",this->evaluatorInstance.H1},
                }),
                this->totalPulls
            );
        }
    }
};
