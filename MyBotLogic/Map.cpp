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

void Map::connectNodes()
{
    for (std::pair<unsigned int, Node*> curNode : m_nodeMap)
    {
        // connecting
        Node* nw;
        Node* ne;
        Node* e;
        Node* se;
        Node* sw;
        Node* w;
        if (curNode.second->getPosition()->y % 2 == 0)
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
    if (x < 0 || x > getWidth() - 1 || y < 0 || y > getHeight() - 1)
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

std::vector<unsigned int> Map::getNearUnVisitedTile(unsigned int a_currentId)
{
    Node* current = getNode(a_currentId);
    std::vector<unsigned int> v;

    for (int i = N; i <= NW; ++i)
    {
        if (current->getNeighboor(static_cast<EDirection>(i)))
        {
            testAddTile(v, current->getNeighboor(static_cast<EDirection>(i))->getId());
        }
    }

    return v;
}

void Map::testAddTile(std::vector<unsigned int> &v, int x, int y)
{
    Node *temp = getNode(x, y);
    if(canMoveOnTile(x, y) && !isVisited(temp->getId()))
    {
        v.push_back(temp->getId());
    }
}

void Map::testAddTile(std::vector<unsigned int>& v, unsigned int tileId)
{
    if (canMoveOnTile(tileId) && !isVisited(tileId))
    {
        v.push_back(tileId);
    }
}