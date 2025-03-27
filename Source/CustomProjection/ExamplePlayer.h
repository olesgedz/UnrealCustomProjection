// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "ExamplePlayer.generated.h"

UCLASS()
class CUSTOMPROJECTION_API AExamplePlayer : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AExamplePlayer();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditAnywhere, Category = "MRCameraCapture")
	float MinZ = 0.f;

	UPROPERTY(EditAnywhere, Category = "MRCameraCapture")
	float MaxZ = 1000000.f;

	UPROPERTY(EditAnywhere, Category = "MRCameraCapture")
	float UnitScale = 0.01f;

	UPROPERTY(EditAnywhere, Category = "MRCameraCapture")
	FIntVector2 PixelSize = FIntVector2(1920, 1080);

	UPROPERTY(EditAnywhere, Category = "MRCameraCapture")
	FVector2D FocalLength = FVector2D(2096.1739109581968f, 2096.1739109581968f);

	UPROPERTY(EditAnywhere, Category = "MRCameraCapture")
	FVector2D PrincipalPoint = FVector2D(926.2432220949044f, 501.4977953220995f);

	UPROPERTY(EditAnywhere, Category = "MRCameraCapture")
	FVector TVector = FVector(0.f, 0.f, 0.f);

	UPROPERTY(EditAnywhere, Category = "MRCameraCapture")
	FRotator Rotation = FRotator(0.f, 0.f, 0.f);

	UFUNCTION(CallInEditor, Category = "MRCameraCapture")
	void UpdateCamera();


	UPROPERTY()
	TObjectPtr<USceneCaptureComponent2D> SceneCaptureComponent = nullptr;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
  static FMatrix CalculateInverseFieldTransformMatrix(const FVector &InTVector,
                                                      const double InYaw,
                                                      const double InPitch,
                                                      const double InRoll);
  static FMatrix CalculateProjectionMatrix(const FVector2d &InFocalLength,
                                           const FVector2d &InPrincipalPoint,
                                           const FVector2d &InPixelSize,
                                           double InNearZ, double InFarZ);

  void SetupProjectionMatrix(
      USceneCaptureComponent2D *InSceneCaptureComponent) const;
  void SetupViewMatrix();

  static inline const FMatrix InverseCoordinateSystemRotationMatrix =
      FMatrix(FPlane(1, 0, 0, 0), FPlane(0, 0, -1, 0), FPlane(0, 1, 0, 0),
              FPlane(0, 0, 0, 1));

  // identical to inverse
  static inline const FMatrix CameraRotationMatrix =
      FMatrix(FPlane(0, 1, 0, 0), FPlane(1, 0, 0, 0), FPlane(0, 0, -1, 0),
              FPlane(0, 0, 0, 1));

  // only needed for the debug plane mesh
  static inline const FMatrix FieldPlaneMatrix =
      FMatrix(FPlane(28, 0, 0, 0), FPlane(0, 15, 0, 0), FPlane(0, 0, 1, 0),
              FPlane(1400, 750, 0, 1));
	
};
