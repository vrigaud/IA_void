#include "Npc.h"
#include <windows.h>

Npc::Npc(unsigned int a_id, std::string a_path)
    : m_currentState{ NONE }, m_nextState{ NONE }, m_id{ a_id }
{
#ifdef BOT_LOGIC_DEBUG_NPC
    m_logger.Init(a_path, "Npc_" + std::to_string(m_id) + ".log");
#endif

    BOT_LOGIC_NPC_LOG(m_logger, "Configure", true);

};

void Npc::update()
{
    while (m_currentState != m_nextState)
    {
        // TODO - Change State to another
        switch (m_currentState)
        {
        case(MOVING):

            break;
        case(WAITING):

            break;
        case(EXPLORING):

            break;
        case(INTERACTING):

            break;
        case(ARRIVED):

            break;
        }
    }
}