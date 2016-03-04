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

	//根据行列初始化一个空的宝石容器大小
	m_vCrushesBox.resize(m_col);
	for (auto &vec : m_vCrushesBox)
		vec.resize(m_row);

	//1.根据布局大小创建出宝石阵列
	//2.布局坐标以左下角为原点，x右y上为正方向
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

	//加入触摸监听
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
	//模拟交换，判断交换后是否能消除，如不能，那么就是个死图
	auto swap = [](CrushNode** a, CrushNode** b)
	{
		auto temp = *a;
		*a = *b;
		*b = temp;
	};

	bool isDeadMap = true;

	//遍历每一颗宝石
	for (int x = 0; x < m_col; x++)
	{
		for (int y = 0; y < m_row; y++)
		{
			//跟左边的交换
			if (x > 0)
			{
				swap(&m_vCrushesBox[x][y], &m_vCrushesBox[x-1][y]);
				if (canCrush())
					isDeadMap = false;
				swap(&m_vCrushesBox[x][y], &m_vCrushesBox[x-1][y]);
			}

			//跟右边的交换
			if (x < m_col - 1)
			{
				swap(&m_vCrushesBox[x][y], &m_vCrushesBox[x+1][y]);
				if (canCrush())
					isDeadMap = false;
				swap(&m_vCrushesBox[x][y], &m_vCrushesBox[x+1][y]);
			}

			//跟上面的交换
			if (y < m_row - 1)
			{
				swap(&m_vCrushesBox[x][y], &m_vCrushesBox[x][y+1]);
				if (canCrush())
					isDeadMap = false;
				swap(&m_vCrushesBox[x][y], &m_vCrushesBox[x][y+1]);
			}

			//跟下面的交换
			if (y > 0)
			{
				swap(&m_vCrushesBox[x][y], &m_vCrushesBox[x][y-1]);
				if (canCrush())
					isDeadMap = false;
				swap(&m_vCrushesBox[x][y], &m_vCrushesBox[x][y-1]);
			}
		}
	}

	//canCrush会存储能消除的宝石进去，由于是模拟交换，所以还要清空
	m_VCrushNodeBox.clear();

	return isDeadMap;
}

