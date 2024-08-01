// Fill out your copyright notice in the Description page of Project Settings.

#include "WeaponActor.h"
#include "GunDamageType.h"
#include "Components/SceneComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Sound/SoundCue.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Particles/ParticleSystem.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Animation/AnimMontage.h"
#include "Animation/AnimSequence.h"
#include "Materials/Material.h"
#include "DrawDebugHelpers.h"
#include "FirstPersonFPSCharacter.h"

// Sets default values
AWeaponActor::AWeaponActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneRootComp = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	RootComponent = SceneRootComp;

	SkeletalMeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	SkeletalMeshComp->SetupAttachment(RootComponent);

	Decay_BulletHole = ConstructorHelpers::FObjectFinderOptional<UMaterial>(TEXT("Material'/Game/Materials/BulletHole.BulletHole'")).Get();
}

void AWeaponActor::Init(UWepon_PrimaryDataAsset* NewWeaponPrimaryDataAsset, const int CurAmmo)
{
	if (NewWeaponPrimaryDataAsset)
	{
		ATK = NewWeaponPrimaryDataAsset->ATK;
		MaxAmmo = NewWeaponPrimaryDataAsset->MaxAmmo;
		Rate = NewWeaponPrimaryDataAsset->Rate;
		RecoilValue = NewWeaponPrimaryDataAsset->RecoilValue;
		Name = NewWeaponPrimaryDataAsset->Name;
		FirstType = NewWeaponPrimaryDataAsset->FirstType;
		ReloadTime = NewWeaponPrimaryDataAsset->ReloadTime;

		Ammo = CurAmmo;

		if (NewWeaponPrimaryDataAsset->SkeletonMesh)
		{
			SkeletalMeshComp->SetSkeletalMesh(NewWeaponPrimaryDataAsset->SkeletonMesh);
		}
		if (NewWeaponPrimaryDataAsset->FireSound)
		{
			FireSound = NewWeaponPrimaryDataAsset->FireSound;
		}
		if (NewWeaponPrimaryDataAsset->DryFireSound)
		{
			DryFireSound = NewWeaponPrimaryDataAsset->DryFireSound;
		}
		if (NewWeaponPrimaryDataAsset->ReloadSound)
		{
			ReloadSound = NewWeaponPrimaryDataAsset->ReloadSound;
		}
		if (NewWeaponPrimaryDataAsset->P_FireMuzzle)
		{
			P_FireMuzzle = NewWeaponPrimaryDataAsset->P_FireMuzzle;
		}
		if (NewWeaponPrimaryDataAsset->AM_Fire)
		{
			AM_Fire = NewWeaponPrimaryDataAsset->AM_Fire;
		}
		if (NewWeaponPrimaryDataAsset->AM_WeaponFire)
		{
			AM_WeaponFire = NewWeaponPrimaryDataAsset->AM_WeaponFire;
		}
		if (NewWeaponPrimaryDataAsset->AM_Reload)
		{
			AM_Reload = NewWeaponPrimaryDataAsset->AM_Reload;
		}
		if (NewWeaponPrimaryDataAsset->AM_WeaponReload)
		{
			AM_WeaponReload = NewWeaponPrimaryDataAsset->AM_WeaponReload;
		}

		WeaponPrimaryDataAsset = NewWeaponPrimaryDataAsset;

		SetActorHiddenInGame(true);
	}
}

void AWeaponActor::ModifyShadow(bool bShadow)
{
	SkeletalMeshComp->CastShadow = bShadow;
	SkeletalMeshComp->bReceiveMobileCSMShadows = bShadow;
}

EWeaponName AWeaponActor::GetWeaponName() const
{
	return Name;
}

UAnimMontage* AWeaponActor::GetAMFire() const
{
	return AM_Fire;
}

UAnimMontage* AWeaponActor::GetAMReload() const
{
	return AM_Reload;
}


int AWeaponActor::GetAmmo() const
{
	return Ammo;
}

