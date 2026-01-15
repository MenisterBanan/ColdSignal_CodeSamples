#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GeometryCollection/GeometryCollectionComponent.h"
#include "GeometryCollection/GeometryCollectionActor.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Character.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Field/FieldSystemTypes.h"
#include "Chaos/ChaosGameplayEventDispatcher.h"
#include "Chaos/ChaosSolverActor.h"
#include "PhysicsField/PhysicsFieldComponent.h"
#include "Field/FieldSystemObjects.h"
#include "Field/FieldSystemComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "TimerManager.h"
//#include "Engine/World.h"
#include "ThinIce.generated.h"


UCLASS()
class COLDTURKEY_API AThinIce : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AThinIce();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UGeometryCollectionComponent* GeometryCollection;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UGeometryCollectionComponent* GeometryCollection2;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UGeometryCollectionComponent* GeometryCollection3;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UGeometryCollectionComponent* GeometryCollection4;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UBoxComponent* TriggerBox;

	UFUNCTION()
	void OnIceFractured(const FChaosBreakEvent& BreakEvent);

private:

	UPROPERTY(EditAnywhere, Category = "ThinIce|Detection")
	UPhysicalMaterial* ThinIcePhysMaterial;

	bool bBreakSoundPlayed = false;

	FTransform InitialTransform;

	float TotalDamage = 0.0f;

	float DamageCooldown = -100.0f;

	UPROPERTY()
	ACharacter* OverlappingCharacter = nullptr;

	FTimerHandle DamageTimerHandle;
	FTimerHandle RecoveryTimerHandle;
	FTimerHandle RespawnTimerHandle;

	UFUNCTION()
	void OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void BreakIce(FVector BreakPosition);

	void DamageTick();

	void RecoverHealth();
	
	void Respawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "ThinIce|Audio")
	USoundBase* CrackingLightSound;

	UPROPERTY(EditAnywhere, Category = "ThinIce|Audio")
	USoundBase* CrackingHeavySound;

	UPROPERTY(EditAnywhere, Category = "ThinIce|Audio")
	USoundBase* BreakSound;

	UPROPERTY(EditAnywhere, Category = "ThinIce|Breaker")
	TSubclassOf<AActor> BreakerBlock;

	UPROPERTY(EditAnywhere, Category = "ThinIce|Damage")
	float StepDamage = 1.0f;

	UPROPERTY(EditAnywhere, Category = "ThinIce|Damage")
	float BreakThreshold = 3.0f;

	UPROPERTY(EditAnywhere, Category = "ThinIce|Damage")
	int32 MinBreakThreshold = 3;

	UPROPERTY(EditAnywhere, Category = "ThinIce|Damage")
	int32 MaxBreakThreshold = 5;

	UPROPERTY(EditAnywhere, Category = "ThinIce|Damage")
	float DamageRadius = 100.0f;

	UPROPERTY(EditAnywhere, Category = "ThinIce|Damage")
	float DamageInterval = 1.0f;

	UPROPERTY(EditAnywhere, Category = "ThinIce|Recovery")
	float RecoveryDelay = 5.0f;

	UPROPERTY(EditAnywhere, Category = "ThinIce|Respawn")
	float RespawnDelay = 5.0f;


	TEnumAsByte<EFieldFalloffType> FalloffType = Field_Falloff_Linear;



};
