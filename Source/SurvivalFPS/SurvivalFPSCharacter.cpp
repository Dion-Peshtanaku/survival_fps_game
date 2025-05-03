// Copyright Epic Games, Inc. All Rights Reserved.

#include "SurvivalFPSCharacter.h"
#include "SurvivalFPSProjectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Engine/LocalPlayer.h"
#include "SurvivalFPSWeaponComponent.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "GlobalManager.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// ASurvivalFPSCharacter

ASurvivalFPSCharacter::ASurvivalFPSCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);
		
	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-10.f, 0.f, 60.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->SetRelativeLocation(FVector(-30.f, 0.f, -150.f));

}

//////////////////////////////////////////////////////////////////////////// Input

void ASurvivalFPSCharacter::NotifyControllerChanged()
{
	Super::NotifyControllerChanged();

	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

void ASurvivalFPSCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{	
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Bind Reload Event
		PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &ASurvivalFPSCharacter::Reload);

		// Bind Interact Event
		PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &ASurvivalFPSCharacter::Interact);

		// Bind Shooting Event
		PlayerInputComponent->BindAction("Shoot", IE_Pressed, this, &ASurvivalFPSCharacter::Shoot);
		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ASurvivalFPSCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ASurvivalFPSCharacter::Look);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input Component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void ASurvivalFPSCharacter::HandleOnMontageEnd(UAnimMontage* a_pMontage, bool a_bInterrupted) 
{
	// Get reload montage logic
	if (a_pMontage->GetName().Contains("reload") && !a_bInterrupted) {
		// Check that weapon is equipped
		if (m_pEquippedWeapon == nullptr) return;

		// Check how much wee need to reload
		if (totalAmmo >= m_pEquippedWeapon->m_iClipSize) {
			totalAmmo -= m_pEquippedWeapon->m_iClipSize;

			int reloadAmt = m_pEquippedWeapon->m_iClipSize - currentAmmo;
			currentAmmo += reloadAmt;
		}
		else {
			int reloadAmt = abs(totalAmmo - currentAmmo);
			currentAmmo += reloadAmt;
			totalAmmo = 0;
		}
	}
}

void ASurvivalFPSCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Bind events
	GetMesh1P()->GetAnimInstance()->OnMontageEnded.AddDynamic(this, &ASurvivalFPSCharacter::HandleOnMontageEnd);

	EquipWeapon();

	if (m_cPlayerHUD != nullptr) {
		// Add the HUD to our viewport
		UUserWidget* HUD = CreateWidget<UUserWidget>(Cast<APlayerController>(GetController()), m_cPlayerHUD);
		HUD->AddToViewport(9999);
	}
}


void ASurvivalFPSCharacter::TakeDamage(float a_fDamage)
{
	// Take damage
	m_fHealth -= a_fDamage;

	// Death Handling
	if (m_fHealth <= 0) {
		UGameplayStatics::OpenLevel(GetWorld(), FName(GetWorld()->GetName()), true);
	}
}

void ASurvivalFPSCharacter::EquipWeapon()
{
	APlayerController* pController = Cast<APlayerController>(GetController());

	const FRotator pRotation = pController->PlayerCameraManager->GetCameraRotation();
	const FVector pLocation = GetOwner()->GetActorLocation();

	FActorSpawnParameters pSpawnParams;
	pSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	pSpawnParams.Owner = this;

	AActor* pPistol = GetWorld()->SpawnActor<AWeapon>(m_cPistol, pLocation, pRotation, pSpawnParams);

	// Set equipped weapon
	m_pEquippedWeapon = Cast<AWeapon>(pPistol);

	// Set ammo values
	currentAmmo = m_pEquippedWeapon->m_iClipSize;
	totalAmmo = m_pEquippedWeapon->m_iMaxAmmo;


	USurvivalFPSWeaponComponent* pWeapon = m_pEquippedWeapon->m_pWeaponComponent;
	pWeapon->AttachWeapon(this);
}

void ASurvivalFPSCharacter::Reload()
{
	// Check if reloading is necessary
	if (currentAmmo < m_pEquippedWeapon->m_iClipSize && totalAmmo > 0) {
		UAnimInstance* pAnimInstance = GetMesh1P()->GetAnimInstance();
		if (pAnimInstance != nullptr) {
			if (m_pReloadMontage != nullptr) {
				//Play reload montage
				pAnimInstance->Montage_Play(m_pReloadMontage, 1.f);
			}
		}
	}

}

void ASurvivalFPSCharacter::Shoot()
{
	UAnimInstance* pAnimInstance = GetMesh1P()->GetAnimInstance();

	if (pAnimInstance != nullptr) {
		if (m_pReloadMontage != nullptr) {
			// Stop reload montage
			pAnimInstance->Montage_Stop(0.2f, m_pReloadMontage);
		}
		if (m_pShootMontage != nullptr) {
			// Play shooting montage
			pAnimInstance->Montage_Play(m_pShootMontage, 1.f);
		}
	}
}

void ASurvivalFPSCharacter::Interact()
{
	// Ensure player can interact
	if (m_bCanInteract) {
		UGlobalManager* pMngr = Cast<UGlobalManager>(UGameplayStatics::GetGameInstance(GetWorld()));
		if (pMngr->m_iPoints >= 1000) // Ensure player has at least 1000 points in order to refill ammo
		{
			// Negate points
			pMngr->m_iPoints -= 1000;

			// Refill ammo
			currentAmmo = m_pEquippedWeapon->m_iClipSize;
			totalAmmo = m_pEquippedWeapon->m_iMaxAmmo;
		}
	}
}


void ASurvivalFPSCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add movement 
		AddMovementInput(GetActorForwardVector(), MovementVector.Y);
		AddMovementInput(GetActorRightVector(), MovementVector.X);
	}
}

void ASurvivalFPSCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}