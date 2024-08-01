// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Engine/SkeletalMesh.h"
#include "Engine/StaticMesh.h"
#include "Sound/SoundCue.h"
#include "Particles/ParticleSystem.h"
#include "Animation/AnimMontage.h"
#include "Animation/AnimSequence.h"
#include "Wepon_PrimaryDataAsset.generated.h"

UENUM(BlueprintType)
enum class EWeaponName : uint8
{
	NoName,
	Ark47,
	G18
};

UENUM(BlueprintType)
enum class EWeaponFirstType : uint8
{
	NoWeapon,
	PrimaryWeapon,
	SecondaryWeapon,
	MeleeWeapon,
	ThrowingWeapon,
};

/**
 * 
 */
UCLASS()
class FIRSTPERSONFPS_API UWepon_PrimaryDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USkeletalMesh* SkeletonMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMesh* StaticMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int ATK;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int MaxAmmo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Rate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float RecoilValue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EWeaponName Name;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EWeaponFirstType FirstType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	USoundCue* FireSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	USoundCue* DryFireSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	USoundCue* ReloadSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UParticleSystem* P_FireMuzzle;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Anim",meta=(AllowPrivateAccess="true"))
	UAnimMontage* AM_Fire;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Anim",meta=(AllowPrivateAccess="true"))
	UAnimSequence* AM_WeaponFire;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Anim",meta=(AllowPrivateAccess="true"))
	UAnimMontage* AM_Reload;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Anim",meta=(AllowPrivateAccess="true"))
	UAnimSequence* AM_WeaponReload;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Attribute",meta=(AllowPrivateAccess="true"))
	float ReloadTime;
};
