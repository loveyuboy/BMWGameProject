#include "Util.h"
#include "CrushGrid.h"
#include "CrushNode.h"
#include "SimpleAudioEngine.h"

using namespace CocosDenshion;
CrushGrid::CrushGrid()
{
	m_isOver = false;
}
CrushGrid* CrushGrid::create(int row, int col)
{
	auto t_CrushGrid = new CrushGrid();
	if (t_CrushGrid && t_CrushGrid->init(row, col))
	{
		t_CrushGrid->autorelease();
		return t_CrushGrid;
	}
	else
	{
		CC_SAFE_DELETE(t_CrushGrid);
		return nullptr;
	}
}

bool CrushGrid::init(int row, int col)
{
	Node::init();

	m_row = row;
	m_col = col;

	m_pCrushNodeSelected = nullptr;
	m_pCrushNodeSwapped = nullptr;

	//�������г�ʼ��һ���յı�ʯ������С
	m_vCrushesBox.resize(m_col);
	for (auto &vec : m_vCrushesBox)
		vec.resize(m_row);

	//1.���ݲ��ִ�С��������ʯ����
	//2.�������������½�Ϊԭ�㣬x��y��Ϊ������
	for (int x = 0; x < m_col; x++)
	{
		for (int y = 0; y < m_row; y++)
		{
			m_vCrushesBox[x][y] = createCrushNode(x, y); 
		}
	}

	while (isDeadMap())
	{
		log("dead map! need to update");
		updateMap();
	}

	//���봥������
	auto listener = EventListenerTouchOneByOne::create();
	listener->setSwallowTouches(true);
	listener->onTouchBegan = CC_CALLBACK_2(CrushGrid::onTouchBegan, this);
	listener->onTouchMoved = CC_CALLBACK_2(CrushGrid::onTouchMoved, this);

	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
	
	log("CrushGrid init!");
	return true;
}

void CrushGrid::updateMap()
{
	for (int x = 0; x < m_col; x++)
	{
		for (int y = 0; y < m_row; y++)
		{
			m_vCrushesBox[x][y]->removeFromParent();
			m_vCrushesBox[x][y] = createCrushNode(x, y); 
		}
	}

	log("update a new map!");
}

bool CrushGrid::isDeadMap()
{
	//ģ�⽻�����жϽ������Ƿ����������粻�ܣ���ô���Ǹ���ͼ
	auto swap = [](CrushNode** a, CrushNode** b)
	{
		auto temp = *a;
		*a = *b;
		*b = temp;
	};

	bool isDeadMap = true;

	//����ÿһ�ű�ʯ
	for (int x = 0; x < m_col; x++)
	{
		for (int y = 0; y < m_row; y++)
		{
			//����ߵĽ���
			if (x > 0)
			{
				swap(&m_vCrushesBox[x][y], &m_vCrushesBox[x-1][y]);
				if (canCrush())
					isDeadMap = false;
				swap(&m_vCrushesBox[x][y], &m_vCrushesBox[x-1][y]);
			}

			//���ұߵĽ���
			if (x < m_col - 1)
			{
				swap(&m_vCrushesBox[x][y], &m_vCrushesBox[x+1][y]);
				if (canCrush())
					isDeadMap = false;
				swap(&m_vCrushesBox[x][y], &m_vCrushesBox[x+1][y]);
			}

			//������Ľ���
			if (y < m_row - 1)
			{
				swap(&m_vCrushesBox[x][y], &m_vCrushesBox[x][y+1]);
				if (canCrush())
					isDeadMap = false;
				swap(&m_vCrushesBox[x][y], &m_vCrushesBox[x][y+1]);
			}

			//������Ľ���
			if (y > 0)
			{
				swap(&m_vCrushesBox[x][y], &m_vCrushesBox[x][y-1]);
				if (canCrush())
					isDeadMap = false;
				swap(&m_vCrushesBox[x][y], &m_vCrushesBox[x][y-1]);
			}
		}
	}

	//canCrush��洢�������ı�ʯ��ȥ��������ģ�⽻�������Ի�Ҫ���
	m_VCrushNodeBox.clear();

	return isDeadMap;
}

