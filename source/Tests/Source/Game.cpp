/// created by Andrey Vasiliev

#include "pch.h"
#include "Game.h"
#include "SystemLocator.h"

namespace gge {

void Game::OnClean() { GetComponent<SystemLocator>().OnClean(); }
void Game::OnSetUp() {}
void Game::OnUpdate(float time) {}

}  // namespace gge
