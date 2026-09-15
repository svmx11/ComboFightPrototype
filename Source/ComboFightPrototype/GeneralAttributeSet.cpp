// Fill out your copyright notice in the Description page of Project Settings.


#include "GeneralAttributeSet.h"
#include "GameplayEffectExtension.h"
#include "Net/UnrealNetwork.h"

UGeneralAttributeSet::UGeneralAttributeSet()
	: Health(100.0f)
	, MaxHealth(100.0f)
	, IncomingDamage(0.0f)
{
}

void UGeneralAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UGeneralAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGeneralAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
}

void UGeneralAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxHealth());
	}
}

void UGeneralAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	if (Data.EvaluatedData.Attribute == GetIncomingDamageAttribute())
	{
		const float LocalDamage = GetIncomingDamage();
		SetIncomingDamage(0.0f); // сразу гасим канал, чтобы значение не "залипало"

		if (LocalDamage > 0.0f)
		{
			const float NewHealth = FMath::Clamp(GetHealth() - LocalDamage, 0.0f, GetMaxHealth());
			SetHealth(NewHealth);

			if (NewHealth <= 0.0f)
			{
				if (AActor* OwnerActor = GetOwningActor())
				{
					OnDeath.Broadcast(OwnerActor);
				}
			}
		}
	}
}

void UGeneralAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGeneralAttributeSet, Health, OldHealth);
}

void UGeneralAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGeneralAttributeSet, MaxHealth, OldMaxHealth);
}