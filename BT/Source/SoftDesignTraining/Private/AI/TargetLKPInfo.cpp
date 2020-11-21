// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/TargetLKPInfo.h"
#include "SoftDesignTraining.h"

TargetLKPInfo::TargetLKPInfo()
    :m_currentLKPState(ELKPState::LKPState_Invalid)
    ,m_pos(FVector::ZeroVector)
{

}

TargetLKPInfo::TargetLKPInfo(const FString& targetLabel)
{
    m_targetLabel = targetLabel;
    m_pos = FVector::ZeroVector;
    m_currentLKPState = ELKPState::LKPState_Invalid;
}

TargetLKPInfo::~TargetLKPInfo()
{
}
