#ifndef __CRUSHSCENE__CRUSHGAME__
#define __CRUSHSCENE__CRUSHGAME__

#include "Util.h"
#include "cocos2d.h"
#include "ui\CocosGUI.h"
USING_NS_CC;

#define CRUSH_COL_NUM 10
#define CRUSH_ROW_NUM 7
using namespace ui;

class CrushGrid;

class CrushGameScene : public Layer
{
public:
	CrushGameScene();
	static Scene* createScene();
	CREATE_FUNC(CrushGameScene);
	bool init();

public:
	static void addBonus(int bonus);

private:
	static LoadingBar* m_bonusbar;
	static int m_score;
	static Label* m_scorelabel;

	virtual void update(float delta);
private:
	void onUpdateMenuCallback(Ref*);

	void onReducingBonus(float dt); //开启倒计时

	void publishScore(); //存储游戏分数

	void initStudioUI();

	void exitCrushGame();
private:
	CrushGrid* m_jewelsgrid;
	float	m_fCrushTime;
	Text*	m_pLabelTime;
	Text*	m_pLabelCurrScore;
	Text*	m_pLabelTargetScore;
	Sprite* m_bg;
	Node*	m_pGameUI;
};
#endif