/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
///			Copyright 2018 (C) Bruno Xavier B. Leite
//////////////////////////////////////////////////////////////

#pragma once

#include "PTMTool.h"
#include "PTM_MainWidget.h"
#include "FTM_MainWidget.h"

#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "Framework/MultiBox/MultiBoxExtender.h"

#include "Runtime/Core/Public/Modules/ModuleManager.h"
#include "Runtime/Slate/Public/Widgets/Docking/SDockTab.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class IPTMTool : public IModuleInterface
{
public:
	static inline IPTMTool &Get() {return FModuleManager::LoadModuleChecked<IPTMTool>("PTMTool");}
	static inline bool IsAvailable() {return FModuleManager::Get().IsModuleLoaded("PTMTool");}
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define LOCTEXT_NAMESPACE "Synaptech"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static const FName PTMTool_TAB("PTMTool_Window");
static const FName FTMTool_TAB("FTMTool_Window");

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class FPTMTool : public IPTMTool
{
protected:
	TSharedPtr<FExtender>PTM_Extender; 
	TSharedPtr<FExtender>FTM_Extender;
	TSharedPtr<FUICommandList>Commands;
protected:
	TSharedPtr<FExtender>PTM_MenuExtender;
	TSharedPtr<FExtender>FTM_MenuExtender;
protected:
	TSharedRef<SDockTab>OnSpawnPTMToolTab(const FSpawnTabArgs &SpawnTabArgs);
	TSharedRef<SDockTab>OnSpawnFTMToolTab(const FSpawnTabArgs &SpawnTabArgs);
protected:
	void AddToolBarButton(FToolBarBuilder &Builder);
public:
	static void PTM_ExtendMenu(FMenuBuilder &MenuBuilder);
	static void FTM_ExtendMenu(FMenuBuilder &MenuBuilder);
public:
	static void PTM_InvokeTab();
	static void FTM_InvokeTab();
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
public:
	void OnToolbarButtonClicked();
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class FPTM_Command : public TCommands<FPTM_Command>
{
public:
	TSharedPtr<FUICommandInfo>SpawnPTMToolWindow;
	FPTM_Command():TCommands<FPTM_Command>(TEXT("PTMTool"),NSLOCTEXT("Contexts","PTMTool","Blueprint Transfer Manager"),NAME_None,FPTM_Style::GetStyleSetName()){}
	//
	virtual void RegisterCommands() override;
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#undef LOCTEXT_NAMESPACE

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////