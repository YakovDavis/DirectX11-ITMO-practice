#include "Game.h"
#include "TriangleComponent.h"

class MyGame : public Game
{
protected:
	void DestroyResources() override;
	void Draw() override;
	void EndFrame() override;
	void Initialize() override;
	void PrepareFrame() override;
	void PrepareResources() override;
	void Update() override;
	void UpdateInternal() override;
public:
	MyGame();
	~MyGame();
};