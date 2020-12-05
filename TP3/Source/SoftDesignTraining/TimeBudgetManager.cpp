#include "TimeBudgetManager.h"

TimeBudgetManager* TimeBudgetManager::m_Instance;

TimeBudgetManager::TimeBudgetManager() {

}

TimeBudgetManager* TimeBudgetManager::GetInstance() {
	if (!m_Instance) {
		m_Instance = new TimeBudgetManager();
	}

	return m_Instance;
}

void TimeBudgetManager::Destroy() {
	delete m_Instance;
	m_Instance = nullptr;
}

void TimeBudgetManager::RegisterAIAgent(ASDTBaseAIController* aiAgent) {
	m_registeredAgents.Add(aiAgent);
}

void TimeBudgetManager::UnregisterAIAgent(ASDTBaseAIController* aiAgent) {
	m_registeredAgents.Remove(aiAgent);

}

void TimeBudgetManager::UpdateTimer(float updateTime, FString pawnName) {
	m_currentTime += updateTime;
	m_currentIndex++;

	if (m_currentIndex >= m_registeredAgents.Num() - 1) {
		ResetUpdated();
	}
}

bool TimeBudgetManager::CanUpdate() {
	return m_currentTime < m_timeBudget;
}

void TimeBudgetManager::ResetTimer() {
	m_currentTime = 0.0f;
}

void TimeBudgetManager::ResetUpdated() {
	m_currentIndex = 0;
	for (int i = 0; i < m_registeredAgents.Num(); ++i) {
		m_registeredAgents[i]->updated = false;
	}
}