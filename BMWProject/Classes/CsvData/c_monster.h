#ifndef __MONSTER_MODEL_CSVDATA__
#define __MONSTER_MODEL_CSVDATA__
#include <iostream>
#include <vector>
#include <map>
#include "singletonDef.h"

#define c_monster_name "Monster.csv"

struct st_node_monster
{
	int ID;
	std::string Name;
	int Level;
	int HP;
	int MP;
	int HappyValue;

};

class c_monster
{
	DECLARE_SINGLETON(c_monster)
public:
	c_monster();
	void		Init();
	const st_node_monster&	GetNode( int i ) const;
	int		GetIndexByKey( int key );
	int		GetSize( ) { return hight - 1; };
	void	UnInit(){eleList.clear();}
private:
	int hight;
	std::vector<st_node_monster> 			eleList;
	std::map<int, int>			keyToIndex;//(key, index)
};



#endif