CrushNode* CrushGrid::createCrushNode(int x, int y)
{
	//1.根据布局坐标创建一颗宝石，类型随机
	//2.判断该宝石是否合法（不会三消）
	//3.设置该宝石的世界坐标
	//4.将该宝石加入渲染节点
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
	//1.分别判断新加入的宝石在x轴y轴方向是否会三消
	//2.由于是从正方向加入宝石，因此只需往负方向判断
	//3.x，y坐标小于等于1不必判断
	//4.两轴同时合法方合法
	bool isXLegal = true;
	bool isYLegal = true;

	if (x > 1)
	{
		//向x轴负方向分别比较两位，如果宝石类型都一样，那么表示三消，该宝石不合法
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
		//向y轴负方向分别比较两位，如果宝石类型都一样，那么表示三消，该宝石不合法
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
	//将触摸点的坐标转化为模型坐标
	auto pos = this->convertToNodeSpace(pTouch->getLocation());

	//是否有按在宝石布局上
	if (Rect(0, 0, m_col*GRID_WIDTH, m_row*GRID_WIDTH).containsPoint(pos))
	{
		//得到布局坐标
		int x = pos.x / GRID_WIDTH;
		int y = pos.y / GRID_WIDTH;

		//得到当前选中的宝石
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
	//如果没有选择宝石，那么返回
	if (!m_pCrushNodeSelected)
	{
		return;
	}

	//已选择宝石的布局坐标
	int startX = m_pCrushNodeSelected->getX();
	int startY = m_pCrushNodeSelected->getY();

	//触摸点的布局坐标
	auto pos = this->convertToNodeSpace(pTouch->getLocation());
	int touchX = pos.x / GRID_WIDTH;
	int touchY = pos.y / GRID_WIDTH;

	//如果触摸点不在布局内，或者触摸点布局坐标和已选宝石布局坐标一样，那么返回
	if (!Rect(0, 0, m_col*GRID_WIDTH, m_row*GRID_WIDTH).containsPoint(pos) || Vec2(touchX, touchY) == Vec2(startX, startY)) 
	{
		return;
	}

	//判断已选宝石的布局坐标与触摸点的布局坐标是否直角相隔一个单位
	if (abs(startX - touchX) + abs(startY - touchY) != 1)
	{
		//log("touch pos not on border");
		return;
	}

	//余下的情况，触摸点上面的宝石就是欲进行交换的宝石
	//通过坐标索引，获取欲交换的宝石
	m_pCrushNodeSwapped = m_vCrushesBox[touchX][touchY];

	//交换宝石，开启交换状态捕捉函数（在交换完成后，判断是否可以消除）
	swapCrushNodes(m_pCrushNodeSelected, m_pCrushNodeSwapped);
	schedule(schedule_selector(CrushGrid::onCrushNodesSwaping));
}

void CrushGrid::swapCrushNodes(CrushNode *jewelA, CrushNode *jewelB)
{
	_eventDispatcher->pauseEventListenersForTarget(this); //交换开始，关闭触摸监听

	//1.交换宝石容器内的宝石指针
	//2.交换宝石坐标
	//3.宝石移动到新的位置
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
	//移动开始设置宝石交换状态为真，移动结束再设置为假
	CrushNode->setSwapingState(true);
	auto move = MoveTo::create(MOVE_SPEED, Vec2(CrushNode->getX() * GRID_WIDTH, CrushNode->getY() * GRID_WIDTH));
	auto call = CallFunc::create([CrushNode](){
		CrushNode->setSwapingState(false);
	});
	CrushNode->runAction(Sequence::create(move, call, nullptr));
}

bool CrushGrid::canCrush()
{
	//该函数判断当前状态的宝石阵列是否能消除
	//将能消除的宝石加入消除宝石盒子，等待消除

	int count = 0; //连续数
	CrushNode *JewelBegin = nullptr; //起始遍历的宝石
	CrushNode *JewelNext = nullptr; //从起始宝石开始往前遍历的宝石

	//遍历每一列
	for (int x = 0; x < m_col; x++) 
	{
		for (int y = 0; y < m_row - 1;)
		{
			count = 1;
			JewelBegin = m_vCrushesBox[x][y];
			JewelNext = m_vCrushesBox[x][y + 1];

			//如果连续出现同类型
			while (JewelBegin->getType() == JewelNext->getType())
			{
				count++;
				int nextIndex = y + count;
				if (nextIndex > m_row - 1)
					break;
				JewelNext = m_vCrushesBox[x][nextIndex];
			}
			//如果连续数大于等于3，那么遍历的这些宝石应当消除，把它们存入消除宝石盒子
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

	//遍历每一行，逻辑和遍历每一列是一样的
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
						//有可能有宝石同时行列可消除，那么不能重复存储到消除宝石盒子，故需添加一次判断
						if (m_VCrushNodeBox.find(t_CrushNode) != m_VCrushNodeBox.end())
						{
							continue;
						}
						t_CrushNode->setBecomeFire(true);
					}

					//有可能有宝石同时行列可消除，那么不能重复存储到消除宝石盒子，故需添加一次判断
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

	//如果消除宝石盒子不为空，那么说明该阵列可消除，返回真
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

	//遍历消除宝石盒子，对其中的宝石进行消除操作
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
		//生成新的宝石，类型随机，初始位置在最上面一行的上边一行（布局外一格）
		auto newJewel = CrushNode::createByType(random(FIRST_CRUSH_NODE_ID, LAST_CRUSH_NODE_ID), t_CrushNode->getX(), m_row + t_iter->second ); 
		setCrushNodePixPos(newJewel, newJewel->getX(), m_row); 
		addChild(newJewel);

		//将新宝石放到新宝石盒子内，等待加入布局
		m_VNewCrushNodeBox.pushBack(newJewel);

		//宝石盒子内应当刷新的宝石暂时置为空
		m_vCrushesBox[t_CrushNode->getX()][t_CrushNode->getY()] = nullptr;

		//原有宝石对象消除
		t_CrushNode->crush();
	}
	SimpleAudioEngine::getInstance()->playEffect("crush1.mp3");
}

void CrushGrid::refreshCrushNodesGrid()
{
	//遍历列，如果该列有空位，那么应当刷新
	for (int x = 0; x < m_col; x++)
	{
		int empty_count = 0; //一列总的空格子数

		for (int y = 0; y < m_row; y++)
		{
			//根据坐标索引宝石盒子内的宝石指针，如果为空，那么说明该坐标位置为空
			auto CrushNode = m_vCrushesBox[x][y];
			if (!CrushNode)
				empty_count++;
		}
		if (empty_count)
		{
			//log("the %d col has %d empty", x, empty_count);
			//找到有空位的列，刷新该列的宝石
			refreshCrushNodesToNewPos(x);
		}
	}
}

void CrushGrid::refreshCrushNodesToNewPos(int col)
{
	//刷新该列上面的宝石
	int n = 0; //当前遍历到的空位数
	auto pJewelsbox = &m_vCrushesBox; //保存一个宝石盒子的指针，这是为了让其能传入lamda

	//先让现有的宝石下落
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
				//更新宝石盒子内的数据
				(*pJewelsbox)[t_CrushNode->getX()][t_CrushNode->getY()] = t_CrushNode;
			});

			t_CrushNode->runAction(Sequence::create(move, call, nullptr));
		}
	}

	//再让新宝石下落
	int i = n;
	int delta = 1;

	for (auto CrushNode : m_VNewCrushNodeBox)
	{
		if (CrushNode->getX() == col)
		{
			CrushNode->setY(m_row - i);

			auto delay = DelayTime::create(0.2);
			//后下落的速度设置慢一些
			auto move = MoveBy::create(0.2*delta++, Vec2(0, -i--*GRID_WIDTH));
			auto call = CallFunc::create([CrushNode, pJewelsbox, this](){
				(*pJewelsbox)[CrushNode->getX()][CrushNode->getY()] = CrushNode;
				//从新宝石盒子中移除该宝石
				m_VNewCrushNodeBox.eraseObject(CrushNode);
			});

			CrushNode->runAction(Sequence::create(delay, move, call, nullptr));
		}
	}
}

