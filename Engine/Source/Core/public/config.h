#pragma once
#include <Core/export.h>
#include<unordered_map>

namespace GameEngine
{
    namespace Core
    {
        typedef char ControlKey;
        const ControlKey MovementForward = 0;
        const ControlKey MovementRight = 1;
        const ControlKey MovementLeft = 2;
        const ControlKey MovementBackward = 3;

        class CORE_API Config final
        {
        public:
            Config() = default;
            void ReadIni();
            bool ControlPressed(ControlKey key);

            float Speed = 0.5;

        private:
            std::unordered_map<ControlKey, short> keybinds;

        };

        extern CORE_API Config* g_globalConfig;
    }
}