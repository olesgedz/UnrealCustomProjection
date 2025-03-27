// Fill out your copyright notice in the Description page of Project Settings.
#include "Components/SceneCaptureComponent2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Misc/DefaultValueHelper.h"

#include "ExamplePlayer.h"

// Sets default values
AExamplePlayer::AExamplePlayer() {
  // Set this pawn to call Tick() every frame.  You can turn this off to improve
  // performance if you don't need it.
  PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AExamplePlayer::BeginPlay() {
  Super::BeginPlay();
  SceneCaptureComponent = GetComponentByClass<USceneCaptureComponent2D>();
}

// Called every frame
void AExamplePlayer::Tick(float DeltaTime) {
  Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AExamplePlayer::SetupPlayerInputComponent(
    UInputComponent *PlayerInputComponent) {
  Super::SetupPlayerInputComponent(PlayerInputComponent);
}

FMatrix AExamplePlayer::CalculateProjectionMatrix(
    const FVector2d &InFocalLength, const FVector2d &InPrincipalPoint,
    const FVector2d &InPixelSize, const double InNearZ, const double InFarZ) {
  const FVector2d RelativePrincipalPoint =
      (2 * InPrincipalPoint + 1) / InPixelSize - 1;
  const FVector2d RelativeFocalLength = 2 * InFocalLength / InPixelSize;

  return FMatrix(
      FPlane(RelativeFocalLength.X, 0, 0, 0),
      FPlane(0, RelativeFocalLength.Y, 0, 0),
      FPlane(RelativePrincipalPoint.X, -RelativePrincipalPoint.Y,
             (InNearZ == InFarZ) ? 0 : InNearZ / (InNearZ - InFarZ), 1),
      FPlane(0, 0,
             (InNearZ == InFarZ) ? InNearZ
                                 : -InFarZ * InNearZ / (InNearZ - InFarZ),
             0));
}

void AExamplePlayer::SetupProjectionMatrix(
    USceneCaptureComponent2D *InSceneCaptureComponent) const {
  const TObjectPtr<class UTextureRenderTarget2D> TextureRenderTarget2D =
      InSceneCaptureComponent->TextureTarget;
  if (!TextureRenderTarget2D) {
    return;
  }
  TextureRenderTarget2D->SizeX = PixelSize.X;
  TextureRenderTarget2D->SizeY = PixelSize.Y;

  InSceneCaptureComponent->bUseCustomProjectionMatrix = true;
  InSceneCaptureComponent->CustomProjectionMatrix = CalculateProjectionMatrix(
      FocalLength, PrincipalPoint, FVector2d(PixelSize.X, PixelSize.Y), MinZ,
      MaxZ);
}

void AExamplePlayer::SetupViewMatrix() {
  const FMatrix InverseFieldTransformMatrix =
      CalculateInverseFieldTransformMatrix(
          TVector * UnitScale, FMath::DegreesToRadians(Rotation.Yaw),
          FMath::DegreesToRadians(Rotation.Pitch),
          FMath::DegreesToRadians(Rotation.Roll));
  SetActorTransform(
      FTransform(CameraRotationMatrix * InverseFieldTransformMatrix));
}

void AExamplePlayer::UpdateCamera() {
  if (!SceneCaptureComponent) {
    return;
  }

  FTransform DefaultTransform;
  DefaultTransform.SetLocation(FVector::ZeroVector);
  DefaultTransform.SetRotation(FQuat::Identity);
  DefaultTransform.SetScale3D(FVector(1.0f, 1.0f, 1.0f));

  SetActorTransform(DefaultTransform);

  SetupViewMatrix();
  SetupProjectionMatrix(SceneCaptureComponent);
}


FMatrix AExamplePlayer::CalculateInverseFieldTransformMatrix(
    const FVector &InTVector, const double InYaw, const double InPitch,
    const double InRoll) {
  const double RollSin = FMath::Sin(InRoll);
  const double RollCos = FMath::Cos(InRoll);
  const FMatrix InverseRollMatrix =
      FMatrix(FPlane(1, 0.0, 0.0, 0.0), FPlane(0, RollCos, -RollSin, 0),
              FPlane(0, RollSin, RollCos, 0), FPlane(0, 0, 0, 1));

  const double PitchSin = FMath::Sin(InPitch);
  const double PitchCos = FMath::Cos(InPitch);
  const FMatrix InversePitchMatrix = FMatrix(
      FPlane(PitchCos, -PitchSin, 0, 0), FPlane(PitchSin, PitchCos, 0, 0),
      FPlane(0, 0, 1, 0), FPlane(0, 0, 0, 1));

  const double YawSin = FMath::Sin(InYaw);
  const double YawCos = FMath::Cos(InYaw);
  const FMatrix InverseYawMatrix =
      FMatrix(FPlane(YawCos, 0, YawSin, 0), FPlane(0, 1, 0, 0),
              FPlane(-YawSin, 0, YawCos, 0), FPlane(0, 0, 0, 1));

  const FMatrix InverseFieldLocationMatrix =
      FMatrix(FPlane(1, 0, 0, 0), FPlane(0, 1, 0, 0), FPlane(0, 0, 1, 0),
              FPlane(-InTVector.X, -InTVector.Z, -InTVector.Y, 1));

  const FMatrix InverseFieldMatrix = InverseFieldLocationMatrix *
                                     InverseYawMatrix * InversePitchMatrix *
                                     InverseRollMatrix;

  return InverseFieldMatrix * InverseCoordinateSystemRotationMatrix;
}