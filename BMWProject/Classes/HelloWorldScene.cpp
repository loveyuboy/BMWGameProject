#include "HelloWorldScene.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"
#include "CsvData/CsvUtil.h"
#include "CsvData/c_monster.h"

USING_NS_CC;
using namespace cocos2d::ui;

using namespace cocostudio::timeline;

Scene* HelloWorld::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = HelloWorld::create();

     //add layer as a child to scene
    scene->addChild(layer);

	

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }
	auto rootNode = CSLoader::createNode("MainMenu.csb");
	addChild(rootNode);

  	Button *button1 = static_cast<Button*>(rootNode->getChildByName("Btn_SanXiao"));
  	button1->addClickEventListener(CC_CALLBACK_1(HelloWorld::onBtnCKick,this));
	Button *button2 = static_cast<Button*>(rootNode->getChildByName("btn_Exit"));
	button2->addClickEventListener(CC_CALLBACK_1(HelloWorld::onBtnCKick,this));

    
	//TestCsvDataFileRead();

    return true;
}
void HelloWorld::TestCsvDataFileRead()
{
// 	/* 测试读取Csv文件 */
// 	const char* sPath = "Monster.csv";
// 
// 	/* 加载文件 */
// 	CsvUtil::getInstance()->loadFile(sPath);
// 
// 	/* 获取第一个怪物的名字 */
// 	Value firstMonsterName = CsvUtil::getInstance()->getValue(2, 1, sPath);
// 
// 	/* 获取第二个怪物的HP值 */
// 	Value secMonsterHP = CsvUtil::getInstance()->getValue(3, 3, sPath);
	std::string str = c_monster::GetSingleton().GetNode(0).Name;
	log("firstMonsterName = %s", str.c_str());

}

void HelloWorld::BtnSanXiaoClickCallBack(cocos2d::Ref* pSender)
{

	log("- - - - - - - - -");
}

void HelloWorld::onBtnCKick(cocos2d::Ref* sender)
{
	Button* t_button = dynamic_cast<Button*>(sender);
	if (t_button)
	{
		log("On button click ,tag : %d",t_button->getTag());
		switch (t_button->getTag())
		{
		case 1:
			{
				break;
			}
		case 2:
			{

				break;
			}
		case 3:
			{

				break;
			}
		case 4:
			{
				CCDirector::getInstance()->end();
				break;
			}
		default:
			break;
		}
	}
	
}