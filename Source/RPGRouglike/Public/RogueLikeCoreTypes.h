#pragma once
#include "Engine/DataTable.h"
#include "DamageTypes/RogueLikeDamageType_Lighting.h"
#include "DamageTypes/RogueLikeDamageType_Fire.h"
#include "DamageTypes/RogueLikeDamageType_Poison.h"
#include "DamageTypes/RogueLikeDamageType_Stun.h"

#include "RogueLikeCoreTypes.generated.h"

class UNiagaraSystem;
class USoundBase;
class ARogueLikeWeaponBase;

UENUM(BlueprintType)
enum class EAttackEffectChance : uint8
{
    Critical UMETA(DisplayName = "Critical"),
    Lighting UMETA(DisplayName = "Lighting"),
    Fire UMETA(DisplayName = "Fire"),
    Poison UMETA(DisplayName = "Poison"),
    Stun UMETA(DisplayName = "Stun"),
};

static const TMap<EAttackEffectChance, int32> DefaultEffectChances = {
    {EAttackEffectChance::Critical, 0},  //
    {EAttackEffectChance::Lighting, 0},  //
    {EAttackEffectChance::Fire, 0},      //
    {EAttackEffectChance::Poison, 0},    //
    {EAttackEffectChance::Stun, 0}       //
};

#pragma region Weapons
UENUM(BlueprintType)
enum class EAttackEffect : uint8
{
    Lighting UMETA(DisplayName = "Lighting"),
    Fire UMETA(DisplayName = "Fire"),
    Poison UMETA(DisplayName = "Poison"),
};
UENUM(BlueprintType)
enum class EWeaponType : uint8
{
    Melee UMETA(DisplayName = "Melee"),
    Range UMETA(DisplayName = "Range"),
};

static const TMap<EAttackEffectChance, FName> EffectsNames = {
    {EAttackEffectChance::Critical, FName("_Critical")},  //
    {EAttackEffectChance::Lighting, FName("_Lighting")},  //
    {EAttackEffectChance::Fire, FName("_Fire")},          //
    {EAttackEffectChance::Poison, FName("_Poison")},      //
    {EAttackEffectChance::Stun, FName("_Stun")},          //
};

static const TMap<EAttackEffectChance, TSubclassOf<UDamageType>> EffectsDamageTypes = {
    {EAttackEffectChance::Lighting, URogueLikeDamageType_Lighting::StaticClass()},  //
    {EAttackEffectChance::Fire, URogueLikeDamageType_Poison::StaticClass()},        //
    {EAttackEffectChance::Poison, URogueLikeDamageType_Poison::StaticClass()},      //
    {EAttackEffectChance::Stun, URogueLikeDamageType_Stun::StaticClass()},          //
};

struct FEffectData
{
    FEffectData(){};
    void operator=(FEffectData Data)
    {
        EffectName = Data.EffectName;
        DamageType = Data.DamageType;
    };
    FEffectData(FName InitName, TSubclassOf<UDamageType> InitDamageType)
    {
        EffectName = InitName;
        DamageType = InitDamageType;
    };
    FName EffectName;
    TSubclassOf<UDamageType> DamageType;
};

USTRUCT(BlueprintType)
struct FAttackData
{
    GENERATED_USTRUCT_BODY()

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Attack info")
    float Damage = 0.f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Attack info")
    TSubclassOf<UDamageType> DamageType;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Attack info",
        meta = (UIMin = 1.0f, ClampMin = 1.0f, UIMax = 10.f, ClampMax = 10.f))
    float Speed = 1.f;

    // Do nothing for melee weapon
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Attack info")
    UStaticMesh* Mesh = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Attack info")
    UMaterialInterface* Material = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Attack info")
    UNiagaraSystem* TraceFX = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Attack info")
    UNiagaraSystem* ImpactFX = nullptr;
};

USTRUCT(BlueprintType)
struct FWeaponCharacteristic
{
    GENERATED_USTRUCT_BODY()

    // Can be only >= 1
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon", meta = (UIMin = 1.0f, ClampMin = 1.0f))
    float BaseDamage = 1.0f;

    // Can be only >= 1
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon", meta = (UIMin = 1.0f, ClampMin = 1.0f))
    float SpeedMultiplier = 1.0f;

    // 0..100
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
    TMap<EAttackEffectChance, int32> EffectsChanses = DefaultEffectChances;
};
#pragma endregion

USTRUCT(BlueprintType)
struct FUIData
{
    GENERATED_USTRUCT_BODY()
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
    UTexture2D* Icon = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
    FText Name;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
    FText Desctiprion;
};

template <typename T>
static bool IsValueBetween(T value, T Min, T Max)
{
    return value >= Min && value <= Max;
};

DEFINE_LOG_CATEGORY_STATIC(LogCharacteristic, All, All);

static const TMap<EAttackEffect, float> DefaultEffectDamages = {
    {EAttackEffect::Lighting, 0.0f},  //
    {EAttackEffect::Fire, 0.0f},      //
    {EAttackEffect::Poison, 0.0f},    //
};

