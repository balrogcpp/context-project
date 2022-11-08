/// created by Andrey Vasiliev

#include "pch.h"
#include "Game.h"
#include "SystemLocator.h"

namespace Glue {
void Game::OnClean() { GetComponent<SystemLocator>().OnClean(); }
void Game::OnSetUp() {}
void Game::OnUpdate(float time) {}
}  // namespace Glue
