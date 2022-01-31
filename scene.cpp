#include "precomp.h"

#include "scene.h"
#include "color.h"
#include "material.h"
#include "sphere.h"
#include "surface.h"

#include "bvh.h"
#include "ellipsoid.h"
#include "round_box.h"
#include "sphere4.h"
#include "jobsys.h"


namespace Tmpl8
{
	scene::scene(Surface* surface)
	{
		screen = surface;
		generateNewScene = true;
		m_cam = new camera(lookfrom, lookat, vup, camFOV, aspect_ratio, camAperture, dist_to_focus);
		skyDome = new image_texture("textures/venice_sunset.jpg");
		moveableCam = { 0.0f, 3.5f,  0.f };
		accumulationBuff = new color[ScreenWidth * ScreenHeight];
		for (int i = 0; i < ScreenWidth * ScreenHeight; i++) { accumulationBuff[i] = { 0, 0, 0 }; }
		destinationBuff = screen->GetBuffer();

		JobSystem::Initialize();

		BuildMaterials();
		LoadNewScene();
	}

	void scene::GenerateSphereFlake(hittable_list& world, vec3 center, float radius, int depth, int& iteration, int ignoreDirection)
	{
		depth--;
		if (depth <= 0) { return; }

		iteration++;
		const float radandhalf = radius + radius / 2;

		if (ignoreDirection != 3) {
			world.add(new sphere4(point3(center.x, center.y, center.z - radandhalf), radius / 2, m_material[GLASS]));
			GenerateSphereFlake(world, point3(center.x, center.y, center.z - radandhalf), radius / 2, depth, iteration, -3);
		}

		if (ignoreDirection != -3) {
			world.add(new sphere4(point3(center.x, center.y, center.z + radandhalf), radius / 2, m_material[GLASS]));
			GenerateSphereFlake(world, point3(center.x, center.y, center.z + radandhalf), radius / 2, depth, iteration, 3);
		}

		if (ignoreDirection != 2) {
			world.add(new sphere4(point3(center.x, center.y - radandhalf, center.z), radius / 2, m_material[GLASS]));
			GenerateSphereFlake(world, point3(center.x, center.y - radandhalf, center.z), radius / 2, depth, iteration, -2);
		}

		if (ignoreDirection != -2) {
			world.add(new sphere4(point3(center.x, center.y + radandhalf, center.z), radius / 2, m_material[GLASS]));
			GenerateSphereFlake(world, point3(center.x, center.y + radandhalf, center.z), radius / 2, depth, iteration, 2);
		}

		if (ignoreDirection != 1) {
			world.add(new sphere4(point3(center.x - radandhalf, center.y, center.z), radius / 2, m_material[GLASS]));
			GenerateSphereFlake(world, point3(center.x - radandhalf, center.y, center.z), radius / 2, depth, iteration, -1);
		}

		if (ignoreDirection != -1) {
			world.add(new sphere4(point3(center.x + radandhalf, center.y, center.z), radius / 2, m_material[GLASS]));
			GenerateSphereFlake(world, point3(center.x + radandhalf, center.y, center.z), radius / 2, depth, iteration, 1);
		}
	}

