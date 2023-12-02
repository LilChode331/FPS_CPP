// Copyright Epic Games, Inc. All Rights Reserved.

// Copyright Epic Games, Inc. All Rights Reserved.
#include "FPS_CPPProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "PhysicsEngine/PhysicsSettings.h"
#include "Components/SphereComponent.h"

AFPS_CPPProjectile::AFPS_CPPProjectile()
{
	// Use a sphere as a simple collision representation
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(5.0f);
	CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");
	CollisionComp->OnComponentHit.AddDynamic(this, &AFPS_CPPProjectile::OnHit);

	// Players can't walk on it
	CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionComp->CanCharacterStepUpOn = ECB_No;

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 5000.f;
	ProjectileMovement->MaxSpeed = 5000;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = true;

	// Set initial velocity based on custom gravity direction
	FVector InitialVelocity = CustomGravityDirection.RotateVector(FVector::ForwardVector) * ProjectileMovement->InitialSpeed;
	ProjectileMovement->Velocity = InitialVelocity;

	// Set as root component
	RootComponent = CollisionComp;

	// Die after 3 seconds by default
	InitialLifeSpan = 3.0f;
}

void AFPS_CPPProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// Only add impulse and destroy projectile if we hit a physics
	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr) && OtherComp->IsSimulatingPhysics())
	{
		OtherComp->AddImpulseAtLocation(GetVelocity() * 100.0f, GetActorLocation());

		Destroy();
	}
}





void AFPS_CPPProjectile::Tick(float DeltaTime)
{
	// Call the parent class's Tick function
	Super::Tick(DeltaTime);

	// Apply custom gravity
	FVector GravityVector = CustomGravityDirection.RotateVector(FVector::UpVector) * CustomGravity.Size();
	ProjectileMovement->Velocity += GravityVector * DeltaTime;
}

