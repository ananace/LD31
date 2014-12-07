#include "Extensions.hpp"
#include "Extensions.inl"
#include <Script/ScriptExtensions.hpp>

#include <SFML/Network/Packet.hpp>

#include <angelscript.h>

#include <cassert>

namespace
{
    void create_packet(sf::Packet* memory)
    {
        new (memory)sf::Packet();
    }

    void destruct_packet(sf::Packet* memory)
    {
        memory->~Packet();
    }

    std::string packet_getData(const sf::Packet& packet)
    {
        return std::string((const char*)packet.getData(), packet.getDataSize());
    }


    bool Reg()
    {
        Script::ScriptExtensions::AddExtension([](asIScriptEngine* eng){
            int r = 0;

            r = eng->RegisterObjectType("Packet", sizeof(sf::Packet), asOBJ_VALUE | asGetTypeTraits<sf::Packet>()); assert(r >= 0);

            r = eng->RegisterObjectBehaviour("Packet", asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(create_packet), asCALL_CDECL_OBJLAST); assert(r >= 0);
            r = eng->RegisterObjectBehaviour("Packet", asBEHAVE_DESTRUCT, "void f()", asFUNCTION(destruct_packet), asCALL_CDECL_OBJLAST); assert(r >= 0);

            r = eng->RegisterObjectMethod("Packet", "Packet& opAssign(Packet&in)", asMETHODPR(sf::Packet, operator=, (const sf::Packet&), sf::Packet&), asCALL_THISCALL); assert(r >= 0);

            r = eng->RegisterObjectMethod("Packet", "void Clear()", asMETHOD(sf::Packet, clear), asCALL_THISCALL); assert(r >= 0);

            r = eng->RegisterObjectMethod("Packet", "bool opConvImpl()", asMETHOD(sf::Packet, operator sf::Packet::BoolType), asCALL_THISCALL); assert(r >= 0);

            r = eng->RegisterObjectMethod("Packet", "Packet& opShr(bool&out)", asMETHODPR(sf::Packet, operator>>, (bool&), sf::Packet&), asCALL_THISCALL); assert(r >= 0);
            r = eng->RegisterObjectMethod("Packet", "Packet& opShr(int8&out)", asMETHODPR(sf::Packet, operator>>, (int8_t&), sf::Packet&), asCALL_THISCALL); assert(r >= 0);
            r = eng->RegisterObjectMethod("Packet", "Packet& opShr(uint8&out)", asMETHODPR(sf::Packet, operator>>, (uint8_t&), sf::Packet&), asCALL_THISCALL); assert(r >= 0);
            r = eng->RegisterObjectMethod("Packet", "Packet& opShr(int16&out)", asMETHODPR(sf::Packet, operator>>, (int16_t&), sf::Packet&), asCALL_THISCALL); assert(r >= 0);
            r = eng->RegisterObjectMethod("Packet", "Packet& opShr(uint16&out)", asMETHODPR(sf::Packet, operator>>, (uint16_t&), sf::Packet&), asCALL_THISCALL); assert(r >= 0);
            r = eng->RegisterObjectMethod("Packet", "Packet& opShr(int32&out)", asMETHODPR(sf::Packet, operator>>, (int32_t&), sf::Packet&), asCALL_THISCALL); assert(r >= 0);
            r = eng->RegisterObjectMethod("Packet", "Packet& opShr(uint32&out)", asMETHODPR(sf::Packet, operator>>, (uint32_t&), sf::Packet&), asCALL_THISCALL); assert(r >= 0);
            r = eng->RegisterObjectMethod("Packet", "Packet& opShr(int64&out)", asMETHODPR(sf::Packet, operator>>, (int64_t&), sf::Packet&), asCALL_THISCALL); assert(r >= 0);
            r = eng->RegisterObjectMethod("Packet", "Packet& opShr(uint64&out)", asMETHODPR(sf::Packet, operator>>, (uint64_t&), sf::Packet&), asCALL_THISCALL); assert(r >= 0);
            r = eng->RegisterObjectMethod("Packet", "Packet& opShr(float&out)", asMETHODPR(sf::Packet, operator>>, (float&), sf::Packet&), asCALL_THISCALL); assert(r >= 0);
            r = eng->RegisterObjectMethod("Packet", "Packet& opShr(double&out)", asMETHODPR(sf::Packet, operator>>, (double&), sf::Packet&), asCALL_THISCALL); assert(r >= 0);
            r = eng->RegisterObjectMethod("Packet", "Packet& opShr(string&out)", asMETHODPR(sf::Packet, operator>>, (std::string&), sf::Packet&), asCALL_THISCALL); assert(r >= 0);

            r = eng->RegisterObjectMethod("Packet", "Packet& opShl(bool)", asMETHODPR(sf::Packet, operator<<, (bool), sf::Packet&), asCALL_THISCALL); assert(r >= 0);
            r = eng->RegisterObjectMethod("Packet", "Packet& opShl(int8)", asMETHODPR(sf::Packet, operator<<, (int8_t), sf::Packet&), asCALL_THISCALL); assert(r >= 0);
            r = eng->RegisterObjectMethod("Packet", "Packet& opShl(uint8)", asMETHODPR(sf::Packet, operator<<, (uint8_t), sf::Packet&), asCALL_THISCALL); assert(r >= 0);
            r = eng->RegisterObjectMethod("Packet", "Packet& opShl(int16)", asMETHODPR(sf::Packet, operator<<, (int16_t), sf::Packet&), asCALL_THISCALL); assert(r >= 0);
            r = eng->RegisterObjectMethod("Packet", "Packet& opShl(uint16)", asMETHODPR(sf::Packet, operator<<, (uint16_t), sf::Packet&), asCALL_THISCALL); assert(r >= 0);
            r = eng->RegisterObjectMethod("Packet", "Packet& opShl(int32)", asMETHODPR(sf::Packet, operator<<, (int32_t), sf::Packet&), asCALL_THISCALL); assert(r >= 0);
            r = eng->RegisterObjectMethod("Packet", "Packet& opShl(uint32)", asMETHODPR(sf::Packet, operator<<, (uint32_t), sf::Packet&), asCALL_THISCALL); assert(r >= 0);
            r = eng->RegisterObjectMethod("Packet", "Packet& opShl(int64)", asMETHODPR(sf::Packet, operator<<, (int64_t), sf::Packet&), asCALL_THISCALL); assert(r >= 0);
            r = eng->RegisterObjectMethod("Packet", "Packet& opShl(uint64)", asMETHODPR(sf::Packet, operator<<, (uint64_t), sf::Packet&), asCALL_THISCALL); assert(r >= 0);
            r = eng->RegisterObjectMethod("Packet", "Packet& opShl(float)", asMETHODPR(sf::Packet, operator<<, (float), sf::Packet&), asCALL_THISCALL); assert(r >= 0);
            r = eng->RegisterObjectMethod("Packet", "Packet& opShl(double)", asMETHODPR(sf::Packet, operator<<, (double), sf::Packet&), asCALL_THISCALL); assert(r >= 0);
            r = eng->RegisterObjectMethod("Packet", "Packet& opShl(string&in)", asMETHODPR(sf::Packet, operator<<, (const std::string&), sf::Packet&), asCALL_THISCALL); assert(r >= 0);

            r = eng->RegisterObjectMethod("Packet", "uint64 get_Size() const", asMETHOD(sf::Packet, getDataSize), asCALL_THISCALL); assert(r >= 0);
            r = eng->RegisterObjectMethod("Packet", "string get_Data() const", asFUNCTION(packet_getData), asCALL_CDECL_OBJLAST); assert(r >= 0);
            r = eng->RegisterObjectMethod("Packet", "bool get_EOF()", asMETHOD(sf::Packet, endOfPacket), asCALL_THISCALL); assert(r >= 0);

        });

        return true;
    }

}

bool Script::SFML::Extensions::Packet = Reg();
