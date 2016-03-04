#include "c_monster.h"
#include "CsvUtil.h"

IMPLEMENT_SINGLETON(c_monster);

c_monster::c_monster()
{
	Init();
	hight = 0;
}
void c_monster::Init()
{
	cocos2d::Size size  = CsvUtil::getInstance()->getFileRowColNum(c_monster_name);
	hight = size.width;
	for (int i = 2; i< size.width ; i++)
	{
		int j = 0;
		st_node_monster node;
		node.ID = CsvUtil::getInstance()->getValue(i, j, c_monster_name).asInt();
		node.Name = CsvUtil::getInstance()->get(i, j + 1, c_monster_name);
		node.Level = CsvUtil::getInstance()->getValue(i, j + 2, c_monster_name).asInt();
		node.HP = CsvUtil::getInstance()->getValue(i, j + 3, c_monster_name).asInt();
		node.MP = CsvUtil::getInstance()->getValue(i, j + 4, c_monster_name).asInt();
		node.HappyValue = CsvUtil::getInstance()->getValue(i, j + 5, c_monster_name).asInt();
		eleList.push_back(node);
		keyToIndex[node.ID] = i - 2;
		
	}
}
const st_node_monster& c_monster::GetNode( int i ) const
{
	assert(i > hight -1);
	return eleList.at(i);
}


int	c_monster::GetIndexByKey( int key )
{
	std::map<int,int>::iterator iter = keyToIndex.find(key);
	assert(iter != keyToIndex.end());
	return iter->second;
}
