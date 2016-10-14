#include "MyBotLogic.h"
#include "TurnInfo.h"
#include "LevelInfo.h"
#include "SearchMap.h"
#include <windows.h>
#include <algorithm>
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

    myMap->connectNodes();

    // Init npcs
    BOT_LOGIC_LOG(mLogger, "init npcs", true);
    for(std::pair<unsigned int, NPCInfo> curNpcs : _levelInfo.npcs)
    {
        m_npcs[curNpcs.second.npcID] = new Npc(curNpcs.second.npcID, curNpcs.second.tileID, m_logPath);
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
            myMap->addSeenTile(tileInfo.tileID);
        }
        else if(ITisDescriptor != tileInfo.tileAttributes.end())
        {
            myMap->setNodeType(tileInfo.tileID, Node::PATH);
            myMap->addSeenTile(tileInfo.tileID);
        }
    }
    BOT_LOGIC_LOG(mLogger, "Update Edges", true);
    for(std::pair<unsigned, ObjectInfo> info : _turnInfo.objects)
    {
        Node* node = myMap->getNode(info.second.tileID);
        for(int i = N; i <= NW; ++i)
        {
            if(info.second.edgesCost[i] == 0)
            {
                BOT_LOGIC_LOG(mLogger, "\tTileID : " + std::to_string(info.second.tileID) + " - Dir : " + std::to_string(i), true);
                node->setEdgeCost(static_cast<EDirection>(i), 0);
            }
        }
    }

    std::map<unsigned, unsigned> goalMap = myMap->getBestGoalTile(_turnInfo.npcs);
    // Calcul path for npc and set goal tile
    for(std::pair<unsigned int, NPCInfo> curNpc : _turnInfo.npcs)
    {
        Npc* myNpc = m_npcs[curNpc.first];
        myMap->visitTile(myNpc->getCurrentTileId());
        if(!myNpc->hasGoal() && goalMap.find(curNpc.second.npcID) != end(goalMap))
        {
            unsigned int goalTile = goalMap[curNpc.second.npcID];
            myNpc->setGoal(goalTile);
        }
        myNpc->calculPath();
    }

    // Move Npcs
    for each(auto info in _turnInfo.npcs)
    {
        // Get my search map
        Npc* myNpc = m_npcs[info.first];

        // If my npc path is not correct anymore, we try to find another path
        myNpc->update();

        // Get next npc tile
        int nextNpcTile = myNpc->getNextPathTile();

        if(nextNpcTile > 0)
        {
            // check if npc can move on nextTile
            for(std::pair<unsigned int, Npc*> curP : m_npcs)
            {

                if(curP.first != myNpc->getId()
                   && curP.second->getNextPathTile() == nextNpcTile)
                {
                    // Handle
                    if(myNpc->getPathSize() < curP.second->getPathSize())
                    {
                        myNpc->stopMoving();
                        break;
                    }
                    // else prioritize by npcs id
                    if(curP.first < myNpc->getId()
                       && myNpc->getPathSize() == curP.second->getPathSize())
                    {
                        myNpc->stopMoving();
                        break;
                    }
                }
            }
            // copy npc's action list into the action list
            for(Action* curAction : myNpc->getActions())
            {
                _actionList.push_back(curAction->Clone());
            }

            if(myNpc->getNextPathTile() < 0)
            {
                // If it's -1, this NPC finished his path
                myMap->setNodeType(nextNpcTile, Node::OCCUPIED);
            }
        }
    }
    std::for_each(begin(m_npcs),
                  end(m_npcs),
                  [](std::pair<unsigned int, Npc*> myNpc) {myNpc.second->unstackActions(); });
}

/*virtual*/ void MyBotLogic::Exit()
{
    //Write Code Here
}