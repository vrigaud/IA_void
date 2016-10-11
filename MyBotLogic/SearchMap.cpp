#include "SearchMap.h"
#include "Map.h"
#include "SearchNode.h"

void SearchMap::prepareNode(int x, int y, unsigned int newGValue, SearchNode* parent)
{
    Map* myMap = Map::get();
    if(x < 0 || x > myMap->getWidth() - 1 || y < 0 || y > myMap->getHeight() - 1)
    {
        return;
    }
    auto nodeType = myMap->getNode(x, y)->getType();
    if(nodeType == Node::FORBIDDEN || nodeType == Node::NONE)
    {
        return;
    }

    int id = myMap->getWidth() * y + x;

    for(int i = 0; i < closedList.size(); i++)
    {
        if(id == closedList[i]->getId())
        {
            return;
        }
    }

    SearchNode* node = new SearchNode(x, y, id, parent);
    node->setG(newGValue);
    node->setH(calculateManathan(node, m_goal));

    for(int i = 0; i < openList.size(); i++)
    {
        if(id == openList[i]->getId())
        {
            if(node->getF() < openList[i]->getF())
            {
                openList[i]->setG(newGValue);
                openList[i]->setParent(parent);
            }
            else
            {
                delete node;
                return;
            }
        }
    }

    openList.push_back(node);
}

std::vector<unsigned int> SearchMap::search()
{
    while (!m_isGoalFound)
    {
        if (openList.empty())
        {
            return std::vector<unsigned int>{};
        }

        SearchNode* current = getNextNodeToSearch();

        if (current->getId() == m_goal->getId())
        {
            SearchNode* getPath;
            for (getPath = current; getPath != nullptr; getPath = getPath->getParent())
            {
                m_pathToGoal.push_back(getPath->getId());
            }
            m_isGoalFound = true;
            return m_pathToGoal;
        }
        if (current->getY() % 2 == 0)
        {
            prepareNode(current->getX() - 1, current->getY() - 1, current->getG() + 10, current);
            prepareNode(current->getX(), current->getY() - 1, current->getG() + 10, current);

            prepareNode(current->getX() - 1, current->getY(), current->getG() + 10, current);
            prepareNode(current->getX() + 1, current->getY(), current->getG() + 10, current);

            prepareNode(current->getX() - 1, current->getY() + 1, current->getG() + 10, current);
            prepareNode(current->getX(), current->getY() + 1, current->getG() + 10, current);
        }
        else
        {
            prepareNode(current->getX(), current->getY() - 1, current->getG() + 10, current);
            prepareNode(current->getX() + 1, current->getY() - 1, current->getG() + 10, current);

            prepareNode(current->getX() - 1, current->getY(), current->getG() + 10, current);
            prepareNode(current->getX() + 1, current->getY(), current->getG() + 10, current);

            prepareNode(current->getX(), current->getY() + 1, current->getG() + 10, current);
            prepareNode(current->getX() + 1, current->getY() + 1, current->getG() + 10, current);
        }

        for (int i = 0; i < openList.size(); i++)
        {
            if (current->getId() == openList[i]->getId())
            {
                openList.erase(openList.begin() + i);
            }
        }
    }
}

SearchMap::SearchMap(Node* start, Node* goal)
{
    initSearchMap(start, goal);
}

void SearchMap::initSearchMap(Node* start, Node* goal)
{
    if(!m_isInitialized)
    {
        for(int i = 0; i < openList.size(); i++)
        {
            delete openList[i];
        }
        openList.clear();

        for(int i = 0; i < closedList.size(); i++)
        {
            delete closedList[i];
        }
        closedList.clear();

        m_pathToGoal.clear();

        m_isPathFinished = false;
        m_isGoalFound = false;

        setStartNode(new SearchNode(start->getPosition()->x, start->getPosition()->y, start->getId()));
        setGoalNode(new SearchNode(goal->getPosition()->x, goal->getPosition()->y, goal->getId()));

        m_start->setH(calculateManathan(m_start, m_goal));

        openList.push_back(m_start);
        m_isInitialized = true;
    }
}

SearchNode* SearchMap::getNextNodeToSearch()
{
    unsigned int bestF = 99999;
    int index = -1;
    SearchNode* nextNode = nullptr;

    for(int i = 0; i < openList.size(); i++)
    {
        if(openList[i]->getF() < bestF)
        {
            index = i;
            bestF = openList[i]->getF();
        }
    }

    if(index >= 0)
    {
        nextNode = openList[index];
        closedList.push_back(nextNode);
        openList.erase(openList.begin() + index);
    }

    return nextNode;
}

unsigned int SearchMap::calculateManathan(const SearchNode* start, const SearchNode* goal) const
{
    int x = goal->getX() - start->getX();
    int y = goal->getY() - start->getY();
    return (abs(x) + abs(y)) * 10;
}

int SearchMap::getNextPathTile()
{
    if(m_pathToGoal.size() == 1)
    {
        return -1;
    }
    unsigned int index = m_pathToGoal[m_pathToGoal.size() - 2];
    return index;
}

void SearchMap::FindAnotherPath()
{
    m_isInitialized = false;
    initSearchMap(Map::get()->getNode(m_pathToGoal.back()), Map::get()->getNode(m_goal->getId()));
    search();
}

bool SearchMap::checkPathIntegrity()
{
    if(m_isGoalFound)
    {
        for(int i = 0; i < m_pathToGoal.size(); i++)
        {
            if(Map::get()->getNode(m_pathToGoal[i])->getType() == Node::FORBIDDEN)
            {
                return false;
            }
        }
    }
    return true;
}
