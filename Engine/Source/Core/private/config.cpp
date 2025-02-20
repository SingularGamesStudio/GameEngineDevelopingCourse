#include <config.h>
#include <INIReader.h>
#include <windows.h>

namespace GameEngine
{
    namespace Core
    {
        Config* g_globalConfig = nullptr;

        void Config::ReadIni() {
            INIReader reader("../config.ini");
            keybinds[MovementForward] = reader.GetInteger("KeyBindings", "KEY_FORWARD", 0x26);//default bindings are arrow keys
            keybinds[MovementRight] = reader.GetInteger("KeyBindings", "KEY_RIGHT", 0x27);
            keybinds[MovementLeft] = reader.GetInteger("KeyBindings", "KEY_LEFT", 0x25);
            keybinds[MovementBackward] = reader.GetInteger("KeyBindings", "KEY_BACKWARD", 0x28);
        }

        bool Config::ControlPressed(ControlKey key) {
            return GetAsyncKeyState(keybinds[key]);
        }
    }
}