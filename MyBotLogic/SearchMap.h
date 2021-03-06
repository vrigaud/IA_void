#ifndef SEARCHMAP_HEADER
#define SEARCHMAP_HEADER
#include "Node.h"
#include <vector>

class Map;
class SearchNode;

class SearchMap
{
    SearchNode* m_start;
    SearchNode* m_goal;
    std::vector<SearchNode*> openList;
    std::vector<SearchNode*> closedList;
    std::vector<unsigned int> m_pathToGoal;
    Map* m_map;
    bool m_isGoalFound = false;
    bool m_isInitialized = false;
    bool m_isPathFinished = false;
public:
    SearchMap() = delete;
    SearchMap(Map* m) : m_start(nullptr), m_goal(nullptr), m_map(m)
    {}
    SearchMap(Map* m, SearchNode* start, SearchNode* goal) : m_start(start), m_goal(goal), m_map(m)
    {}
    SearchMap(Map* m, Node* start, Node* goal);

    bool isFinished() const
    {
        return m_isGoalFound;
    }
    void initSearchMap(Node*, Node*);
    void prepareNode(int, int, unsigned int, SearchNode*);
    void search();
    SearchNode* getNextNodeToSearch();
    unsigned int calculateManathan(const SearchNode* start, const SearchNode* goal) const;
    void setStartNode(SearchNode* n)
    {
        m_start = n;
    }
    void setGoalNode(SearchNode* n)
    {
        m_goal = n;
    }
    int getNextPathTile();
    int getNextPathTileAndErase();
    void FindAnotherPath();
    bool checkPathIntegrity();
    bool isPathFinished() const
    {
        return m_isPathFinished;
    }
    unsigned int pathSize()
    {
        return m_pathToGoal.size();
    };
    void popLastTile() 
    {
        m_pathToGoal.pop_back();
    };
};

#endif // SEARCHMAP_HEADER
