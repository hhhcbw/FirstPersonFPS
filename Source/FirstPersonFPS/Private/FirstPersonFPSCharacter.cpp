// Fill out your copyright notice in the Description page of Project Settings.


#include "FirstPersonFPSCharacter.h"
#include "WeaponActor.h"
#include "PickUpActor.h"
#include "Camera/CameraComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "DrawDebugHelpers.h"
#include "Math/Color.h"
#include "Blueprint/UserWidget.h"
#include "BaseUserWidget.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Animation/AnimMontage.h"
#include "Components/PawnNoiseEmitterComponent.h"

//FHitResult Items_HitResult;

// Sets default values
AFirstPersonFPSCharacter::AFirstPersonFPSCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//GetCapsuleComponent()->SetNotifyRigidBodyCollision(true);

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-39.56f, 1.75f, 64.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a SkeletalMeshComponent
	FirstPersonSkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FirstPersonSkeletalMesh"));
	FirstPersonSkeletalMeshComponent->SetupAttachment(FirstPersonCameraComponent);
	//FirstPersonSkeletalMeshComponent->CastShadow = false; // will be set false in blueprint

	// Create a NoiseEmitterComponent
	FirstPersonNoiseEmitterComponent = CreateDefaultSubobject<UPawnNoiseEmitterComponent>(TEXT("FirstPersonNoiseEmitter"));

	// Create a TimeLineComponent
	FireTimeLine = CreateDefaultSubobject<UTimelineComponent>("FireTimeLine");
	FireRecoilTimeLine = CreateDefaultSubobject<UTimelineComponent>("FireRecoilTimeLine");

	GetCharacterMovement()->MaxWalkSpeed = 300.0f;

	// Initialize Member Variable
	CurWeaponFirstType = EWeaponFirstType::NoWeapon;
	b_Fire = b_isOnFire = false;
	b_IsPlayingFootStepSound = false;
	b_isOnReload = false;
	EquippedWeapons.Init(nullptr, 4);
	EquippedWeaponsAmmos.Init(200, 4);

}

// Called when the game starts or when spawned
void AFirstPersonFPSCharacter::BeginPlay()
{
	Super::BeginPlay();
	APlayerController* PlayerControl = Cast<APlayerController>(Controller);
	GetCapsuleComponent()->OnComponentHit.AddDynamic(this, &AFirstPersonFPSCharacter::OnComponentHit);

	CurHealth = MaxHealth;

	//if (AimCurve)
	//{
	//	AimTimeLine = CreateDefaultSubobject<UTimelineComponent>("AimTimeLine");
	//	FOnTimelineFloat AimTimeLineFloat;
	//	AimTimeLineFloat.BindUFunction
	//}

	if (PlayerControl)
	{
		BaseHUD = CreateWidget<UBaseUserWidget>(PlayerControl, BaseUserWidget);
		if (BaseHUD)
		{
			BaseHUD->AddToViewport();
		}
		UpdateAmmoOfHUD();
		BaseHUD->UpdateHealth(CurHealth, MaxHealth);
	}

	if (FireCurve)
	{
		FireTimeLineFloat.BindUFunction(this, "FireCHUpdate");
		FireTimeLine->AddInterpFloat(FireCurve, FireTimeLineFloat);
		FireTimeLineFinish.BindUFunction(this, "FireCHFinish");
		FireTimeLine->SetTimelineFinishedFunc(FireTimeLineFinish);
	}

	if (FireRecoilCurve)
	{
		FireRecoilTimeLineFloat.BindUFunction(this, "FireRecoilUpdate");
		FireRecoilTimeLine->AddInterpFloat(FireRecoilCurve, FireRecoilTimeLineFloat);
		FireRecoilTimeLineFinish.BindUFunction(this, "FireRecoilFinish");
		FireRecoilTimeLine->SetTimelineFinishedFunc(FireRecoilTimeLineFinish);
	}
}