	//Scenes
	void scene::LoadNewScene()
	{
		bool BVHON = true;
		world.clear();
		m_lights.clear();

		//Sphere Flake Variables
		const vec3 center = { 0.f, 0.f, 0.f };
		constexpr float radius = 2.f;
		constexpr int flakeDepth = 4;
		int iteration = -1 ;
		solarSceneRotation = false;
		hittable_list BVH_Objects;
		switch (m_sceneC) {
		case SHOWCASE_SCENE:
			nameOfScene = "Showcase_scene_";
			printf(nameOfScene);
			printf("loading... \n");

			skyDome->replaceTexture("textures/country_club_2k.hdr");
			moveableLightPos = vec3(1.f, 1.f, -1.5f);
			
			m_lights.add(make_shared<area_light>(moveableLightPos, vec3(.5f, 1.f, -1.f), 25.f, 64.f, blueCol));

			moveableCam = { 0.0f, 5.f,  10.0f };
			BVH_Objects.add(new sphere4(point3(0.0f, .0f, 0.0f), 1.f, m_material[GLASS_RED]));
			BVH_Objects.add(new ellipsoid(point3(0.f, 2.0f, .0f), point3(0.25f, 0.5f, 0.45f) * 2, m_material[GLASS_GREEN]));
			BVH_Objects.add(new sphere(point3(0.f, 4.0f, 0.f), 1.f, m_material[GLASS_BLUE]));

			BVH_Objects.add(new sphere4(point3(4.0f, .0f, 0.0f), 1.f, m_material[METAL_mRED]));
			BVH_Objects.add(new ellipsoid(point3(4.f, 2.0f, .0f), point3(0.25f, 0.5f, 0.45f) * 2, m_material[METAL_mGREEN]));
			BVH_Objects.add(new sphere(point3(4.f, 4.0f, 0.f), 1.f, m_material[METAL_mBLUE]));

			BVH_Objects.add(new sphere4(point3(-4.0f, .0f, 0.0f), 1.f, m_material[LAM_mRED]));
			BVH_Objects.add(new ellipsoid(point3(-4.f, 2.0f, .0f), point3(0.25f, 0.5f, 0.45f) * 2, m_material[LAM_mGREEN]));
			BVH_Objects.add(new sphere(point3(-4.f, 4.0f, 0.f), 1.f, m_material[LAM_mBLUE]));

			BVH_Objects.add(new sphere4(point3(0.0f, -31.f, -0.0f), 30.f, m_material[MIRROR]));

			break;

		case PRIMITIVES_STRESS_TEST_80K:
			nameOfScene = "Primitive_stress_test_";
			printf(nameOfScene);
			printf("loading... \n");
			skyDome->replaceTexture("textures/galaxySkydome.jpg");

			m_lights.clear();
			moveableLightPos = vec3(-5.f, 25.f, -50.f);
			m_lights.add(make_shared<point_light>(moveableLightPos, 4000.f, 64.f, greenCol));

			//GROUND
			world.add(new sphere4(point3(0.f, -251.0f, 0.f), 250.f, m_material[TEX_CHECKERS]));
			moveableCam = { 0.0f, 0.5f,  4.5f };

			//Creates two layers of 40 000 spheres each
			for (int z = 0; z < 200; z++) {
				for (int x = 0; x < 200; x++) {
					BVH_Objects.add(new sphere(point3(static_cast<float>(x - 100 / 2), 0, -static_cast<float>(z) - 1), 0.2f, m_material[LAM_mRED]));
					BVH_Objects.add(new sphere(point3(static_cast<float>(x - 100 / 2), 1.f, -static_cast<float>(z) - 1), 0.2f, m_material[random_mat(15, 17)]));
				}
			}
			break;

		case PRIMITIVES_STRESS_TEST_800:
			nameOfScene = "Primitive_stress_test_";
			printf(nameOfScene);
			printf("loading... \n");
			skyDome->replaceTexture("textures/galaxySkydome.jpg");

			m_lights.clear();
			moveableLightPos = vec3(0.f);

			m_lights.add(make_shared<area_light>(vec3( -0.05f, 16.f, -0.05f), vec3(0.05f, 16.f, 0.05f), 120.f, 32.f, purpleCol));

			//GROUND
			world.add(new sphere4(point3(0.f, -251.0f, 0.f), 250.f, m_material[TEX_CHECKERS]));
			//world.add(new ellipsoid(point3(0.f, -0.75f, 0.f), point3(100.f, 0.1f, 100.f), m_material[TEX_CHECKERS]));

			moveableCam = { 0.0f, 2.0f,  0.0f };

			//Creates a total of 800 spheres each with a randomly assigned materials
			for (int z = 0; z < 25; z++) {
				for (int x = 0; x < 32; x++) {
					BVH_Objects.add(new sphere(point3(static_cast<float>(x - 32 / 2), 0, -static_cast<float>(z - 25 / 2)), 0.2f, m_material[random_mat(2, (NUM_MATERIALS - 1) + randGeneratedMats)]));
					
				}
			}
			break;

		case MIRROR:
			nameOfScene = "Mirror_scene_";
			printf(nameOfScene);
			printf("loading... \n");
			skyDome->replaceTexture("textures/abandoned_workshop_02_2k.hdr");

			m_lights.clear();
			moveableLightPos = vec3(0.f);

			m_lights.add(make_shared<area_light>(vec3(-0.05f, 16.f, -0.05f), vec3(0.05f, 16.f, 0.05f), 120.f, 32.f, purpleCol));

			//GROUND
			world.add(new sphere4(point3(0.f, -251.0f, 0.f), 250.f, m_material[NOISE_PERLIN]));
			//world.add(new ellipsoid(point3(0.f, -0.75f, 0.f), point3(100.f, 0.1f, 100.f), m_material[TEX_CHECKERS]));

			moveableCam = { -8.0f, 2.0f,  26.0f };
			BVH_Objects.add(new sphere4(point3(0.f, 25.f, -10.f), 5.f, m_material[MIRROR_RED]));
			BVH_Objects.add(new sphere4(point3(0.f, 15.f, -10.f), 5.f, m_material[GLASS_GREEN]));
			BVH_Objects.add(new sphere4(point3(0.f, 5.f, -10.f), 5.f, m_material[MIRROR_BLUE]));

			//COLOURFUL MIRROR BALLS
			for (int z = 0; z < 1; z++) {
				for (int y = 0; y < 10; y++) {
					for (int x = 0; x < 10; x++) {

						BVH_Objects.add(new sphere(point3(static_cast<float>(x - 32 / 2), static_cast<float>(y), -static_cast<float>(z - 25 / 2)), 0.5f, m_material[random_mat(31, 33)]));

					}
					
				}
			}

			//NORMAL MIRROR BALLS
			for (int x = -10; x < -1; x++){
				for (int z = -10; z < -6; z++) {
					for (int y = 0; y < 5; y++) {
						if(y != 1 || y != 5)
						if (z != -8 && y != 2) {
						
							BVH_Objects.add(new sphere(point3(static_cast<float>(x - 32 / 2), static_cast<float>(y), -static_cast<float>(z - 25 / 2)), 0.5f, m_material[MIRROR]));
						}
					}
				}
			}

		break;

		case ELLIPSOIDS:
			nameOfScene = "Ellipsoids_";
			printf(nameOfScene);
			printf("loading... \n");

			skyDome->replaceTexture("textures/kloppenheim_02_2k.hdr");

			m_lights.clear();
			moveableLightPos = vec3(0.f, 12.f, -0.f);
			m_lights.add(make_shared<point_light>(moveableLightPos, 175.f, 1.f, blueCol));
			world.add(new sphere4(point3(0.f, -251.0f, 0.f), 250.f, m_material[GROUND]));
			moveableCam = { 0.0f, 1.f,  12.0f };

			for (int z = 0; z < 15; z++) {
				for (int x = 0; x < 15; x++) {
					BVH_Objects.add(new ellipsoid(point3((float)(x - 15 / 2), 0, -(float)(z - 15 / 2)), vec3(random_float(0.1f, 0.5f), random_float(0.1f, 0.5f), random_float(0.1f, 0.5f)), m_material[random_mat(10, 29 )]));
				}
			}
			break;

		case SPHERE4_FLAKES:
			moveableLightPos = vec3(0.5f, 5.5f, 0.5f);
			nameOfScene = "sphere_flakes_";
			printf(nameOfScene);
			printf("loading... \n");
			skyDome->replaceTexture("textures/venice_sunset.jpg");
			m_lights.add(make_shared<area_light>(moveableLightPos, vec3(.5f, 1.f, -1.f), 25.f, 64.f, blueCol));

			moveableCam = { 0.0f, 0.f,  10.0f };
			BVH_Objects.add(new sphere4(center, radius, m_material[GLASS]));
			GenerateSphereFlake(BVH_Objects, center, radius, flakeDepth, iteration, 0);

			break;

		case SOLAR_SYSTEM: 
			BVHON = false;
			solarSceneRotation = true;
			nameOfScene = "solar_system_";
			printf(nameOfScene);
			printf("loading... \n");
			skyDome->replaceTexture("textures/galaxySkydome.jpg");
			moveableLightPos = vec3(0.f, 0.81f, 0.f);
			moveableCam = { 0.0f, 1.2f, 0.0f };

			m_lights.add(make_shared<point_light>(moveableLightPos, 400.f, 1.f, orangeCol));

			world.add(new sphere(camCircleLogic(point3(0.f, 0.f, 0.f), 0, 5.f, orbinDegree[1] = random_float(0.f, 1080.f)), 0.125f, m_material[21]));
			world.add(new sphere(camCircleLogic(point3(0.f, 0.f, 0.f), 0, 7.5f, orbinDegree[2] = random_float(0.f, 1080.f)), 0.35f, m_material[22]));
			world.add(new sphere(camCircleLogic(point3(0.f, 0.f, 0.f), 0, 10.f, orbinDegree[3] = random_float(0.f, 1080.f)), 0.8f, m_material[23]));
			world.add(new sphere(camCircleLogic(world.objects[2]->center, 0, 1.f, orbinDegree[4] = random_float(0.f, 1080.f)), 0.35f, m_material[24]));
			world.add(new sphere(camCircleLogic(point3(0.f, 0.f, 0.f), 0, 12.5f, orbinDegree[5] = random_float(0.f, 1080.f)), 0.6f, m_material[25]));
			world.add(new sphere(camCircleLogic(point3(0.f, 0.f, 0.f), 0, 15.f, orbinDegree[6] = random_float(0.f, 1080.f)), 1.5f, m_material[26]));
			world.add(new sphere(camCircleLogic(point3(0.f, 0.f, 0.f), 0, 17.5f, orbinDegree[7] = random_float(0.f, 1080.f)), 2.3f, m_material[27]));
			world.add(new sphere(camCircleLogic(point3(0.f, 0.f, 0.f), 0, 20.f, orbinDegree[8] = random_float(0.f, 1080.f)), 1.2f, m_material[28]));
			world.add(new sphere(camCircleLogic(point3(0.f, 0.f, 0.f), 0, 22.5f, orbinDegree[9] = random_float(0.f, 1080.f)), 1.f, m_material[29]));
			world.add(new sphere(camCircleLogic(point3(0.f, 0.f, 0.f), 0, 22.5f, orbinDegree[9] = random_float(0.f, 1080.f)), 1.2f, m_material[29]));
			world.add(new sphere(point3(0.f, 0.f, 0.f), 0.8f, m_material[PSEUDO_SUN]));



			break;
		}

		numOfObjects = static_cast<int>(world.objects.size() + BVH_Objects.objects.size());
		bool firstNode = true;
		printf("Building BVH");
		if (BVHON) { JobSystem::Execute([&] {world.add(new bvh_node(BVH_Objects, 0.f, 0.f)); }); }

		//Timer running while creating BVH so the user knows something is happening
		const auto start = std::chrono::system_clock::now();
		while (JobSystem::IsBusy()) { 
			std::this_thread::sleep_for(std::chrono::milliseconds(1000));
			printf(".");
		}

		const auto end = std::chrono::system_clock::now();
		const std::chrono::duration<float> diff = end - start;
		printf("\nBVH created for: %f seconds. \n", diff.count());

		BVH_Objects.clear();

	}

