/// created by Andrey Vasiliev

#include "pch.h"
#include "AppStateManager.h"

namespace Glue {
AppStateManager::AppStateManager() {}
AppStateManager::~AppStateManager() {}

void AppStateManager::RegAppState(AppState *appState) { appStateList[appState->GetName()] = appState; }
void AppStateManager::UnregAppState(AppState *appState) { appStateList.erase(appState->GetName()); }

void AppStateManager::SetActiveAppState(AppState *appState) { activeAppState = appState; }
AppState *AppStateManager::GetActiveAppState() { return activeAppState; }
}  // namespace Glue