// Called every frame
void AFirstPersonFPSCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	FHitResult ItemHitResult;
	bool bHit = ItemsTraceLine(ItemHitResult);
	BaseHUD->UpdateHealth(CurHealth, MaxHealth); // TODO: Update better method
	if (bHit)
	{
		APickUpActor* CurDetectedPickUpActor = Cast<APickUpActor>(ItemHitResult.GetActor());
		if (CurDetectedPickUpActor)
		{
			if (LastDetectedPickUpActor != CurDetectedPickUpActor)
			{
				if (LastDetectedPickUpActor)
				{
					LastDetectedPickUpActor->SetWidgetVisibility(false);
				}
				CurDetectedPickUpActor->SetWidgetVisibility(true);
				LastDetectedPickUpActor = CurDetectedPickUpActor;
			}
		}
		else
		{
			if (LastDetectedPickUpActor)
			{
				LastDetectedPickUpActor->SetWidgetVisibility(false);
				LastDetectedPickUpActor = nullptr;
			}
		}
	}
	else
	{
		if (LastDetectedPickUpActor)
		{
			LastDetectedPickUpActor->SetWidgetVisibility(false);
			LastDetectedPickUpActor = nullptr;
		}
	}
	//UKismetSystemLibrary::PrintString(this, FString::FromInt(MaxHealth));
}

void AFirstPersonFPSCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
		HandleMoveSound();
	}
}

void AFirstPersonFPSCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
		HandleMoveSound();
	}
}


void AFirstPersonFPSCharacter::Run()
{
	GetCharacterMovement()->MaxWalkSpeed = 600.0f;
}

void AFirstPersonFPSCharacter::StopRun()
{
	GetCharacterMovement()->MaxWalkSpeed = 300.0f;
}

void AFirstPersonFPSCharacter::Init()
{
	UGameplayStatics::GetGameInstance(this);

}

// Call On Fire
void AFirstPersonFPSCharacter::OnFire()
{
	b_Fire = true;
	if (CurWeaponFirstType == EWeaponFirstType::NoWeapon || b_isOnFire || b_isOnReload)
	{
		// No armed weapon or is fire or is reload, can't fire.
		return;
	}

	const unsigned int EquippedWeaponsIndex = static_cast<unsigned int>(CurWeaponFirstType);
	if (EquippedWeaponsIndex > 0 && EquippedWeaponsIndex < 5 && EquippedWeapons[EquippedWeaponsIndex - 1])
	{ 
		AWeaponActor* EquippedWeapon = EquippedWeapons[EquippedWeaponsIndex - 1];
		const int EquippedWeaponAmmo = EquippedWeapon->GetAmmo();
		if (EquippedWeaponAmmo > 0)
		{
			// Fire
			b_isOnFire = true;
			const float FireDuration = 0.5f / EquippedWeapon->GetRate();
			GetWorldTimerManager().SetTimer(FireTimer, this, &AFirstPersonFPSCharacter::EndFire, FireDuration);
			EquippedWeapon->OnFire();
			FireTimeLine->Play();
			FireRecoilTimeLine->Play();
			MakeNoise(1.0, nullptr, GetActorLocation());
			
			// Update HUD
			BaseHUD->UpdateWeaponAmmo(EquippedWeapon->GetAmmo());

			// Play Animation
			UAnimInstance* PlayerAnimInstance = FirstPersonSkeletalMeshComponent->GetAnimInstance();
			if (PlayerAnimInstance)
			{
				PlayerAnimInstance->Montage_Play(EquippedWeapon->GetAMFire());
			}
		}
		else
		{
			// Play Sound
			EquippedWeapon->PlayDryFireSound();
		}
	}
}

// Finish fire
void AFirstPersonFPSCharacter::EndFire()
{
	b_isOnFire = false;
	if (b_Fire)
	{ 
		OnFire();
	}
}

// Stop fire
void AFirstPersonFPSCharacter::StopFire()
{
	//UKismetSystemLibrary::PrintString(this, "Stop Fire!!!");
	b_Fire = false;
	FireTimeLine->Reverse();
}

