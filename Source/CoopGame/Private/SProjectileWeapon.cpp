// Fill out your copyright notice in the Description page of Project Settings.


#include "SProjectileWeapon.h"
#include "Engine/World.h"
#include "Engine/Public/TimerManager.h"
#include "Net/UnrealNetwork.h"

void ASProjectileWeapon::BeginPlay()
{
	Super::BeginPlay();
}

void ASProjectileWeapon::Fire()
{
	if (GetLocalRole() < ROLE_Authority)
	{
		ServerFire();
	}
	else
	{
		// Only spawn projectile on server
		AActor* MyOwner = GetOwner();
		if (MyOwner)
		{
			FVector EyeLocation;
			FRotator EyeRotation;
			MyOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);
			if (ProjectileClass)
			{
				FVector MuzzleLocation = MeshComp->GetSocketLocation(MuzzleSocketName);
				FRotator MuzzleRotation = MeshComp->GetSocketRotation(MuzzleSocketName);

				FActorSpawnParameters ActorSpawnParams;
				ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

				GetWorld()->SpawnActor<AActor>(ProjectileClass, MuzzleLocation, EyeRotation, ActorSpawnParams);
			}
		}

		HitScanTrace.TraceTo = FVector::ZeroVector;
		HitScanTrace.ShotCount++;
	}

	PlayFireEffects(FVector::ZeroVector);

	LastFireTime = GetWorld()->TimeSeconds;
}

void ASProjectileWeapon::OnRep_HitScanTrace()
{
	// Play cosmetic FX
	PlayFireEffects(HitScanTrace.TraceTo);

}

void ASProjectileWeapon::StartFire()
{
	float FirstDelay = FMath::Max(LastFireTime + TimeBetweenShots - GetWorld()->TimeSeconds, 0.0f);

	GetWorldTimerManager().SetTimer(TimerHandle_TimeBetweenShots, this, &ASProjectileWeapon::Fire, TimeBetweenShots, true, FirstDelay);
}