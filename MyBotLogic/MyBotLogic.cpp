#include "MyBotLogic.h"
#include <algorithm>
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
    Sleep(2000);
#endif
}

/*virtual*/ void MyBotLogic::Init(LevelInfo& _levelInfo)
{
    unsigned int rowCount = _levelInfo.rowCount;
    unsigned int colCount = _levelInfo.colCount;
    MyMap = Map(colCount, rowCount);
    unsigned int countIndex = 0;
    for(int i = 0; i < rowCount; ++i)
    {
        for(int j = 0; j < colCount; ++j)
        {
            // TODO - changer normal en NONE et mettre en place un type PATH
            MyMap.createNode(new Node{j, i, countIndex, Node::Normal});
            countIndex++;
        }
    }

    // Init npcs
    BOT_LOGIC_LOG(mLogger, "init npcs", true);
    for (std::pair<unsigned int, NPCInfo> curNpcs : _levelInfo.npcs)
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
    // Update graph
    for each(auto info in _turnInfo.tiles)
    {
        auto tileInfo = info.second;

        auto ITisForbidden = tileInfo.tileAttributes.find(TileAttribute_Forbidden);
        auto ITisTarget = tileInfo.tileAttributes.find(TileAttribute_Target);
        if(ITisForbidden != tileInfo.tileAttributes.end())
        {
            MyMap.setNodeType(tileInfo.tileID, Node::Forbidden);
        }
        else if(ITisTarget != tileInfo.tileAttributes.end())
        {
            MyMap.setNodeType(tileInfo.tileID, Node::Goal);
            MyMap.addGoalTile(tileInfo.tileID);
        }
    }

    // Initialize map and goal for npcs
    for each(auto info in _turnInfo.npcs)
    {
        // TODO - check between all npcs to find wich one is the best to go on the goal tile
        if (MyMap.getSearchMap(info.second.npcID) == nullptr)
        {
            int goalTile = MyMap.getBestGoalTile(info.second.tileID);
            auto npcSMap = new SearchMap(&MyMap, MyMap.getNode(info.second.tileID), MyMap.getNode(goalTile));
            MyMap.addSearchMap(info.second.npcID, npcSMap);
            npcSMap->search();
        }
        SearchMap* npcMap = MyMap.getSearchMap(info.second.npcID);
        int nextTile = npcMap->getNextPathTile();
        if (nextTile > 0)
        {
            m_nextNpcMove[info.second.npcID] = nextTile;
        }
    }

    // Move Npcs
    for each(auto info in _turnInfo.npcs)
    {
        // Get my search map
        SearchMap* npcMap = MyMap.getSearchMap(info.second.npcID);

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
                if (curP.first != info.second.npcID 
                    && curP.second == nextNpcTile)
                {
                    // Handle 
                    if (MyMap.getSearchMap(curP.first)->pathSize() > npcMap->pathSize())
                    {
                        canMoveOnThisTile = false;
                        break;
                    }
                    // else prioritize by npcs id
                    if (curP.first < info.second.npcID
                        && MyMap.getSearchMap(curP.first)->pathSize() == npcMap->pathSize())
                    {
                        canMoveOnThisTile = false;
                        break;
                    }
                }
            }

            if(canMoveOnThisTile)
            {
                // Add the move in provate array to block other npc to go on this case
                //m_nextNpcMove.push_back(nextNpcTile);

                // Add the action in the list
                _actionList.push_back(new Move(info.second.npcID, MyMap.getNextDirection(info.second.npcID, info.second.tileID)));

                // Check the next npc move
                if(npcMap->getNextPathTile() < 0)
                {
                    // If it's -1, this NPC finished his path
                    MyMap.setNodeType(nextNpcTile, Node::Forbidden);
                }
            }
        }
    }
    //m_nextNpcMove.clear();
    for (Action* pAction : _actionList)
    {
        auto npcMap = MyMap.getSearchMap(pAction->unitID);
        npcMap->popLastTile();
    }
}

/*virtual*/ void MyBotLogic::Exit()
{
    //Write Code Here
}

//
//void MyBotLogic::FillActionList(TurnInfo& _turnInfo, std::vector<Action*>& _actionList)
//{
//    // Update map
//    for each(auto info in _turnInfo.tiles)
//    {
//        auto tileInfo = info.second;
//        auto ITisForbidden = std::find_if(tileInfo.tileAttributes.begin(), tileInfo.tileAttributes.end(), [](ETileAttribute test) {
//            return test == TileAttribute_Forbidden;
//        });
//        auto ITisTarget = std::find(tileInfo.tileAttributes.begin(), tileInfo.tileAttributes.end(), TileAttribute_Target);
//        if (ITisForbidden != tileInfo.tileAttributes.end())
//        {
//            MyMap.setNodeType(tileInfo.tileID, Node::Forbidden);
//        }
//        else if (ITisTarget != tileInfo.tileAttributes.end())
//        {
//            MyMap.setNodeType(tileInfo.tileID, Node::Goal);
//            MyMap.addGoalTile(tileInfo.tileID);
//        }
//    }
//    // Find the path
//    for each(auto info in _turnInfo.npcs)
//    {
//        // TODO - check between all npcs to find wich one is the best to go on the goal tile
//        if (MyMap.getSearchMap(info.second.npcID) == nullptr)
//        {
//            int goalTile = MyMap.getBestGoalTile(info.second.tileID);
//            auto npcSMap = new SearchMap(&MyMap, MyMap.getNode(info.second.tileID), MyMap.getNode(goalTile));
//            MyMap.addSearchMap(info.second.npcID, npcSMap);
//            npcSMap->search();
//        }
//        SearchMap* npcMap = MyMap.getSearchMap(info.second.npcID);
//        int nextTile = npcMap->getNextPathTile();
//        if (nextTile > 0)
//        {
//            m_nextNpcMove[info.second.npcID] = nextTile;
//        }
//    }
//    // Move the NPC
//    for each(auto info in _turnInfo.npcs)
//    {
//        // Get my search map
//        SearchMap* npcMap = MyMap.getSearchMap(info.second.npcID);
//        // If my npc path is not correct anymore, we try to find another path
//        if (!npcMap->checkPathIntegrity())
//        {
//            npcMap->FindAnotherPath();
//        }
//        // Check if we can move on expected tile
//        bool canMoveOnThisTile = true;
//        for (std::pair<unsigned int, unsigned int> p : m_nextNpcMove)
//        {
//            if (p.second == m_nextNpcMove.at(info.second.npcID) && p.first != info.second.npcID)
//            {
//                SearchMap* secondNpcMap = MyMap.getSearchMap(p.first);
//                if (secondNpcMap->getPathSize() > npcMap->getPathSize())
//                {
//                    // Don't move on the same tile
//                    canMoveOnThisTile = false;
//                }
//            }
//        }
//        if (canMoveOnThisTile)
//        {
//            // Add the action in the list
//            _actionList.push_back(new Move(info.second.npcID, MyMap.getNextDirection(info.second.npcID, info.second.tileID)));
//            // Check the next npc move
//            if (npcMap->getNextPathTile() < 0)
//            {
//                // If it's -1, this NPC finished his path
//                MyMap.setNodeType(m_nextNpcMove[info.second.npcID], Node::Forbidden);
//            }
//        }
//    }
//    m_nextNpcMove.clear();
//}