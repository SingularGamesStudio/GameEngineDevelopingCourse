#include <Level.h>

namespace GameEngine::World
{
	Level::Level(std::string& name)
		: m_Name(std::move(name))
	{
	}

	LevelObject* Level::AddLevelObject(const LevelObject& levelObject)
	{
		m_LevelObjectList.push_back(levelObject);
		return &m_LevelObjectList[m_LevelObjectList.size() - 1];
	}
}