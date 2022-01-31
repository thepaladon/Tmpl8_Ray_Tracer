#include "precomp.h"
#include "game.h"
#include "scene.h"

namespace Tmpl8
{


	void Game::Init()
	{
		myScene = new scene(screen);
		const auto seed = static_cast<unsigned>(time(nullptr));
		srand(seed);
	}

	void Game::Tick(float deltaTime)
	{
		myScene->passDeltatime(deltaTime);
		myScene->Tick();
	}

	void Game::Shutdown()
	{
		delete myScene;
		myScene = nullptr; 
	}

	void Game::KeyUp(int key) { myScene->passKeyUpInput(key);}

	void Game::KeyDown(int key)
	{
		myScene->passKeyDownInput(key);
	}

	void Game::MouseMove(int x, int y) { myScene->passMouseInput(x, y); }

	void Game::MouseMoveRel(int xrel, int yrel) { myScene->passMouseInputRel(xrel, yrel); }

	void Game::MouseWheel(int  yScroll) { myScene->passMouseScroll(yScroll); }

	void Game::MouseDown(int button) { myScene->passMouseDown(button); }

	void Game::MouseUp(int button) { myScene->passMouseUp(button); }


};