//Easy Multi Save - Copyright (C) 2022 by Michael Hegemann.  

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Engine/EngineTypes.h"
#include "Serialization/ObjectAndNameAsStringProxyArchive.h"
#include "EMSData.generated.h"

static const FName HasLoadedTag(TEXT("EMS_HasLoaded"));
static const FName SkipSaveTag(TEXT("EMS_SkipSave"));
static const FName PersistentTag(TEXT("EMS_Persistent"));
static const FName SkipTransformTag(TEXT("EMS_SkipTransform"));
static const FName PlayerPawnAsLevelActorTag(TEXT("EMS_PawnAsLevelActor"));

static const FString Underscore(TEXT("_"));
static const FString Slash(TEXT("/"));
static const FString SaveType(TEXT(".sav"));
static const FString PlayerSuffix(TEXT("Player"));
static const FString ActorSuffix(TEXT("Level"));
static const FString SlotSuffix(TEXT("Slot"));

UENUM()
enum class EUpdateActorResult: uint8
{
	RES_Success,
	RES_Skip,
	RES_ShouldSpawnNewActor,
};

UENUM()
enum class EDataLoadType : uint8
{
	DATA_Level,
	DATA_Player, 
	DATA_Object,
};

UENUM()
enum class EActorType : uint8
{
	AT_Runtime,           
	AT_Placed,           
	AT_LevelScript,       
	AT_Player,
	AT_GameObject,
	AT_Persistent,
};

UENUM()
enum class ESaveGameMode : uint8
{
	MODE_Player,
	MODE_Level,
	MODE_All,
};


UENUM()
enum class ELoadMethod: uint8
{
	/** Useful for small amounts of Actors. */
	LM_Default   UMETA(DisplayName = "Default"),

	/** Useful for medium amounts of Actors with lots of data or components. */
	LM_Deferred  UMETA(DisplayName = "Deferred"),

	/** Useful for large amounts of Actors without many components and data. */
	LM_Thread   UMETA(DisplayName = "Multi-Thread"),
};

UENUM()
enum class EFileSaveMethod : uint8
{
	/** Each slot has it's own folder. */
	FM_Desktop  UMETA(DisplayName = "Desktop"),

	/** No folders. Each slot has it's own files with '_SlotName' suffix. */
	FM_Console   UMETA(DisplayName = "Console"),
};

UENUM(BlueprintType, meta = (Bitflags))
enum class ESaveTypeFlags : uint8
{
	/** Save Player Controller, Pawn and Player State. */
	SF_Player		= 0		UMETA(DisplayName = "Player Actors"),

	/** Save Level Actors and Level Blueprints. */
	SF_Level		= 1		UMETA(DisplayName = "Level Actors"),
};

UENUM(BlueprintType, meta = (Bitflags))
enum class ELoadTypeFlags : uint8
{
	/** Load Player Controller, Pawn and Player State. */
	LF_Player		= 0		UMETA(DisplayName = "Player Actors"),

	/** Load Level Actors and Level Blueprints. */
	LF_Level		= 1		UMETA(DisplayName = "Level Actors"),
};

UENUM()
enum class EMultiLevelSaveMethod : uint8
{
	/** Multi Level Saving Disabled. */
	ML_Disabled   UMETA(DisplayName = "Disabled"),

	/** Basic Multi Level Saving without support for Persistent Actors. */
	ML_Normal  UMETA(DisplayName = "Basic"),

	/** Multi Level Saving for World Partition and Streaming Levels. */
	ML_Stream  UMETA(DisplayName = "Streaming"),

	/** Multi Level Saving with support for Persistent Actors.  */
	ML_Slow   UMETA(DisplayName = "Persistent"),
};

#define ENUM_TO_FLAG(Enum) (1 << static_cast<uint8>(Enum)) 

USTRUCT(BlueprintType)
struct FSaveSlotInfo
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "SaveSlotInfo")
	FString Name;

	UPROPERTY(BlueprintReadOnly, Category = "SaveSlotInfo")
	FDateTime TimeStamp;

	UPROPERTY(BlueprintReadOnly, Category = "SaveSlotInfo")
	FName Level;

	UPROPERTY(BlueprintReadOnly, Category = "SaveSlotInfo")
	TArray<FString> Players;
};

USTRUCT()
struct FComponentSaveData
{
	GENERATED_USTRUCT_BODY()

	TArray<uint8> Name;
	FTransform RelativeTransform;
	TArray<uint8> Data;

	friend FArchive& operator<<(FArchive& Ar, FComponentSaveData& ComponentData)
	{
		Ar << ComponentData.Name;
		Ar << ComponentData.RelativeTransform;
		Ar << ComponentData.Data;
		return Ar;
	}
};

USTRUCT()
struct FGameObjectSaveData
{
	GENERATED_USTRUCT_BODY()

	TArray<uint8> Data;
	TArray<FComponentSaveData> Components;

	friend FArchive& operator<<(FArchive& Ar, FGameObjectSaveData& GameObjectData)
	{
		Ar << GameObjectData.Data;
		Ar << GameObjectData.Components;
		return Ar;
	}
};

USTRUCT()
struct FActorSaveData
{
	GENERATED_USTRUCT_BODY()

	TArray<uint8> Class;     
	TArray<uint8> Name;
	FTransform Transform;  
	uint8 Type;
	FGameObjectSaveData SaveData;

	friend FArchive& operator<<(FArchive& Ar, FActorSaveData& ActorData)
	{
		Ar << ActorData.Class;
		Ar << ActorData.Name;
		Ar << ActorData.Transform;
		Ar << ActorData.Type;
		Ar << ActorData.SaveData;
		return Ar;
	}