void AFirstPersonFPSCharacter::PickUp()
{
	if (LastDetectedPickUpActor)
	{
		UWepon_PrimaryDataAsset* WeaponPDA = LastDetectedPickUpActor->GetWeaponPrimaryDataAsset();
		if (WeaponPDA)
		{
			const EWeaponFirstType WeaponFirstType = WeaponPDA->FirstType;
			const unsigned int EquippedWeaponsIndex = static_cast<unsigned int>(WeaponFirstType);
			bool b_isPickUp = false;
			if (EquippedWeaponsIndex > 0 && EquippedWeaponsIndex < 5 && EquippedWeapons[EquippedWeaponsIndex - 1] == nullptr)
			{
				const int CurAmmo = LastDetectedPickUpActor->GetAmmo();
				b_isPickUp = SpawnWeapon(EquippedWeaponsIndex, WeaponPDA, CurAmmo);
				if (b_isPickUp)
				{
					if (CurWeaponFirstType == EWeaponFirstType::NoWeapon)
					{
						CurWeaponFirstType = WeaponFirstType;
						AWeaponActor* EquippedWeapon = EquippedWeapons[EquippedWeaponsIndex - 1];
						EquippedWeapon->SetActorHiddenInGame(false);
						UpdateAmmoOfHUD();
					}
				}
			}

			if (b_isPickUp)
			{
				if (LastDetectedPickUpActor->Destroy())
				{
					LastDetectedPickUpActor = nullptr;
				}
			}
		}
	}
}

bool AFirstPersonFPSCharacter::ItemsTraceLine(FHitResult& HitResult)
{
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
	const FVector ItemsTraceStart = CrossHairWP;
	const FVector ItemsTraceEnd = CrossHairWP + CrossHairWD * 300.f;


	GetWorld()->LineTraceSingleByChannel(HitResult, ItemsTraceStart, ItemsTraceEnd, ECC_Visibility);

	return HitResult.bBlockingHit;
}

bool AFirstPersonFPSCharacter::SpawnWeapon(const int EquippedWeaponsIndex, UWepon_PrimaryDataAsset* WeaponPDA, const int CurAmmo)
{
	AWeaponActor*& EquippedWeapon = EquippedWeapons[EquippedWeaponsIndex - 1];
	if (EquippedWeapon == nullptr && WeaponPDA != nullptr)
	{
		EquippedWeapon = GetWorld()->SpawnActor<AWeaponActor>();
		EquippedWeapon->Init(WeaponPDA, CurAmmo);
		EquippedWeapon->ModifyShadow(false);

		const USkeletalMeshSocket* WeaponSocket = FirstPersonSkeletalMeshComponent->GetSocketByName("hand_rWeaponSocket");
		WeaponSocket->AttachActor(EquippedWeapon, FirstPersonSkeletalMeshComponent);
		// Temporal handle
		if (EquippedWeaponsIndex == 2)
		{
			EquippedWeapon->SetActorRelativeLocation(FVector{ 1.31, 4.88, 0.61 });
			EquippedWeapon->SetActorRelativeRotation(FQuat{ FRotator{-0.36, -9.68, -12.60} });
		}
		return true;
	}

	return false;
}

void AFirstPersonFPSCharacter::FireCHUpdate(float Alpha)
{
	BaseHUD->UpdateCHTranslation(Alpha, 10.f);
}

void AFirstPersonFPSCharacter::FireCHFinish()
{

}

void AFirstPersonFPSCharacter::SwitchWeaponArmed(EWeaponFirstType SwitchWeaponFirstType)
{
	if (SwitchWeaponFirstType == CurWeaponFirstType || b_isOnReload)
	{
		// Same Weapon First Type or is reloading, no switch.
		return;
	}

	if (IsWeaponValid(SwitchWeaponFirstType))
	{
		FirstPersonSkeletalMeshComponent->GetAnimInstance()->Montage_Stop(0.3);
		HiddenWeaponNoArmed(true);
		CurWeaponFirstType = SwitchWeaponFirstType;
		UpdateAmmoOfHUD();
		HiddenWeaponNoArmed(false);
	}
}

void AFirstPersonFPSCharacter::HiddenWeaponNoArmed(bool bHiddenWeapon)
{
	const unsigned int EquippedWeaponsIndex = static_cast<unsigned int>(CurWeaponFirstType);
	if (EquippedWeaponsIndex > 0 && EquippedWeaponsIndex < 5 && EquippedWeapons[EquippedWeaponsIndex - 1])
	{
		EquippedWeapons[EquippedWeaponsIndex - 1]->SetActorHiddenInGame(bHiddenWeapon);
	}
}

