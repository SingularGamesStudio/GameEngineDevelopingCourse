#include <Camera.h>
#include <DefaultGeometry.h>
#include <Game.h>
#include <GameObject.h>
#include <Input/InputHandler.h>

namespace GameEngine
{
	Game::Game(
		std::function<bool()> PlatformLoopFunc
	) :
		PlatformLoop(PlatformLoopFunc)
	{
		Core::g_MainCamera = new Core::Camera();
		Core::g_MainCamera->SetPosition(Math::Vector3f(0.0f, 30.0f, -30.0f));
		Core::g_MainCamera->SetViewDir(Math::Vector3f(0.0f, -6.0f, 6.0f).Normalized());

		m_renderThread = std::make_unique<Render::RenderThread>();

		// How many objects do we want to create
		for (int i = 0; i < 10; ++i)
		{
			for (int j = 0; j < 10; ++j)
			{
				m_Objects.push_back(new GameObject());
				int type = rand() % 3;
				if (type == 0) {
					m_Objects.back()->Behaviour = new GOBehaviour::Physical();
				}
				else if (type == 1) {
					m_Objects.back()->Behaviour = new GOBehaviour::Controlled();
				}
				else {
					m_Objects.back()->Behaviour = new GOBehaviour::Dynamic(rand() % 10);
				}
				m_Objects.back()->SetPosition(Math::Vector3f((i - 5) * 3, rand() % 20 + 5, (j - 5) * 3), m_renderThread->GetMainFrame());
				Render::RenderObject** renderObject = m_Objects.back()->GetRenderObjectRef();
				m_renderThread->EnqueueCommand(Render::ERC::CreateRenderObject, RenderCore::DefaultGeometry::Cube(), renderObject);
			}
		}

		Core::g_InputHandler->RegisterCallback("GoForward", [&]() { Core::g_MainCamera->Move(Core::g_MainCamera->GetViewDir()); });
		Core::g_InputHandler->RegisterCallback("GoBack", [&]() { Core::g_MainCamera->Move(-Core::g_MainCamera->GetViewDir()); });
		Core::g_InputHandler->RegisterCallback("GoRight", [&]() { Core::g_MainCamera->Move(Core::g_MainCamera->GetRightDir()); });
		Core::g_InputHandler->RegisterCallback("GoLeft", [&]() { Core::g_MainCamera->Move(-Core::g_MainCamera->GetRightDir()); });
	}

	void Game::Run()
	{
		assert(PlatformLoop != nullptr);

		m_GameTimer.Reset();

		bool quit = false;
		while (!quit)
		{
			m_GameTimer.Tick();
			float dt = m_GameTimer.GetDeltaTime();

			Core::g_MainWindowsApplication->Update();
			Core::g_InputHandler->Update();
			Core::g_MainCamera->Update(dt);

			Update(dt);

			m_renderThread->OnEndFrame();

			// The most common idea for such a loop is that it returns false when quit is required, or true otherwise
			quit = !PlatformLoop();
		}
	}

	void Game::Update(float dt)
	{
		for (int i = 0; i < m_Objects.size(); ++i)
		{
			m_Objects[i]->Update(dt, m_renderThread->GetMainFrame());
		}
		if (GOBehaviour::g_controlledMove != nullptr)
			*GOBehaviour::g_controlledMove = Math::Vector3f::Zero();
	}
}