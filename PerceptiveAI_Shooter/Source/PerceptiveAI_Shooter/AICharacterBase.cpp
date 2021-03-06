// Copyright DevDasTour 2021 All Right Reserved.

#include "AICharacterBase.h"
#include "HealthComponent.h"
#include "Components/CapsuleComponent.h"
#include "WeaponBase.h"
#include "AIFunctionLibrary.h"
#include "AIAnimationInstance.h"
#include "PatrolRoute.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "AICGuard.h"
#include "BrainComponent.h"

AAICharacterBase::AAICharacterBase()
{
	PrimaryActorTick.bCanEverTick = true;

	HealthComp = CreateDefaultSubobject<UHealthComponent>(FName("HealthComponent"));

	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
}

void AAICharacterBase::BeginPlay()
{
	Super::BeginPlay();

	HealthComp->OnDeath.AddDynamic(this, &AAICharacterBase::HandleDeath);
	HealthComp->OnDamageTaken.AddDynamic(this, &AAICharacterBase::OnDamageTaken);

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	Weapon = GetWorld()->SpawnActor<AWeaponBase>(WeaponClass, GetMesh()->GetSocketTransform(FName("WeaponSocket")), SpawnParams);
	if (Weapon)
	{
		Weapon->SetOwner(this);
		Weapon->SetInstigator(this);

		Weapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("WeaponSocket"));
	}
}

void AAICharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AAICharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AAICharacterBase::HandleDeath()
{
	if (DoOnce_COND)
	{
		DoOnce_COND = false;
		UE_LOG(LogTemp, Warning, TEXT("HandleDeath Occur"));
		GetMesh()->SetCollisionProfileName("Ragdoll");
		GetMesh()->SetAllBodiesSimulatePhysics(true);
		GetMesh()->WakeAllRigidBodies();
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		AAICGuard* AIGuardController = Cast<AAICGuard>(UAIBlueprintHelperLibrary::GetAIController(this));
		if (AIGuardController)
		{
			UBrainComponent* BrainComp = AIGuardController->GetBrainComponent();
			if (BrainComp)
			{
				BrainComp->StopLogic("Dead");
			}
		}
	}
}

bool AAICharacterBase::GetIsDead_Implementation()
{
	return HealthComp->IsDead();
}

void AAICharacterBase::NotifyChangeState_Implementation(ENPCState NPCState)
{
	UAIAnimationInstance* ABP = Cast<UAIAnimationInstance>(GetMesh()->GetAnimInstance());
	if (ABP)
	{
		Execute_NotifyChangeState(ABP, NPCState);
	}
}

bool AAICharacterBase::GetTeamID_Implementation()
{
	return false;
}

void AAICharacterBase::ModifyMovementSpeed_Implementation(ENPCState NPCState)
{
	float MovementSpeed;

	switch (NPCState)
	{
	case Passive: MovementSpeed = 160.f;
		break;
	case Investigating: MovementSpeed = 160.f;
		break;
	case Seeking: MovementSpeed = 400.f;
		break;
	case Attack: MovementSpeed = 600.f;
		break;
	case Dead: MovementSpeed = 0.f;
		break;
	}

	GetCharacterMovement()->MaxWalkSpeed = MovementSpeed;
}

APatrolRoute* AAICharacterBase::GetPatrolRoute_Implementation()
{
	return PatrolRoute;
}

void AAICharacterBase::ModifyAimingStance_Implementation(bool bIsAimingParam)
{
	UAIAnimationInstance* ABP = Cast<UAIAnimationInstance>(GetMesh()->GetAnimInstance());
	if (ABP)
	{
		Execute_ModifyAimingStance(ABP, bIsAimingParam);
	}
}

float AAICharacterBase::GetWeaponRange_Implementation()
{
	if (Weapon)
	{
		return Weapon->GetWeaponRange();
	}

	return 0.f;
}

void AAICharacterBase::OnDamageTaken()
{
	PlayAnimMontage(AnimMontageAsset);
}

AWeaponBase* AAICharacterBase::GetWeapon_Implementation()
{
	if (Weapon)
	{
		return Weapon;
	}
	return nullptr;
}

void AAICharacterBase::TryShootEquipWeapon_Implementation()
{
	if (Weapon != nullptr)
	{
		Weapon->OnFireRequested.Broadcast();
	}
}