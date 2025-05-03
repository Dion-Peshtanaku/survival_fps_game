// Fill out your copyright notice in the Description page of Project Settings.


#include "AmmoStation.h"
#include "Components/CapsuleComponent.h"
#include "SurvivalFPSCharacter.h"

// Sets default values
AAmmoStation::AAmmoStation()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create the mesh
	m_pMesh = CreateDefaultSubobject<UStaticMeshComponent>("Box Mesh");
	m_pMesh->SetupAttachment(RootComponent);

	// Create the capsule component
	m_pCapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>("Collider");
	m_pCapsuleComponent->InitCapsuleSize(55.0f, 96.0f);
	m_pCapsuleComponent->SetupAttachment(m_pMesh);

	// Register overlap events
	m_pCapsuleComponent->OnComponentBeginOverlap.AddDynamic(this, &AAmmoStation::OnCapsuleBeginOverlap);
	m_pCapsuleComponent->OnComponentEndOverlap.AddDynamic(this, &AAmmoStation::OnCapsuleEndOverlap);
}

// Called when the game starts or when spawned
void AAmmoStation::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AAmmoStation::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AAmmoStation::OnCapsuleBeginOverlap(UPrimitiveComponent* a_pOverlappedComponent, AActor* a_pOtherActor, UPrimitiveComponent* a_pOtherComp, int32 a_iOtherBodyIndex, bool a_bfromSweep, const FHitResult& a_pSweepResult)
{
	// Set the player to being able to interact with the station
	ASurvivalFPSCharacter* pPlayer = Cast<ASurvivalFPSCharacter>(a_pOtherActor);
	pPlayer->m_bCanInteract = true;
}
void AAmmoStation::OnCapsuleEndOverlap(UPrimitiveComponent* a_pOverlappedComponent, AActor* a_pOtherActor, UPrimitiveComponent* a_pOtherComp, int32 a_iOtherBodyIndex)
{
	// Set the player to not being able to interact with the station
	ASurvivalFPSCharacter* pPlayer = Cast<ASurvivalFPSCharacter>(a_pOtherActor);
	pPlayer->m_bCanInteract = false;
}
