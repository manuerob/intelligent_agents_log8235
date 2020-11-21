// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/AiAgentGroupManager.h"
#include "SoftDesignTraining.h"

AiAgentGroupManager* AiAgentGroupManager::m_Instance;

AiAgentGroupManager::AiAgentGroupManager()
{
}

AiAgentGroupManager* AiAgentGroupManager::GetInstance()
{
    if (!m_Instance)
    {
        m_Instance = new AiAgentGroupManager();
    }

    return m_Instance;
}

void AiAgentGroupManager::Destroy()
{
    delete m_Instance;
    m_Instance = nullptr;
}

void AiAgentGroupManager::RegisterAIAgent(AAIBase* aiAgent)
{
    m_registeredAgents.Add(aiAgent);
}

void AiAgentGroupManager::UnregisterAIAgent(AAIBase* aiAgent)
{
    m_registeredAgents.Remove(aiAgent);
}

TargetLKPInfo AiAgentGroupManager::GetLKPFromGroup(const FString& targetLabel,bool& targetfound)
{
    int agentCount = m_registeredAgents.Num();
    TargetLKPInfo outLKPInfo = TargetLKPInfo();
    for (AAIBase* agent : m_registeredAgents) {
        if (agent->IsTargetPlayerSeen()) {
            outLKPInfo.SetLKPPos(agent->GetTargetPlayerPos());
            outLKPInfo.SetTargetLabel(targetLabel);
            targetfound = true;

        } else if (agent->GetCurrentTargetLKPInfo().GetTargetLabel() == targetLabel /*ajouter && avec variable temporelle*/)
            outLKPInfo = agent->GetCurrentTargetLKPInfo();
    }
        
    return outLKPInfo;
}