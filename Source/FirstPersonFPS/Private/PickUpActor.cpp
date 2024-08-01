// Fill out your copyright notice in the Description page of Project Settings.


#include "PickUpActor.h"
#include "Wepon_PrimaryDataAsset.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values
APickUpActor::APickUpActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BoxRootComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxRoot"));
	RootComponent = BoxRootComp;

	StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMeshComp->SetupAttachment(RootComponent);

	ItemPickUpWidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("ItemPickUpWidget"));
	ItemPickUpWidgetComp->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void APickUpActor::BeginPlay()
{
	Super::BeginPlay();
	
	if (WeaponPrimaryDataAsset && WeaponPrimaryDataAsset->StaticMesh)
		StaticMeshComp->SetStaticMesh(WeaponPrimaryDataAsset->StaticMesh);

	if (WeaponPrimaryDataAsset)
		Ammo = WeaponPrimaryDataAsset->MaxAmmo;

	SetWidgetVisibility(false);
}

// Called every frame
void APickUpActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APickUpActor::SetWidgetVisibility(bool IsVisible)
{
	if (ItemPickUpWidgetComp)
	{
		//UKismetSystemLibrary::PrintString(this, "Detected!!");
		ItemPickUpWidgetComp->SetVisibility(IsVisible);
	}
}

int APickUpActor::GetAmmo() const
{
	return Ammo;
}

class UWepon_PrimaryDataAsset* APickUpActor::GetWeaponPrimaryDataAsset() const
{
	return WeaponPrimaryDataAsset;
}

