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
    unsigned int m_edgesCost[8] = {1, 1, 1, 1, 1, 1, 1, 1};
    Node* m_neighboors[8] = {nullptr};
    unsigned int m_npcId = {0};
    // TODO - Ajouter une zone a nos nodes pour les differencier et permettre de tout de suite savoir si on peut acceder a ce node ou pas

    // TODO - Faire en sorte de definir si on sait tout du node ou pas pour eviter d'aller dessus, pour optimiser la recherche de chemin

    // TODO - si jamais le node est entouré d'obstacles, faire en sorte de l'ignorer pour la recherche de chemin
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

    bool isEdgeBlocked(EDirection dir) const
    {
        return m_edgesCost[dir] == 0 ? true : false;
    }

    void setNeighboor(EDirection dir, Node* p)
    {
        m_neighboors[dir] = p;
    }

    Node* getNeighboor(EDirection dir)
    {
        return m_neighboors[dir];
    }

    void setNpcIdOnNode(unsigned npcId)
    {
        m_npcId = npcId;
    }

    unsigned getNpcIdOnNode() const
    {
        return m_npcId;
    }

    unsigned int calculateManathan(const Node* goal) const
    {
        int x = goal->getPosition()->x - this->m_pos->x;
        int y = goal->getPosition()->y - this->m_pos->y;
        return (abs(x) + abs(y)) * 10;
    }
};

#endif // NODE_HEADER
