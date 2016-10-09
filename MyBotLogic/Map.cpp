#include "Map.h"
#include "TurnInfo.h"
#include "SearchMap.h"

void Map::setNodeType(unsigned int tileId, Node::NodeType tileType)
{
    m_nodeMap[tileId]->setType(tileType);
}

void Map::createNode(Node* node)
{
    std::pair<unsigned int, Node*> temp{node->getId(), node};
    m_nodeMap.insert(temp);
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

unsigned Map::getBestGoalTile(int start)
{
    float bestDistance = 999999.0f;
    int index = -1;
    for(int i = 0; i < m_goalTiles.size(); i++)
    {
        float distance = calculateDistance(start, m_goalTiles[i]);
        if(distance < bestDistance)
        {
            index = i;
            bestDistance = distance;
        }
    }
    int goalIndex = m_goalTiles[index];
    m_goalTiles.erase(m_goalTiles.begin() + index);
    return goalIndex;
}

EDirection Map::getNextDirection(unsigned npcId, unsigned int tileId)
{
    SearchMap* npcMap = m_searchMap[npcId];

    //int getNextTile = npcMap->getNextPathTileAndErase();
    int getNextTile = npcMap->getNextPathTile();

    std::string direction = getStringDirection(tileId, getNextTile);

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

std::string Map::getStringDirection(unsigned start, unsigned end)
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
