#include "stdafx.h"
#include "GameStateHandler.hpp"

GameStateHandler::GameStateHandler()
	: _gameState(EGameState::EMenu)
{
	
}

GameStateHandler& GameStateHandler::GetInstance()
{
	static GameStateHandler instance;
	return instance;
}

EGameState GameStateHandler::GetGameState() const
{
	return _gameState;
}

void GameStateHandler::SetGameState(EGameState gameState)
{
	_gameState = gameState;
}
