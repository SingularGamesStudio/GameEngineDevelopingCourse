#pragma once

#include <ECS/ecsSystems.h>
#include <flecs.h>
#include <Geometry.h>
#include <RenderThread.h>
#include <RenderObject.h>

namespace GameEngine::EntitySystem::ECS
{
	void RegisterEcsCoreSystems(flecs::world& world)
	{
		static const RenderThreadPtr* renderThread = world.get<RenderThreadPtr>();

		world.system<const GeometryPtr>()
			.each([&](flecs::entity e, const GeometryPtr& geometry)
				{
					Render::RenderObject* renderObjectPtr = new Render::RenderObject();
					e.set(RenderObjectPtr{ renderObjectPtr });
					renderThread->ptr->EnqueueCommand(Render::ERC::CreateRenderObject, geometry.ptr, renderObjectPtr);
					e.remove<GeometryPtr>();
				});

		world.component<RenderObjectPtr>()
			.on_remove([&](flecs::entity e, RenderObjectPtr& renderObject)
				{
					renderThread->ptr->EnqueueCommand(Render::ERC::DeleteRenderObject, static_cast<RenderCore::Geometry*>(nullptr), renderObject.ptr);
				});
	}
}
