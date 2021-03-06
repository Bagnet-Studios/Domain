//Easy Multi Save - Copyright (C) 2022 by Michael Hegemann.  

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "EMSInfoSaveGame.h"
#include "EMSPluginSettings.generated.h"

UCLASS(config=Engine, defaultconfig)
class EASYMULTISAVE_API UEMSPluginSettings : public UObject
{
	GENERATED_BODY()

public:

	/**The default save game slot name, that is used if no name is set with 'Set Current Save Slot Name'.*/
	UPROPERTY(config, EditAnywhere, Category = "General Settings", meta = (DisplayName = "Default Save Slot Name"))
	FString DefaultSaveGameName = "MySaveGame";

	/**The Blueprint class that you want to use for the slot info, usually does not need a custom class.*/
	UPROPERTY(config, EditAnywhere, Category = "General Settings", meta = (MetaClass = "EMSInfoSaveGame", DisplayName = "Slot Info Class"))
	FSoftClassPath SlotInfoSaveGameClass;

	/**The file system that is used depending on the platform.*/
	UPROPERTY(config, EditAnywhere, Category = "General Settings", meta = (DisplayName = "File System"))
	EFileSaveMethod FileSaveMethod = EFileSaveMethod::FM_Desktop;

	/**If enabled, the system runs a more expensive check for spawned Actors. This is useful if you spawn Actors at the beginning of a level and experience issues.*/
	UPROPERTY(config, EditAnywhere, Category = "Actors", meta = (DisplayName = "Advanced Spawn Check"))
	bool bAdvancedSpawnCheck = false;

	/**
	Struct and Map variables with 'Save Game' checked are saved automatically for all Actors. Otherwise only the desired variables inside a struct are saved.
	*/
	UPROPERTY(config, EditAnywhere, Category = "Actors", meta = (DisplayName = "Auto Save Structs"))
	bool bAutoSaveStructs = true;

	/**If enabled, saving player and level actors is outsourced to a background thread.*/
	UPROPERTY(config, EditAnywhere, Category = "Save and Load", meta = (DisplayName = "Multi-Thread Saving"))
	bool bMultiThreadSaving = false;

	/**The method that is used to load level-actors.*/
	UPROPERTY(config, EditAnywhere, Category = "Save and Load", meta = (DisplayName = "Level Load Method"))
	ELoadMethod LoadMethod = ELoadMethod::LM_Default;

	/**
	* How Level Actor Save Data is kept between levels.
	* Prevents old save data from being overwritten. 
	* 
	* It is recommended to use this in combination with Persistent Player.
	* Game Mode is always persistent and not saved per level. 
	* 
	* The level name from the slot is always the last saved level.
	*/
	UPROPERTY(config, EditAnywhere, Category = "Save and Load", meta = (DisplayName = "Multi-Level Saving Mode"))
	EMultiLevelSaveMethod MultiLevelSaving = EMultiLevelSaveMethod::ML_Disabled;

	/**The controller, pawn and player state can be loaded independent of the level without transforms.*/
	UPROPERTY(config, EditAnywhere, Category = "Persistence", meta = (DisplayName = "Persistent Player"))
	bool bPersistentPlayer;

	/**The game mode and game state can be loaded independent of the level.*/
	UPROPERTY(config, EditAnywhere, Category = "Persistence", meta = (DisplayName = "Persistent Game Mode", EditCondition = "MultiLevelSaving == EMultiLevelSaveMethod::ML_Disabled"))
	bool bPersistentGameMode;

	static FORCEINLINE UEMSPluginSettings* Get()
	{
		UEMSPluginSettings* Settings = GetMutableDefault<UEMSPluginSettings>();
		check(Settings);

		return Settings;
	}
};
