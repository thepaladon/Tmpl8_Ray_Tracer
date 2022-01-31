#pragma once

#include <future>
#include <cmath>

#include "camera.h"
#include "hittable.h"
#include "hittable_list.h"
#include "surface.h"
#include "ray.h"
#include "material.h"
#include "light.h"

namespace Tmpl8 {
	class bvh_node;

	// Usings
	using std::shared_ptr;
	using std::make_shared;
	using std::unique_ptr;
	using std::make_unique;
	using std::sqrt;

	class scene
	{
	public:
		scene(Surface* surface);
		~scene();
		void getTarget(Surface* surface) { screen = surface; }
		void Tick();

		//Rendering Function
		void RayTracer(const int startHeight, const int endHeight, const int startWidth, const int endWidth);
		color ray_color(const ray& r, const hittable& world, int depth, float u, float v);
		color getSkyDome(vec3 direction, const texture* skydome) const;

		//Screen Dividers (Multithreading) 
		void HardcodedThreads(int screen_separation_x, int screen_separation_y);
		void RayThreaded();
		void waitForThreads();


		//Scene & Materials
		void LoadNewScene();
		std::function<void()> BuildMaterials();
		void GenerateSphereFlake(hittable_list& world, vec3 center, float radius, int depth, int& iteration, int ignoreDirection);


		//Inputs
		void Input();
		void setKeyState(int key, bool state);
		void passKeyDownInput(int& key);
		void passKeyUpInput(int& key);
		void passMouseInput(int& x, int& y);
		void passMouseInputRel(int32_t xrel, int32_t yrel);
		void passMouseScroll(int32_t yScroll);
		void passMouseDown(int button);
		void passMouseUp(int button);
		static vec3 camCircleLogic(vec3 pos, float y_offset, float r, float degree); //Used for the camera rotations

		void passDeltatime(float passDT) { deltaTime = passDT; FPS = 1.f / deltaTime; }

		int random_mat() const
		{
			int num = (rand() % (NUM_MATERIALS - 2)) + 2;
			if (num == MISSING_MAT) { return random_mat(); }
			else return num;
		}
		int random_mat(int from, int to) const
			//5 , 7
		{
			int num = (rand() % (to - from + 1)) + from;
			if (num == MISSING_MAT) { return random_mat(from, to); }
			else return num;
		}

	private:
		Surface* screen;

		Pixel* destinationBuff;
		color* accumulationBuff;
		int sceneRender = 1;
		int iterations = 1;
		std::atomic_int32_t raysShot = 0;

		//tests
		int test_iterations = 1;
		float ms_taken_for_scene = 0;

		//Camera and options
		camera* m_cam;
		point3 lookfrom = point3(0, 2, 7);
		point3 lookat = point3(0.f, 1.f, 0.f);
		vec3 vup = vec3(0.f, 1.f, 0.f);
		float dist_to_focus = (lookfrom - lookat).length();
		float camFOV = 90;
		float camAperture = 1.f;

		texture* skyDome;
		color lightLvlInScene = { 0, 0, 0 };

		float verticalDegree = 0;
		float verticalDegreeVel = 0;
		float horizontalDegree = 0;
		float horizontalDegreeVel = 0;

		float offsetX = 0;
		float offsetY = 0;

		bool generateNewScene = true;

		int runningWorkers = 0;
		int numWorkers = 0;
		int processor_count = 0;

		//Scene Vars
		hittable_list world;
		enum sceneChoice
		{
			SHOWCASE_SCENE,
			SPHERE4_FLAKES,
			MIRROR_SCENE,
			ELLIPSOIDS,
			SOLAR_SYSTEM,
			PRIMITIVES_STRESS_TEST_80K,
			PRIMITIVES_STRESS_TEST_800,
			NUM_SCENES,
		};
		bool solarSceneRotation = true;
		float orbinDegree[10];
		int m_sceneC = SHOWCASE_SCENE;
		int numOfObjects = 0;

		//Material VARs
		enum materials
		{
			GROUND,
			CAM,
			MIRROR,
			GLASS,
			GLASS_DENSE,
			GLASS_UNDENSE,
			GLASS_RED,
			GLASS_BLUE,
			GLASS_GREEN,
			DIAMOND,
			METAL_GRAY,
			METAL_YELLOW,
			METAL_mRED,
			METAL_mGREEN,
			METAL_mBLUE,
			LAM_mRED,
			LAM_mGREEN,
			LAM_mBLUE,
			TEX_CHECKERS,
			NOISE_PERLIN,

			SUN,
			MERCURY,
			VENUS,
			EARTH,
			MOON,
			MARS,
			JUPITER,
			SATURN,
			URANUS,
			NEPTUNE,
			PSEUDO_SUN,
			MIRROR_RED,
			MIRROR_BLUE,
			MIRROR_GREEN,
			MISSING_MAT,
			NUM_MATERIALS,
		};
		shared_ptr<material> m_material[NUM_MATERIALS + 20];
		int randGeneratedMats = 0;
		//10 extra materials with rand colors

		light_list m_lights;

		//Screen variables
		bool squareDone[squaresX * squaresY];

		const int max_depth = 50;
		int samples_per_pixel = 1;

		const float speed = 1.8f;
		const float mouseSensitivity = 128;

		vec3 origin = point3(0, 0, 0);
		vec3 moveableLightPos = { 0.f, 25.f, 0.f };
		vec3 moveableCam = lookfrom;
		vec3 velocityFromInput = point3(0.f, 0.f, 0.f);
		vec3 velocityFromInputCAM = point3(0.f, 0.f, 0.f);

		//UI
		const char* nameOfScene = "bruh";
		float deltaTime = 0;
		float FPS = 0;

		int mouse_x = 0, mouse_y = 0;

		//Keyboard Inputs
		bool key_w{ false };
		bool key_a{ false };
		bool key_s{ false };
		bool key_d{ false };
		bool key_r{ false };
		bool key_f{ false };
		bool key_z{ false };
		bool key_leftBracket{ false };
		bool key_rightBracket{ false };

		bool key_m1{ false };
		bool key_m2{ false };

		bool key_fwSlash{ false };
		bool key_ctrl{ false };
		bool key_shift{ false };
		bool key_c{ false };
		bool key_p{ false };

		bool key_up {false};
		bool key_down {false};
		bool key_left {false};
		bool key_right {false};
		bool key_Menu { true }; //key_m

		bool key_NUM[10] = {false};

		//misc random lights
		color blueCol = { 0.078f, 0.207f, 1.f };
		color orangeCol = { 0.921f, 0.568f, 0.117f };
		color greenCol = { 0.305f, 0.913f, 0.474f };
		color purpleCol = { 0.486f, 0.180f, 0.760f };
		color pinkCol = { 0.933f, 0.447f, 0.796f };
		color fullBright = vec3(1.f, 1.f, 1.f);
		color halfBright = { 0.5f, 0.5f, 0.5f };
		color noLight = { 0.0f, 0.0f, 0.0f };

		enum skydomes
		{
			VENICE,
			GALAXY,
			VOID,
			PHILLIAM,
			STUDIO,
			COUNTRY_CLUB,
			KLOPPENHEIM,
			WORK_SHOP,
			SNOWY,
			NUM_SKYDOMES
		};

		int skydomeLoaded = VENICE;
	};

	



};