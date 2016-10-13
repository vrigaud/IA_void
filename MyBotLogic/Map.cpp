#include "Map.h"
#include "TurnInfo.h"
#include "SearchMap.h"
#include "NPCInfo.h"

Map Map::m_instance;

void Map::setNodeType(unsigned int tileId, Node::NodeType tileType)
{
    m_nodeMap[tileId]->setType(tileType);
}

void Map::createNode(Node* node)
{
    m_nodeMap[node->getId()] = node;
}

Node* Map::getNode(unsigned int x, unsigned int y)
{
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
    for(unsigned goal : m_goalTiles)
    {
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

    return goalMap;
}


void Map::addGoalTile(unsigned int number)
{
    // TODO - Ajout protection car ca pue un peu !!
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

bool Map::canMoveOnTile(unsigned int a_tileId)
{
    return !(getNode(a_tileId)->getType() == Node::FORBIDDEN || getNode(a_tileId)->getType() == Node::OCCUPIED || getNode(a_tileId)->getType() == Node::NONE);
}
bool Map::canMoveOnTile(unsigned int a_x, unsigned int a_y)
{
    if(a_x < 0 || a_x > m_width - 1 || a_y < 0 || a_y > m_height - 1)
    {
        return false;
    }
    return !(getNode(a_x, a_y)->getType() == Node::FORBIDDEN || getNode(a_x, a_y)->getType() == Node::OCCUPIED || getNode(a_x, a_y)->getType() == Node::NONE);
}

std::vector<unsigned int> Map::getNearInterestingTile(unsigned int a_currentId)
{
    Node* current = getNode(a_currentId);
    std::vector<unsigned int> v;

    if(current->getPosition()->y % 2 == 0)
    {
        // UP Left
        testAddTile(v, current->getPosition()->x - 1, current->getPosition()->y - 1);
        // UP Right
        testAddTile(v, current->getPosition()->x, current->getPosition()->y - 1);
        // MIDDLE Right
        testAddTile(v, current->getPosition()->x + 1, current->getPosition()->y);
        // BOTTOM Right
        testAddTile(v, current->getPosition()->x, current->getPosition()->y + 1);
        // BOTTOM Left
        testAddTile(v, current->getPosition()->x - 1, current->getPosition()->y + 1);
        // MIDDLE left
        testAddTile(v, current->getPosition()->x - 1, current->getPosition()->y);
    }
    else
    {
        // UP Left
        testAddTile(v, current->getPosition()->x, current->getPosition()->y - 1);
        // UP Right
        testAddTile(v, current->getPosition()->x + 1, current->getPosition()->y - 1);
        // MIDDLE Right
        testAddTile(v, current->getPosition()->x + 1, current->getPosition()->y);
        // BOTTOM Right
        testAddTile(v, current->getPosition()->x + 1, current->getPosition()->y + 1);
        // BOTTOM Left
        testAddTile(v, current->getPosition()->x, current->getPosition()->y + 1);
        // MIDDLE left
        testAddTile(v, current->getPosition()->x - 1, current->getPosition()->y);
    }

    return v;
}

void Map::testAddTile(std::vector<unsigned int> &v, int x, int y)
{
    Node *temp = getNode(x, y);
    if(canMoveOnTile(x, y))
    {
        v.push_back(temp->getId());
    }
}