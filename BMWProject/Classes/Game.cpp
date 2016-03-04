#include "Game.h"
#include "Util.h"

static Game* s_InstanceGame = NULL;

Game* Game::Instance()
{
	if (s_InstanceGame == NULL)
	{
		s_InstanceGame = new Game();
	}

	return s_InstanceGame;
}

Game::Game()
{
	m_mapScoreData[GAME_TYPE_CRUSH] = ScoreData(1000);
	m_mapScoreData[GAME_TYPE_TRANK] = ScoreData();
	m_mapScoreData[GAME_TYPE_SHOOT] = ScoreData();
	m_mapGameResult[GAME_TYPE_CRUSH] = GAME_TYPE_RESULT_NONE;
	m_mapGameResult[GAME_TYPE_TRANK] = GAME_TYPE_RESULT_NONE;
	m_mapGameResult[GAME_TYPE_SHOOT] = GAME_TYPE_RESULT_NONE;
}

int Game::getTarScoreByGameType(E_GAME_TYPE ptype)
{

	return m_mapScoreData[ptype].m_nTarScore;
}

int Game::getCurrentScoreByGameType(E_GAME_TYPE ptype)
{
	return m_mapScoreData[ptype].m_nCurrentScore;
}
void Game::setCurrentScoreByGameType(E_GAME_TYPE ptype, int pScore)
{
	m_mapScoreData[ptype].m_nCurrentScore = pScore ;
}

void Game::addCurrentScoreByGameType(E_GAME_TYPE ptype,int pScore)
{
	m_mapScoreData[ptype].m_nCurrentScore += pScore ;
}

bool Game::getGameResult(E_GAME_TYPE pType)
{
	m_mapScoreData[pType].m_isWin = m_mapScoreData[pType].m_nCurrentScore >= m_mapScoreData[pType].m_nTarScore;
	m_mapGameResult[pType] = m_mapScoreData[pType].m_isWin ? GAME_TYPE_RESULT_WIN : GAME_TYPE_RESULT_NONE;
	return m_mapScoreData[pType].m_isWin;
}