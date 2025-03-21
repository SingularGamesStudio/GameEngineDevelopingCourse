// This define is essential for the scripts to expose the ECS components to lua syntax
// There is a task to rework this behavior
#define GAME_FRAMEWORK

#include <Camera.h>
#include <DefaultGeometry.h>
#include <ecsControl.h>
#include <ecsMesh.h>
#include <ecsPhys.h>
#include <ECS/ecsSystems.h>
#include <GameFramework/GameFramework.h>
#include <Input/Controller.h>
#include <RenderObject.h>
#include <RenderEngine.h>
#include <flecs.h>

using namespace GameEngine;

void GameFramework::Init()
{
	RegisterComponents();
	RegisterSystems();

	flecs::entity cubeControl = m_World.entity()
		.set(Position{ -2.f, 0.f, 0.f })
		.set(Velocity{ 0.f, 0.f, 0.f })
		.set(Speed{ 10.f })
		.set(FrictionAmount{ 0.9f })
		.set(JumpSpeed{ 10.f })
		.set(Gravity{ 0.f, -9.8065f, 0.f })
		.set(BouncePlane{ 0.f, 1.f, 0.f, 5.f })
		.set(Bounciness{ 0.3f })
		.set(SceneObject{})
		.set(EntitySystem::ECS::GeometryPtr{ RenderCore::DefaultGeometry::Cube() })
		.set(EntitySystem::ECS::RenderObjectPtr{ new Render::RenderObject() })
		.set(ControllerPtr{ new Core::Controller(Core::g_FileSystem->GetConfigPath("Input_default.ini")) });

	flecs::entity cubeMoving = m_World.entity()
		.set(Position{ 2.f, 0.f, 0.f })
		.set(Velocity{ 0.f, 3.f, 0.f })
		.set(Gravity{ 0.f, -9.8065f, 0.f })
		.set(BouncePlane{ 0.f, 1.f, 0.f, 5.f })
		.set(Bounciness{ 1.f })
		.set(SceneObject{})
		.set(EntitySystem::ECS::GeometryPtr{ RenderCore::DefaultGeometry::Cube() })
		.set(EntitySystem::ECS::RenderObjectPtr{ new Render::RenderObject() });

	flecs::entity camera = m_World.entity()
		.set(Position{ 0.0f, 12.0f, -10.0f })
		.set(Speed{ 10.f })
		.set(Player{})
		.set(CameraPtr{ Core::g_MainCamera })
		.set(ControllerPtr{ new Core::Controller(Core::g_FileSystem->GetConfigPath("Input_default.ini")) });
}

//adding it here because DefaultGeometry.h cannot be imported from different files, as the Cube function is defined in .h file. And when i try to move the function definition in it into a .cpp, it fails to find the declaration.
void GameFramework::RegisterAdditionalSystems(flecs::world& world)
{
	//Shooting system
	world.system<Position, CameraPtr, const Speed, const ControllerPtr>()
		.each([&](flecs::entity e, Position& position, CameraPtr& camera, const Speed& speed, const ControllerPtr& controller)
			{
				if (controller.ptr->IsPressed("Shoot") && !e.has<ShootCountdown>())
				{
					e.set(ShootCountdown{ 30 });
					Math::Vector3f vel = camera.ptr->GetViewDir() * 100;
					//shooting is bound to space button, because for some reason if i bind lmb, after a single click it records a press every frame
					flecs::entity cubeMoving = world.entity()
						.set(Position{ position.x, position.y, position.z })
						.set(Velocity{ vel.x, vel.y, vel.z })
						.set(Gravity{ 0.f, -9.8065f, 0.f })
						.set(BouncePlane{ 0.f, 1.f, 0.f, 5.f })
						.set(Bounciness{ 0.3f })
						.set(BulletDestructor{ 100 })
						.set(EntitySystem::ECS::GeometryPtr{ RenderCore::DefaultGeometry::Cube() })
						.set(EntitySystem::ECS::RenderObjectPtr{ new Render::RenderObject() });
				}
			});

	world.system<ShootCountdown>()
		.each([&](flecs::entity e, ShootCountdown& countdown) {
		countdown.ttl--;
		if (countdown.ttl <= 0) {
			e.remove<ShootCountdown>();
		}
			});

	world.system<SceneObject, const EntitySystem::ECS::RenderObjectPtr>()
		.each([&](flecs::entity e, SceneObject& switcher, const EntitySystem::ECS::RenderObjectPtr& controller) {
		if (switcher.switchColor) {
			switcher.switchColor = false;
			controller.ptr->SetAlbedo(RenderCore::Colors::Blue);
		}
			});
}

void GameFramework::RegisterComponents()
{
	// Exposing these components for the lua system
	ECS_META_COMPONENT(m_World, Position);
	ECS_META_COMPONENT(m_World, Velocity);
	ECS_META_COMPONENT(m_World, Gravity);
	ECS_META_COMPONENT(m_World, BouncePlane);
	ECS_META_COMPONENT(m_World, Bounciness);
	ECS_META_COMPONENT(m_World, ShiverAmount);
	ECS_META_COMPONENT(m_World, FrictionAmount);
	ECS_META_COMPONENT(m_World, Speed);

	ECS_META_COMPONENT(m_World, Player);
	ECS_META_COMPONENT(m_World, BulletDestructor);
	ECS_META_COMPONENT(m_World, ShootCountdown);
	ECS_META_COMPONENT(m_World, SceneObject);
}

void GameFramework::RegisterSystems()
{
	RegisterEcsMeshSystems(m_World);
	RegisterEcsControlSystems(m_World);
	RegisterAdditionalSystems(m_World);
}

void GameFramework::Update(float dt)
{

}