#pragma region DataTablesStructs
USTRUCT(BlueprintType)
struct FUITableData : public FTableRowBase
{
    GENERATED_USTRUCT_BODY()

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI Data")
    FUIData UIData;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI Data")
    FName RowName = "Enter name there";
};
USTRUCT(BlueprintType)
struct FPassiveCharacteristic : public FTableRowBase
{
    GENERATED_USTRUCT_BODY()

    // Max health can be only > 0
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Characteristic", meta = (UIMin = 0.0f, ClampMin = 0.0f))
    float MaxHealth = 100.f;

    // Health regeneration rate, can be only > 0,the less, the faster the health is restored
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Characteristic", meta = (UIMin = 0.0f, ClampMin = 0.0f))
    float HealthRegeneration = 1.f;

    // Max stamina , can be only > 0
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Characteristic", meta = (UIMin = 0.0f, ClampMin = 0.0f))
    float MaxStamina = 100.f;

    // Stamina regeneration rate, can be only > 0, the less, the faster the stamina is restored
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Characteristic", meta = (UIMin = 0.0f, ClampMin = 0.0f))
    float StaminaRegeneration = 1.f;

    // 0..1 Percentage damage resistance, can be only >= 0
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Characteristic",
        meta = (UIMin = 0.0f, ClampMin = 0.0f, UIMax = 1.0f, ClampMax = 1.0f))
    float DamageResistance = 0.f;

    // Multiplier for playing the attack animation. The more, the more damage the player will be able deal, can be only > 1
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Characteristic", meta = (UIMin = 1.0f, ClampMin = 1.0f))
    float AttackSpeed = 1.f;

    // Base Damage, can be only > 0
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Characteristic", meta = (UIMin = 0.0f, ClampMin = 0.0f))
    float Damage = 1.f;

    // Base ProjectileSpeed, can be only >= 1, do nothing for melee characters
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Characteristic", meta = (UIMin = 1.0f, ClampMin = 1.0f))
    float ProjectileSpeed = 1.f;

    // 0..100 Chance to deal effect damage
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Characteristic")
    TMap<EAttackEffectChance, int32> EffectChances = DefaultEffectChances;

    // Movement speed multiplier, can be only > 1
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Characteristic",
        meta = (UIMin = 1.0f, ClampMin = 1.0f, UIMax = 10.0f, ClampMax = 10.0f))
    float MovementSpeed = 1.f;

    // Can be negative (reducing cooldown time) and positive(add cooldown time)
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Characteristic")
    float ReducingSkillsCooldownTime = 0.f;

    // Additional lighting damage, can be only positive
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Characteristic")
    TMap<EAttackEffect, float> EffectsDamages = DefaultEffectDamages;

    // 0..1 Percentage of health replenished from damage inflicted
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Characteristic",
        meta = (UIMin = 0.0f, ClampMin = 0.0f, UIMax = 1.0f, ClampMax = 1.0f))
    float VampirismPrecent = 0.f;
};

USTRUCT(BlueprintType)
struct FCharacterData : public FTableRowBase
{
    GENERATED_USTRUCT_BODY()

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Character Data")
    USkeletalMesh* SkeletalMesh = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Character Data")
    TSubclassOf<UAnimInstance> AnimationClass = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Character Data")
    FPassiveCharacteristic StartCharacteristic;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Character Data")
    TSubclassOf<AActor> ActiveSkillClass = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Character Data")
    TArray<FName> AvalibleWeaponNames;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Character Data")
    TArray<FName> StartSkillsPoolNames;
};
/** The structure that defines the appearance and some characteristics of the weapon is necessary
to create a datatable from which this information will be extracted*/
USTRUCT(BlueprintType)
struct FWeaponEffectAttribute : public FTableRowBase
{
    GENERATED_USTRUCT_BODY()
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon attribute")
    bool bItsProjectile = false;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon attribute", meta = (EditCondition = "bItsProjectile"))
    UStaticMesh* Mesh = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon attribute", meta = (EditCondition = "bItsProjectile"))
    UMaterialInterface* ProjectileMaterial = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon attribute")
    TSubclassOf<UDamageType> DamageType;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon attribute", meta = (EditCondition = "!bItsProjectile"))
    UMaterialInterface* TrailMaterial = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon attribute", meta = (EditCondition = "bItsProjectile"))
    UNiagaraSystem* TraceFX = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon attribute")
    UNiagaraSystem* ImpactFX = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon attribute")
    USoundBase* Audio = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon attribute",
        meta = (UIMin = 1.0f, ClampMin = 1.0f, UIMax = 10.f, ClampMax = 10.f))
    float DamageMultiplier = 1.0f;

    /**In melee weapons, it is responsible for the speed of animation playback, in ranged combat for the speed of projectile flight*/
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon attribute",
        meta = (UIMin = 1.0f, ClampMin = 1.0f, UIMax = 10.f, ClampMax = 10.f))
    float SpeedMultiplier = 1.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon attribute")
    bool CanBeCritical = true;
};

