#pragma once
#include "SDTBaseAIController.h"

class TimeBudgetManager
{
public:
    static TimeBudgetManager* GetInstance();
    static void Destroy();

    void RegisterAIAgent(ASDTBaseAIController* aiAgent);
    void UnregisterAIAgent(ASDTBaseAIController* aiAgent);

    void UpdateTimer(float updateTime, FString pawnName);
    bool CanUpdate();
    void ResetTimer();

private:
    //SINGLETON
    TimeBudgetManager();
    void ResetUpdated();

    static TimeBudgetManager* m_Instance;

    TArray<ASDTBaseAIController*> m_registeredAgents;
    int m_currentIndex = 0;

    float m_timeBudget = 0.0006f;
    float m_currentTime = 0.0f;
};
