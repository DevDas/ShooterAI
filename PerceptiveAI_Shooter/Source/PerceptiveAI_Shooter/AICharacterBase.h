// Copyright DevDasTour 2021 All Right Reserved.

#pragma once

#include "CoreMinimal.h"
#include "InterfaceAIHelper.h"
#include "GameFramework/Character.h"
#include "AICharacterBase.generated.h"

UCLASS()
class PERCEPTIVEAI_SHOOTER_API AAICharacterBase : public ACharacter, public IInterfaceAIHelper
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AAICharacterBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
		class UHealthComponent* HealthComp = nullptr;

	UPROPERTY(EditAnywhere)
		TSubclassOf<class AWeaponBase> WeaponClass;

	UPROPERTY(BlueprintReadWrite, Category = "Weapons")
	class AWeaponBase* Weapon = nullptr;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION()
		void HandleDeath();

	virtual bool GetIsDead_Implementation() override;
	virtual void NotifyChangeState_Implementation(ENPCState NPCState) override;
	virtual bool GetTeamID_Implementation() override;

private:

	bool DoOnce_COND = true;
};