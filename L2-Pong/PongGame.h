#pragma once
#include "Game.h"
#include "RacketComponent.h"
#include "BallComponent.h"
#include <d2d1.h>
#include <dwrite.h>

enum GameState
{
	PONG_STATE_NORMAL,
	PONG_STATE_COOLDOWN,
	PONG_STATE_GAMEOVER
};

class PongGame : public Game
{
protected:
	void Update() override;
	unsigned s1;
	unsigned s2;
	ID2D1Factory* D2DFactory;
	IDWriteFactory* DWFactory;
	IDWriteTextFormat* DWTextFormat;
	ID2D1RenderTarget* D2DRenderTarget;
	ID2D1SolidColorBrush* D2Dbrush;
	ID2D1StrokeStyle* D2DLineStrokeStyle;
	IDXGISurface* D2DBackBuff;
public:
	std::chrono::steady_clock::time_point ContinueTime;
	RacketComponent* racket1;
	RacketComponent* racket2;
	BallComponent* ball;
	GameState state;
	PongGame();
	~PongGame();
	void Draw() override;
	void Initialize() override;
	void DestroyResources() override;
	void IncScore(bool p);
};