USTRUCT(BlueprintType)
struct FWeaponData : public FTableRowBase
{
    GENERATED_USTRUCT_BODY()

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
    USkeletalMesh* WeaponMesh = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
    UMaterialInterface* Material = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
    EWeaponType WeaponType = EWeaponType::Melee;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
    TSubclassOf<ARogueLikeWeaponBase> WeaponClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
    TArray<FWeaponCharacteristic> LevelWeaponCharacteristics;

    // The name of the weapon /projectile for calculating the effect
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
    FName WeaponBaseName = "Enter name there";

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
    TArray<UAnimMontage*> Animations;
};
#pragma endregion

// Return true, if Characteristic valid
static bool CheckPassiveCharacteristic(FPassiveCharacteristic CharacteristicToCheck) noexcept
{
    UE_LOG(LogCharacteristic, All,
        TEXT("\nMaxHealth %f\nHealthRegeneration %f\nMaxStamina %f \nStaminaRegeneration %f\nDamageResistance %f\nAttackSpeed "
             "%f\nDamage "
             "%f\nProjectileSpeed %f\nCriticalChance %d\nLightingChance %d\nFireChance %d\nPoisonChance %d\nStunChance "
             "%d\nReducingSkillsCooldownTime %f\nMovementSpeed %f\nLightingDamage %f\nFireDamage %f\nPoisonDamage "
             "%f\nVampirismPrecent %f"),
        CharacteristicToCheck.MaxHealth, CharacteristicToCheck.HealthRegeneration,              //
        CharacteristicToCheck.MaxStamina, CharacteristicToCheck.StaminaRegeneration,            //
        CharacteristicToCheck.DamageResistance, CharacteristicToCheck.AttackSpeed,              //
        CharacteristicToCheck.Damage, CharacteristicToCheck.ProjectileSpeed,                    //
        CharacteristicToCheck.EffectChances[EAttackEffectChance::Critical],                     //
        CharacteristicToCheck.EffectChances[EAttackEffectChance::Lighting],                     //
        CharacteristicToCheck.EffectChances[EAttackEffectChance::Fire],                         //
        CharacteristicToCheck.EffectChances[EAttackEffectChance::Poison],                       //
        CharacteristicToCheck.EffectChances[EAttackEffectChance::Stun],                         //
        CharacteristicToCheck.ReducingSkillsCooldownTime, CharacteristicToCheck.MovementSpeed,  //
        CharacteristicToCheck.EffectsDamages[EAttackEffect::Lighting],                          //
        CharacteristicToCheck.EffectsDamages[EAttackEffect::Fire],                              //
        CharacteristicToCheck.EffectsDamages[EAttackEffect::Poison],                            //
        CharacteristicToCheck.VampirismPrecent);

    return CharacteristicToCheck.MaxHealth > 0.0f                                                         //
           && CharacteristicToCheck.HealthRegeneration > 0.0f                                             //
           && CharacteristicToCheck.MaxStamina > 0.0f                                                     //
           && CharacteristicToCheck.StaminaRegeneration > 0.0f                                            //
           && IsValueBetween(CharacteristicToCheck.DamageResistance, 0.0f, 1.0f)                          //
           && CharacteristicToCheck.AttackSpeed >= 1.0f                                                   //
           && CharacteristicToCheck.Damage > 0.0f                                                         //
           && CharacteristicToCheck.ProjectileSpeed >= 1.0f                                               //
           && IsValueBetween(CharacteristicToCheck.EffectChances[EAttackEffectChance::Critical], 0, 100)  //
           && IsValueBetween(CharacteristicToCheck.EffectChances[EAttackEffectChance::Lighting], 0, 100)  //
           && IsValueBetween(CharacteristicToCheck.EffectChances[EAttackEffectChance::Fire], 0, 100)      //
           && IsValueBetween(CharacteristicToCheck.EffectChances[EAttackEffectChance::Poison], 0, 100)    //
           && IsValueBetween(CharacteristicToCheck.EffectChances[EAttackEffectChance::Stun], 0, 100)      //
           && CharacteristicToCheck.MovementSpeed >= 1.0f                                                 //
           && CharacteristicToCheck.EffectsDamages[EAttackEffect::Lighting] >= 0.0f                       //
           && CharacteristicToCheck.EffectsDamages[EAttackEffect::Fire] >= 0.0f                           //
           && CharacteristicToCheck.EffectsDamages[EAttackEffect::Poison] >= 0.0f                         //
           && IsValueBetween(CharacteristicToCheck.VampirismPrecent, 0.0f, 1.0f);
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPassiveCharacteristicUpdated);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnHealthUpdate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeathSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStaminaUpdate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStaminaConsumed);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCharacterDataInit);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnNotifiedSignature, USkeletalMeshComponent*, Mesh);
