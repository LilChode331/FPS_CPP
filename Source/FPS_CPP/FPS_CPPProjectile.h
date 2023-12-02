// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Math/Quat.h"
#include "FPS_CPPProjectile.generated.h"

class USphereComponent;
class UProjectileMovementComponent;

UCLASS(config=Game)
class AFPS_CPPProjectile : public AActor
{
	GENERATED_BODY()



	/** Sphere collision component */
	UPROPERTY(VisibleDefaultsOnly, Category=Projectile)
	USphereComponent* CollisionComp;

	/** Projectile movement component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	UProjectileMovementComponent* ProjectileMovement;



public:
	AFPS_CPPProjectile();

	/** called when projectile hits something */
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	/** Returns CollisionComp subobject **/
	USphereComponent* GetCollisionComp() const { return CollisionComp; }
	/** Returns ProjectileMovement subobject **/
	UProjectileMovementComponent* GetProjectileMovement() const { return ProjectileMovement; }

	/*
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void GravityShift();
	*/
public:
	// Custom gravity vector
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom Gravity")
	FVector CustomGravity = FVector(0.0f, 980.0f, 0.0f); // You can adjust the magnitude as needed

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom Gravity")
	FRotator CustomGravityDirection = FRotator(0.0f, 0.0f, -980.0f); // Default gravity points down

	virtual void Tick(float DeltaTime) override;




};