	scene::~scene()
	{
		delete skyDome;
		delete m_cam;
		delete accumulationBuff;
	}

	std::function<void()> scene::BuildMaterials()
	{
		printf("Building Materials:... \n");
		auto start = std::chrono::system_clock::now();
		m_material[GROUND] = make_shared<lambertian>(color(0.2f, 0.2f, 0.2f));
		m_material[CAM] = make_shared<lambertian>(color(0.5f, 0.1f, 0.1f));
		m_material[MIRROR] = make_shared<dielectric>(0.f, 0.f, 0.f);
		m_material[GLASS] = make_shared<dielectric>(0.f, 1.52f, 0.3f); //glass index of refraction
		m_material[DIAMOND] = make_shared<dielectric>(0.f, 2.42f, 4.f); //diamond index of refraction
		m_material[GLASS_RED] = make_shared<dielectric>(vec3(1.f, 0.f, 0.f), 1.52f, 0.3f);
		m_material[GLASS_GREEN] = make_shared<dielectric>(vec3(0.f, 1.f, 0.f), 1.52f, 0.3f);
		m_material[GLASS_BLUE] = make_shared<dielectric>(vec3(0.f, 0.f, 1.f), 1.52f, 0.3f);
		m_material[GLASS_DENSE] = make_shared<dielectric>(0.f, 2.42f, 4.f); 
		m_material[GLASS_UNDENSE] = make_shared<dielectric>(0.f, 2.42f, 4.f); 
		m_material[METAL_GRAY] = make_shared<metal>(color(0.5f, 0.5f, 0.5f), 0.f);
		m_material[METAL_YELLOW] = make_shared<metal>(color(0.8f, 0.6f, 0.2f), 0.2f);
		m_material[METAL_mRED] = make_shared<metal>(color(1.f, 0.1f, 0.1f), 0.1f);
		m_material[METAL_mGREEN] = make_shared<metal>(color(0.1f, 1.f, 0.1f), 0.1f);
		m_material[METAL_mBLUE] = make_shared<metal>(color(0.1f, 0.1f, 1.f), 0.1f);
		m_material[LAM_mRED] = make_shared<lambertian>(color(1.f, 0.1f, 0.1f));
		m_material[LAM_mGREEN] = make_shared<lambertian>(color(0.1f, 1.f, 0.1f));
		m_material[LAM_mBLUE] = make_shared<lambertian>(color(0.1f, 0.1f, 1.f));
		m_material[TEX_CHECKERS] = make_shared<lambertian>(make_shared<checker_texture>(color(0.345f, 0.443f, 0.835f), color(0.9f, 0.9f, 0.9f)));
		m_material[NOISE_PERLIN] = make_shared<lambertian>(make_shared<noise_texture>(0.5f));

		m_material[SUN] = make_shared<lambertian>(make_shared<image_texture>("textures/8k_sun.jpg"));
		m_material[MERCURY] = make_shared<lambertian>(make_shared<image_texture>("textures/8k_mercury.jpg"));
		m_material[VENUS] = make_shared<lambertian>(make_shared<image_texture>("textures/4k_venus_atmosphere.jpg"));
		m_material[EARTH] = make_shared<lambertian>(make_shared<image_texture>("textures/8k_earth_daymap.jpg"));
		m_material[MOON] = make_shared<lambertian>(make_shared<image_texture>("textures/8k_moon.jpg"));
		m_material[MARS] = make_shared<lambertian>(make_shared<image_texture>("textures/8k_mars.jpg"));
		m_material[JUPITER] = make_shared<lambertian>(make_shared<image_texture>("textures/8k_jupiter.jpg"));
		m_material[SATURN] = make_shared<lambertian>(make_shared<image_texture>("textures/8k_saturn.jpg"));
		m_material[URANUS] = make_shared<lambertian>(make_shared<image_texture>("textures/2k_uranus.jpg"));
		m_material[NEPTUNE] = make_shared<lambertian>(make_shared<image_texture>("textures/2k_neptune.jpg"));

		m_material[PSEUDO_SUN] = make_shared<dielectric>(vec3(0.921f, 0.568f, 0.117f), 1.f, -1.5f);

		m_material[MIRROR_RED] = make_shared<dielectric>(vec3(1.f, 0.f, 0.f), 0.f, 0.3f);
		m_material[MIRROR_GREEN] = make_shared<dielectric>(vec3(0.f, 1.f, 0.f), 0.f, 0.3f);
		m_material[MIRROR_BLUE] = make_shared<dielectric>(vec3(0.f, 0.f, 1.f), 0.f, 0.3f);

		m_material[MISSING_MAT] = make_shared<lambertian>(make_shared<checker_texture>(color(0.f, 0.f, 0.f), color(1.f, 0.f, 1.f)));

		for (int i = NUM_MATERIALS; i < NUM_MATERIALS + 10; i++) {
			randGeneratedMats++;
			m_material[i] = make_shared<lambertian>(color(random_float(), random_float(), random_float()));
		}

		for (int i = NUM_MATERIALS + 10; i < NUM_MATERIALS + 20; i++) {
			randGeneratedMats++;
			m_material[i] = make_shared <metal>(color(random_float(), random_float(), random_float()), random_float());
		}

		//Check for missing textures and replace them with the missing texture material so it doesn't crash.
		for (int i = 0; i < NUM_MATERIALS + 20; i++) {
			if (m_material[i] == nullptr)
			{
				printf("Missing material in array num %i. \n", i);
				m_material[i] = make_shared<lambertian>(make_shared<checker_texture>(color(0.f, 0.f, 0.f), color(1.f, 0.f, 1.f)));
			}
		}

		auto end = std::chrono::system_clock::now();
		std::chrono::duration<float> diff = end - start;
		printf("Materials built for: %f seconds. \n", diff.count());

		return 0;
	}

