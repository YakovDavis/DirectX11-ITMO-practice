#include "Game.h"
#include "QuadComponent.h"

class MyGame : public Game
{
protected:
	void Draw() override;
public:
	MyGame();
	~MyGame();
};