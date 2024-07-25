template <typename ArmTemplate, typename RewardTemplate, typename AllocatorTemplate, typename EvaluatorTemplate>
class banditAlgorithm{
protected:
    int ARMS; 
    int BUDGET; 
    std::vector<ArmTemplate> arms;
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
    
    virtual std::vector<ArmTemplate> createArms(int ARMS, std::mt19937* generatorptr){
        std::vector<ArmTemplate> localArms;
        for(int i = 0; i<ARMS; i++){
            localArms.push_back(ArmTemplate(*(generatorptr),i));
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
        std::vector<RewardTemplate> rewardsforArm(this->ARMS,0);
        std::vector<int> timesArmPulled(this->ARMS,0);
        for(int i = 0; i<this->BUDGET; i++){
            rewardsforArm[this->armSelectorInstance.pastArms[i].id]+=this->armSelectorInstance.pastRewards[i];
            timesArmPulled[this->armSelectorInstance.pastArms[i].id]+=1;
        }
        std::vector<RewardTemplate> averageRewards(this->ARMS,0);
        for(int i = 0; i<this->ARMS; i++){
            averageRewards[i] = timesArmPulled[i]>0? rewardsforArm[i]/timesArmPulled[i]: 0;
        }
        int bestArmIndex = std::max_element(averageRewards.begin(),averageRewards.end())-averageRewards.begin();
        return this->arms[bestArmIndex];
    }

    virtual RewardTemplate evaluate(){
        ArmTemplate bestArm = findBestArm();
        std::cout<<"Empirical Best Arm: "<<bestArm.id<<"\n";
        return this->evaluatorInstance.evaluateRegret(bestArm);
    }
};