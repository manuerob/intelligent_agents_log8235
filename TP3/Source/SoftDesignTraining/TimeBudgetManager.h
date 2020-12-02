#pragma once
#include <SoftDesignTraining\SDTBaseAIController.h>

class TimeBudgetManager
{
public:
    static TimeBudgetManager* GetInstance();
    static void Destroy();

    void RegisterAIAgent(ASDTBaseAIController* aiAgent);
    void UnregisterAIAgent(ASDTBaseAIController* aiAgent);
    void UpdateAgents();

private:

    //SINGLETON
    TimeBudgetManager();
    static TimeBudgetManager* m_Instance;

    TArray<ASDTBaseAIController*> m_registeredAgents;
    int m_CurrentIndex = 0;

    float m_timeBudget = 0.1f;
};