CrushNode* CrushGrid::createCrushNode(int x, int y)
{
	//1.���ݲ������괴��һ�ű�ʯ���������
	//2.�жϸñ�ʯ�Ƿ�Ϸ�������������
	//3.���øñ�ʯ����������
	//4.���ñ�ʯ������Ⱦ�ڵ�
	CrushNode* CrushNode = nullptr;

	while(1)
	{
		CrushNode = CrushNode::createByType(random(FIRST_CRUSH_NODE_ID, LAST_CRUSH_NODE_ID),x, y);

		if (isCrushNodeLegal(CrushNode, x, y))
		{
			break;
		}
	}	

	setCrushNodePixPos(CrushNode, x, y);
	addChild(CrushNode);

	//log("add a CrushNode!---type:%d---x:%d---y:%d", CrushNode->getType(), x, y);

	return CrushNode;
}

bool CrushGrid::isCrushNodeLegal(CrushNode* CrushNode, int x, int y)
{
	//1.�ֱ��ж��¼���ı�ʯ��x��y�᷽���Ƿ������
	//2.�����Ǵ���������뱦ʯ�����ֻ�����������ж�
	//3.x��y����С�ڵ���1�����ж�
	//4.����ͬʱ�Ϸ����Ϸ�
	bool isXLegal = true;
	bool isYLegal = true;

	if (x > 1)
	{
		//��x�Ḻ����ֱ�Ƚ���λ�������ʯ���Ͷ�һ������ô��ʾ�������ñ�ʯ���Ϸ�
		if (CrushNode->getType() == m_vCrushesBox[x-1][y]->getType() && 
			CrushNode->getType() == m_vCrushesBox[x-2][y]->getType()
			)
		{
			isXLegal = false;
		}
		else
			isXLegal = true;
	}

	if (y > 1)
	{
		//��y�Ḻ����ֱ�Ƚ���λ�������ʯ���Ͷ�һ������ô��ʾ�������ñ�ʯ���Ϸ�
		if (CrushNode->getType() == m_vCrushesBox[x][y-1]->getType() && 
			CrushNode->getType() == m_vCrushesBox[x][y-2]->getType())
		{
			isYLegal = false;
		}
		else
			isYLegal = true;
	}

	return isXLegal && isYLegal;
}

void CrushGrid::setCrushNodePixPos(CrushNode* CrushNode, float x, float y)
{
	CrushNode->setPosition(x * GRID_WIDTH + 5, y * GRID_WIDTH); 
}

bool CrushGrid::onTouchBegan(Touch* pTouch, Event* pEvent)
{

	if (m_isOver)
	{
		return false;
	}
	//�������������ת��Ϊģ������
	auto pos = this->convertToNodeSpace(pTouch->getLocation());

	//�Ƿ��а��ڱ�ʯ������
	if (Rect(0, 0, m_col*GRID_WIDTH, m_row*GRID_WIDTH).containsPoint(pos))
	{
		//�õ���������
		int x = pos.x / GRID_WIDTH;
		int y = pos.y / GRID_WIDTH;

		//�õ���ǰѡ�еı�ʯ
		m_pCrushNodeSelected = m_vCrushesBox[x][y];

		//log("touch coordinate: x=%d,y=%d CrushNode's type:%d", x, y, m_pCrushNodeSelected->getType());

		return true;
	}
	else
	{
		return false;
	}
}

void CrushGrid::onTouchMoved(Touch* pTouch, Event* pEvent)
{
	//���û��ѡ��ʯ����ô����
	if (!m_pCrushNodeSelected)
	{
		return;
	}

	//��ѡ��ʯ�Ĳ�������
	int startX = m_pCrushNodeSelected->getX();
	int startY = m_pCrushNodeSelected->getY();

	//������Ĳ�������
	auto pos = this->convertToNodeSpace(pTouch->getLocation());
	int touchX = pos.x / GRID_WIDTH;
	int touchY = pos.y / GRID_WIDTH;

	//��������㲻�ڲ����ڣ����ߴ����㲼���������ѡ��ʯ��������һ������ô����
	if (!Rect(0, 0, m_col*GRID_WIDTH, m_row*GRID_WIDTH).containsPoint(pos) || Vec2(touchX, touchY) == Vec2(startX, startY)) 
	{
		return;
	}

	//�ж���ѡ��ʯ�Ĳ��������봥����Ĳ��������Ƿ�ֱ�����һ����λ
	if (abs(startX - touchX) + abs(startY - touchY) != 1)
	{
		//log("touch pos not on border");
		return;
	}

	//���µ����������������ı�ʯ���������н����ı�ʯ
	//ͨ��������������ȡ�������ı�ʯ
	m_pCrushNodeSwapped = m_vCrushesBox[touchX][touchY];

	//������ʯ����������״̬��׽�������ڽ�����ɺ��ж��Ƿ����������
	swapCrushNodes(m_pCrushNodeSelected, m_pCrushNodeSwapped);
	schedule(schedule_selector(CrushGrid::onCrushNodesSwaping));
}

