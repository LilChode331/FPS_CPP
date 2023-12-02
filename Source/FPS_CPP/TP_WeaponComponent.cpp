// Copyright Epic Games, Inc. All Rights Reserved.


#include "TP_WeaponComponent.h"
#include "FPS_CPPCharacter.h"
#include "FPS_CPPProjectile.h"
#include "GameFramework/PlayerController.h"
#include "Camera/PlayerCameraManager.h"
#include "Kismet/GameplayStatics.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/ProjectileMovementComponent.h"

// Sets default values for this component's properties
UTP_WeaponComponent::UTP_WeaponComponent()
{
	// Default offset from the character location for projectiles to spawn
	MuzzleOffset = FVector(100, 0.0f, 0);
	
}







void UTP_WeaponComponent::Fire()
{
	if (Character == nullptr || Character->GetController() == nullptr)
	{
		return;
	}

	// Try and fire a projectile
	if (ProjectileClass != nullptr)
	{
		UWorld* const World = GetWorld();
		if (World != nullptr)
		{
			APlayerController* PlayerController = Cast<APlayerController>(Character->GetController());
			const FRotator SpawnRotation = PlayerController->PlayerCameraManager->GetCameraRotation();
			// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
			const FVector SpawnLocation = GetOwner()->GetActorLocation() + SpawnRotation.RotateVector(MuzzleOffset);

	
			//Set Spawn Collision Handling Override
			FActorSpawnParameters ActorSpawnParams;
			ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

			AFPS_CPPProjectile* proj = ProjectileClass.GetDefaultObject();
			proj->GetProjectileMovement()->InitialSpeed = projectileSpeed;
			


			// Spawn the projectile at the muzzle
			World->SpawnActor<AFPS_CPPProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, ActorSpawnParams);

			
		}
	}
	
	// Try and play the sound if specified
	/*
	if (FireSound != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, Character->GetActorLocation());
	}
	*/
	// Try and play a firing animation if specified
	if (FireAnimation != nullptr)
	{
		// Get the animation object for the arms mesh
		UAnimInstance* AnimInstance = Character->GetMesh1P()->GetAnimInstance();
		if (AnimInstance != nullptr)
		{
			AnimInstance->Montage_Play(FireAnimation, 1.f);
		}
	}
}

void UTP_WeaponComponent::AttachWeapon(AFPS_CPPCharacter* TargetCharacter)
{
	Character = TargetCharacter;
	if (Character == nullptr)
	{
		return;
	}

	// Attach the weapon to the First Person Character
	FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);
	AttachToComponent(Character->GetMesh1P(), AttachmentRules, FName(TEXT("GripPoint")));
	
	// switch bHasRifle so the animation blueprint can switch to another animation set
	Character->SetHasRifle(true);

	// Set up action bindings
	if (APlayerController* PlayerController = Cast<APlayerController>(Character->GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			// Set the priority of the mapping to 1, so that it overrides the Jump action with the Fire action when using touch input
			Subsystem->AddMappingContext(FireMappingContext, 1);
		}

		if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerController->InputComponent))
		{
			// Fire
			EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Triggered, this, &UTP_WeaponComponent::Fire);
		}
	}
}


void UTP_WeaponComponent::AdjustProjectileSpeed(float SpeedIncrement)
{
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();

	if (PlayerController)
	{
		// Check if key 1 is pressed
		if (PlayerController->IsInputKeyDown(EKeys::One))
		{
			projectileSpeed += 100;
		}
		// Check if key 2 is pressed
		else if (PlayerController->IsInputKeyDown(EKeys::Two))
		{
			projectileSpeed -= 100;
		}
	}
}





void UTP_WeaponComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (Character == nullptr)
	{
		return;
	}

	if (APlayerController* PlayerController = Cast<APlayerController>(Character->GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->RemoveMappingContext(FireMappingContext);
		}
	}
}

void UTP_WeaponComponent::DrawLine()
{
	
	
	//GEngine->AddOnScreenDebugMessage(-1, 0.1f, FColor::Red, "Hello World");
	
	SimulateProjectileTrajectory(5.0f);
	


}








void UTP_WeaponComponent::SimulateProjectileTrajectory(float DeltaTime)
{
	FVector StartLocation = GetOwner()->GetActorLocation();
	FVector CurrentLocation = StartLocation;
	FVector InitialVelocity = GetOwner()->GetActorRightVector() * InitialSpeed;
	FVector Acceleration = FVector(0, 0, Gravity);
	float TimeStep = DeltaTime / Precision;

	FHitResult HitResult;
	bool bReflected = false; // Flag to track whether reflection has occurred


	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (PlayerController)
	{
		// Check if key 4 is pressed
		if (PlayerController->IsInputKeyDown(EKeys::Four))
		{
			Acceleration.X = -980.0f;
		}
		// Check if key 5 is pressed
		else if (PlayerController->IsInputKeyDown(EKeys::Five))
		{
			Acceleration.X = 980.0f;
		}
	}

	for (int i = 0; i < Precision; ++i)
	{
		// Update velocity and position using projectile motion equations
		InitialVelocity += Acceleration * TimeStep;
		FVector NextLocation = CurrentLocation + InitialVelocity * TimeStep + 0.5f * Acceleration * FMath::Square(TimeStep);

		DrawDebugLine(GetWorld(), CurrentLocation, NextLocation, FColor::Green);

		FCollisionQueryParams CollisionParams;
		CollisionParams.AddIgnoredActor(GetOwner()); // Ignore the weapon itself
		bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, CurrentLocation, NextLocation, ECC_Visibility, CollisionParams);

		if (bHit && !bReflected)
		{
			// Handle reflection upon hitting an object
			FVector Normal = HitResult.Normal;
			InitialVelocity = FMath::GetReflectionVector(InitialVelocity, Normal);

			// Move slightly away from the hit point to avoid self-collision
			CurrentLocation = HitResult.Location + Normal * 10.0f;

			bReflected = true; // Set the flag to true after the first reflection
		}
		else
		{
			CurrentLocation = NextLocation;
		}
	}
}

void UTP_WeaponComponent::SetAIDestination(FVector Destination)
{
	AActor* OwnerActor = GetOwner();
	ACharacter* AICharacter = Cast<ACharacter>(OwnerActor);

	if (AICharacter)
	{
		UCharacterMovementComponent* MovementComp = AICharacter->GetCharacterMovement();
		if (MovementComp)
		{
			// Set the AI's destination using its movement component
			AICharacter->SetActorLocation(Destination);
		}
	}
}