	void scene::Tick()
	{
		//Check whether scene is still and TAA should be active
		if (!velocityFromInput.lower_precision_near_zero() || !velocityFromInputCAM.lower_precision_near_zero() || 
			fabsf(horizontalDegreeVel) > 1e-2 || fabsf(verticalDegreeVel) > 1e-2)
		{
			generateNewScene = true;
			memset(accumulationBuff, 0, ScreenWidth * ScreenHeight * sizeof(vec3));
			iterations = 1;

		}else 
		{
			velocityFromInput = velocityFromInputCAM = verticalDegreeVel = horizontalDegreeVel = 0.f;
		}

		Input();

		if (generateNewScene) { //all dynamic movement in scene.
			moveableLightPos = moveableLightPos + velocityFromInput;
			moveableCam = moveableCam + velocityFromInputCAM;
			horizontalDegree = horizontalDegree + horizontalDegreeVel;
			verticalDegree = clamp(verticalDegree + verticalDegreeVel, -40.f, 40.f);
			m_lights.lightObjects[0]->setPos(moveableLightPos);
			lookat = camCircleLogic(moveableCam, verticalDegree, 10.f, horizontalDegree);
			m_cam->setCameraPosition(moveableCam, lookat, vup, camFOV, key_z);

			horizontalDegreeVel = horizontalDegreeVel / friction;
			verticalDegreeVel = verticalDegreeVel / friction;
			velocityFromInput = velocityFromInput / friction;
			velocityFromInputCAM = velocityFromInputCAM / friction;

			if(solarSceneRotation)
			{
				generateNewScene = true;
				memset(accumulationBuff, 0, ScreenWidth * ScreenHeight * sizeof(vec3));
				iterations = 1;

				for (int i = 0; i <= 9; i++) {
					if(i == 3)
					{
						world.objects[i]->setPosition(camCircleLogic(world.objects[2]->center, 0, 1.5f, orbinDegree[i] += 2.f ));
					}
					else {
						world.objects[i]->setPosition(camCircleLogic(point3(0.f, 0.f, 0.f), 0, i * 5.f + 3.5f, orbinDegree[i] += 2.f / (i + 1.f)  ));
					}
				}
			}


			//const auto test_start = std::chrono::high_resolution_clock::now(); // - part of the BVH test
#if SINGLETHREADED

			//Everything on the Main Thread
			RayTracer(0, ScreenHeight, 0, ScreenWidth);

#else
#if JOBSYSTEM

			//Multithreaded with a Job System - BEST PERFORMANCE
			RayThreaded();

#else

			//Badly Multithreaded (12 cores  = 12 tiles of the screen)
			HardcodedThreads(ScreenWidth / 4, ScreenHeight / 3);

#endif
#endif

			while (JobSystem::IsBusy()) { /*wait for all tasks to be ready to proceed to final image generation*/ }
			write_color(destinationBuff, accumulationBuff, iterations, key_p, nameOfScene);
			iterations++;
			if (iterations > 150) { generateNewScene = false; }

			//BVH TEST CODE using CHRONO:
			/*const auto test_end = std::chrono::high_resolution_clock::now();
			
			const std::chrono::duration<float> diff = test_end - test_start;
			ms_taken_for_scene = (ms_taken_for_scene + diff.count());
			test_iterations++;

			if (test_iterations % 5 == 0) {
				printf("Time Taken: %f \n", diff.count());
				printf("Time taken: %f averaged over %i samples. \n \n",  ms_taken_for_scene / test_iterations, test_iterations);
			} //*/
		}


		// ------------- ON SCREEN INFO DISPLAY ------------- //
#if ON_SCREEN_INFO

#ifdef _DEBUG
	screen->Print("DEBUG BUILD ", 10, 15, 0xff0000);

	#if defined _M_IX86
			screen->Print("x86", 85, 15, 0xff0000);
	#elif defined _M_X64
			screen->Print("x64", 85, 15, 0xff0000);
	#endif

#endif

#ifdef NDEBUG

		screen->Print("RELEASE BUILD ", 10, 15, 0xff0000);
	#if defined _M_IX86
			screen->Print("x86", 95, 15, 0xff0000);
	#elif defined _M_X64
			screen->Print("x64", 95, 15, 0xff0000);
	#endif

#endif

		screen->Print("Scene: ", 10, 25, 0xff0000);
		screen->Print(nameOfScene, 50, 25, 0xff0000);

		screen->Print("Objects: ", 10, 35, 0xff0000);
		screen->PrintNum(numOfObjects, 60, 35, 0xff0000);

		screen->Print("Rays Shot: ", 10, 45, 0xff0000);
		screen->PrintNum(raysShot, 75, 45, 0xff0000);

		screen->Print("FPS: ", 10, 55, 0xff0000);
		screen->PrintNum(static_cast<int>(FPS), 35, 55, 0xff0000);

		screen->Print("Iterations: ", 10, 65, 0xff0000);
		screen->PrintNum(iterations - 1, 80, 65, 0xff0000);

		screen->Print("Camera FOV:", 10, 75, 0xff0000);
		screen->PrintNum(static_cast<int>(camFOV), 80, 75, 0xff0000);
#endif

		raysShot = 0;		//Rays Counter

		//MENU INFORMATION
		if(key_Menu)
		{
			screen->Box(50, 50, ScreenWidth - 50, ScreenHeight - 50, 0xff0000);
			screen->Box(75, 75, ScreenWidth - 75, ScreenHeight - 75, 0xff0000);

			screen->Print("MENU", ScreenWidth /2 , 100, 0xff0000);
			screen->Line((ScreenWidth / 2) - 10, 106, (ScreenWidth / 2) + 30, 106, 0xff0000);

			screen->Print("PRESS 'M' TO TOGGLE THIS MENU SCREEN on and off", 80, 110, 0xff0000);
			screen->Print("'W A S D' is used to move the camera in the world", 80, 120, 0xff0000);
			screen->Print("'R' and 'F' are used to move the camera on the Y axis", 80, 130, 0xff0000);
			screen->Print("The 'MOUSE' is used to rotate the camera", 80, 140, 0xff0000);
			screen->Print("The 'SCROLL WHEEL' is used to change FOV ", 80, 150, 0xff0000);
			screen->Print("The Key 'C' is used to change the scene to the next one", 80, 170, 0xff0000);
			screen->Print("The Key 'V' is used to change the texture of the sky dome", 80, 180, 0xff0000);
			screen->Print("The Key 'Z' is used to toggle Depth of Field", 80, 190, 0xff0000);
			screen->Print("'Right' and 'Left' Square brackets are used to ", 80, 200, 0xff0000);
			screen->Print("change the focus distance ", 80, 210, 0xff0000);
			screen->Print("'ARROW KEYS' are used to change the position of the light", 80, 230, 0xff0000);
			screen->Print("'SHIFT' and 'CONTROL' are used to move the light ", 80, 240, 0xff0000);
			screen->Print("on the y axis.", 80, 250, 0xff0000);
			screen->Print("The Key 'P' can be used to take a screen shot ", 80, 270, 0xff0000);
			screen->Print("Output file path can be seen in the console", 80, 280, 0xff0000);
			screen->Print("NOTE: The BVH for the 80 000 sphere scene takes", 80, 300, 0xff0000);
			screen->Print("16 seconds to build on my laptop. Please have patience", 80, 310, 0xff0000);
			screen->Print("The console shows whether anything is happening,", 80, 320, 0xff0000);
		}


//In the precomp header. Used for debug
#if DEBUG_LINES

		for (int x = 0; x < squaresX; x++) {
			const int xPlus = x + 1;
			for (int y = 0; y < squaresY; y++) {
				const int yPlus = y + 1;
				screen->Box(screenSeparationX * x, screenSeparationY * y, screenSeparationX * xPlus, screenSeparationY * yPlus, 0x880000);
			}
		}
#endif

	}

