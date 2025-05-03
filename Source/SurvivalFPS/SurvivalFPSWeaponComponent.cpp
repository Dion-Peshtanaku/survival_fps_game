// Copyright Epic Games, Inc. All Rights Reserved.


#include "SurvivalFPSWeaponComponent.h"
#include "SurvivalFPSCharacter.h"
#include "SurvivalFPSProjectile.h"
#include "GameFramework/PlayerController.h"
#include "Camera/PlayerCameraManager.h"
#include "Kismet/GameplayStatics.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputAction.h" 
#include "InputMappingContext.h"
#include "Animation/AnimInstance.h"
#include "Engine/LocalPlayer.h"
#include "Engine/World.h"
#include "Enemy.h"
#include "GlobalManager.h"

// Sets default values for this component's properties
USurvivalFPSWeaponComponent::USurvivalFPSWeaponComponent()
{
	// Initialize the variables here to avoid them being NULL
	FireAction = Cast<UInputAction>(StaticLoadObject(UInputAction::StaticClass(), nullptr, TEXT("InputAction'/Game/FirstPerson/Input/Actions/IA_Shoot.IA_Shoot'")));
	FireMappingContext = Cast<UInputMappingContext>(StaticLoadObject(UInputMappingContext::StaticClass(), nullptr, TEXT("InputMappingContext'/Game/FirstPerson/Input/IMC_Weapons.IMC_Weapons'")));

}


void USurvivalFPSWeaponComponent::Fire()
{

	if (Character == nullptr || Character->GetController() == nullptr)
	{
		return;
	}

	// Check that our clip isnt empty
	if (Character->currentAmmo <= 0) return;
	// Try and line trace
		UWorld* const World = GetWorld();
		if (World != nullptr)
		{
			APlayerController* PlayerController = Cast<APlayerController>(Character->GetController());
			const FRotator SpawnRotation = PlayerController->PlayerCameraManager->GetCameraRotation();
			// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
			const FVector SpawnLocation = GetOwner()->GetActorLocation() + SpawnRotation.RotateVector(m_vMuzzleOffset);
			
			// Setup Query params
			FCollisionQueryParams queryParams;
			queryParams.AddIgnoredActor(PlayerController->GetPawn());

			// Setup hit result
			FHitResult outHit;

			// Line trace
			World->LineTraceSingleByChannel(outHit, SpawnLocation, SpawnLocation + (SpawnRotation.Vector() * 3000), ECollisionChannel::ECC_Pawn, queryParams);

			// Try to cast hit to an enemy
			AEnemy* pEnemy = Cast<AEnemy>(outHit.GetActor());
			if (pEnemy) {
				pEnemy->TakeDamage(10.0f);
				Cast<UGlobalManager>(UGameplayStatics::GetGameInstance(GetWorld()))->m_iPoints += 10;
			}
			else // Play environmental sounds
			{
				// Check that there sounds in the list
				if (m_pEnvironmentalSounds.Num() > 0) {
					UGameplayStatics::PlaySoundAtLocation(GetWorld(), m_pEnvironmentalSounds[FMath::RandRange(0, m_pEnvironmentalSounds.Num() - 1)], Character->GetActorLocation());
				}
			}
			
			// Update ammo count
			Character->currentAmmo = Character->currentAmmo - 1;
		}
	// Try and play the sound if specified
	if (m_pFireSound != nullptr)
	{

		UGameplayStatics::PlaySoundAtLocation(this, m_pFireSound, Character->GetActorLocation());
	}
	// Try and spawn muzzle flash particle system
	if (m_pMuzzleFlash != nullptr) {
		APlayerController* PlayerController = Cast<APlayerController>(Character->GetController());
		const FRotator SpawnRotation = PlayerController->PlayerCameraManager->GetCameraRotation();
		// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
		const FVector SpawnLocation = GetOwner()->GetActorLocation() + SpawnRotation.RotateVector(m_vMuzzleOffset);

		UNiagaraFunctionLibrary::SpawnSystemAtLocation(World, m_pMuzzleFlash, SpawnLocation, SpawnRotation);
	}
}

bool USurvivalFPSWeaponComponent::AttachWeapon(ASurvivalFPSCharacter* TargetCharacter)
{
	Character = TargetCharacter;

	// Check that the character is valid, and has no weapon component yet
	if (Character == nullptr || Character->GetInstanceComponents().FindItemByClass<USurvivalFPSWeaponComponent>())
	{
		return false;
	}
	// Attach the weapon to the First Person Character
	FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);
	AttachToComponent(Character->GetMesh1P(), AttachmentRules, FName("WeaponSocket"));


	// Set up action bindings
	if (APlayerController* PlayerController = Cast<APlayerController>(Character->GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			// Set the priority of the mapping to 1, so that it overrides the Jump action with the Fire action when using touch input
			Subsystem->AddMappingContext(FireMappingContext, 1);
		}

		if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerController->InputComponent))
		{

			// Fire
			EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Triggered, this, &USurvivalFPSWeaponComponent::Fire);
		}
	}

	return true;
}

void USurvivalFPSWeaponComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// ensure we have a character owner
	if (Character != nullptr)
	{
		// remove the input mapping context from the Player Controller
		if (APlayerController* PlayerController = Cast<APlayerController>(Character->GetController()))
		{
			if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
			{
				Subsystem->RemoveMappingContext(FireMappingContext);
			}
		}
	}

	// maintain the EndPlay call chain
	Super::EndPlay(EndPlayReason);
}