	FORCEINLINE bool operator ==(const FActorSaveData& A) const
	{
		return A.Name == Name;
	}
};

USTRUCT()
struct FLevelScriptSaveData
{
	GENERATED_USTRUCT_BODY()

	FName Name;
	FGameObjectSaveData SaveData;

	friend FArchive& operator<<(FArchive& Ar, FLevelScriptSaveData& ScriptData)
	{
		Ar << ScriptData.Name;
		Ar << ScriptData.SaveData;
		return Ar;
	}

	FORCEINLINE bool operator ==(const FLevelScriptSaveData& A) const
	{
		return A.Name == Name;
	}
};

USTRUCT()
struct FPawnSaveData
{
	GENERATED_USTRUCT_BODY()

	FVector Position;
	FRotator Rotation;
	FGameObjectSaveData SaveData;

	friend FArchive& operator<<(FArchive& Ar, FPawnSaveData& PawnData)
	{
		Ar << PawnData.Position;
		Ar << PawnData.Rotation;
		Ar << PawnData.SaveData;
		return Ar;
	}
};

USTRUCT()
struct FControllerSaveData
{
	GENERATED_USTRUCT_BODY()

	FRotator Rotation;
	FGameObjectSaveData SaveData;

	friend FArchive& operator<<(FArchive& Ar, FControllerSaveData& ControllerData)
	{
		Ar << ControllerData.Rotation;
		Ar << ControllerData.SaveData;
		return Ar;
	}
};

USTRUCT()
struct FLevelArchive
{
	GENERATED_USTRUCT_BODY()

	TArray<FActorSaveData> SavedActors;
	TArray<FLevelScriptSaveData> SavedScripts;
	FGameObjectSaveData SavedGameMode;
	FGameObjectSaveData SavedGameState;
	FName Level;

	friend FArchive& operator<<(FArchive& Ar, FLevelArchive& LevelArchive)
	{
		Ar << LevelArchive.SavedActors;
		Ar << LevelArchive.SavedScripts;
		Ar << LevelArchive.SavedGameMode;
		Ar << LevelArchive.SavedGameState;
		Ar << LevelArchive.Level;
		return Ar;
	}

	FORCEINLINE bool operator ==(const FLevelArchive& A) const
	{
		return A.Level == Level;
	}

	FORCEINLINE bool operator !=(const FLevelArchive& A) const
	{
		return A.Level != Level;
	}

	FORCEINLINE void ReplaceWith(const FLevelArchive& A)
	{
		//Mode is skipped here, since it is always persistent when using level stack.
		SavedActors = A.SavedActors;
		SavedScripts = A.SavedScripts;
		Level = A.Level;
	}

};

USTRUCT()
struct FLevelStackArchive
{
	GENERATED_USTRUCT_BODY()

	TArray<FLevelArchive> Archives;

	FGameObjectSaveData SavedGameMode;
	FGameObjectSaveData SavedGameState;

	friend FArchive& operator<<(FArchive& Ar, FLevelStackArchive& StackedArchive)
	{
		Ar << StackedArchive.Archives;
		Ar << StackedArchive.SavedGameMode;
		Ar << StackedArchive.SavedGameState;
		return Ar;
	}

	FORCEINLINE void AddTo(const FLevelArchive& A)
	{
		Archives.Add(A);
	}
};

USTRUCT()
struct FPlayerArchive
{
	GENERATED_USTRUCT_BODY()

	FControllerSaveData SavedController;
	FPawnSaveData SavedPawn;
	FGameObjectSaveData SavedPlayerState;
	FName Level;

	friend FArchive& operator<<(FArchive& Ar, FPlayerArchive& PlayerArchive)
	{
		Ar << PlayerArchive.SavedController;
		Ar << PlayerArchive.SavedPawn;
		Ar << PlayerArchive.SavedPlayerState;
		Ar << PlayerArchive.Level;
		return Ar;
	}
};

struct FSaveGameArchive : public FObjectAndNameAsStringProxyArchive
{
	FSaveGameArchive(FArchive& InInnerArchive) : FObjectAndNameAsStringProxyArchive(InInnerArchive, true)
	{
		ArIsSaveGame = true; //Requires structs to be prepared for serialization(See SerializeStructProperties)
		ArNoDelta = true;  //Allow to save default values
	}
};

USTRUCT(BlueprintType)
struct FMultiLevelStreamingData
{
	GENERATED_USTRUCT_BODY()

	TArray<FActorSaveData> Actors;
	TArray<FLevelScriptSaveData> Scripts;

	FORCEINLINE void CopyFrom(const FLevelArchive& A)
	{
		if (Actors.IsEmpty())
		{
			Actors = A.SavedActors;
		}

		if (Scripts.IsEmpty())
		{
			Scripts = A.SavedScripts;
		}
	}

	template <typename TSaveData, typename TSaveDataArray>
	FORCEINLINE void ReplaceOrAddToArray(const TSaveData& Data, TSaveDataArray& OuputArray)
	{
		//This will replace an existing element or add a new one. 
		const uint32 Index = OuputArray.Find(Data);
		if (Index != INDEX_NONE)
		{
			OuputArray[Index] = Data;
		}
		else
		{
			OuputArray.Add(Data);
		}	
	}

	FORCEINLINE void ReplaceOrAdd(const FLevelArchive& A)
	{
		for (const FActorSaveData& ActorData : A.SavedActors)
		{
			ReplaceOrAddToArray(ActorData, Actors);
		}
		
		for (const FLevelScriptSaveData& ScriptData : A.SavedScripts)
		{
			ReplaceOrAddToArray(ScriptData, Scripts);
		}		
	}
};




