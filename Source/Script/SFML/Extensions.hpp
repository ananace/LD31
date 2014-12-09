#pragma once

#include <Defines.hpp>

#if !(defined LD31_LINUX && defined LD31_64)
#define AS_SUPPORT_VALRET
#endif

namespace Script
{

namespace SFML
{
    class Extensions
    {
        static bool CircleShape; // Value
        static bool Color; // Value
        static bool ConvexShape; // Value
        static bool Font; // Ref, No count
        static bool Joystick; // Interface
        static bool Keyboard; // Interface
        static bool Mouse; // Interface
        static bool Music; // Ref
        static bool Packet; // Value
        static bool RectangleShape; // Value
        static bool RenderTarget; // Ref, No count
        static bool Shader; // Ref
        static bool Sound; // Value (Sound buffer - Ref)
        static bool Sprite; // Value
        static bool Text; // Value
        static bool Texture; // Ref
        static bool View; // Ref, No count

    public:
        static bool RegisteredExtensions();
    };
}

}