void CrushGrid::swapCrushNodes(CrushNode *jewelA, CrushNode *jewelB)
{
	_eventDispatcher->pauseEventListenersForTarget(this); //������ʼ���رմ�������

	//1.������ʯ�����ڵı�ʯָ��
	//2.������ʯ����
	//3.��ʯ�ƶ����µ�λ��
	auto temp = m_vCrushesBox[jewelA->getX()][jewelA->getY()];
	m_vCrushesBox[jewelA->getX()][jewelA->getY()] = m_vCrushesBox[jewelB->getX()][jewelB->getY()];
	m_vCrushesBox[jewelB->getX()][jewelB->getY()] = temp;

	auto tempX = jewelA->getX();
	jewelA->setX(jewelB->getX());
	jewelB->setX(tempX);

	auto tempY = jewelA->getY();
	jewelA->setY(jewelB->getY());
	jewelB->setY(tempY);

	swapCrushNodesToNewPos(jewelA);
	swapCrushNodesToNewPos(jewelB);
}

void CrushGrid::swapCrushNodesToNewPos(CrushNode* CrushNode)
{
	//�ƶ���ʼ���ñ�ʯ����״̬Ϊ�棬�ƶ�����������Ϊ��
	CrushNode->setSwapingState(true);
	auto move = MoveTo::create(MOVE_SPEED, Vec2(CrushNode->getX() * GRID_WIDTH, CrushNode->getY() * GRID_WIDTH));
	auto call = CallFunc::create([CrushNode](){
		CrushNode->setSwapingState(false);
	});
	CrushNode->runAction(Sequence::create(move, call, nullptr));
}

bool CrushGrid::canCrush()
{
	//�ú����жϵ�ǰ״̬�ı�ʯ�����Ƿ�������
	//���������ı�ʯ����������ʯ���ӣ��ȴ�����

	int count = 0; //������
	CrushNode *JewelBegin = nullptr; //��ʼ�����ı�ʯ
	CrushNode *JewelNext = nullptr; //����ʼ��ʯ��ʼ��ǰ�����ı�ʯ

	//����ÿһ��
	for (int x = 0; x < m_col; x++) 
	{
		for (int y = 0; y < m_row - 1;)
		{
			count = 1;
			JewelBegin = m_vCrushesBox[x][y];
			JewelNext = m_vCrushesBox[x][y + 1];

			//�����������ͬ����
			while (JewelBegin->getType() == JewelNext->getType())
			{
				count++;
				int nextIndex = y + count;
				if (nextIndex > m_row - 1)
					break;
				JewelNext = m_vCrushesBox[x][nextIndex];
			}
			//������������ڵ���3����ô��������Щ��ʯӦ�������������Ǵ���������ʯ����
			if (count >= 3)
			{
				for (int n = 0; n < count; n++)
				{
					auto t_CrushNode = m_vCrushesBox[x][y+n];
					if (n == 0 && t_CrushNode->getType() == 3 && count >= 4)
					{
						t_CrushNode->setBecomeFire(true);
					}
					m_VCrushNodeBox.pushBack(t_CrushNode);
				}
			}
			y += count;
		}
	}

	//����ÿһ�У��߼��ͱ���ÿһ����һ����
	for (int y = 0; y < m_row; y++) 
	{
		for (int x = 0; x < m_col - 1;)
		{
			count = 1;
			JewelBegin = m_vCrushesBox[x][y];
			JewelNext = m_vCrushesBox[x+1][y];

			while (JewelBegin->getType() == JewelNext->getType())
			{


				count++;
				int nextIndex = x + count;
				if (nextIndex > m_col - 1)
					break;
				JewelNext = m_vCrushesBox[nextIndex][y];
			}
			if (count >= 3)
			{
				for (int n = 0; n < count; n++)
				{
					auto t_CrushNode = m_vCrushesBox[x+n][y];
					if (n == 0 && t_CrushNode->getType() == 3 && count >= 4)
					{
						//�п����б�ʯͬʱ���п���������ô�����ظ��洢��������ʯ���ӣ��������һ���ж�
						if (m_VCrushNodeBox.find(t_CrushNode) != m_VCrushNodeBox.end())
						{
							continue;
						}
						t_CrushNode->setBecomeFire(true);
					}

					//�п����б�ʯͬʱ���п���������ô�����ظ��洢��������ʯ���ӣ��������һ���ж�
					if (m_VCrushNodeBox.find(t_CrushNode) != m_VCrushNodeBox.end())
					{
						continue;
					}
					m_VCrushNodeBox.pushBack(t_CrushNode);
				}
			}
			x += count;
		}
	}	

	//���������ʯ���Ӳ�Ϊ�գ���ô˵�������п�������������
	if (!m_VCrushNodeBox.empty())
	{
		return true;
	}
	else
	{
		return false;
	}
}