void AWeaponActor::OnFire()
{
	if (Ammo > 0)
	{
		Ammo--;
		if (FireSound)
		{
			UGameplayStatics::PlaySound2D(this, FireSound);
		}

		if (P_FireMuzzle)
		{
			const FVector SocketLocation = SkeletalMeshComp->GetSocketLocation("Fire_FX_Slot");
			const FRotator SocketRotaion = SkeletalMeshComp->GetSocketRotation("Fire_FX_Slot");
			const FVector SocketScale{ 0.5f, 0.5f, 0.5f };
			UGameplayStatics::SpawnEmitterAttached(P_FireMuzzle, SkeletalMeshComp, "Fire_FX_Slot", SocketLocation, SocketRotaion, SocketScale, EAttachLocation::KeepWorldPosition);
		}


		if (AM_WeaponFire)
		{
			SkeletalMeshComp->PlayAnimation(AM_WeaponFire, false);
		}

		//UKismetSystemLibrary::PrintString(this, "On Fire!!!");
		FHitResult FireHit;

		/* Get FireStart and FireEnd */
		// Plan1: Get from camera
		//const FVector FireStart = FirstPersonCameraComponent->GetComponentLocation();
		//const FRotator FireRotator = FirstPersonCameraComponent->GetComponentRotation();
		//const FVector FireEnd = FireStart + FireRotator.Vector() * 30000.f;

		// Plan2: Get from viewport crosshair
		FVector2D Size_ViewPort;
		if (GEngine->GameViewport)
		{
			GEngine->GameViewport->GetViewportSize(Size_ViewPort);
		}
		FVector2D CrossHair2D(Size_ViewPort.X / 2.f, Size_ViewPort.Y / 2.f);
		bool SetScreenToWorld = false;
		FVector CrossHairWP;
		FVector CrossHairWD;
		SetScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(UGameplayStatics::GetPlayerController(this, 0), CrossHair2D, CrossHairWP, CrossHairWD);

		if (SetScreenToWorld)
		{
			const FVector FireStart = CrossHairWP;
			const FVector FireEnd = CrossHairWP + CrossHairWD * 30000.f;

			GetWorld()->LineTraceSingleByChannel(FireHit, FireStart, FireEnd, ECC_Visibility);
			DrawDebugLine(GetWorld(), FireStart, FireEnd, FColor::Red, false, 3.f);
			if (FireHit.bBlockingHit)
			{
				DrawDebugPoint(GetWorld(), FireHit.Location, 6.f, FColor::Red, false, 3.f);
				const auto hitActor = FireHit.GetActor();
				if (!hitActor)
					return;

				bool isEnemy = hitActor->ActorHasTag("Enemy");

				if (Decay_BulletHole && !isEnemy)
				{
					FRotator TempRotator = UKismetMathLibrary::Conv_VectorToRotator(FireHit.ImpactNormal);
					UGameplayStatics::SpawnDecalAtLocation(this, Decay_BulletHole, FVector{ 4, 4, 4 }, FireHit.Location, UKismetMathLibrary::Conv_VectorToRotator(FireHit.ImpactNormal), 5.0f);
				}

				if (isEnemy)
				{
					UGameplayStatics::ApplyPointDamage(hitActor, static_cast<float>(ATK), FVector{FireEnd - FireStart}, FireHit, UGameplayStatics::GetPlayerController(this, 0), this, UGunDamageType::StaticClass());
					Cast<AFirstPersonFPSCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0))->HitEnemy();
				}
			}
		}
	}
}

float AWeaponActor::GetRate() const
{
	return Rate;
}

float AWeaponActor::GetRecoilValue() const
{
	return RecoilValue;
}

float AWeaponActor::GetMaxAmmo() const
{
	return MaxAmmo;
}

float AWeaponActor::GetReloadTime() const
{
	return ReloadTime;
}

bool AWeaponActor::IsFullAmmo() const
{
	return Ammo == MaxAmmo;
}

int AWeaponActor::Reload(const int PlayerAmmo)
{
	const int ReloadAmmo = FMath::Min(PlayerAmmo, MaxAmmo - Ammo);
	Ammo += ReloadAmmo;

	if (AM_WeaponReload)
	{
		SkeletalMeshComp->PlayAnimation(AM_WeaponReload, false);
	}

	if (ReloadSound)
	{
		UGameplayStatics::PlaySound2D(this, ReloadSound);
	}

	// return rest player ammo
	return PlayerAmmo - ReloadAmmo;
}

void AWeaponActor::PlayDryFireSound() const
{
	if (DryFireSound)
	{
		UGameplayStatics::PlaySound2D(this, DryFireSound);
	}
}

// Called when the game starts or when spawned
void AWeaponActor::BeginPlay()
{
	Super::BeginPlay();
	//UKismetSystemLibrary::PrintString(this, "Hello!!");
}

// Called every frame
void AWeaponActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

