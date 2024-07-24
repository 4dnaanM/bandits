template <typename ArmTemplate, typename RewardTemplate>
class banditAlgorithm{
protected:
    int ARMS; 
    int BUDGET; 
    std::vector<ArmTemplate> arms;
    std::mt19937* generatorptr;
public:
    banditAlgorithm(int ARMS, int BUDGET, std::mt19937& generator):ARMS(ARMS),BUDGET(BUDGET),generatorptr(&generator){
        this->arms=createArms(this->ARMS,this->generatorptr);
    }
    virtual std::vector<ArmTemplate> createArms(int ARMS, std::mt19937* generatorptr){
        std::vector<ArmTemplate> localArms;
        for(int i = 0; i<ARMS; i++){
            localArms.push_back(ArmTemplate(*(generatorptr)));
        }
        std::cout<<"\n";
        return localArms;
    }
    virtual void run() = 0;
    // virtual RewardTemplate evaluate() = 0;
};

template <typename ArmTemplate, typename RewardTemplate>
class UniformExploreAlgorithm : public banditAlgorithm<ArmTemplate,RewardTemplate>{
private:
    UniformAllocator<ArmTemplate,RewardTemplate> armSelectorInstance;
public:
    UniformExploreAlgorithm(int ARMS, int BUDGET, std::mt19937& generator): banditAlgorithm<ArmTemplate,RewardTemplate>(ARMS,BUDGET,generator),armSelectorInstance(this->arms){}
    // RewardTemplate evaluate() = 0;
    void run(){
        for(int i = 0; i< this->BUDGET; i++){
            std::cout<<"ROUND: "<<i<<"\n";
            ArmTemplate selectedArm = this->armSelectorInstance.selectArm();
            RewardTemplate reward = this->armSelectorInstance.playArm(selectedArm);
            std::cout<<"\n";
        }
    }
};

template <typename ArmTemplate, typename RewardTemplate>
class RandomExploreAlgorithm : public banditAlgorithm<ArmTemplate,RewardTemplate>{
private:
    RandomArmSelector<ArmTemplate,RewardTemplate> armSelectorInstance;
public:
    RandomExploreAlgorithm(int ARMS, int BUDGET, std::mt19937& generator): banditAlgorithm<ArmTemplate,RewardTemplate>(ARMS,BUDGET,generator),armSelectorInstance(this->arms,generator){}
    // RewardTemplate evaluate() = 0;
    void run(){
        for(int i = 0; i< this->BUDGET; i++){
            std::cout<<"ROUND: "<<i<<"\n";
            ArmTemplate selectedArm = this->armSelectorInstance.selectArm();
            RewardTemplate reward = this->armSelectorInstance.playArm(selectedArm);
            std::cout<<"\n";
        }
    }
};