// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemyManager.generated.h"

UCLASS()
class SURVIVALFPS_API AEnemyManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEnemyManager();

	// Container for enemies
	TArray<AActor*> m_pEnemies;

	// Configurable fields
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Managment")
	TArray<FVector> m_pSpawnLocations;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Managment")
	int m_iFirstWaveSpawnCount = 5;

	// Wave tracker for UI purposes
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Managment")
	int m_iCurrentWave = 1;

	// Count and kill trackers
	int m_iCurrentWaveSize;
	int m_iWaveKills = 0;

	// Methods for spawning and updating enemies
	void SpawnFirstWave();
	void UpdateWaveParameters();
	void StartNextWave();
	void SpawnMoreEnemies();
	TArray<AActor*> GetAllEnemiesInCombat();
	void ModifyWaveSpeeds();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	// Min and max wave speeds
	float m_fGlobalMaxWalkSpeed = 120.0f;
	float m_fGlobalMinWalkSpeed = 70.0f;

	// Cap for enemies in arena
	int m_iMaxEnemiesInArena = 5;
};
