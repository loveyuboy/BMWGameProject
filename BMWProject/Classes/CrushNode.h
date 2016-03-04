#ifndef __CRUSHNODE__CRUSHGAME__
#define __CRUSHNODE__CRUSHGAME__

#include "Util.h"
#include "CrushGameDef.h"

class CrushNode : public Node
{
public:

	CrushNode();
	~CrushNode();

	static CrushNode* createByType(int pType,int x ,int y);

	bool init(int pType,int x,int y);
	
	bool isSwaping() { return m_isSwaping;}
	bool isCrushing() { return m_isCrushing;}
	inline void setSwapingState(bool isSwaping) {m_isSwaping = isSwaping;};
	inline int getType() { return m_eType; }
	int getX(){ return m_x;}
	int getY(){ return m_y;}
	void  setY(float y){ m_y = y;}
	void  setX(float x){ m_x = x;}
	void crush();
	void setFireState(bool isTrue);
	bool isFireState(){return m_isFireState;}
	void setBecomeFire(bool isTrue) {m_isBecomeFire = isTrue;}
	bool getBecomeFire(){ return m_isBecomeFire;}
private:
	E_CRUSHNODE_TYPE m_eType; 
	bool m_isBecomeFire;
	bool m_isFireState;
	bool m_isSwaping; //是否正在交换
	bool m_isCrushing; // 是否正在消除
	int m_x;
	int m_y;
	Sprite* m_pFireSprite;
	Sprite* m_pCrushSprite;
};

#endif