void AFirstPersonFPSCharacter::UpdateAmmoOfHUD()
{
	const unsigned int EquippedWeaponsIndex = static_cast<unsigned int>(CurWeaponFirstType);
	if (EquippedWeaponsIndex > 0 && EquippedWeaponsIndex < 5 && EquippedWeapons[EquippedWeaponsIndex - 1])
	{
		BaseHUD->UpdatePlayerAmmo(EquippedWeaponsAmmos[EquippedWeaponsIndex - 1]);
		BaseHUD->UpdateWeaponAmmo(EquippedWeapons[EquippedWeaponsIndex - 1]->GetAmmo());
	}
	else
	{
		BaseHUD->UpdatePlayerAmmo(0);
		BaseHUD->UpdateWeaponAmmo(0);
	}
}

bool AFirstPersonFPSCharacter::IsWeaponValid(EWeaponFirstType CheckedWeaponFirstType) const
{
	const unsigned int EquippedWeaponsIndex = static_cast<unsigned int>(CheckedWeaponFirstType);
	if (EquippedWeaponsIndex > 0 && EquippedWeaponsIndex < 5)
	{
		return EquippedWeapons[EquippedWeaponsIndex - 1] != nullptr;
	}

	return EquippedWeaponsIndex == 0;
}

void AFirstPersonFPSCharacter::HandleMoveSound()
{
	FVector CharacterVelocity = GetVelocity();
	float f_Speed = CharacterVelocity.Size();
	if (f_Speed > 0.f && !GetCharacterMovement()->IsFalling())
	{
		if (!b_IsPlayingFootStepSound)
		{
			//UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("Speed is %f"), f_Speed));
			b_IsPlayingFootStepSound = true;
			UGameplayStatics::PlaySound2D(this, FootStepSound, 0.2);
			GetWorld()->GetTimerManager().SetTimer(FootStepTimer, this, &AFirstPersonFPSCharacter::EndPlayingFootStepSound,300.f / FMath::Max(300.f, f_Speed));
		}
	}
}

void AFirstPersonFPSCharacter::FireRecoilUpdate(float Alpha)
{
	AWeaponActor* CurWeaponActor = GetCurWeaponActor();
	if (CurWeaponActor)
	{
		const float RecoilValue = CurWeaponActor->GetRecoilValue();
		if (!FireRecoilTimeLine->IsReversing())
		{
			AddControllerPitchInput(FMath::Lerp(0.f, -RecoilValue, Alpha));
		}
		else
		{
			AddControllerPitchInput(FMath::Lerp(RecoilValue, 0.f, Alpha));
			//AddControllerYawInput(FMath::RandRange(-RecoilValue, RecoilValue));
		}
	}
}

void AFirstPersonFPSCharacter::FireRecoilFinish()
{
	FireRecoilTimeLine->Reverse();
}

AWeaponActor* AFirstPersonFPSCharacter::GetCurWeaponActor() const
{
	const unsigned int EquippedWeaponsIndex = static_cast<unsigned int>(CurWeaponFirstType);
	if (EquippedWeaponsIndex > 0 && EquippedWeaponsIndex < 5 && EquippedWeapons[EquippedWeaponsIndex - 1])
	{
		return EquippedWeapons[EquippedWeaponsIndex - 1];
	}

	return nullptr;
}

void AFirstPersonFPSCharacter::Reload()
{
	if (CurWeaponFirstType != EWeaponFirstType::NoWeapon && !b_isOnReload)
	{
		const unsigned int EquippedWeaponsIndex = static_cast<unsigned int>(CurWeaponFirstType);
		if (EquippedWeaponsIndex > 0 && EquippedWeaponsIndex < 5 && EquippedWeapons[EquippedWeaponsIndex - 1])
		{
			AWeaponActor* EquippedWeapon = EquippedWeapons[EquippedWeaponsIndex - 1];
			int& WeaponAmmo = EquippedWeaponsAmmos[EquippedWeaponsIndex - 1];
			if (WeaponAmmo > 0 && !EquippedWeapon->IsFullAmmo())
			{
				// True Reload
				b_isOnReload = true;
				WeaponAmmo = EquippedWeapon->Reload(WeaponAmmo);
				
				// Set Timer
				float ReloadDuration = EquippedWeapon->GetReloadTime();
				GetWorldTimerManager().SetTimer(ReloadTimer, this, &AFirstPersonFPSCharacter::EndReload, ReloadDuration);

				// Update HUD
				UpdateAmmoOfHUD();

				// Play Animation
				UAnimInstance* PlayerAnimInstance = FirstPersonSkeletalMeshComponent->GetAnimInstance();
				if (PlayerAnimInstance)
				{
					PlayerAnimInstance->Montage_Play(EquippedWeapon->GetAMReload());
				}
			}
		}
	}
}

