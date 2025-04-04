#include <LevelEditor/LevelEditor.h>
#include <LevelEditor/LevelSerializer.h>
#include <LevelEditor/ECS/ecsLevelEditor.h>

#include <Filesystem.h>
#include <ECS/ecsSystems.h>
#include <Parser/WorldParser.h>

#include <imgui.h>
#include <imgui_stdlib.h>

namespace GameEngine
{
	RenderCore::Geometry* Cube();

	namespace Editor
	{
		LevelEditor::LevelEditor(flecs::world& world) : world(world)
		{
			m_Level = LevelSerializer::Deserialize(Core::g_FileSystem->GetFilePath("Levels/Main.xml").generic_string());

			for (World::LevelObject& levelObject : m_Level->GetLevelObjects())
			{
				flecs::entity entity = world.entity(levelObject.GetName().c_str());

				World::LevelObject::ComponentList& componentList = levelObject.GetComponents();

				World::LevelObject::ComponentList::iterator positionAttribute = std::ranges::find_if(componentList,
					[](World::LevelObject::Component& component)
					{
						return !std::strcmp(component.first.c_str(), "Position");
					}
				);

				World::LevelObject::ComponentList::iterator geometryAttribute = std::ranges::find_if(componentList,
					[](World::LevelObject::Component& component)
					{
						return !std::strcmp(component.first.c_str(), "GeometryPtr");
					}
				);

				if (positionAttribute != componentList.end() &&
					geometryAttribute != componentList.end())
				{
					assert(World::WorldParser::GetCustomComponents().contains(geometryAttribute->second));

					entity.set(EntitySystem::LevelEditorECS::PositionDesc{ &positionAttribute->second });

					// Can be set to 0 since it doesn't matter now, will be updated by the system
					entity.set(EntitySystem::EditorECS::Position{ 0.0f, 0.0f, 0.0f });
					entity.set(GeometryPtr{
						reinterpret_cast<RenderCore::Geometry*>(
							World::WorldParser::GetCustomComponents()[geometryAttribute->second]
							)
						});
				}
			}

			EntitySystem::LevelEditorECS::RegisterLevelEditorEcsSystems(world);
		}

		void LevelEditor::Draw()
		{
			ImGui::Begin(GetName());

			if (m_Level.has_value()) [[likely]]
			{
				for (World::LevelObject& levelObject : m_Level->GetLevelObjects())
				{
					if (ImGui::TreeNode(levelObject.GetName().c_str()))
					{
						for (World::LevelObject::Component& component : levelObject.GetComponents())
						{
							if (component.first == "Position") {
								float pos[3];
								sscanf(component.second.c_str(), "%f,%f,%f", &pos[0], &pos[1], &pos[2]);
								ImGui::InputFloat3("Position", pos);
								char buf[100];
								sprintf(buf, "%f,%f,%f", pos[0], pos[1], pos[2]);
								component.second = buf;
							}
							else {
								ImGui::InputText(component.first.c_str(), &component.second);
							}
						}

						ImGui::TreePop();
					}
				}
			}

			if (ImGui::Button("New")) {
				NewObject();
			}

			if (ImGui::Button("Save"))
			{
				m_SaveButtonMessageTimer.Reset();
				m_SaveButtonPressed = true;

				Save();
			}

			if (m_SaveButtonPressed)
			{
				ImGui::SameLine();
				ImGui::Text("Saved!");
			}

			ImGui::End();
		}

		void LevelEditor::Update(float dt)
		{
			m_SaveButtonMessageTimer.Tick();

			if (m_SaveButtonMessageTimer.GetTotalTime() > m_TimeToShowSaveButtonMessage)
			{
				m_SaveButtonPressed = false;
			}
		}

		void LevelEditor::Save()
		{
			assert(m_Level.has_value());
			LevelSerializer::Serialize(Core::g_FileSystem->GetFilePath("Levels/Main.xml").generic_string(), m_Level.value());
		}

		void LevelEditor::NewObject() {
			flecs::entity entity;
			World::LevelObject* newLevelObject = m_Level->AddLevelObject(World::LevelObject());

			entity = world.entity("New Object");
			newLevelObject->SetName("New Object");

			newLevelObject->AddComponent("GeometryPtr", "Cube");
			entity.set(GeometryPtr{ Cube() });

			newLevelObject->AddComponent("Position", "0.0, 0.0, 0.0");
			entity.set(EntitySystem::LevelEditorECS::PositionDesc{ &newLevelObject->GetComponents()[1].second });

			// Can be set to 0 since it doesn't matter now, will be updated by the system
			entity.set(EntitySystem::EditorECS::Position{ 0.0f, 0.0f, 0.0f });
		}

	}


	RenderCore::Geometry* Cube()
	{
		constexpr Core::array<RenderCore::Geometry::VertexType, 8> vertices =
		{
			Math::Vector3f(-1.0f, -1.0f, -1.0f),
			Math::Vector3f(-1.0f, +1.0f, -1.0f),

			Math::Vector3f(+1.0f, +1.0f, -1.0f),
			Math::Vector3f(+1.0f, -1.0f, -1.0f),
			Math::Vector3f(-1.0f, -1.0f, +1.0f),
			Math::Vector3f(-1.0f, +1.0f, +1.0f),
			Math::Vector3f(+1.0f, +1.0f, +1.0f),
			Math::Vector3f(+1.0f, -1.0f, +1.0f)
		};

		constexpr Core::array<RenderCore::Geometry::IndexType, 36> indices =
		{
			// front face
			0, 1, 2,
			0, 2, 3,

			// back face
			4, 6, 5,
			4, 7, 6,

			// left face
			4, 5, 1,
			4, 1, 0,

			// right face
			3, 2, 6,
			3, 6, 7,

			// top face
			1, 5, 6,
			1, 6, 2,

			// bottom face
			4, 0, 3,
			4, 3, 7
		};

		return new RenderCore::Geometry((RenderCore::Geometry::VertexType*)vertices.begin(), vertices.size(), (RenderCore::Geometry::IndexType*)indices.begin(), indices.size());
	}
}

