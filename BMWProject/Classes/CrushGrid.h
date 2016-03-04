#ifndef __CRUSHGRID__CRUSHGAME__
#define __CRUSHGRID__CRUSHGAME__

#include "cocos2d.h"
#include <vector>

USING_NS_CC;
using namespace std;

#define GRID_WIDTH 67 //һ����������Ϊ40
#define MOVE_SPEED 0.2 //��ʯ�ƶ��ٶ�
#define FIRST_CRUSH_NODE_ID 1 //��һ����ʯ��ID����ʯIDĬ�����ε�����
#define LAST_CRUSH_NODE_ID 5 //���һ����ʯ��ID


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
	int m_row;//����
	int m_col;//����

	bool m_isOver;
	CrushNode* m_pCrushNodeSelected;//��ǰѡ��ı�ʯ
	CrushNode* m_pCrushNodeSwapped; //׼�������ı�ʯ

	vector<vector<CrushNode*> > m_vCrushesBox; //��ű�ʯ���������
	Vector<CrushNode*> m_VCrushNodeBox;// ׼�������ı�ʯ������
	Vector<CrushNode*> m_VNewCrushNodeBox;// ׼�����벼�ֵı�ʯ������
};

#endif