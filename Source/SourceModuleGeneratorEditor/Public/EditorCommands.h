// Copyright 2022 U.N.Owen, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"

class FEditorCommands : public TCommands<FEditorCommands>
{
private:
	TSharedPtr<FUICommandInfo> ButtonAction;

public:
	FEditorCommands(); 

	virtual void RegisterCommands() override;
};