	void scene::RayThreaded()
	{
		offsetY = random_float(0.f, 1.f);
		offsetX = random_float(0.f, 1.f);

		for(int x = 0; x < squaresX; x++)
		{
			for(int y = 0; y < squaresY; y++)
			{
				JobSystem::Execute([this, x, y]
				{
					RayTracer(screenSeparationY * y , screenSeparationY * (y+1),
					          screenSeparationX * x, screenSeparationX * (x + 1));
					squareDone[x * y + y] = true;
				});
			}
		}
	}

	void scene::HardcodedThreads(int screenSeparationX, int screenSeparationY) 
	{
		std::thread test_thread([this, screenSeparationX, screenSeparationY] { RayTracer(screenSeparationY * 0, screenSeparationY * 1, screenSeparationX * 0, screenSeparationX * 1); });
		std::thread test_thread1([this, screenSeparationX, screenSeparationY] { RayTracer(screenSeparationY * 1, screenSeparationY * 2, screenSeparationX * 0, screenSeparationX * 1); });
		std::thread test_thread2([this, screenSeparationX, screenSeparationY] { RayTracer(screenSeparationY * 2, screenSeparationY * 3, screenSeparationX * 0, screenSeparationX * 1); });

		std::thread test_thread3([this, screenSeparationX, screenSeparationY] { RayTracer(screenSeparationY * 0, screenSeparationY * 1, screenSeparationX * 1, screenSeparationX * 2); });
		std::thread test_thread4([this, screenSeparationX, screenSeparationY] { RayTracer(screenSeparationY * 1, screenSeparationY * 2, screenSeparationX * 1, screenSeparationX * 2); });
		std::thread test_thread5([this, screenSeparationX, screenSeparationY] { RayTracer(screenSeparationY * 2, screenSeparationY * 3, screenSeparationX * 1, screenSeparationX * 2); });

		std::thread test_thread6([this, screenSeparationX, screenSeparationY] { RayTracer(screenSeparationY * 0, screenSeparationY * 1, screenSeparationX * 2, screenSeparationX * 3); });
		std::thread test_thread7([this, screenSeparationX, screenSeparationY] { RayTracer(screenSeparationY * 1, screenSeparationY * 2, screenSeparationX * 2, screenSeparationX * 3); });
		std::thread test_thread8([this, screenSeparationX, screenSeparationY] { RayTracer(screenSeparationY * 2, screenSeparationY * 3, screenSeparationX * 2, screenSeparationX * 3); });

		std::thread test_thread9([this, screenSeparationX, screenSeparationY] { RayTracer(screenSeparationY * 0, screenSeparationY * 1, screenSeparationX * 3, screenSeparationX * 4); });
		std::thread test_thread10([this, screenSeparationX, screenSeparationY] { RayTracer(screenSeparationY * 1, screenSeparationY * 2, screenSeparationX * 3, screenSeparationX * 4); });
		std::thread test_thread11([this, screenSeparationX, screenSeparationY] { RayTracer(screenSeparationY * 2, screenSeparationY * 3, screenSeparationX * 3, screenSeparationX * 4); });

		test_thread.join();
		test_thread1.join();
		test_thread2.join();
		test_thread3.join();
		test_thread4.join();
		test_thread5.join();
		test_thread6.join();
		test_thread7.join();
		test_thread8.join();
		test_thread9.join();
		test_thread10.join();
		test_thread11.join();
	}

