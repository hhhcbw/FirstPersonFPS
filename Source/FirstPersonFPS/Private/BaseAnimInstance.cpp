// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseAnimInstance.h"
#include "FirstPersonFPSCharacter.h"
#include "GameFramework/Actor.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

void UBaseAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	BaseCharacter = Cast<AFirstPersonFPSCharacter>(TryGetPawnOwner());
	WeaponName = EWeaponName::NoName;
}

void UBaseAnimInstance::UpdateAnim(float DeltaTime)
{
	if (BaseCharacter == nullptr)
	{
		BaseCharacter = Cast<AFirstPersonFPSCharacter>(TryGetPawnOwner());
	}
	else
	{
		FVector CharacterVelocity = BaseCharacter->GetVelocity();
		f_Speed = CharacterVelocity.Size();
		b_IsInAir = BaseCharacter->GetCharacterMovement()->IsFalling();
		f_Acceleration = BaseCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size();
		if (f_Acceleration > 0.f)
		{
			b_IsMove = true;
		}
		else
		{
			b_IsMove = false;
		}

		WeaponName = BaseCharacter->GetCurWeaponName();		
	}
}
