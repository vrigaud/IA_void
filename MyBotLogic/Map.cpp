#include "Map.h"
#include "TurnInfo.h"
#include "SearchMap.h"
#include "NPCInfo.h"

Map Map::m_instance;

void Map::setLoggerPath(const std::string &a_path)
{
#ifdef BOT_LOGIC_DEBUG_MAP
    m_logger.Init(a_path, "Map.log");
#endif

    BOT_LOGIC_MAP_LOG(m_logger, "Configure", true);
}

void Map::setNodeType(unsigned int tileId, Node::NodeType tileType)
{
    m_nodeMap[tileId]->setType(tileType);
}

void Map::createNode(Node* node)
{
    m_nodeMap[node->getId()] = node;
}

void Map::connectNodes()
{
    for(std::pair<unsigned int, Node*> curNode : m_nodeMap)
    {
        // connecting
        Node* nw;
        Node* ne;
        Node* e;
        Node* se;
        Node* sw;
        Node* w;
        if(curNode.second->getPosition()->y % 2 == 0)
        {
            nw = getNode(curNode.second->getPosition()->x - 1, curNode.second->getPosition()->y - 1);
            ne = getNode(curNode.second->getPosition()->x, curNode.second->getPosition()->y - 1);
            e = getNode(curNode.second->getPosition()->x + 1, curNode.second->getPosition()->y);
            se = getNode(curNode.second->getPosition()->x, curNode.second->getPosition()->y + 1);
            sw = getNode(curNode.second->getPosition()->x - 1, curNode.second->getPosition()->y + 1);
            w = getNode(curNode.second->getPosition()->x - 1, curNode.second->getPosition()->y);
        }
        else
        {
            nw = getNode(curNode.second->getPosition()->x, curNode.second->getPosition()->y - 1);
            ne = getNode(curNode.second->getPosition()->x + 1, curNode.second->getPosition()->y - 1);
            e = getNode(curNode.second->getPosition()->x + 1, curNode.second->getPosition()->y);
            se = getNode(curNode.second->getPosition()->x + 1, curNode.second->getPosition()->y + 1);
            sw = getNode(curNode.second->getPosition()->x, curNode.second->getPosition()->y + 1);
            w = getNode(curNode.second->getPosition()->x - 1, curNode.second->getPosition()->y);
        }
        curNode.second->setNeighboor(NW, nw);
        curNode.second->setNeighboor(NE, ne);
        curNode.second->setNeighboor(E, e);
        curNode.second->setNeighboor(SE, se);
        curNode.second->setNeighboor(SW, sw);
        curNode.second->setNeighboor(W, w);
    }
}

Node* Map::getNode(unsigned int x, unsigned int y)
{
    if(x < 0 || x > getWidth() - 1 || y < 0 || y > getHeight() - 1)
    {
        return nullptr;
    }
    unsigned int index = x + y * m_width;
    return m_nodeMap[index];
}

Node* Map::getNode(unsigned int index)
{
    return m_nodeMap[index];
}

float Map::calculateDistance(int indexStart, int indexEnd)
{
    Node* start = m_nodeMap[indexStart];
    Node* end = m_nodeMap[indexEnd];
    int x = start->getPosition()->x - end->getPosition()->x;
    int y = start->getPosition()->y - end->getPosition()->y;
    return abs(x) + abs(y);
}

std::map<unsigned, unsigned> Map::getBestGoalTile(std::map<unsigned, NPCInfo> npcInfo)
{
    std::map<unsigned, unsigned> goalMap;
    if(m_goalTiles.size() > npcInfo.size())
    {
        auto copyMapGoal = m_goalTiles;
        for(std::pair<unsigned, NPCInfo> npc : npcInfo)
        {
            int bestDist = 666;
            unsigned goalId = -1;
            std::vector<unsigned>::iterator goalIt = begin(copyMapGoal);
            for(; goalIt != end(copyMapGoal); ++goalIt)
            {
                float distance = calculateDistance(npc.second.tileID, *goalIt);
                if(distance < bestDist)
                {
                    goalId = *goalIt;
                    bestDist = distance;
                }
            }
            goalMap[npc.second.npcID] = goalId;
            copyMapGoal.erase(std::find(begin(copyMapGoal), end(copyMapGoal), goalId));
        }
    }
    else
    {
        for(unsigned goal : m_goalTiles)
        {
            if(npcInfo.size() <= 0)
            {
                break;
            }
            int bestDist = 666;
            int npcId = -1;
            for(std::pair<unsigned, NPCInfo> npc : npcInfo)
            {
                float distance = calculateDistance(npc.second.tileID, goal);
                if(distance < bestDist)
                {
                    npcId = npc.second.npcID;
                    bestDist = distance;
                }
            }
            goalMap[npcId] = goal;
            npcInfo.erase(npcId);
        }
    }    

    return goalMap;
}


