// Copyright 2022 U.N.Owen, All Rights Reserved.

#include "ModuleDeclarer.h"

const TCHAR* EModuleImplementType::ToString(const EModuleImplementType::Type Value)
{
	switch (Value)
	{
	case EModuleImplementType::NormalModule:
		return TEXT("NormalModule");
	case EModuleImplementType::GameModule:
		return TEXT("GameModule");
	default:
		checkf(false, TEXT("Unrecognized module implement enum type."));
		return nullptr;
	}
}

const EModuleImplementType::Type EModuleImplementType::FromString(const TCHAR* Value)
{
	for (EModuleImplementType::Type Type = (EModuleImplementType::Type)0; Type < EModuleImplementType::Max; Type = EModuleImplementType::Type(Type + 1))
	{
		if (FCString::Strcmp(Value, EModuleImplementType::ToString(Type)) == 0)
		{
			return Type;
		}
	}
	checkf(false, TEXT("Unrecognized module implement enum TCHAR type: %s"), Value);
	return EModuleImplementType::NormalModule;
}

