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

void TimeBudgetManager::UpdateAgents() {
	double current_time = FPlatformTime::Seconds();
	while(current_time < m_timeBudget){
		//blah blah
		ASDTBaseAIController curr_agent = m_registeredAgents[m_CurrentIndex];
		curr_Agent.m_canUpadte = true;

		while (!curr_agent.m_doneUpdate);

		curr_Agent.m_canUpadte = false;
		curr_agent.m_doneUpdate = false;
		current_time += FPlatformTime::Seconds();
		m_CurrentIndex++;
	}
}