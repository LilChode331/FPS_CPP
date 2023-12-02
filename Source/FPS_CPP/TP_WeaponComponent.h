// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "TP_WeaponComponent.generated.h"

class AFPS_CPPCharacter;

UCLASS(Blueprintable, BlueprintType, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FPS_CPP_API UTP_WeaponComponent : public USkeletalMeshComponent
{
	GENERATED_BODY()

public:
	/** Projectile class to spawn */
	UPROPERTY(EditDefaultsOnly, Category=Projectile)
	TSubclassOf<class AFPS_CPPProjectile> ProjectileClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite , Category = "Gun")
	float projectileSpeed = 5000.0f;


	UFUNCTION(BlueprintCallable, Category = "Gun")
	void AdjustProjectileSpeed(float SpeedIncrement);





	//UFUNCTION(BlueprintCallable, Category = "Gravity9")
	//void GravityChange(float TimeDelta );

	
	
	AFPS_CPPProjectile* MyProjectile;

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void SetAIDestination(FVector Destination);

	/** Sound to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	USoundBase* FireSound;


	
	/** AnimMontage to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	UAnimMontage* FireAnimation;

	/** Gun muzzle's offset from the characters location */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	FVector MuzzleOffset;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	float Offset = 0;

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputMappingContext* FireMappingContext;

	/** Fire Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputAction* FireAction;

	/** Sets default values for this component's properties */
	UTP_WeaponComponent();

	/** Attaches the actor to a FirstPersonCharacter */
	UFUNCTION(BlueprintCallable, Category="Weapon")
	void AttachWeapon(AFPS_CPPCharacter* TargetCharacter);

	/** Make the weapon Fire a Projectile */
	UFUNCTION(BlueprintCallable, Category="Weapon")
	void Fire();

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void DrawLine();

	void ReflectProjectile()
	{

	}
	//float Precision = 0.1f;
	
	 //Projectile Motion **************
	
	UPROPERTY(EditAnywhere)
	float InitialSpeed = 5000.0f; 

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gravity)
	float Gravity = -980.0f;       

	float Precision = 500.0f;
	float ReflectionPrecision = 500;
	
	//**********************************
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void SimulateProjectileTrajectory(float DeltaTime);

protected:
	/** Ends gameplay for this component. */
	UFUNCTION()
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;


private:
	/** The Character holding this weapon*/
	AFPS_CPPCharacter* Character;
};
