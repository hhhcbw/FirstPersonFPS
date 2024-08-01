// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseUserWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "Animation/WidgetAnimation.h"


void UBaseUserWidget::NativeConstruct()
{ 
	Super::NativeConstruct();
	TakeDamageAnimName = "Anim_TakeDamage_INST";
	HitMarkerAnimName = "Anim_HitMarker_INST";
}

void UBaseUserWidget::UpdateCHTranslation(float Alpha, float Value)
{
	float CrossHair_L_x = 0.f;
	float CrossHair_R_x = 0.f;
	float CrossHair_U_y = 0.f;
	float CrossHair_D_y = 0.f;

	//float CrossHair_L_x = CrossHair_L->RenderTransform.Translation.X;
	//float CrossHair_R_x = CrossHair_R->RenderTransform.Translation.X;
	//float CrossHair_U_y = CrossHair_U->RenderTransform.Translation.Y;
	//float CrossHair_D_y = CrossHair_D->RenderTransform.Translation.Y;

	CrossHair_L_x = FMath::Lerp(CrossHair_L_x, CrossHair_L_x - Value, Alpha);
	CrossHair_R_x = FMath::Lerp(CrossHair_R_x, CrossHair_R_x + Value, Alpha);
	CrossHair_U_y = FMath::Lerp(CrossHair_U_y, CrossHair_U_y - Value, Alpha);
	CrossHair_D_y = FMath::Lerp(CrossHair_D_y, CrossHair_D_y + Value, Alpha);

	CrossHair_L->SetRenderTranslation(FVector2D(CrossHair_L_x, 0.f));
	CrossHair_R->SetRenderTranslation(FVector2D(CrossHair_R_x, 0.f));
	CrossHair_U->SetRenderTranslation(FVector2D(0.f, CrossHair_U_y));
	CrossHair_D->SetRenderTranslation(FVector2D(0.f, CrossHair_D_y));
}

void UBaseUserWidget::UpdateWeaponAmmo(int WeaponAmmo)
{
	FString WeaponAmmo_Text = FString::Printf(TEXT("%03d"), WeaponAmmo);
	WeaponAmmoText->SetText(FText::FromString(WeaponAmmo_Text));
}

void UBaseUserWidget::UpdatePlayerAmmo(int PlayerAmmo)
{
	FString PlayerAmmo_Text = FString::Printf(TEXT("%03d"), PlayerAmmo);
	PlayerAmmoText->SetText(FText::FromString(PlayerAmmo_Text));
}

void UBaseUserWidget::UpdateHealth(int Health, int MaxHealth)
{
	FString Health_Text = FString::Printf(TEXT("%d"), Health);
	HealthText->SetText(FText::FromString(Health_Text));

	float HealthRatio = static_cast<float>(Health) / static_cast<float>(MaxHealth);
	HealthBar->SetPercent(HealthRatio);
}

void UBaseUserWidget::PlayDamageAnim()
{
	//PlayAnimation(Anim_TakeDamage);
	PlayAnimation(GetNameWidgetAnimation(TakeDamageAnimName));
}

void UBaseUserWidget::PlayHitMarkerAnim()
{
	PlayAnimation(GetNameWidgetAnimation(HitMarkerAnimName));
}

UWidgetAnimation* UBaseUserWidget::GetNameWidgetAnimation(FString& InWidgetAnimName)
{
	if (UWidgetBlueprintGeneratedClass* WidgetBlueprintGenerated = Cast<UWidgetBlueprintGeneratedClass>(GetClass()))
	{
		TArray<UWidgetAnimation*> TArrayAnimations = WidgetBlueprintGenerated->Animations;

		UWidgetAnimation** MyTempAnimation = TArrayAnimations.FindByPredicate(
			[&](const UWidgetAnimation* OurAnimation)
			{
				return OurAnimation->GetFName().ToString() == (InWidgetAnimName);
			}
		);
		
		return *MyTempAnimation;
	}

	return nullptr;
}
