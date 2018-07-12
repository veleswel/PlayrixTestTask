#pragma once
#include "singleton.hpp"
#include "Utils.hpp"

// Класс, который позволяет управлять текущим состоянием игры

enum class EGameState
{
	EMenu, EPlaying, EPause, EFinish
};

class GameStateHandler
{
private:
	GameStateHandler();
	
	GameStateHandler(const GameStateHandler& handler) = delete;
	GameStateHandler& operator = (GameStateHandler& handler) = delete;
	
public:
	static GameStateHandler& GetInstance();
	
	EGameState GetGameState() const;
	void SetGameState(EGameState gameState);
	
private:
	EGameState _gameState;
};
