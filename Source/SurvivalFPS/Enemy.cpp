// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"
#include "Kismet/GameplayStatics.h"
#include "SurvivalFPSCharacter.h"
#include "GlobalManager.h"

// Sets default values
AEnemy::AEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	// Bind events
	GetMesh()->GetAnimInstance()->OnMontageEnded.AddDynamic(this, &AEnemy::HandleOnMontageEnded);

	// Set world spawn location
	//m_vWorldSpawnLocation = GetActorLocation();
	m_vWorldSpawnLocation = FVector(2222.423834f, 3629.231895f, -731.492867f); 

	// Get reference to enemy manager
	m_pEnemyManager = Cast<AEnemyManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AEnemyManager::StaticClass()));

}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemy::Attack()
{
	UAnimInstance* pAnimInstance = GetMesh()->GetAnimInstance();
	if (m_pAttackMontage != nullptr) {
		pAnimInstance->Montage_Play(m_pAttackMontage);
	}

	// Get reference to player
	ASurvivalFPSCharacter* pPlayer = Cast<ASurvivalFPSCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	pPlayer->TakeDamage(10.0f);
	if (m_pAttackSounds.Num() > 0) {
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), m_pAttackSounds[FMath::RandRange(0, m_pAttackSounds.Num() - 1)], GetActorLocation());
	}
}

void AEnemy::TakeDamage(float a_fDamage)
{
	// Check if death montage is playing
	UAnimInstance* pAnimInstance = GetMesh()->GetAnimInstance();
	if (pAnimInstance != nullptr) {
		if (pAnimInstance->Montage_IsPlaying(m_pDeathMontage)) return;
	}

	// Play damage indicator
	if (m_pEnemyDamageSound != nullptr) {
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), m_pEnemyDamageSound, GetActorLocation());
	}

	// Deal Damage
	m_fHealth -= a_fDamage;

	// Death Handling
	if (m_fHealth <= 0) Die();
}
 
void AEnemy::Die()
{
	// Play death sounds
	if (m_pDeathSounds.Num() > 0) {
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), m_pDeathSounds[FMath::RandRange(0, m_pDeathSounds.Num() - 1)], GetActorLocation());
	}
	// Play death Montage
	UAnimInstance* pAnimInstance = GetMesh()->GetAnimInstance();
	if (m_pDeathMontage != nullptr) {
		pAnimInstance->Montage_Play(m_pDeathMontage);
	}
}

void AEnemy::Reset()
{
	// Reset enemy back to spawn location
	TeleportTo(m_vWorldSpawnLocation, GetActorRotation(), false, true);

	// Reset health and combat status
	m_fHealth = 30.0f;
	m_bInCombat = false;

}

void AEnemy::HandleOnMontageEnded(UAnimMontage* a_pMontage, bool a_bInterrupted)
{
	// Check for death montage
	if (a_pMontage->GetName().Contains("Death")) {
		 Reset();	

		 // Add points
		 Cast<UGlobalManager>(UGameplayStatics::GetGameInstance(GetWorld()))->m_iPoints += 100;

		 // Update enemy manager
		 m_pEnemyManager->m_iWaveKills++;

		 if (m_pEnemyManager->m_iWaveKills >= m_pEnemyManager->m_iCurrentWaveSize) {

			 // Begin the next wave and update params
			 m_pEnemyManager->UpdateWaveParameters();
			 m_pEnemyManager->StartNextWave();
		 }
		 else {

			 // Try to spawn more enemies
			 m_pEnemyManager->SpawnMoreEnemies();
		 }
	}

}

