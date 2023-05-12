// Fill out your copyright notice in the Description page of Project Settings.

#include "TPS_StateEffect.h"

#include "Character/TPSCharacter.h"
#include "Character/TPSHealthComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Interface/TPS_IGameActor.h"
#include "Kismet/GameplayStatics.h"

bool UTPS_StateEffect::InitObject(AActor* Actor, FName NameBoneHit)
{
	
	myActor = Actor;

	ITPS_IGameActor* myInterface = Cast<ITPS_IGameActor>(myActor);
	if (myInterface)
	{
		myInterface->AddEffect(this);
	}

	return true;
}

void UTPS_StateEffect::DestroyObject()
{
	ITPS_IGameActor* myInterface = Cast<ITPS_IGameActor>(myActor);
	if (myInterface)
	{
		myInterface->RemoveEffect(this);
	}

	myActor = nullptr;
	if (this && this->IsValidLowLevel())
	{
		this->ConditionalBeginDestroy();
	}	
}

bool UTPS_StateEffect_ExecuteOnce::InitObject(AActor* Actor, FName NameBoneHit)
{
	Super::InitObject(Actor, NameBoneHit);
	ExecuteOnce();
	return true;
}

void UTPS_StateEffect_ExecuteOnce::DestroyObject()
{
	Super::DestroyObject();
}

void UTPS_StateEffect_ExecuteOnce::ExecuteOnce()
{
	if (myActor)
	{
		UTPSHealthComponent* myHealthComp = Cast<UTPSHealthComponent>(myActor->GetComponentByClass(UTPSHealthComponent::StaticClass()));
		if (myHealthComp)
		{
			myHealthComp->ChangeHealthValue(Power);
		}
	}	

	DestroyObject();
}

bool UTPS_StateEffect_ExecuteTimer::InitObject(AActor* Actor, FName NameBoneHit)
{
	Super::InitObject(Actor, NameBoneHit);

	GetWorld()->GetTimerManager().SetTimer(TimerHandle_EffectTimer, this, &UTPS_StateEffect_ExecuteTimer::DestroyObject, Timer, false);
	GetWorld()->GetTimerManager().SetTimer(TimerHandle_ExecuteTimer, this, &UTPS_StateEffect_ExecuteTimer::Execute, RateTime, true);
	
	if (ParticleEffect)
	{
		FName NameBoneToAttached;
		FVector Loc = FVector(0);
		
		ParticleEmitter = UGameplayStatics::SpawnEmitterAttached(ParticleEffect,myActor->GetRootComponent(), NameBoneToAttached, Loc, FRotator::ZeroRotator,EAttachLocation::SnapToTarget,false);
	}
	
	return true;
}

void UTPS_StateEffect_ExecuteTimer::DestroyObject()
{
	if(ParticleEmitter)
	{
		ParticleEmitter->DestroyComponent();
		ParticleEmitter = nullptr;
	}
	Super::DestroyObject();
}

void UTPS_StateEffect_ExecuteTimer::Execute()
{
	if (myActor)
	{	
		//UGameplayStatics::ApplyDamage(myActor,Power,nullptr,nullptr,nullptr);	
		UTPSHealthComponent* myHealthComp = Cast<UTPSHealthComponent>(myActor->GetComponentByClass(UTPSHealthComponent::StaticClass()));
		if (myHealthComp)
		{
			myHealthComp->ChangeHealthValue(Power);
		}
	}
}

void UTPS_StateEffect_DisableInput::DestroyObject()
{
	if(myActor)
	{
		ChangeCharacterInputStatus(true);

		auto Character = Cast<ACharacter>(myActor);
		if(Character)
		{
			Character->GetCharacterMovement()->MaxWalkSpeed = 450.0;
		}
	}

	Super::DestroyObject();
}

void UTPS_StateEffect_DisableInput::Execute()
{
	if(myActor)
	{
		ChangeCharacterInputStatus(false);
	}
}

void UTPS_StateEffect_DisableInput::ChangeCharacterInputStatus(bool bStatus)
{
	if(LoopAnimation)
	{
		auto Character = Cast<ACharacter>(myActor);

		if(Character && !bStatus)
		{
			Character->PlayAnimMontage(LoopAnimation);
			Character->GetCharacterMovement()->MaxWalkSpeed = 0.0;
			Character->GetCharacterMovement()->StopMovementImmediately();
			Character->DisableInput(Cast<APlayerController>(Character->GetController()));
		}
		else if(Character && bStatus)
		{
			Character->EnableInput(Cast<APlayerController>(Character->GetController()));
		}
	}
}