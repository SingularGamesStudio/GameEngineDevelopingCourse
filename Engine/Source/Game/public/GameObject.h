#pragma once

#include <RenderObject.h>
#include <RenderThread.h>
#include <Vector.h>
#include <Input/InputHandler.h>

namespace GameEngine
{
	namespace GOBehaviour {
		struct Behaviour {
			virtual void Update(float dt, Math::Vector3f& pos) = 0;
		};

		extern Math::Vector3f* g_controlledMove;

		class Controlled :public Behaviour {
		private:
			float velocity;
			static void move(Math::Vector3f dir) {
				*g_controlledMove = *g_controlledMove + dir;
			}
		public:
			Controlled(float velocity = 2) : velocity(velocity) {
				if (g_controlledMove == nullptr) {
					g_controlledMove = new Math::Vector3f(0, 0, 0);
					Core::g_InputHandler->RegisterCallback("ObjectsRight", [&]() { move(Math::Vector3f(1, 0, 0)); });
					Core::g_InputHandler->RegisterCallback("ObjectsLeft", [&]() { move(Math::Vector3f(-1, 0, 0)); });
					Core::g_InputHandler->RegisterCallback("ObjectsForward", [&]() { move(Math::Vector3f(0, 0, 1)); });
					Core::g_InputHandler->RegisterCallback("ObjectsBack", [&]() { move(Math::Vector3f(0, 0, -1)); });
				}
			}

			void Update(float dt, Math::Vector3f& pos) override {
				pos = pos + g_controlledMove->Normalized() * dt * velocity;
			}
		};

		class Physical :public Behaviour {
		private:
			float velocity;
			float acceleration;
		public:
			Physical(float acceleration = 1.0 / 100.0) : velocity(0), acceleration(acceleration) {}

			void Update(float dt, Math::Vector3f& pos) override {
				velocity += dt * acceleration;
				pos = pos + Math::Vector3f(0, -velocity, 0);
				if (pos.y < 0 && velocity>0) {
					velocity *= -1;
				}
			}
		};

		class Dynamic :public Behaviour {
		private:
			float progress;
			float velocity;
		public:
			Dynamic(float velocity = 5) : progress(5), velocity(velocity) {}

			void Update(float dt, Math::Vector3f& pos) override {
				progress += velocity * dt;
				pos = pos + Math::Vector3f(velocity * dt, 0, 0);
				if (progress >= 10 && velocity > 0) {
					velocity *= -1;
				}
				if (progress < 0 && velocity < 0) {
					velocity *= -1;
				}
			}
		};
	}

	class GameObject final
	{
	public:
		GOBehaviour::Behaviour* Behaviour = nullptr;

		GameObject() = default;

	public:
		Render::RenderObject** GetRenderObjectRef() { return &m_RenderObject; }

		void SetPosition(Math::Vector3f position, size_t frame)
		{
			m_Position = position;

			if (m_RenderObject) [[likely]]
			{
				m_RenderObject->SetPosition(position, frame);
			}
		}

		Math::Vector3f GetPosition()
		{
			return m_Position;
		}

		void Update(float dt, size_t frame) {
			if (Behaviour != nullptr) {
				Behaviour->Update(dt, m_Position);
			}

			SetPosition(m_Position, frame);
		}

	protected:
		Render::RenderObject* m_RenderObject = nullptr;

		Math::Vector3f m_Position = Math::Vector3f::Zero();
	};
}