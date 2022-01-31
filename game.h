#pragma once
#include "surface.h"

namespace Tmpl8 {
	class scene;
	class Surface;

class Game
{

public:
	void SetTarget( Surface* surface ) { screen = surface; }
	void Init();
	void Shutdown();
	void Tick( float deltaTime );
	void MouseUp(int button);
	void MouseDown(int button); 
	void MouseMove(int x, int y);
	void MouseMoveRel(int32_t xrel, int32_t yrel);
	void KeyUp(int key);
	void KeyDown(int key);
	void MouseWheel(int32_t yScroll);

private:
	Surface* screen = nullptr;
	scene* myScene = nullptr;

	bool space = 0;

};

}; // namespace Tmpl8


