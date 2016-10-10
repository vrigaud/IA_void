#ifndef NPC_H
#define NPC_H

#include "Logger.h"

#include <string>
#include <vector>

#ifdef _DEBUG
#define BOT_LOGIC_DEBUG_NPC
#endif

#ifdef BOT_LOGIC_DEBUG_NPC
#define BOT_LOGIC_NPC_LOG(logger, text, autoEndLine) logger.Log(text, autoEndLine)
#else
#define BOT_LOGIC_NPC_LOG(logger, text, autoEndLine) 0
#endif

struct Action;

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
    unsigned int m_goal;
    bool m_hasGoal;
    std::vector<unsigned int> m_path;
    std::vector<Action> m_nextActions;
    Logger m_logger;

public:
    Npc(unsigned int a_id, std::string a_path);

    // Main fonction for our Npc's FSM 
    void update();

    // Stop everything the npc is doing
    bool stopEverything();

    // Remove all move action from next_action List
    void stopMoving();

    // Remove all interact action from next_action List
    void stopInteract();

    // Unstack m_nextAction and act according to action_type
    void unstackActions();

    // check path integrity, if pass is corrupted, try to find a new path return true if found
    bool updatePath();
    unsigned int getCurrentTileId()
    {
        return m_path.front();
    }
    int getNextPathTile()const;
    unsigned int getPathSize() const
    {
        return m_path.size();
    }
    std::vector<unsigned int> getWholePath() const
    {
        return m_path;
    }
    
    void setGoal(unsigned int a_id)
    {
        m_goal = a_id;
    }
    bool isOnGoalTile() const
    {
        return m_currentState == ARRIVED;
    }
    void moveForwardOnPath() // remove last tileId from m_path
    {
        m_path.pop_back();
    }

    std::vector<Action>& getActions()
    {
        return m_nextActions;
    }

private:
    void explore();
    void followPath();
    void wait();
    void interact();
};


#endif //NPC_H
