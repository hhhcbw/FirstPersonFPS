// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BaseUserWidget.generated.h"

/**
 * 
 */
UCLASS()
class FIRSTPERSONFPS_API UBaseUserWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	
	UFUNCTION()
	void UpdateCHTranslation(float Alpha, float Value);

	UFUNCTION()
	void UpdateWeaponAmmo(int WeaponAmmo);

	UFUNCTION()
	void UpdatePlayerAmmo(int PlayerAmmo);

	UFUNCTION()
	void UpdateHealth(int Health, int MaxHealth);

	UFUNCTION()
	void PlayDamageAnim();

	UFUNCTION()
	void PlayHitMarkerAnim();

	UFUNCTION()
	UWidgetAnimation* GetNameWidgetAnimation(FString& InWidgetAnimName);

private:
	UPROPERTY(Transient, meta=(BindWidget))
	class UImage* CrossHair_L;

	UPROPERTY(Transient, meta=(BindWidget))
	class UImage* CrossHair_R;

	UPROPERTY(Transient, meta=(BindWidget))
	class UImage* CrossHair_U;

	UPROPERTY(Transient, meta=(BindWidget))
	class UImage* CrossHair_D;

	UPROPERTY(Transient, meta=(BindWidget))
	class UTextBlock* WeaponAmmoText;

	UPROPERTY(Transient, meta=(BindWidget))
	class UTextBlock* PlayerAmmoText;

	UPROPERTY(Transient, meta=(BindWidget))
	class UProgressBar* HealthBar;

	UPROPERTY(Transient, meta=(BindWidget))
	class UTextBlock* HealthText;

	UPROPERTY(Transient, meta=(BindWidget))
	class UImage* TakeDamage;

	FString TakeDamageAnimName;
	FString HitMarkerAnimName;

	//UPROPERTY(Transient, meta=(BindWidget))
	//class UWidgetAnimation* Anim_TakeDamage;
};
