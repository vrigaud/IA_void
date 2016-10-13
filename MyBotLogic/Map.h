#ifndef MAP_HEADER
#define MAP_HEADER

#include "Node.h"
#include "Globals.h"
#include <map>
#include <vector>
#include "Singleton.h"
#include "NPCInfo.h"

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
    void testAddTile(std::vector<unsigned>& v, int x, int y);
public:
    void setNodeType(unsigned int, Node::NodeType);
    void createNode(Node*);
    Node* getNode(unsigned int, unsigned int);
    Node* getNode(unsigned int);
    std::map<unsigned, unsigned> getBestGoalTile(std::map<unsigned, NPCInfo> npcInfo);
    EDirection getNextDirection(unsigned int a_start, unsigned int a_end);
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

    void addGoalTile(unsigned int number);
    void addSearchMap(unsigned int index, SearchMap* smap)
    {
        m_searchMap[index] = smap;
    }
    SearchMap* getSearchMap(unsigned int index)
    {
        return m_searchMap[index];
    }

    std::vector<unsigned int> getNpcPath(unsigned int a_start, unsigned int a_end);

    bool canMoveOnTile(unsigned int a_tileId);
    bool canMoveOnTile(unsigned int a_x, unsigned int a_y);

    // TODO - Return all interesting node near one tile as vector
    /*
     * Do not add forbidden tile or blocked tile
     * Do not add blocked tile (by other npc)
     * And do not add impass tile (5 wall)
     */
    std::vector<unsigned int> getNearInterestingTile(unsigned int a_currentId);
};

#endif // MAP_HEADER
