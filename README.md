# bandits


A stochastic multi-armed bandit game is parameterized by the number of arms K, the number of rounds (or budget)n,and K probability distributions ν1,...,νK associated respectively with arm1,...,armK.These distributions are unknown to the forecaster. For t = 1, . . . , n, at round t, the forecaster chooses an arm It
in the set of arms {1, . . . , K }, and observes a reward drawn from νIt independently from the past (actions
and observations). At the end of the n rounds, the forecaster selects an arm, denoted Jn, and is evaluated in
terms of the difference between the mean reward of the optimal arm and the mean reward of Jn.

explore(BUDGET)
    Memory = {"Arms":[],"Rewards":[]}
    for round less than BUDGET["Explore"]: 
        J = pickArm(Memory["Arms"], Memory["Rewards"])
        R = getReward(J,Memory["Arms"], Memory["Rewards"])
        Memory["Arms"].append(J)
        Memory["Rewards"].append(R)
    processEvals(Memory)
    exploit(Memory, BUDGET)

exploit(Memory,BUDGET):
    for round less than BUDGET["Exploit"]: 
        J = pickArm(Memory["Arms"], Memory["Rewards"])
        R = getReward(J,Memory["Arms"], Memory["Rewards"])
        Memory["Arms"].append(J)
        Memory["Rewards"].append(R)
    processEvals(Memory)

classes: 
explore
exploit
pickArm -> algorithm
getReward -> getReward
Evaluate -> processEvals