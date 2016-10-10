#ifndef MAP_HEADER
#define MAP_HEADER

#include "Node.h"
#include "Globals.h"
#include <map>
#include <vector>
#include "Singleton.h"

class SearchMap;
class TileInfo;

class Map : Singleton
{
    static Map m_instance;
    unsigned int m_width;
    unsigned int m_height;
    std::map<unsigned int, Node*> m_nodeMap;
    std::vector<unsigned int> m_goalTiles;
    std::map<unsigned int, SearchMap*> m_searchMap;

private:
    Map() : m_width(0), m_height(0)
    {}
    float calculateDistance(int start, int end);
    std::string getStringDirection(unsigned int, unsigned int);
public:
    void setNodeType(unsigned int, Node::NodeType);
    void createNode(Node*);
    Node* getNode(unsigned int, unsigned int);
    Node* getNode(unsigned int);
    unsigned int getBestGoalTile(int start);
    EDirection getNextDirection(unsigned int npcId, unsigned int tileId);
    static Map *get() noexcept
    {
        return &m_instance;
    }

    unsigned int getWidth() const
    {
        return m_width;
    }
    void setWidth(unsigned int w)
    {
        m_width = w;
    }
    unsigned int getHeight() const
    {
        return m_height;
    }
    void setHeight(unsigned int h)
    {
        m_height = h;
    }

    void addGoalTile(unsigned int number)
    {
        m_goalTiles.push_back(number);
    }
    void addSearchMap(unsigned int index, SearchMap* smap)
    {
        m_searchMap[index] = smap;
    }
    SearchMap* getSearchMap(unsigned int index)
    {
        return m_searchMap[index];
    }

    // TODO - Return the npc path
    std::vector<unsigned int> getNpcPath(unsigned int);
};

#endif // MAP_HEADER