void CrushGrid::goCrush()
{

	map<int,int> t_map;
	map<int,int>::iterator t_iter;

	auto t_CrushIter = m_VCrushNodeBox.begin();
	while (t_CrushIter != m_VCrushNodeBox.end())
	{
		if ((*t_CrushIter)->getBecomeFire())
		{
			(*t_CrushIter)->setFireState(true);
			m_VCrushNodeBox.erase(t_CrushIter);
		}
		t_CrushIter++;

	}

	//����������ʯ���ӣ������еı�ʯ������������
	for (auto t_CrushNode : m_VCrushNodeBox)
	{
		t_iter = t_map.find(t_CrushNode->getX());
		if (t_iter != t_map.end())
		{
			t_iter->second += 1;
		}
		else
		{
			t_iter->second = 0;
		}
		//�����µı�ʯ�������������ʼλ����������һ�е��ϱ�һ�У�������һ��
		auto newJewel = CrushNode::createByType(random(FIRST_CRUSH_NODE_ID, LAST_CRUSH_NODE_ID), t_CrushNode->getX(), m_row + t_iter->second ); 
		setCrushNodePixPos(newJewel, newJewel->getX(), m_row); 
		addChild(newJewel);

		//���±�ʯ�ŵ��±�ʯ�����ڣ��ȴ����벼��
		m_VNewCrushNodeBox.pushBack(newJewel);

		//��ʯ������Ӧ��ˢ�µı�ʯ��ʱ��Ϊ��
		m_vCrushesBox[t_CrushNode->getX()][t_CrushNode->getY()] = nullptr;

		//ԭ�б�ʯ��������
		t_CrushNode->crush();
	}
	SimpleAudioEngine::getInstance()->playEffect("crush1.mp3");
}

void CrushGrid::refreshCrushNodesGrid()
{
	//�����У���������п�λ����ôӦ��ˢ��
	for (int x = 0; x < m_col; x++)
	{
		int empty_count = 0; //һ���ܵĿո�����

		for (int y = 0; y < m_row; y++)
		{
			//��������������ʯ�����ڵı�ʯָ�룬���Ϊ�գ���ô˵��������λ��Ϊ��
			auto CrushNode = m_vCrushesBox[x][y];
			if (!CrushNode)
				empty_count++;
		}
		if (empty_count)
		{
			//log("the %d col has %d empty", x, empty_count);
			//�ҵ��п�λ���У�ˢ�¸��еı�ʯ
			refreshCrushNodesToNewPos(x);
		}
	}
}

void CrushGrid::refreshCrushNodesToNewPos(int col)
{
	//ˢ�¸�������ı�ʯ
	int n = 0; //��ǰ�������Ŀ�λ��
	auto pJewelsbox = &m_vCrushesBox; //����һ����ʯ���ӵ�ָ�룬����Ϊ�������ܴ���lamda

	//�������еı�ʯ����
	for (int y = 0; y < m_row; y++)
	{
		auto t_CrushNode = m_vCrushesBox[col][y];

		if (!t_CrushNode)
		{
			n++;
			continue;
		}
		else if (n != 0)
		{
			t_CrushNode->setY(t_CrushNode->getY() - n);
			auto move = MoveBy::create(0.2, Vec2(0, -n*GRID_WIDTH));
			auto call = CallFunc::create([pJewelsbox, t_CrushNode](){
				//���±�ʯ�����ڵ�����
				(*pJewelsbox)[t_CrushNode->getX()][t_CrushNode->getY()] = t_CrushNode;
			});

			t_CrushNode->runAction(Sequence::create(move, call, nullptr));
		}
	}

	//�����±�ʯ����
	int i = n;
	int delta = 1;

	for (auto CrushNode : m_VNewCrushNodeBox)
	{
		if (CrushNode->getX() == col)
		{
			CrushNode->setY(m_row - i);

			auto delay = DelayTime::create(0.2);
			//��������ٶ�������һЩ
			auto move = MoveBy::create(0.2*delta++, Vec2(0, -i--*GRID_WIDTH));
			auto call = CallFunc::create([CrushNode, pJewelsbox, this](){
				(*pJewelsbox)[CrushNode->getX()][CrushNode->getY()] = CrushNode;
				//���±�ʯ�������Ƴ��ñ�ʯ
				m_VNewCrushNodeBox.eraseObject(CrushNode);
			});

			CrushNode->runAction(Sequence::create(delay, move, call, nullptr));
		}
	}
}

