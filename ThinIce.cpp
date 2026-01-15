#include "ThinIce.h"
#include <UObject/FastReferenceCollector.h>


// Sets default values
AThinIce::AThinIce()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	GeometryCollection = CreateDefaultSubobject<UGeometryCollectionComponent>(TEXT("GeometryCollection"));
	GeometryCollection->SetupAttachment(RootComponent);

	GeometryCollection2 = CreateDefaultSubobject<UGeometryCollectionComponent>(TEXT("GeometryCollection2"));
	GeometryCollection2->SetupAttachment(RootComponent);

	GeometryCollection3 = CreateDefaultSubobject<UGeometryCollectionComponent>(TEXT("GeometryCollection3"));
	GeometryCollection3->SetupAttachment(RootComponent);

	GeometryCollection4 = CreateDefaultSubobject<UGeometryCollectionComponent>(TEXT("GeometryCollection4"));
	GeometryCollection4->SetupAttachment(RootComponent);

	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	TriggerBox->SetupAttachment(RootComponent);

}

// Called when the game starts or when spawned
void AThinIce::BeginPlay()
{
	Super::BeginPlay();

	InitialTransform = GetActorTransform();

	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &AThinIce::OnOverlap);

	if (GeometryCollection)
	{
		GeometryCollection->SetNotifyBreaks(true);
		GeometryCollection->OnChaosBreakEvent.AddDynamic(this, &AThinIce::OnIceFractured);
	}
	if (GeometryCollection2)
	{
		GeometryCollection2->SetNotifyBreaks(true);
		GeometryCollection2->OnChaosBreakEvent.AddDynamic(this, &AThinIce::OnIceFractured);
	}
	if (GeometryCollection3)
	{
		GeometryCollection3->SetNotifyBreaks(true);
		GeometryCollection3->OnChaosBreakEvent.AddDynamic(this, &AThinIce::OnIceFractured);
	}
	if (GeometryCollection4)
	{
		GeometryCollection4->SetNotifyBreaks(true);
		GeometryCollection4->OnChaosBreakEvent.AddDynamic(this, &AThinIce::OnIceFractured);
	}

	BreakThreshold = FMath::RandRange(MinBreakThreshold, MaxBreakThreshold);
}

// Called every frame
void AThinIce::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!OverlappingCharacter) return;

	FVector Start = OverlappingCharacter->GetActorLocation();
	FVector End = Start - FVector(0, 0, 200.0f);

	FHitResult HitResult;

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(OverlappingCharacter);


	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, Params);

	if (bHit)
	{
		UPrimitiveComponent* HitComponent = HitResult.GetComponent();

		if (HitComponent == GeometryCollection || HitComponent == GeometryCollection2 || HitComponent == GeometryCollection3 || HitComponent == GeometryCollection4)
		{
			GetWorldTimerManager().ClearTimer(RecoveryTimerHandle);

			float CurrentTime = GetWorld()->GetTimeSeconds();
			if (CurrentTime - DamageCooldown >= DamageInterval)
			{
				if (!GetWorldTimerManager().IsTimerActive(DamageTimerHandle))
				{
					DamageTick();
					GetWorldTimerManager().SetTimer(DamageTimerHandle, this, &AThinIce::DamageTick, DamageInterval, true);
				}
			}
		}
		else
		{
			GetWorldTimerManager().ClearTimer(DamageTimerHandle);
		}
	}
	else
	{
		GetWorldTimerManager().ClearTimer(DamageTimerHandle);
	}

}


void AThinIce::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ACharacter* Character = Cast<ACharacter>(OtherActor);

	if (!Character) return;

	OverlappingCharacter = Character;

}