void CrushGrid::onCrushNodesSwaping(float dt)
{
	//捕捉两个正在交换的宝石的交换动作是否已经停止，如果没停止，返回继续捕捉
	if (m_pCrushNodeSelected->isSwaping() || m_pCrushNodeSwapped->isSwaping())
	{
		return;
	}
	//如果宝石交换动作执行完毕
	else
	{
		unschedule(schedule_selector(CrushGrid::onCrushNodesSwaping));  //停止捕捉

		log("swap over!");

		log("is it can crush?");

		//判断是否当前状态可以消除
		if (canCrush())
		{
			log("yes,crush!");

			

			//开始消除，开启消除状态捕捉函数(捕捉到消除完毕后，刷新布局)，这一轮消除正式开始
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
			//不能消除，交换回去，开启交换返回时的捕捉函数(捕捉到消除完毕后，开启触摸接听)
			SimpleAudioEngine::getInstance()->playEffect("swapback.mp3");
			swapCrushNodes(m_pCrushNodeSelected, m_pCrushNodeSwapped);
			schedule(schedule_selector(CrushGrid::onCrushNodesSwapingBack));
		}
	}
}

void CrushGrid::onCrushNodesSwapingBack(float dt)
{
	//捕捉两个正在交换的宝石的交换动作是否已经停止，如果没停止，返回继续捕捉
	if (m_pCrushNodeSelected->isSwaping() || m_pCrushNodeSwapped->isSwaping())
	{
		return;
	}
	else
	{
		unschedule(schedule_selector(CrushGrid::onCrushNodesSwapingBack)); //停止捕捉

		log("swap back!");

		m_pCrushNodeSelected = nullptr;

		_eventDispatcher->resumeEventListenersForTarget(this); //重新开始触摸接听
	}
}

void CrushGrid::onCrushNodesCrushing(float dt)
{
	//捕捉宝石消除状态，如果有宝石还在消除，那么继续捕捉
	for (auto t_CrushNode : m_VCrushNodeBox)
	{
		if ( t_CrushNode->isCrushing())
		{
			//log("crushing");
			return;
		}
		t_CrushNode->setBecomeFire(false);
	}

	//如果全部宝石已经消除完毕，停止捕捉函数
	unschedule(schedule_selector(CrushGrid::onCrushNodesCrushing));

	m_VCrushNodeBox.clear(); //清空消除宝石盒子

	log("crush over!");
	log("begin to refresh!");

	//刷新宝石阵列，并开启刷新状态捕捉函数（刷新一遍结束，重新判断新阵列是否可消除）
	refreshCrushNodesGrid();
	schedule(schedule_selector(CrushGrid::onCrushNodesRefreshing));
}

void CrushGrid::onCrushNodesRefreshing(float dt)
{
	//捕捉宝石刷新状态，如果新宝石盒子还有宝石（即新宝石还在刷新当中），那么继续捕捉
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

			//如果能消除，那么继续消除
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

			//判断是否为死图，如果是，则执行一段文字动画，提示即将更新地图
			if (isDeadMap())
			{
				log("cant crush any more, updating a new map!");

				auto winSize = Director::getInstance()->getWinSize();
				auto label = Label::createWithTTF("Cant Crush Any More, Change!", "fonts/Marker Felt.ttf", 24);
				label->setTextColor(Color4B::BLACK);
				label->setPosition(winSize.width / 2, winSize.height / 2);
				label->setOpacity(0);
				this->getParent()->addChild(label);

				//提示文字淡入淡出后，更新地图，再开启触摸监听
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
				//如果不是死图，那么就直接开启触摸监听，等待下一轮的交互操作
				_eventDispatcher->resumeEventListenersForTarget(this);
			}
		}
	}
}

void  CrushGrid::setGameOver()
{
	m_isOver  = true;
}