void CrushGrid::onCrushNodesSwaping(float dt)
{
	//��׽�������ڽ����ı�ʯ�Ľ��������Ƿ��Ѿ�ֹͣ�����ûֹͣ�����ؼ�����׽
	if (m_pCrushNodeSelected->isSwaping() || m_pCrushNodeSwapped->isSwaping())
	{
		return;
	}
	//�����ʯ��������ִ�����
	else
	{
		unschedule(schedule_selector(CrushGrid::onCrushNodesSwaping));  //ֹͣ��׽

		log("swap over!");

		log("is it can crush?");

		//�ж��Ƿ�ǰ״̬��������
		if (canCrush())
		{
			log("yes,crush!");

			

			//��ʼ��������������״̬��׽����(��׽��������Ϻ�ˢ�²���)����һ��������ʽ��ʼ
			//SimpleAudioEngine::getInstance()->playEffect("crush1.mp3");

// 			if (m_pCrushNodeSelected->isFireState() || m_pCrushNodeSwapped->isFireState())
// 			{
// 
// 				int t_x = m_pCrushNodeSwapped->isFireState() ? m_pCrushNodeSwapped->getX() : m_pCrushNodeSelected->getX();
// 				int t_y = m_pCrushNodeSwapped->isFireState() ? m_pCrushNodeSwapped->getY() : m_pCrushNodeSelected->getY();
// 				for (int x = 0; x < m_col; x++) 
// 				{
// 					m_VCrushNodeBox.pushBack( m_vCrushesBox[x][t_y]);
// 				}
// 
// 				for (int y = 0; y < m_row - 1;y++)
// 				{
// 					m_VCrushNodeBox.pushBack( m_vCrushesBox[t_x][y]);
// 					log("y = %d",y);
// 				}
// 			}
			for (auto t_crushNode : m_VCrushNodeBox)
			{
				if (t_crushNode->isFireState())
				{
					int t_x = t_crushNode->getX();
					int t_y = t_crushNode->getY();
					for (int x = 0; x < m_col; x++) 
					{
						m_VCrushNodeBox.pushBack( m_vCrushesBox[x][t_y]);
					}

					for (int y = 0; y < m_row - 1;y++)
					{
						m_VCrushNodeBox.pushBack( m_vCrushesBox[t_x][y]);
						log("y = %d",y);
					}
 				}
			}
			m_pCrushNodeSelected = nullptr;
			goCrush();
			schedule(schedule_selector(CrushGrid::onCrushNodesCrushing));
		}
		else
		{
			log("no, cant crush!");
			if (m_pCrushNodeSelected->isFireState() || m_pCrushNodeSwapped->isFireState())
			{
				
				int t_x = m_pCrushNodeSwapped->isFireState() ? m_pCrushNodeSwapped->getX() : m_pCrushNodeSelected->getX();
				int t_y = m_pCrushNodeSwapped->isFireState() ? m_pCrushNodeSwapped->getY() : m_pCrushNodeSelected->getY();
				for (int x = 0; x < m_col; x++) 
				{
					m_VCrushNodeBox.pushBack( m_vCrushesBox[x][t_y]);
				}

				for (int y = 0; y < m_row - 1;y++)
				{
					m_VCrushNodeBox.pushBack( m_vCrushesBox[t_x][y]);
					log("y = %d",y);
				}
				goCrush();
				schedule(schedule_selector(CrushGrid::onCrushNodesCrushing));
				return;
			}
			//����������������ȥ��������������ʱ�Ĳ�׽����(��׽��������Ϻ󣬿�����������)
			SimpleAudioEngine::getInstance()->playEffect("swapback.mp3");
			swapCrushNodes(m_pCrushNodeSelected, m_pCrushNodeSwapped);
			schedule(schedule_selector(CrushGrid::onCrushNodesSwapingBack));
		}
	}
}

