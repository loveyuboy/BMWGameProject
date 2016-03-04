#include "MainMenuUI.h"

#include "ui/UIText.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"
#include "CrushGameScene.h"

USING_NS_CC;
using namespace std;
using namespace cocos2d::ui;

CCSTUDIO_CREATE_STUDIOREADER_CLASS_CPP(MainMenuUIReader,MainMenuUI);

MainMenuUI::MainMenuUI()
{
	_touchTypes.push_back("Touch: begin");
	_touchTypes.push_back("Touch: moved");
	_touchTypes.push_back("Touch: ended");
	_touchTypes.push_back("Touch: canceled");
	_click = "Click";
}

Widget::ccWidgetTouchCallback MainMenuUI::onLocateTouchCallback(const string &callBackName)
{
	if (callBackName == "onTouch")
	{
		return CC_CALLBACK_2(MainMenuUI::onTouch, this);
	}

	return nullptr;
}

Widget::ccWidgetClickCallback MainMenuUI::onLocateClickCallback(const string &callBackName)
{
 	if (callBackName == "onClick")
	{
		return CC_CALLBACK_1(MainMenuUI::onClick, this);
	}

	return nullptr;
}


void MainMenuUI::onTouch(cocos2d::Ref* object, cocos2d::ui::Widget::TouchEventType type)
{
	CCLOG("onTouch");

}

void MainMenuUI::onClick(cocos2d::Ref* sender)
{
	CCLOG("onClick");
	//     Text* info = dynamic_cast<Text*>(getChildByName("Info"));
	//     info->setString(_click);

	Button* t_button = dynamic_cast<Button*>(sender);
	if (t_button)
	{
		log("On button click ,tag : %d",t_button->getTag());
		switch (t_button->getTag())
		{
		case 101:
			{
				log("Shoot Game Click");
				break;
			}
		case 102:
			{
				log("Trank Game Click");
				break;
			}
		case 103:
			{
				log("Crush Game Click");
				auto t_CrushScene = CrushGameScene::createScene();
				t_CrushScene = TransitionCrossFade::create(1,t_CrushScene);
				Director::getInstance()->replaceScene(t_CrushScene);
				
				break;
			}
		case 106:
			{
				log("Flash Game Click");
				break;
			}
		case 105:
			{
				CCDirector::getInstance()->end();
				break;
			}
		default:
			break;
		}
	}
}



