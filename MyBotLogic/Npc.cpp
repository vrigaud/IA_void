#include "Npc.h"
#include "Map.h"

/* INCLUDE FROM CARLE */
#include "Globals.h"

#include <algorithm>


Npc::Npc(unsigned int a_id, std::string a_path)
    : m_currentState{ NONE }, m_nextState{ EXPLORING }, m_id{ a_id }, m_goal{}, m_hasGoal{ false }, m_path{}, m_nextActions{}
{
#ifdef BOT_LOGIC_DEBUG_NPC
    m_logger.Init(a_path, "Npc_" + std::to_string(m_id) + ".log");
#endif

    BOT_LOGIC_NPC_LOG(m_logger, "Configure", true);

};

void Npc::update()
{
    do
    {
        m_currentState = m_nextState;
        // TODO - Change State to another
        switch (m_currentState)
        {
        case(MOVING):
            followPath();
            break;
        case(WAITING):
            wait();
            break;
        case(EXPLORING):
            explore();
            break;
        case(INTERACTING):
            interact();
            break;
        case(ARRIVED):
            m_nextState = ARRIVED; // May be useless atm
            m_currentState = ARRIVED;
            break;
        }
    } while (m_currentState != m_nextState);
}

bool Npc::stopEverything()
{
    m_nextActions.clear();
}

void Npc::stopMoving()
{
    auto it = std::partition(std::begin(m_nextActions),
        std::end(m_nextActions),
        [](const Action& curAction) { return curAction.actionType == Action_Move;});

    m_nextActions.erase(it, std::end(m_nextActions));
}

void Npc::stopInteract()
{
    auto it = std::partition(std::begin(m_nextActions),
        std::end(m_nextActions),
        [](const Action& curAction) { return curAction.actionType == Action_Interact;});

    m_nextActions.erase(it, std::end(m_nextActions));
}

void Npc::unstackActions()
{
    while (m_nextActions.size()) 
    {
        Action& curAction{ m_nextActions.back() };
        switch (curAction.actionType)
        {
        case Action_None:
            // Do nothing
            break;
        case Action_Move:
            moveForwardOnPath();
            break;
        case Action_Interact:
            // TODO - do nothing atm
            break;
        }
        m_nextActions.pop_back();
    }
}

bool Npc::updatePath()
{
    // TODO - check path integrity and update if needed.

    return true;
}

int Npc::getNextPathTile() const
{
    if (m_path.size() == 1)
    {
        return -1;
    }
    unsigned int index = m_path[m_path.size() - 2];
    return index;
}

void Npc::explore()
{
    // TODO - explore the map with a lot of efficiency
    if (m_hasGoal)
    {
        m_nextState = MOVING;
        return;
    }

    // Ask map for potential direction

    // Choose a direction

    // Update next action in the vector
}

void Npc::followPath()
{
    // TODO - follow the path
    // Get the direction between the two last nodes of m_path
    if (getCurrentTileId() == m_goal)
    {
        m_nextState = ARRIVED;
        return;
    }
    m_nextActions.push_back(Move{ m_id, Map::get()->getNextDirection(getCurrentTileId(), getNextPathTile())});
    m_nextState = MOVING;
}

void Npc::wait()
{
    // TODO - Test why we are blocked ?
}

void Npc::interact()
{
    // TODO - interact with some fancy stuff
}