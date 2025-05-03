// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SurvivalFPSCharacter.h"
#include "AmmoStation.generated.h"

UCLASS()
class SURVIVALFPS_API AAmmoStation : public AActor
{
	GENERATED_BODY()
	
	// Ammo station mesh
	UPROPERTY(VisibleDefaultsOnly, Category = "Mesh")
	UStaticMeshComponent* m_pMesh;

	// Trigger capsule collider
	UPROPERTY(VisibleAnywhere, Category = "Trigger capsule")
	UCapsuleComponent* m_pCapsuleComponent;

public:	
	// Sets default values for this actor's properties
	AAmmoStation();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// BeginOverlap event handler
	UFUNCTION()
	void OnCapsuleBeginOverlap(UPrimitiveComponent* a_pOverlappedComponent, AActor* a_pOtherActor, UPrimitiveComponent* a_pOtherComp, int32 a_iOtherBodyIndex, bool a_bfromSweep, const FHitResult& a_pSweepResult);

	//// EndOverlap event handler
	UFUNCTION()
	void OnCapsuleEndOverlap(UPrimitiveComponent* a_pOverlappedComponent, AActor* a_pOtherActor, UPrimitiveComponent* a_pOtherComp, int32 a_iOtherBodyIndex);
};
