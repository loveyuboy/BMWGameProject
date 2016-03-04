#include "CrushNode.h"

CrushNode::CrushNode():
	m_pCrushSprite(NULL),
	m_pFireSprite(NULL),
	m_isBecomeFire(false)
{
	m_isFireState = false;
}
CrushNode::~CrushNode()
{

}

CrushNode* CrushNode::createByType(int pType,int x ,int y)
{
	auto t_node = new CrushNode();
	if (t_node && t_node->init(pType,x,y))
	{
		t_node->autorelease();
		return t_node;
	}
	else
	{
		CC_SAFE_DELETE(t_node);
		return nullptr;
	}
}


bool CrushNode::init(int pType,int x,int y)
{

	m_eType = (E_CRUSHNODE_TYPE)pType;
	m_x = x;
	m_y = y;

	m_isCrushing = false;
	m_isSwaping = false;
	char name[100] = {0};
	sprintf(name, "jm_icon_%d.png", pType);
	m_pCrushSprite = Sprite::createWithTexture(TextureCache::getInstance()->getTextureForKey(name));
	char t_name[100] = {0};
	//ToDo 预加载资源 初始化文理，在游戏主场景中加载完毕 tab表 根据类型去读表 取哪个数值
	m_pCrushSprite->setScale(0.5f);
	m_pCrushSprite->setAnchorPoint(Vec2(0,0));
	
	addChild(m_pCrushSprite);

	return true;
}

void CrushNode::crush()
{
	//ToDo CrushGameMgr 加分
	Game::Instance()->addCurrentScoreByGameType(GAME_TYPE_CRUSH,5);
	//开始消除，消除状态为真，直到消除动作结束，将宝石移出渲染节点，并置消除状态为假
	m_isCrushing = true;
	auto action = FadeOut::create(0.2);
	auto call = CallFunc::create([this](){
		if (this->m_pFireSprite)
		{
			log(" SpriteFrame %d - - - - -",this->getReferenceCount());
			this->m_pFireSprite->removeFromParentAndCleanup(true);
		}
		log(" - - - -- - %d - - - - -",this->getReferenceCount());
		this->removeFromParentAndCleanup(true);
		m_isCrushing = false;
	});
	this->runAction(Sequence::create(action,call,nullptr));
}

void CrushNode::setFireState(bool isTrue)
{
	if (!m_pFireSprite)
	{
		m_isBecomeFire = false;
		m_isFireState = isTrue;
		m_pFireSprite = Sprite::create();
		auto animation = Animation::create(); 
		for( int i=1;i<3;i++) 
		{ 
			char szName[100] = {0}; 
			sprintf(szName, "jm_dongxiao_%d.png", i); 
			animation->addSpriteFrameWithFile(szName); 
		} 
		m_pFireSprite->setScale(0.5f);
		m_pFireSprite->setAnchorPoint(Vec2(0,0));
		m_pFireSprite->setPosition(Vec2(-25,-20));
		animation->setDelayPerUnit(0.1f); 
		animation->setRestoreOriginalFrame(true); 
		auto action = Animate::create(animation); 
		auto doAction = RepeatForever::create(Sequence::create(action, action->reverse(),NULL));
		m_pFireSprite->runAction(doAction);
		m_pFireSprite->setZOrder(-1);
		addChild(m_pFireSprite);
		//m_pFireSprite->retain();
	}

}