	void scene::RayTracer(const int startHeight, const int endHeight, const int startWidth, const int endWidth) //Renders one square. Function is passed into a job system which divides the work 
	{
		constexpr float inverseOfScreenWidth = 1.f / (ScreenWidth - 1);
		constexpr float inverseOfScreenHeight = 1.f / (ScreenHeight - 1);

		for (int j = startHeight; j < endHeight; ++j) {
			for (int i = startWidth; i < endWidth; ++i) {

				if (i < 50  || j < 50 || i > ScreenWidth - 50 || j > ScreenHeight - 50 || !key_Menu) {
					//1,f - inverts the image mirroring it
					const float u = 1.f - (i + offsetX) * inverseOfScreenWidth;
					const float v = 1.f - (j + offsetY) * inverseOfScreenHeight;

					ray r = m_cam->get_ray(u, v);

					color color = ray_color(r, world, max_depth, u, v);
					color.clampVec(0.f, 1.f);
					accumulationBuff[i + j * ScreenWidth] += color;
				}
			}
		}

#if PRINT_SCENE_PROGRESS 
		printf("Thread Number %i done. \n", num);
#endif

	}

	color scene::ray_color(const ray& r, const hittable& world, const int depth, const float u, const float v) {

		hit_record rec;

		++raysShot;

		if (depth <= 0)
			return color(.01f, 0.01f, 0.01f);

		const float epsilon = 0.001f;
		if (!world.hit(r, epsilon, infinity, rec))
		{
			const vec3 unit_direction = unit_vector(r.direction());
			return getSkyDome(unit_direction, skyDome);
		}

		if(sceneRender != 3){}

		color light = 0;
		ray scattered;
		color attenuation = 0;
		color emitted = rec.mat_ptr->emitted(rec.u, rec.v, rec.p);

		//RENDERS THE NORMAL 
		if (sceneRender == 2)
			return (rec.normal * 0.5f) + 0.5f;

		if (!rec.mat_ptr->scatter(r, rec, attenuation, scattered))
			return emitted;

		if (sceneRender != 3) {
			if (m_lights.hitLight(r, epsilon, infinity, &world, rec, light))
				return attenuation * light + attenuation * ray_color(scattered, world, depth - 1, u, v);
		}

		if (sceneRender == 3) {
			return attenuation * ray_color(scattered, world, depth - 1, u, v);
		}

		return { 0.f, 0.f, 0.f };

		/*//if it hits nothing checks for skydome texture.
		const vec3 unit_direction = unit_vector(r.direction());
		return getSkyDome(unit_direction, skyDome);*/
	};