void Map::addGoalTile(unsigned int number)
{
    if(std::find(begin(m_goalTiles), end(m_goalTiles), number) == end(m_goalTiles))
    {
        m_goalTiles.push_back(number);
    }
}

EDirection Map::getNextDirection(unsigned int a_start, unsigned int a_end)
{
    std::string direction = getStringDirection(a_start, a_end);

    if(direction == "N")
    {
        return N;
    }
    if(direction == "E")
    {
        return E;
    }
    if(direction == "W")
    {
        return W;
    }
    if(direction == "S")
    {
        return S;
    }
    if(direction == "NW")
    {
        return NW;
    }
    if(direction == "NE")
    {
        return NE;
    }
    if(direction == "SW")
    {
        return SW;
    }
    if(direction == "SE")
    {
        return SE;
    }
    return NE;
}

std::string Map::getStringDirection(unsigned int start, unsigned int end)
{
    Node* nStart = m_nodeMap[start];
    Node* nEnd = m_nodeMap[end];
    int x = nEnd->getPosition()->x - nStart->getPosition()->x;
    int y = nEnd->getPosition()->y - nStart->getPosition()->y;

    std::string direction{""};

    // Set the North/South direction
    if(y < 0)
    {
        direction += "N";
    }
    else if(y > 0)
    {
        direction += "S";
    }

    // Depending on the row, we need to adapt the West.East direction
    if(nStart->getPosition()->y % 2 == 0)
    {
        if(x < 0)
        {
            direction += "W";
        }
        else
        {
            direction += "E";
        }
    }
    else
    {
        if(x > 0)
        {
            direction += "E";
        }
        else
        {
            direction += "W";
        }
    }

    return direction;
}

std::vector<unsigned int> Map::getNpcPath(unsigned int a_start, unsigned int a_end)
{
    SearchMap mySearch{getNode(a_start), getNode(a_end)};
    return mySearch.search();
}

bool Map::canMoveOnTile(unsigned int a_fromTileId, unsigned int a_toTileId)
{
    if(a_fromTileId == a_toTileId)
    {
        return true;
    }

    bool isStateOk = !(getNode(a_toTileId)->getType() == Node::FORBIDDEN || getNode(a_toTileId)->getType() == Node::OCCUPIED);
    EDirection dir = getNextDirection(a_fromTileId, a_toTileId);
    EDirection invDir = static_cast<EDirection>((dir + 4) % 8);
    return isStateOk && !getNode(a_fromTileId)->isEdgeBlocked(dir) && !getNode(a_toTileId)->isEdgeBlocked(invDir);
}

std::vector<unsigned int> Map::getNearUnVisitedTile(unsigned int a_currentId)
{
    Node* current = getNode(a_currentId);
    std::vector<unsigned int> v;

    for(int i = N; i <= NW; ++i)
    {
        if(current->getNeighboor(static_cast<EDirection>(i)))
        {
            testAddTile(v, current->getId(), current->getNeighboor(static_cast<EDirection>(i))->getId());
        }
    }

    return v;
}

void Map::testAddTile(std::vector<unsigned int>& v, unsigned int fromTileId, unsigned int toTileId)
{
    if(canMoveOnTile(fromTileId, toTileId) && !isVisited(toTileId))
    {
        v.push_back(toTileId);
    }
}




//----------------------------------------------------------------------
// LOGGER
//----------------------------------------------------------------------
void Map::logMap(unsigned nbTurn) 
{
#ifdef BOT_LOGIC_DEBUG_MAP
    std::string myLog = "\nTurn #" + std::to_string(nbTurn) + "\n";

    // Printing some infos
    myLog += "\tGoal Number : " + std::to_string(m_goalTiles.size()) + "\n";

    // Printing the map
    myLog += "Map : \n";
    unsigned int currentTileId{};
    for (int row = 0; row < m_height; ++row)
    {
        if (row % 2)
        {
            myLog += "   ";
        }
        for (int col = 0; col < m_width; ++col)
        {
            Node* tempNode = getNode(currentTileId++);
            switch (tempNode->getType())
            {
            case Node::NodeType::NONE:
                myLog += "-----";
                break;

            case Node::NodeType::FORBIDDEN:
                myLog += "F----";
                break;
            case Node::NodeType::GOAL:
                myLog += "G----";
                break;
            case Node::NodeType::OCCUPIED:
                myLog += "X----";
                break;
            case Node::NodeType::PATH:
                myLog += "P----";
                break;
            }
            myLog += "  ";
        }
        myLog += "\n";
    }
    BOT_LOGIC_MAP_LOG(m_logger, myLog, false);
#endif

}