void CrushGrid::onCrushNodesSwapingBack(float dt)
{
	//��׽�������ڽ����ı�ʯ�Ľ��������Ƿ��Ѿ�ֹͣ�����ûֹͣ�����ؼ�����׽
	if (m_pCrushNodeSelected->isSwaping() || m_pCrushNodeSwapped->isSwaping())
	{
		return;
	}
	else
	{
		unschedule(schedule_selector(CrushGrid::onCrushNodesSwapingBack)); //ֹͣ��׽

		log("swap back!");

		m_pCrushNodeSelected = nullptr;

		_eventDispatcher->resumeEventListenersForTarget(this); //���¿�ʼ��������
	}
}

void CrushGrid::onCrushNodesCrushing(float dt)
{
	//��׽��ʯ����״̬������б�ʯ������������ô������׽
	for (auto t_CrushNode : m_VCrushNodeBox)
	{
		if ( t_CrushNode->isCrushing())
		{
			//log("crushing");
			return;
		}
		t_CrushNode->setBecomeFire(false);
	}

	//���ȫ����ʯ�Ѿ�������ϣ�ֹͣ��׽����
	unschedule(schedule_selector(CrushGrid::onCrushNodesCrushing));

	m_VCrushNodeBox.clear(); //���������ʯ����

	log("crush over!");
	log("begin to refresh!");

	//ˢ�±�ʯ���У�������ˢ��״̬��׽������ˢ��һ������������ж��������Ƿ��������
	refreshCrushNodesGrid();
	schedule(schedule_selector(CrushGrid::onCrushNodesRefreshing));
}

void CrushGrid::onCrushNodesRefreshing(float dt)
{
	//��׽��ʯˢ��״̬������±�ʯ���ӻ��б�ʯ�����±�ʯ����ˢ�µ��У�����ô������׽
	if (m_VNewCrushNodeBox.size() != 0)
	{
		//log("refreshing!");
		return;
	}
	else
	{
		unschedule(schedule_selector(CrushGrid::onCrushNodesRefreshing));

		log("refresh over!");
		log("and now, is it can crush?");

		if (canCrush())
		{
			log("yes, crush again!");

			//�������������ô��������
			//SimpleAudioEngine::getInstance()->playEffect("crush.ogg");

			for (auto t_crushNode : m_VCrushNodeBox)
			{
				if (t_crushNode->isFireState())
				{
					int t_x = t_crushNode->getX();
					int t_y = t_crushNode->getY();
					for (int x = 0; x < m_col; x++) 
					{
						auto t_iter = m_VCrushNodeBox.find(m_vCrushesBox[x][t_y]);
						if (t_iter != m_VCrushNodeBox.end())
						{
							m_VCrushNodeBox.pushBack(*t_iter);
						}
					}

					for (int y = 0; y < m_row - 1;y++)
					{
						auto t_iter = m_VCrushNodeBox.find(m_vCrushesBox[t_x][y]);
						if (t_iter != m_VCrushNodeBox.end())
						{
							m_VCrushNodeBox.pushBack(*t_iter);
						}
						log("y = %d",y);
					}
				}
			}
			goCrush();
			schedule(schedule_selector(CrushGrid::onCrushNodesCrushing));
		}
		else
		{
			log("no, cant crush! over!");

			//�ж��Ƿ�Ϊ��ͼ������ǣ���ִ��һ�����ֶ�������ʾ�������µ�ͼ
			if (isDeadMap())
			{
				log("cant crush any more, updating a new map!");

				auto winSize = Director::getInstance()->getWinSize();
				auto label = Label::createWithTTF("Cant Crush Any More, Change!", "fonts/Marker Felt.ttf", 24);
				label->setTextColor(Color4B::BLACK);
				label->setPosition(winSize.width / 2, winSize.height / 2);
				label->setOpacity(0);
				this->getParent()->addChild(label);

				//��ʾ���ֵ��뵭���󣬸��µ�ͼ���ٿ�����������
				auto fadein = FadeIn::create(0.5);
				auto fadeout = FadeOut::create(0.5);

				auto call = CallFunc::create([this, label](){
					do
					{
						updateMap();
					} while (isDeadMap());

					label->removeFromParent();

					_eventDispatcher->resumeEventListenersForTarget(this);
				});

				label->runAction(Sequence::create(fadein, DelayTime::create(2), fadeout, call, nullptr));
			}
			else
			{
				//���������ͼ����ô��ֱ�ӿ��������������ȴ���һ�ֵĽ�������
				_eventDispatcher->resumeEventListenersForTarget(this);
			}
		}
	}
}

void  CrushGrid::setGameOver()
{
	m_isOver  = true;
}