	color scene::getSkyDome(vec3 p, const texture* skydome) const
	{
		const float theta = acos(-p.y);
		const float phi = atan2(-p.z, p.x) + PI;

		const float u = phi / (2 * PI);
		const float v = theta / PI;

		const float t = fabs(p.y);

		return skydome->value(u, v, { 0.f, 0.f, 0.f });
	}


	// ---------------------------- INPUTS ----------------------------
	vec3 scene::camCircleLogic(const vec3 pos, const float y_offset, const float r, const float degree)
	{
		const float r1 = static_cast<float>(degree) * PI / 360;
		return vec3((pos.x - r * sinf(r1)), pos.y + y_offset, (pos.z - r * cosf(r1)));
	}

	void scene::Input()
	{
		const vec3 dir = unit_vector(moveableCam - lookat);
		const vec3 perp_dir = vec3(dir.z, dir.y, -dir.x);

		if (key_up) { velocityFromInput += point3(speed, 0, 0) * deltaTime; }
		else if (key_down) { velocityFromInput += point3(-speed, 0, 0) * deltaTime; }

		if (key_right) { velocityFromInput += point3(0, 0, speed) * deltaTime; }
		else if (key_left) { velocityFromInput += point3(0, 0, -speed) * deltaTime; }

		if (key_s) { velocityFromInputCAM += vec3(dir.x, 0, dir.z) * speed * deltaTime; }
		else if (key_w) { velocityFromInputCAM += -vec3(dir.x, 0, dir.z) * speed * deltaTime; }

		if (key_d) { velocityFromInputCAM += -vec3(perp_dir.x, 0, perp_dir.z) * speed * deltaTime; }
		else if (key_a) { velocityFromInputCAM += vec3(perp_dir.x, 0, perp_dir.z) * speed * deltaTime; }

		if(key_r) { velocityFromInputCAM += point3(0, speed, 0) * deltaTime; }
		else if(key_f) { velocityFromInputCAM += point3(0, -speed, 0) * deltaTime; }

		if (key_shift) { velocityFromInput += point3(0, speed, 0) * deltaTime; }
		else if (key_ctrl) { velocityFromInput += point3(0, -speed, 0) * deltaTime; }

		if (key_leftBracket) { dist_to_focus--; m_cam->setFocusDist(dist_to_focus); }
		else if(key_rightBracket) { dist_to_focus++; m_cam->setFocusDist(dist_to_focus); }

		if (key_fwSlash) {
			moveableCam = point3(0, 0, 5);
			moveableLightPos = point3(0.f, 0.f, -0.f);
			camFOV = 90;
		}

		for(int i = 1; i < 10; i++)
		{
			if (key_NUM[i]) { sceneRender = i; }
		}

	}