void AFirstPersonFPSCharacter::EndReload()
{
	b_isOnReload = false;
}

EWeaponName AFirstPersonFPSCharacter::GetCurWeaponName() const
{
	const unsigned int EquippedWeaponsIndex = static_cast<unsigned int>(CurWeaponFirstType);
	if (EquippedWeaponsIndex > 0 && EquippedWeaponsIndex < 5 && EquippedWeapons[EquippedWeaponsIndex - 1])
	{
		return EquippedWeapons[EquippedWeaponsIndex - 1]->GetWeaponName();
	}

	return EWeaponName::NoName;
}

// Called to bind functionality to input
void AFirstPersonFPSCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Bind jump events
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	// Bind crouch events
	PlayerInputComponent->BindAction<FCrouchDelegate>("Crouch", IE_Pressed, this, &ACharacter::Crouch, false);
	PlayerInputComponent->BindAction<FCrouchDelegate>("Crouch", IE_Released, this, &ACharacter::UnCrouch, false);

	// Bind pick up events
	PlayerInputComponent->BindAction("PickUp", IE_Pressed, this, &AFirstPersonFPSCharacter::PickUp);

	// Bind fire events
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AFirstPersonFPSCharacter::OnFire);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &AFirstPersonFPSCharacter::StopFire);

	// Bind reload events
	PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &AFirstPersonFPSCharacter::Reload);

	// Bind switch weapon events
	PlayerInputComponent->BindAction<FSwitchWeaponDelegate>("SwitchWeapon0", IE_Pressed, this, &AFirstPersonFPSCharacter::SwitchWeaponArmed, EWeaponFirstType::NoWeapon);
	PlayerInputComponent->BindAction<FSwitchWeaponDelegate>("SwitchWeapon1", IE_Pressed, this, &AFirstPersonFPSCharacter::SwitchWeaponArmed, EWeaponFirstType::PrimaryWeapon);
	PlayerInputComponent->BindAction<FSwitchWeaponDelegate>("SwitchWeapon2", IE_Pressed, this, &AFirstPersonFPSCharacter::SwitchWeaponArmed, EWeaponFirstType::SecondaryWeapon);

	// Bind Run
	PlayerInputComponent->BindAction("Run", IE_Pressed, this, &AFirstPersonFPSCharacter::Run);
	PlayerInputComponent->BindAction("Run", IE_Released, this, &AFirstPersonFPSCharacter::StopRun);

	// Bind MoveForward
	PlayerInputComponent->BindAxis("MoveForward", this, &AFirstPersonFPSCharacter::MoveForward);
	// Bind MoveRight
	PlayerInputComponent->BindAxis("MoveRight", this, &AFirstPersonFPSCharacter::MoveRight);

	// Bind Turn
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	// Bind LookUp
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
}

void AFirstPersonFPSCharacter::HitEnemy()
{
	BaseHUD->PlayHitMarkerAnim();
}

float AFirstPersonFPSCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
	class AController* EventInstigator, AActor* DamageCauser)
{
	BaseHUD->PlayDamageAnim();
	ReduceHealth(DamageAmount);
	return DamageAmount;
}

void AFirstPersonFPSCharacter::ReduceHealth(int ReducedHealthAmount)
{
	CurHealth -= ReducedHealthAmount;
	BaseHUD->UpdateHealth(CurHealth, MaxHealth);
	if (CurHealth <= 0)
	{
		OnDead();
	}
}

void AFirstPersonFPSCharacter::OnComponentHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	//UKismetSystemLibrary::PrintString(this, "On Hit!!!");
	//ReduceHealth(5);
}

void AFirstPersonFPSCharacter::OnDead()
{
	UKismetSystemLibrary::PrintString(this, "You Died!!");
}

void AFirstPersonFPSCharacter::EndPlayingFootStepSound()
{
	b_IsPlayingFootStepSound = false;
}
