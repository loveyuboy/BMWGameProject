#ifndef __BMWPROJECT__GAME__
#define __BMWPROJECT__GAME__

#include "CsvData/singletonDef.h"
#include <map>
enum E_GAME_TYPE
{
	GAME_TYPE_CRUSH,
	GAME_TYPE_TRANK,
	GAME_TYPE_SHOOT
};

enum E_GAME_RESULT_TYPE
{
	GAME_TYPE_RESULT_NONE,
	GAME_TYPE_RESULT_WIN,
	GAME_TYPE_RESULT_LOST
};

struct ScoreData
{
	bool m_isWin;
	int m_nTarScore;
	int m_nCurrentScore;
	ScoreData()
	{
		m_isWin = false;
		m_nTarScore = 0;
		m_nCurrentScore = 0;
	}
	ScoreData(int pHighScore)
	{
		m_isWin = false;
		m_nTarScore = pHighScore;
		m_nCurrentScore = 0;
	}

	void clean()
	{
		m_nCurrentScore = 0;
	}
};

class Game
{
public:
	Game();
	~Game();
	static Game* Instance();
	int getCurrentScoreByGameType(E_GAME_TYPE ptyep);
	int getTarScoreByGameType(E_GAME_TYPE ptype);
	void setCurrentScoreByGameType(E_GAME_TYPE pType,int pScore);
	void addCurrentScoreByGameType(E_GAME_TYPE pType,int pScore);
	bool getGameResult(E_GAME_TYPE pType);
private:
	std::map<E_GAME_TYPE,ScoreData> m_mapScoreData;
	std::map<E_GAME_TYPE,E_GAME_RESULT_TYPE> m_mapGameResult;
};


#endif