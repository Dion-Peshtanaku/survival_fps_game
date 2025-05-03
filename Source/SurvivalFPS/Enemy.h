// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnemyManager.h"
#include "Enemy.generated.h"

UCLASS()
class SURVIVALFPS_API AEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemy();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Reference to montages
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimMontage* m_pAttackMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimMontage* m_pDeathMontage;

	// References to sounds
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	TArray<USoundBase*> m_pAttackSounds;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	TArray<USoundBase*> m_pDeathSounds;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	USoundBase* m_pEnemyDamageSound;

	// Combat fields
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float m_fHealth = 30.0f;
	FVector m_vWorldSpawnLocation;
	bool m_bInCombat = false;

	// Combat methods
	void Attack();
	void TakeDamage(float a_fDamage);

	// Death handling methods
	void Die();
	void Reset();

	// Montage End event handler
	UFUNCTION()
	void HandleOnMontageEnded(UAnimMontage* a_pMontage, bool a_bInterrupted);
private:
	// Reference that contains enemy manager
	AEnemyManager* m_pEnemyManager;
};
