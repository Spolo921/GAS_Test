// Fill out your copyright notice in the Description page of Project Settings.


#include "TestCharaBase.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GAS_Test/GameplayAbilitySystem/AttributeSets/BasicAttributeSet.h"


// Sets default values
ATestCharaBase::ATestCharaBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	 AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	 AbilitySystemComponent -> SetIsReplicated(true);
	AbilitySystemComponent -> SetReplicationMode(AscReplicationMode);
	

	GetCapsuleComponent() -> InitCapsuleSize(35.0f, 90.0f);
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement() -> bOrientRotationToMovement = true;
	GetCharacterMovement() -> RotationRate = FRotator(0.0f, 500.0f, 0.0f);
	
	GetCharacterMovement() -> JumpZVelocity = 500.f;
	GetCharacterMovement() -> AirControl = 0.35f;
	GetCharacterMovement() -> MaxWalkSpeed = 500.0f;
	GetCharacterMovement() -> MinAnalogWalkSpeed = 20.0f;
	GetCharacterMovement() -> BrakingDecelerationWalking = 2000.0f;
	GetCharacterMovement() -> BrakingDecelerationFalling = 1500.0f;

	BasicAttributeSet = CreateDefaultSubobject<UBasicAttributeSet>(TEXT("BasicAttributeSet"));
	

}

// Called when the game starts or when spawned
void ATestCharaBase::BeginPlay()
{
	Super::BeginPlay();
	
}

void ATestCharaBase::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	if(AbilitySystemComponent)
	{
		AbilitySystemComponent -> InitAbilityActorInfo(this, this);
		GrantAbilites(StartingAbilities);
	}
}

void ATestCharaBase::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	if(AbilitySystemComponent) AbilitySystemComponent -> InitAbilityActorInfo(this, this);
}

// Called every frame
void ATestCharaBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ATestCharaBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

UAbilitySystemComponent* ATestCharaBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

TArray<FGameplayAbilitySpecHandle> ATestCharaBase::GrantAbilites(TArray<TSubclassOf<UGameplayAbility>> AbilitiesToGrant)
{
	if (!AbilitySystemComponent)
	{
		return TArray<FGameplayAbilitySpecHandle>();
	}

	TArray<FGameplayAbilitySpecHandle> AbilitiesHandles;
	for (auto Ability  : AbilitiesToGrant)
	{
		auto SpecHandle =  AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(Ability, 1, -1, this));
		AbilitiesHandles.Add(SpecHandle);
	}
	SendAbilitesChanged();
	return AbilitiesHandles;
}

void ATestCharaBase::RemoveAbilites(TArray<FGameplayAbilitySpecHandle> AbilitiesToRemove)
{
	if (!AbilitySystemComponent)
	{
		return;
	}
	for (auto AbilityHandle : AbilitiesToRemove)
	{
		AbilitySystemComponent->ClearAbility(AbilityHandle);
	}
	SendAbilitesChanged();
}

void ATestCharaBase::SendAbilitesChanged()
{
	FGameplayEventData EventData;
	EventData.EventTag = FGameplayTag::RequestGameplayTag(FName("Event.Abilities.Changed"));
	EventData.Instigator = this;
	EventData.Target = this; 
	
	UAbilitySystemBlueprintLibrary :: SendGameplayEventToActor(this, EventData.EventTag, EventData);
}

