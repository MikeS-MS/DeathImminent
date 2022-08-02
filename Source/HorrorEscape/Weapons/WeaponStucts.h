// Copyright MikeSMediaStudios™ 2021

#pragma once

#include "CoreMinimal.h"
#include "WeaponStucts.generated.h"

UENUM(BlueprintType)
enum EWeaponSlot
{
	Invalid       UMETA(DisplayName = "Invalid"),
	Stock         UMETA(DisplayName = "Stock"),
	Engine        UMETA(DisplayName = "Engine"),
	Scope         UMETA(DisplayName = "Scope"),
	PrimaryGrip   UMETA(DisplayName = "PrimaryGrip"),
	Magazine      UMETA(DisplayName = "Magazine"),
	Barrel        UMETA(DisplayName = "Barrel"),
	GasChamber    UMETA(DisplayName = "GasChamber"),
	SecondaryGrip UMETA(DisplayName = "SecondaryGrip"),
	Attachment    UMETA(DisplayName = "Attachment")
};

USTRUCT(BlueprintType)
struct FWeaponPartName
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Entry")
	TEnumAsByte<EWeaponSlot> Slot;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Entry")
	FString Name;

	bool Equals(const FWeaponPartName& other) const
	{
		return (Slot.GetValue() == other.Slot.GetValue() && Name.Equals(other.Name));
	}

	bool operator==( const FWeaponPartName& other) const
	{
		return Equals(other);
	}

};

FORCEINLINE uint32 GetTypeHash(const FWeaponPartName& Thing)
{
	uint32 Hash = FCrc::MemCrc32(&Thing, sizeof(FWeaponPartName));
	return Hash;
}

USTRUCT(BlueprintType)
struct FWeaponPartType
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Type")
	TEnumAsByte<EWeaponSlot> Slot;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Type")
	FString Type;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Offset")
	FVector Offset;

};

USTRUCT(BlueprintType)
struct FWeaponPartStats
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	TArray<UStaticMesh*> IngameModels;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	TArray<FWeaponPartType> SupportedParts;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	FString Type;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	FString AmmoType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	uint8 Length;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	bool IsDev;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base Stats")
	int BaseDamage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base Stats")
	int BaseFireRate;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base Stats")
	float BaseRecoil;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tier Stats")
	int PerTierDamage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tier Stats")
	int PerTierFireRate;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tier Stats")
	float PerTierRecoil;
};