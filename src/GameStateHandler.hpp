#pragma once
#include "singleton.hpp"
#include "Utils.hpp"

/* Класс, который позволяет управлять текущим состоянием игры.
Реализован в виде синглтона Майерса и доступен из любой точки приложения.
Реализация синглтона, представленная в движке(singleton.hpp), мне не понравилась, 
некоторые части кода там были закоментированные.*/

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
