#include "MainScene.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"
#include "CsvData/CsvUtil.h"
#include "CsvData/c_monster.h"
#include "CrushNode.h"
#include "MainMenuUI.h"

USING_NS_CC;
using namespace cocos2d::ui;

using namespace cocostudio::timeline;

Scene* MainScene::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = MainScene::create();

     //add layer as a child to scene
    scene->addChild(layer);

	

    // return the scene
    return scene;
}
                        
// on "init" you need to initialize your instance
bool MainScene::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }
// 	auto rootNode = CSLoader::createNode("MainMenuUI.csb");
// 	addChild(rootNode);
	//加载完毕回调
	auto addTextureCallback = [](Texture2D* texture)
	{
		//(*ptexture_num)++;
		log("load a texture async");
	};
	//异步预加载宝石
	TextureCache::getInstance()->addImageAsync("jm_icon_1.png", addTextureCallback);
	TextureCache::getInstance()->addImageAsync("jm_icon_2.png", addTextureCallback);
	TextureCache::getInstance()->addImageAsync("jm_icon_3.png", addTextureCallback);
	TextureCache::getInstance()->addImageAsync("jm_icon_4.png", addTextureCallback);
	TextureCache::getInstance()->addImageAsync("jm_icon_5.png", addTextureCallback);


	//异步预加载背景图
	TextureCache::getInstance()->addImageAsync("bground1.png", addTextureCallback);
	TextureCache::getInstance()->addImageAsync("bground2.png", addTextureCallback);
	TextureCache::getInstance()->addImageAsync("bground3.png", addTextureCallback);
	TextureCache::getInstance()->addImageAsync("bground4.png", addTextureCallback);
	TextureCache::getInstance()->addImageAsync("gameover.png", addTextureCallback);

	//异步加载一些其它纹理图
	TextureCache::getInstance()->addImageAsync("board.png", addTextureCallback);

	TextureCache::getInstance()->addImageAsync("jm_dongxiao_1.png", addTextureCallback);
	TextureCache::getInstance()->addImageAsync("jm_dongxiao_2.png", addTextureCallback);

	CCSTUDIO_RESGISTR_READER_OBJECT(MainMenuUIReader)
	auto layer = CREATE_CSLOADER_NODE(MainMenuUI);
	addChild(layer);

	//layer->addChild(CrushNode::createByType(1,1,1));

    return true;
}

void MainScene::TestCsvDataFileRead()
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

void MainScene::BtnSanXiaoClickCallBack(cocos2d::Ref* pSender)
{

	log("- - - - - - - - -");
}

void MainScene::onBtnCKick(cocos2d::Ref* sender)
{
	Button* t_button = dynamic_cast<Button*>(sender);
	if (t_button)
	{
		log("On button click ,tag : %d",t_button->getTag());
		switch (t_button->getTag())
		{
		case 1:
			{
				CCSTUDIO_RESGISTR_READER_OBJECT(MainMenuUIReader)

				auto layer = CREATE_CSLOADER_NODE(CustomRootNode);
				addChild(layer);

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