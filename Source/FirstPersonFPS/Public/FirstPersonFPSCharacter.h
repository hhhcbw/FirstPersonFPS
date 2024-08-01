// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/TimelineComponent.h"
#include "Wepon_PrimaryDataAsset.h"
#include "FirstPersonFPSCharacter.generated.h"

DECLARE_DELEGATE_OneParam(FCrouchDelegate, bool);
DECLARE_DELEGATE_OneParam(FSwitchWeaponDelegate, EWeaponFirstType);

UCLASS()
class FIRSTPERSONFPS_API AFirstPersonFPSCharacter : public ACharacter
{
	GENERATED_BODY()

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FirstPersonCameraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh", meta = (AllowPrivateAccess = "true"))
	class USkeletalMeshComponent* FirstPersonSkeletalMeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI", meta = (AllowPrivateAccess = "true"))
	class UPawnNoiseEmitterComponent* FirstPersonNoiseEmitterComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	TArray<class AWeaponActor*> EquippedWeapons;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	TArray<int> EquippedWeaponsAmmos;

	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	//class AWeaponActor* PrimaryWeapon;

	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	//class AWeaponActor* SecondaryWeapon;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fire", meta = (AllowPrivateAccess = "true"))
	//class UCurveFloat* AimCurve;
	//class UTimelineComponent* AimTimeLine;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Fire",meta=(AllowPrivateAccess="true"))
	class UCurveFloat* FireCurve;
	class UTimelineComponent* FireTimeLine;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Fire",meta=(AllowPrivateAccess="true"))
	FOnTimelineFloat FireTimeLineFloat;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Fire",meta=(AllowPrivateAccess="true"))
	FOnTimelineEvent FireTimeLineFinish;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Fire",meta=(AllowPrivateAccess="true"))
	class UCurveFloat* FireRecoilCurve;
	class UTimelineComponent* FireRecoilTimeLine;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Fire",meta=(AllowPrivateAccess="true"))
	FOnTimelineFloat FireRecoilTimeLineFloat;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Fire",meta=(AllowPrivateAccess="true"))
	FOnTimelineEvent FireRecoilTimeLineFinish;

public:
	// Sets default values for this character's properties
	AFirstPersonFPSCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION()
		void HitEnemy();

public:
	//virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
	//	class AController* EventInstigator, AActor* DamageCauser) override;

	/** Returns FirstPersonCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

	UFUNCTION()
	EWeaponName GetCurWeaponName() const;

protected:
	void ReduceHealth(int ReducedHealthAmount);

	UFUNCTION(BlueprintCallable, Category = "OnEvent")
	void OnComponentHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit);

	/** Handles moving forward/backward */
	void MoveForward(float Val);

	/** Handles stafing movement, left and right */
	void MoveRight(float Val);

	void Run();

	void StopRun();

	UFUNCTION()
	void Init();

	UFUNCTION()
	void OnFire();

	void EndFire();

	void StopFire();

	void PickUp();

	UFUNCTION()
	bool ItemsTraceLine(FHitResult& HitResult);

	UFUNCTION(BlueprintCallable)
	bool SpawnWeapon(const int EquippedWeaponsIndex, class UWepon_PrimaryDataAsset* WeaponPDA, const int CurAmmo);

	UFUNCTION()
	void FireCHUpdate(float Alpha);

	UFUNCTION()
	void FireCHFinish();

	UFUNCTION(BlueprintCallable)
	void SwitchWeaponArmed(EWeaponFirstType SwitchWeaponFirstType);

	UFUNCTION()
	void HiddenWeaponNoArmed(bool bHiddenWeapon);

	UFUNCTION()
	void UpdateAmmoOfHUD();

	// Check if we have relative weapon
	UFUNCTION()
	bool IsWeaponValid(EWeaponFirstType CheckedWeaponFirstType) const;

	UFUNCTION()
	void HandleMoveSound();

	UFUNCTION()
	void FireRecoilUpdate(float Alpha);

	UFUNCTION()
	void FireRecoilFinish();

	UFUNCTION()
	AWeaponActor* GetCurWeaponActor() const;

	UFUNCTION()
	void Reload();

	UFUNCTION()
	void EndReload();

	// 重载接受伤害函数
	UFUNCTION()
		virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
			class AController* EventInstigator, AActor* DamageCauser) override;

private:
	void OnDead();

	void EndPlayingFootStepSound();

public:
	// UI
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<class UBaseUserWidget> BaseUserWidget;
	class UBaseUserWidget* BaseHUD;

	// Sound
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sound", meta = (AllowPrivateAccess = "true"))
	class USoundBase* FootStepSound;

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Property", meta = (AllowPrivateAccess = "true"))
	int MaxHealth;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Property", meta = (AllowPrivateAccess = "true"))
	int CurHealth;

	class APickUpActor* LastDetectedPickUpActor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Weapon", meta=(AllowPrivateAccess="true"))
	EWeaponFirstType CurWeaponFirstType;

	bool b_Fire;
	bool b_isOnFire;
	bool b_isOnReload;

	FTimerHandle FireTimer;
	FTimerHandle ReloadTimer;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Timer", meta = (AllowPrivateAccess = "true"))
	FTimerHandle FootStepTimer;

	bool b_IsPlayingFootStepSound;
};
