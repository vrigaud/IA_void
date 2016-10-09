#ifndef MAP_HEADER
#define MAP_HEADER

#include "Node.h"
#include "Globals.h"
#include <map>
#include <vector>

class SearchMap;
class TileInfo;

class Map
{
    unsigned int m_width;
    unsigned int m_height;
    std::map<unsigned int, Node*> m_nodeMap;
    std::vector<unsigned int> m_goalTiles;
    std::map<unsigned int, SearchMap*> m_searchMap;
public:
    Map() : m_width(0), m_height(0)
    {}
    Map(unsigned int w, unsigned int h) : m_width(w), m_height(h)
    {}

    void setNodeType(unsigned int, Node::NodeType);
    void createNode(Node*);
    Node* getNode(unsigned int, unsigned int);
    Node* getNode(unsigned int);
    unsigned int getWidth() const
    {
        return m_width;
    }
    unsigned int getHeight() const
    {
        return m_height;
    }
    void addGoalTile(unsigned int number)
    {
        m_goalTiles.push_back(number);
    }
    unsigned int calculateDistance(Node* start, Node* end) const
    {
        return (fabs(start->getPosition()->x - end->getPosition()->x) + fabs(start->getPosition()->y - end->getPosition()->y)) * 10;
    }
    float calculateDistance(int start, int end);
    unsigned int getBestGoalTile(int start);
    void addSearchMap(unsigned int index, SearchMap* smap)
    {
        m_searchMap[index] = smap;
    }
    SearchMap* getSearchMap(unsigned int index)
    {
        return m_searchMap[index];
    }
    EDirection getNextDirection(unsigned int npcId, unsigned int tileId);
    std::string getStringDirection(unsigned int, unsigned int);
};

#endif // MAP_HEADER
