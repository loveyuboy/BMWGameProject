#ifndef __CRUSHGRID__CRUSHGAME__
#define __CRUSHGRID__CRUSHGAME__

#include "cocos2d.h"
#include <vector>

USING_NS_CC;
using namespace std;

#define GRID_WIDTH 67 //一个格子像素为40
#define MOVE_SPEED 0.2 //宝石移动速度
#define FIRST_CRUSH_NODE_ID 1 //第一个宝石的ID，宝石ID默认依次递增的
#define LAST_CRUSH_NODE_ID 5 //最后一个宝石的ID


class CrushNode;

class CrushGrid : public Node
{
public:
	CrushGrid();

	void updateMap();
	static CrushGrid* create(int row, int col);
	bool init(int row, int col);
	CrushNode* createCrushNode(int x , int y);

	bool isCrushNodeLegal(CrushNode* pCrush,int x,int y);
	void setCrushNodePixPos(CrushNode* pCrushNode, float x, float y);
	void swapCrushNodes(CrushNode *jewelA, CrushNode *jewelB);
	void swapCrushNodesToNewPos(CrushNode* CrushNode);
	void refreshCrushNodesGrid();
	void refreshCrushNodesToNewPos(int col);
	void onCrushNodesSwaping(float dt);
	void onCrushNodesCrushing(float dt);
	void onCrushNodesRefreshing(float dt);
	void onCrushNodesSwapingBack(float dt);
	bool isDeadMap();
	bool canCrush();
	void goCrush();

	bool onTouchBegan(Touch* pTouch, Event* pEvent);
	void onTouchMoved(Touch* pTouch, Event* pEvent);

	void setGameOver();
public:
	int getRow() { return m_row; }
	int getCol() { return m_col; }

private:
	int m_row;//行数
	int m_col;//列数

	bool m_isOver;
	CrushNode* m_pCrushNodeSelected;//当前选择的宝石
	CrushNode* m_pCrushNodeSwapped; //准备交换的宝石

	vector<vector<CrushNode*> > m_vCrushesBox; //存放宝石对象的容器
	Vector<CrushNode*> m_VCrushNodeBox;// 准备消除的宝石的容器
	Vector<CrushNode*> m_VNewCrushNodeBox;// 准备加入布局的宝石的容器
};

#endif