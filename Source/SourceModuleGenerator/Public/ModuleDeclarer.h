// Copyright 2022 U.N.Owen, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ModuleDeclarer.generated.h"

USTRUCT(BlueprintType)
struct FModuleDeclarer
{
	GENERATED_BODY()


	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "U.N.Owen|ModuleDeclarer|Structure", Meta = (KeyWords = "Notice"))
	FString CopyrightMessage;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "U.N.Owen|ModuleDeclarer|Structure", Meta = (KeyWords = "Class Name"))
	FString ModuleName;
};