	void scene::passMouseInput(int& x, int& y)
	{
		mouse_x = x;
		mouse_y = y;
	}

	void scene::passMouseInputRel(const int32_t xrel, const int32_t yrel)
	{
#if MOUSE_CONTROLS

		horizontalDegreeVel = (horizontalDegreeVel + xrel / mouseSensitivity * 10);
		verticalDegreeVel = (verticalDegreeVel + -yrel / mouseSensitivity);
#endif
	}

	void scene::passMouseScroll(int32_t yScroll)
	{
		camFOV = clamp(camFOV - yScroll, 5, 175);
		memset(accumulationBuff, 0, ScreenWidth * ScreenHeight * sizeof(vec3));
		iterations = 1;
	}

	void scene::passKeyDownInput(int& key)
	{
		setKeyState(key, true);

		//CHANGES THE SCENE
		if(key == 99)
		{
			m_sceneC = m_sceneC + 1;
			if (m_sceneC >= NUM_SCENES) { m_sceneC = SHOWCASE_SCENE; }
			LoadNewScene();
		}

		//KEY_Z -> Toggles on and off depth of field
		if(key == 122 && key_z)
		{
			key_z = false;
		}
		else if (key == 122 && !key_z)
		{
			key_z = true;
		}

		//KEY_M -> for MENU
		if (key == 109 && key_Menu)
		{
			key_Menu = false;
		}
		else if (key == 109 && !key_Menu)
		{
			key_Menu = true;
		}

		//CHANGES THE SKYDOME
		if (key == 118)
		{
			skydomeLoaded = skydomeLoaded + 1;
			if (skydomeLoaded >= NUM_SKYDOMES) { skydomeLoaded = VENICE; }
			switch(skydomeLoaded)
			{
			case VENICE:
				skyDome->replaceTexture("textures/venice_sunset.jpg");
				break;

			case GALAXY: 
				skyDome->replaceTexture("textures/galaxySkydome.jpg");
				break;

			case VOID: 
				skyDome->replaceTexture("textures/void.png");
				break;

			case STUDIO:
				skyDome->replaceTexture("textures/studio_small_08_1k.hdr");
				break;

			case KLOPPENHEIM:
				skyDome->replaceTexture("textures/kloppenheim_02_2k.hdr");
				break;

			case COUNTRY_CLUB:
				skyDome->replaceTexture("textures/country_club_2k.hdr");
				break;

			case WORK_SHOP:
				skyDome->replaceTexture("textures/abandoned_workshop_02_2k.hdr");
				break;

			case SNOWY:
				skyDome->replaceTexture("textures/snowy_park_01.jpg");
				break;
	
			case PHILLIAM:
				skyDome->replaceTexture("textures/philliam.png");
				break;

			
			default: ;
			}

		}

	}

	void scene::passKeyUpInput(int& key)
	{
		setKeyState(key, false);
	}

	void scene::passMouseDown(int button)
	{
		setKeyState(button, true);
	}

	void scene::passMouseUp(int button)
	{
		setKeyState(button, true);
	}

	void scene::setKeyState(int key, bool state)
	{
		// left bracket 91
		//right bracket 92?
		generateNewScene = true;

		if (key != 112) {
			memset(accumulationBuff, 0, ScreenWidth * ScreenHeight * sizeof(vec3));
			iterations = 1;
		}

		switch (key)
		{
		case 1: //m1
			key_m1 = state;
			break;

		case 2: //m2
			key_m2 = state;
			break;

		case 1073741905: //down
			key_down = state;
			break;
		case 1073741906: //up
			key_up = state;
			break;
		case 1073741904: //left
			key_left = state;
			break;
		case 1073741903: //right	  
			key_right = state;
			break;
		case 114: //control	
			key_r = state;
			break;
		case 102: //shift	  
			key_f = state;
			break;
		case 115: //downt
			key_s = state;
			break;
		case 119: //up
			key_w = state;
			break;
		case 97: //left
			key_a = state;
			break;
		case 100: //right	  
			key_d = state;
			break;

		case 47:
			key_fwSlash = state;
			break;

		case 91:
			key_leftBracket = state;
			break;
		case 93:
			key_rightBracket = state;
			break;

		case 1073742048:
			key_ctrl = state;
			break;
		case 1073742049:
			key_shift = state;
			break;


		case 99:
			key_c = state;
			break;

		case 112:
			key_p = state;
			break;

		case 49: //1
			key_NUM[1] = state;
			break;
		case 50: //2
			key_NUM[2] = state;
			break;
		case 51: //3
			key_NUM[3] = state;
			break;
		case 52: //4
			key_NUM[4] = state;
			break;
		case 53: //5
			key_NUM[5] = state;
			break;
		case 54: //6
			key_NUM[6] = state;
			break;
		case 55: //7
			key_NUM[7] = state;
			break;
		case 56: //8
			key_NUM[8] = state;
			break;
		case 57: //9
			key_NUM[9] = state;
			break;
		case 48: //0
			key_NUM[0] = state;
			break;
		default:
			break;
		}

	}
	
};