void AThinIce::DamageTick()
{
	if (!OverlappingCharacter) return;

	FVector CharacterPosition = OverlappingCharacter->GetActorLocation();

	TotalDamage += StepDamage;

	if (TotalDamage < BreakThreshold - 1)
	{
		UGameplayStatics::PlaySoundAtLocation(this, CrackingLightSound, CharacterPosition);
	}

	if (TotalDamage == BreakThreshold - 1)
	{
		UGameplayStatics::PlaySoundAtLocation(this, CrackingHeavySound, CharacterPosition);
	}

	if (TotalDamage >= BreakThreshold)
	{
		BreakIce(CharacterPosition);


		GetWorldTimerManager().SetTimer(RespawnTimerHandle, this, &AThinIce::Respawn, RespawnDelay, false);

		TotalDamage = 0;

		BreakThreshold = FMath::RandRange(MinBreakThreshold, MaxBreakThreshold);

		bBreakSoundPlayed = false;
	}

	DamageCooldown = GetWorld()->GetTimeSeconds();

	UE_LOG(LogTemp, Warning, TEXT("Ice damage: %f / %f"), TotalDamage, BreakThreshold);
}

void AThinIce::RecoverHealth()
{
	GetWorldTimerManager().ClearTimer(RecoveryTimerHandle);

	TotalDamage = 0.0f;

}

void AThinIce::Respawn()
{
	GetWorldTimerManager().ClearTimer(RespawnTimerHandle);

	FActorSpawnParameters Params;

	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	AThinIce* FreshIce = GetWorld()->SpawnActor<AThinIce>(GetClass(), InitialTransform.GetLocation(), InitialTransform.GetRotation().Rotator(), Params);

	if (FreshIce && GeometryCollection && GeometryCollection2 && GeometryCollection3 && GeometryCollection4)
	{
		FreshIce->GeometryCollection->InitializationFields = GeometryCollection->InitializationFields;

		FreshIce->GeometryCollection->RecreatePhysicsState();

		FreshIce->GeometryCollection2->InitializationFields = GeometryCollection2->InitializationFields;

		FreshIce->GeometryCollection2->RecreatePhysicsState();

		FreshIce->GeometryCollection3->InitializationFields = GeometryCollection3->InitializationFields;

		FreshIce->GeometryCollection3->RecreatePhysicsState();

		FreshIce->GeometryCollection4->InitializationFields = GeometryCollection4->InitializationFields;

		FreshIce->GeometryCollection4->RecreatePhysicsState();
	}

	Destroy();
}

void AThinIce::BreakIce(FVector BreakPosition)
{
	if (!BreakerBlock) return;

	FVector OffsetDirection = FVector::ZeroVector;

	if (OverlappingCharacter)
	{
		FVector Velocity = OverlappingCharacter->GetVelocity();

		Velocity.Z = 0;

		if (!Velocity.IsNearlyZero())
		{
			OffsetDirection = Velocity.GetSafeNormal();
		}
	}

	float OffsetDistance = 200.0f;

	for (int i = 0; i < 3; i++)
	{


		FVector ForwardOffset = OffsetDirection * OffsetDistance;

		FVector SpawnLocation = BreakPosition + FVector(0, 0, -60) + ForwardOffset;

		FRotator SpawnRotation = FRotator::ZeroRotator;

		FActorSpawnParameters Params;

		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		AActor* Breaker = GetWorld()->SpawnActor<AActor>(BreakerBlock, SpawnLocation, SpawnRotation, Params);
		UE_LOG(LogTemp, Warning, TEXT("Spawn Destroyer"));

		if (Breaker)
		{
			UPrimitiveComponent* PrimComp = Cast<UPrimitiveComponent>(Breaker->GetComponentByClass(UStaticMeshComponent::StaticClass()));

			if (PrimComp)
			{
				PrimComp->SetPhysicsLinearVelocity(FVector(0, 0, -1000), false);
			}

			Breaker->SetLifeSpan(0.3f);
		}
	}
}



void AThinIce::OnIceFractured(const FChaosBreakEvent& BreakEvent)
{
	if (!bBreakSoundPlayed)
	{
		UGameplayStatics::PlaySoundAtLocation(this, BreakSound, GetActorLocation());
		bBreakSoundPlayed = true;
	}
}
