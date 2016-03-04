#include "CrushGameScene.h"
#include "CrushGrid.h"
#include "CrushNode.h"
#include "ui/CocosGUI.h"
#include "MainScene.h"
#include "SimpleAudioEngine.h"
#include "cocostudio/CocoStudio.h"
//#include "GameOverScene.h"
using namespace CocosDenshion;

Scene* CrushGameScene::createScene()
{
	auto scene = Scene::create();
	auto layer = CrushGameScene::create();
	scene->addChild(layer);
	return scene;
}

CrushGameScene::CrushGameScene()
	:m_pLabelCurrScore(NULL),
	m_pLabelTargetScore(NULL)
{
	m_fCrushTime = 60.0f; 
	m_pLabelTime = NULL;
}

LoadingBar* CrushGameScene::m_bonusbar = nullptr;
int CrushGameScene::m_score = 0;
Label* CrushGameScene::m_scorelabel = nullptr;

bool CrushGameScene::init()
{
	Layer::init();
	initStudioUI();
	SimpleAudioEngine::getInstance()->playBackgroundMusic("GameSceneBGM.mp3",true);
	auto t_paine = static_cast<Layout*>(m_pGameUI->getChildByName("gamePanel"));
	m_pLabelTime = static_cast<Text*>(m_pGameUI->getChildByName("pTextTime"));
	m_pLabelCurrScore = static_cast<Text*>(m_pGameUI->getChildByName("pTextCurrScore"));
	m_pLabelTargetScore = static_cast<Text*>(m_pGameUI->getChildByName("pTextTarScore"));
	auto t_btn = static_cast<Button*>(m_pGameUI->getChildByName("pBtn_Return"));
	//t_btn->addClickEventListener(CC_CALLBACK_1(CrushGameScene::exitCrushGame,this));
	t_btn->addTouchEventListener([](Ref* obj,Widget::TouchEventType et){
		if(et==Widget::TouchEventType::ENDED)
		{  
			log("Crush Game Click");
			auto t_CrushScene = MainScene::createScene();
			t_CrushScene = TransitionFlipX::create(1,t_CrushScene);
			SimpleAudioEngine::getInstance()->stopBackgroundMusic(true);
			Director::getInstance()->replaceScene(t_CrushScene);Director::getInstance()->replaceScene(t_CrushScene);

		}
	});

	auto visibleSize = t_paine->getContentSize();/*Director::getInstance()->getVisibleSize();*/

	auto texturecache = TextureCache::getInstance();

 	
	//初始化网格数据，网格的原点在左下角
	m_jewelsgrid = CrushGrid::create(CRUSH_ROW_NUM,CRUSH_COL_NUM);
	t_paine->addChild(m_jewelsgrid);
	m_jewelsgrid->setPosition(0, visibleSize.height - m_jewelsgrid->getRow() * GRID_WIDTH);
	
	schedule(schedule_selector(CrushGameScene::onReducingBonus), 1);
	scheduleUpdate();
	

	
	//测试用，刷新宝石阵列按钮
	auto updateMenu = MenuItemFont::create("Update Map", CC_CALLBACK_1(CrushGameScene::onUpdateMenuCallback, this));
	updateMenu->setAnchorPoint(Vec2(1, 0));
	updateMenu->setPosition(visibleSize.width / 2, visibleSize.height / 2);
	auto menu = Menu::create(updateMenu, nullptr);
	t_paine->addChild(menu);
	
	//关闭测试
	menu->setEnabled(false);
	menu->setVisible(false);

	return true;
}
void CrushGameScene::initStudioUI()
{
	m_pGameUI = CSLoader::createNode("CrushGameUI.csb");
	addChild(m_pGameUI);
}

void CrushGameScene::publishScore()
{
	auto userdefault = UserDefault::getInstance();

	//查看路径，测试用
	log(userdefault->getXMLFilePath().c_str()); 

	//存储本次游戏分数
	char score_str[100] = {0};
	sprintf(score_str, "%d", m_score);
	userdefault->setStringForKey("LastScore", score_str);

	//存储最佳游戏分数
	auto bestscore = userdefault->getStringForKey("BestScore");
	if (m_score > atoi(bestscore.c_str()))
		userdefault->setStringForKey("BestScore", score_str);
}
void CrushGameScene::update(float delta)
{
	if (m_pLabelCurrScore)
	{
		int t_score = Game::Instance()->getCurrentScoreByGameType(GAME_TYPE_CRUSH);
		char szName[100] = {0}; 
		sprintf(szName, "%d", t_score); 
		m_pLabelCurrScore->setText(szName);

		t_score = Game::Instance()->getTarScoreByGameType(GAME_TYPE_CRUSH);
		sprintf(szName, "%d", t_score); 
		m_pLabelTargetScore->setText(szName);
	}
	
}
void CrushGameScene::onReducingBonus(float dt)
{
	m_fCrushTime -= dt;
	String* t_str = String::createWithFormat("%d:%d",(int)m_fCrushTime / 60,(int)m_fCrushTime % 60);
	m_pLabelTime->setText(t_str->getCString());
	//倒计时结束，游戏结束，保存游戏分数
	if (m_fCrushTime <= 0.0f)
	{
		unschedule(schedule_selector(CrushGameScene::onReducingBonus));

		bool t_result = Game::Instance()->getGameResult(GAME_TYPE_CRUSH);
		if (t_result)
		{
			MessageBox("YOU  WIN","GameOver");
		}
		{
			MessageBox("LOSE","GameOver");
		}
		m_jewelsgrid->setGameOver();
		unscheduleUpdate();
		log("game over!");
		publishScore();
// 		auto scene = GameOverScene::createScene();
// 		Director::getInstance()->replaceScene(TransitionFade::create(1.0, scene));
	}
}

void CrushGameScene::addBonus(int bonus)
{
	m_score += 20 * bonus;
	char buf[100] = {0};
	sprintf(buf, "%d", m_score);
	m_scorelabel->setString(buf);

	m_bonusbar->setPercent(m_bonusbar->getPercent()+bonus);
}

void CrushGameScene::onUpdateMenuCallback(Ref*)
{
	//只是测试用，宝石消除过程中不能更新布局，不然崩溃
	m_jewelsgrid->updateMap();

	if (m_jewelsgrid->isDeadMap())
	{
		log("this is a dead map!");
	}
	else
	{
		log("this is not a dead map~");
	}
}

void CrushGameScene::exitCrushGame()
{

}