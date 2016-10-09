#ifndef NPC_H
#define NPC_H

#include "Logger.h"

#include <string>

#ifdef _DEBUG
#define BOT_LOGIC_DEBUG_NPC
#endif

#ifdef BOT_LOGIC_DEBUG_NPC
#define BOT_LOGIC_NPC_LOG(logger, text, autoEndLine) logger.Log(text, autoEndLine)
#else
#define BOT_LOGIC_NPC_LOG(logger, text, autoEndLine) 0
#endif


class Npc 
{
private:
    enum npcState {
        NONE,
        MOVING,
        WAITING,
        ARRIVED,
        EXPLORING,
        INTERACTING
    };

    npcState m_currentState, m_nextState;
    unsigned int m_id;
    Logger m_logger;

public:
    Npc(unsigned int a_id, std::string a_path);

    void update();
};


#endif //NPC_H
