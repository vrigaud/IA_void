#include "MyBotLogic.h"
#include "TurnInfo.h"
#include "LevelInfo.h"
#include "SearchMap.h"
#include <windows.h>
#include "Map.h"

MyBotLogic::MyBotLogic()
{
    //Write Code Here
}

/*virtual*/ MyBotLogic::~MyBotLogic()
{
    //Write Code Here
}

/*virtual*/ void MyBotLogic::Configure(int argc, char *argv[], const std::string& _logpath)
{
    m_logPath = _logpath;
#ifdef BOT_LOGIC_DEBUG
    mLogger.Init(_logpath, "MyBotLogic.log");
#endif

    BOT_LOGIC_LOG(mLogger, "Configure", true);


    //Write Code Here
}

/*virtual*/ void MyBotLogic::Load()
{
    //Write Code Here
#ifdef BOT_LOGIC_DEBUG
    Sleep(5000);
#endif
}

/*virtual*/ void MyBotLogic::Init(LevelInfo& _levelInfo)
{
    int rowCount = _levelInfo.rowCount;
    int colCount = _levelInfo.colCount;
    Map *myMap = Map::get();
    myMap->setHeight(rowCount);
    myMap->setWidth(colCount);
    unsigned int countIndex = 0;
    for(int i = 0; i < rowCount; ++i)
    {
        for(int j = 0; j < colCount; ++j)
        {
            myMap->createNode(new Node{j, i, countIndex, Node::NONE});
            countIndex++;
        }
    }

    // Init npcs
    BOT_LOGIC_LOG(mLogger, "init npcs", true);
    for(std::pair<unsigned int, NPCInfo> curNpcs : _levelInfo.npcs)
    {
        m_npcs[curNpcs.second.npcID] = new Npc(curNpcs.second.npcID, m_logPath);
    }
}

/*virtual*/ void MyBotLogic::OnBotInitialized()
{
    //Write Code Here
}

/*virtual*/ void MyBotLogic::OnGameStarted()
{
    //Write Code Here
}

/*virtual*/ void MyBotLogic::FillActionList(TurnInfo& _turnInfo, std::vector<Action*>& _actionList)
{
    Map *myMap = Map::get();
    // Update graph
    for each(auto info in _turnInfo.tiles)
    {
        auto tileInfo = info.second;

        auto ITisForbidden = find(begin(tileInfo.tileAttributes), end(tileInfo.tileAttributes), TileAttribute_Forbidden);
        auto ITisTarget = find(begin(tileInfo.tileAttributes), end(tileInfo.tileAttributes), TileAttribute_Target);
        auto ITisDescriptor = find(begin(tileInfo.tileAttributes), end(tileInfo.tileAttributes), TileAttribute_Descriptor);
        if(ITisForbidden != tileInfo.tileAttributes.end())
        {
            myMap->setNodeType(tileInfo.tileID, Node::FORBIDDEN);
        }
        else if(ITisTarget != tileInfo.tileAttributes.end())
        {
            myMap->setNodeType(tileInfo.tileID, Node::GOAL);
            myMap->addGoalTile(tileInfo.tileID);
        }
        else if (ITisDescriptor != tileInfo.tileAttributes.end())
        {
            myMap->setNodeType(tileInfo.tileID, Node::PATH);
        }
    }

    // Initialize map and goal for npcs
    for each(auto info in _turnInfo.npcs)
    {
        if(myMap->getSearchMap(info.second.npcID) == nullptr)
        {
            // TODO - check between all npcs to find wich one is the best to go on the goal tile
            int goalTile = myMap->getBestGoalTile(info.second.tileID);
            auto npcSMap = new SearchMap(myMap->getNode(info.second.tileID), myMap->getNode(goalTile));
            myMap->addSearchMap(info.second.npcID, npcSMap);
            npcSMap->search();
        }

        SearchMap* npcMap = myMap->getSearchMap(info.second.npcID);
        int nextTile = npcMap->getNextPathTile();
        if(nextTile > 0)
        {
            m_nextNpcMove[info.second.npcID] = nextTile;
        }
    }

    // Move Npcs
    for each(auto info in _turnInfo.npcs)
    {
        // Get my search map
        SearchMap* npcMap = myMap->getSearchMap(info.second.npcID);

        // If my npc path is not correct anymore, we try to find another path
        if(!npcMap->checkPathIntegrity())
        {
            npcMap->FindAnotherPath();
        }

        // Get next npc tile
        int nextNpcTile = npcMap->getNextPathTile();

        if(nextNpcTile > 0)
        {
            // Check if we can move on expected tile handle letting bro pass first
            bool canMoveOnThisTile = true;
            for(std::pair<unsigned int, unsigned int> curP : m_nextNpcMove)
            {
                if(curP.first != info.second.npcID
                   && curP.second == nextNpcTile)
                {
                    // Handle 
                    if(myMap->getSearchMap(curP.first)->pathSize() > npcMap->pathSize())
                    {
                        canMoveOnThisTile = false;
                        break;
                    }
                    // else prioritize by npcs id
                    if(curP.first < info.second.npcID
                       && myMap->getSearchMap(curP.first)->pathSize() == npcMap->pathSize())
                    {
                        canMoveOnThisTile = false;
                        break;
                    }
                }
            }

            if(canMoveOnThisTile)
            {
                // Add the action in the list
                _actionList.push_back(new Move(info.second.npcID, myMap->getNextDirection(info.second.npcID, info.second.tileID)));

                // Check the next npc move
                if(npcMap->getNextPathTile() < 0)
                {
                    // If it's -1, this NPC finished his path
                    myMap->setNodeType(nextNpcTile, Node::FORBIDDEN);
                }
            }
        }
    }
    //m_nextNpcMove.clear();
    for(Action* pAction : _actionList)
    {
        auto npcMap = myMap->getSearchMap(pAction->unitID);
        npcMap->popLastTile();
    }
}

/*virtual*/ void MyBotLogic::Exit()
{
    //Write Code Here
}