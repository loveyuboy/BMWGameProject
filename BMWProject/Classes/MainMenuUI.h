#ifndef __MAINMENUUI__LAYER__
#define __MAINMENUUI__LAYER__

#include "Util.h"
#include "cocos2d.h"
#include "cocostudio/WidgetCallBackHandlerProtocol.h"
#include "cocostudio/CocosStudioExport.h"
#include "cocostudio/WidgetReader/NodeReader/NodeReader.h"

CCSTUDIO_CREATE_STUDIOREADER_CLASS_H(MainMenuUIReader);

class MainMenuUI : public cocos2d::Node, public cocostudio::WidgetCallBackHandlerProtocol
{
public:
	CREATE_FUNC(MainMenuUI)
		MainMenuUI();

	virtual cocos2d::ui::Widget::ccWidgetTouchCallback onLocateTouchCallback(const std::string &callBackName);
	virtual cocos2d::ui::Widget::ccWidgetClickCallback onLocateClickCallback(const std::string &callBackName);

	void onTouch(cocos2d::Ref* sender, cocos2d::ui::Widget::TouchEventType type);
	void onClick(cocos2d::Ref* sender);

private:
	std::vector<std::string> _touchTypes;
	std::string _click;
	std::vector<std::string> _eventTypes;
};

#endif /* defined(__TestCpp__CustomRootNode__) */