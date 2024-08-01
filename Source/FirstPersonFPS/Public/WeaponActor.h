// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Wepon_PrimaryDataAsset.h"
#include "WeaponActor.generated.h"

UCLASS()
class FIRSTPERSONFPS_API AWeaponActor : public AActor
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class USceneComponent* SceneRootComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class USkeletalMeshComponent* SkeletalMeshComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UWepon_PrimaryDataAsset* WeaponPrimaryDataAsset;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sound")
	class USoundCue* FireSound;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sound")
	class USoundCue* DryFireSound;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sound")
	class USoundCue* ReloadSound;

	class UParticleSystem* P_FireMuzzle;

	float ReloadTime;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Anim",meta=(AllowPrivateAccess="true"))
	class UAnimMontage* AM_Fire;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Anim",meta=(AllowPrivateAccess="true"))
	class UAnimSequence* AM_WeaponFire;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Anim",meta=(AllowPrivateAccess="true"))
	class UAnimMontage* AM_Reload;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Anim",meta=(AllowPrivateAccess="true"))
	class UAnimSequence* AM_WeaponReload;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Material",meta=(AllowPrivateAccess="true"))
	class UMaterial* Decay_BulletHole;

private:
	int ATK;
	int MaxAmmo;
	int Ammo;
	float Rate;
	float RecoilValue;
	enum class EWeaponName Name;
	enum class EWeaponFirstType FirstType;

public:	
	// Sets default values for this actor's properties
	AWeaponActor();

	// Initialize with Weapon Primary Data Asset
	UFUNCTION(BlueprintCallable)
	void Init(class UWepon_PrimaryDataAsset* NewWeaponPrimaryDataAsset, int CurAmmo);

	// Enable/Disable shadow
	UFUNCTION()
	void ModifyShadow(bool bShadow);

	UFUNCTION(BlueprintCallable)
	EWeaponName GetWeaponName() const;

	UFUNCTION()
	class UAnimMontage* GetAMFire() const;

	UFUNCTION()
	class UAnimMontage* GetAMReload() const;

	UFUNCTION(BlueprintCallable)
	int GetAmmo() const;

	UFUNCTION()
	void OnFire();

	UFUNCTION()
	float GetRate() const;

	UFUNCTION()
	float GetRecoilValue() const;

	UFUNCTION()
	float GetMaxAmmo() const;

	UFUNCTION()
	float GetReloadTime() const;

	UFUNCTION()
	bool IsFullAmmo() const;

	UFUNCTION()
	int Reload(const int PlayerAmmo);

	UFUNCTION()
	void PlayDryFireSound() const;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
