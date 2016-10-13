#ifndef NODE_HEADER
#define NODE_HEADER
#include <cmath>
#include "Globals.h"

struct Position
{
    int x;
    int y;
    Position(int xVal, int yVal)
        :x{xVal}, y{yVal}
    {}
};

class Node
{
public:
    enum NodeType
    {
        NONE,
        FORBIDDEN,
        GOAL,
        OCCUPIED,
        PATH,
    };
private:
    Position* m_pos;
    unsigned int m_ID;
    NodeType m_type;
    unsigned int m_edgesCost[8] = {0};
public:
    Node() = delete;
    Node(int xVal, int yVal, unsigned int idVal, NodeType typeVal);

    NodeType getType() const noexcept
    {
        return m_type;
    }

    Position* getPosition() const noexcept
    {
        return m_pos;
    }

    unsigned int getId() const noexcept
    {
        return m_ID;
    }

    void setType(NodeType nType)
    {
        m_type = nType;
    }

    void setEdgeCost(EDirection dir, int value)
    {
        m_edgesCost[dir] = value;
    }

    unsigned int calculateManathan(const Node* goal) const
    {
        int x = goal->getPosition()->x - this->m_pos->x;
        int y = goal->getPosition()->y - this->m_pos->y;
        return (abs(x) + abs(y)) * 10;
    }
};

#endif // NODE_HEADER
