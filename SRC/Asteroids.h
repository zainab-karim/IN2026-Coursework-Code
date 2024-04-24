#ifndef __ASTEROIDS_H__
#define __ASTEROIDS_H__

#include "GameUtil.h"
#include "GameSession.h"
#include "IKeyboardListener.h"
#include "IGameWorldListener.h"
#include "IScoreListener.h"
#include "ScoreKeeper.h"
#include "Player.h"
#include "IPlayerListener.h"
#include <vector>
#include <algorithm>
#include <fstream>
#include <chrono>
#include <ctime>

class GameObject;
class Spaceship;
class GUILabel;

// Define the HighScore struct here
struct HighScoreEntry {
	std::string playerName;
	int score;

	HighScoreEntry(std::string n, int s) : playerName(n), score(s) {}

	// To sort in descending order
	bool operator < (const HighScoreEntry& hse) const {
		return score > hse.score;
	}
};
struct Button {
	float x, y, width, height;
	std::string text;
};

enum GameState {
	START_SCREEN,
	GAME_ACTIVE,
	GAME_OVER,
	DEMO_MODE,
	HIGH_SCORE_TABLE
};


class Asteroids : public GameSession, public IKeyboardListener, public IGameWorldListener, public IScoreListener, public IPlayerListener
{
public:
	int windowWidth;
	int windowHeight;

	Asteroids(int argc, char* argv[]);
	virtual ~Asteroids(void);
	void Update(int t);
	void OnMouseClick(int button, int state, int x, int y);
	virtual void Start(void);
	virtual void Stop(void);

	// Declaration of IKeyboardListener interface ////////////////////////////////

	void OnKeyPressed(uchar key, int x, int y);
	void InitialiseGame();
	void DrawStartScreen();
	void DrawHighScoresScreen();
	void DrawGameOverScreen();
	void OnKeyReleased(uchar key, int x, int y);
	void OnSpecialKeyPressed(int key, int x, int y);
	void OnSpecialKeyReleased(int key, int x, int y);

	// Declaration of IScoreListener interface //////////////////////////////////

	void OnScoreChanged(int score);


	// Declaration of the IPlayerLister interface //////////////////////////////

	void OnPlayerKilled(int lives_left);

	// Declaration of IGameWorldListener interface //////////////////////////////

	void OnWorldUpdated(GameWorld* world) {}
	void OnObjectAdded(GameWorld* world, shared_ptr<GameObject> object) {}
	void OnObjectRemoved(GameWorld* world, shared_ptr<GameObject> object);

	// Override the default implementation of ITimerListener ////////////////////
	void OnTimer(int value);

	std::string playerName;
	bool isEnteringName;
	bool demoModeActive = false;

private:
	GameState gameState;

	int playerScore;

	std::vector<shared_ptr<GUILabel>> mHighScoreEntries;

	std::string mPlayerName;
	std::string mScoreFile = "highscores.txt";
	std::vector<HighScoreEntry> scores;

	shared_ptr<Spaceship> mSpaceship;
	shared_ptr<GUILabel> mScoreLabel;
	shared_ptr<GUILabel> mLivesLabel;
	shared_ptr<GUILabel> mGameOverLabel;
	shared_ptr<GUILabel> mStartScreenLabel;
	shared_ptr<GUILabel> mStartScreenLabel2;
	shared_ptr<GUILabel> mStartScreenLabel3;
	shared_ptr<GUILabel> mNameEntryLabel;
	shared_ptr<GUILabel> mHighScoresLabel;
	shared_ptr<GUILabel> mDemoModeLabel;
	shared_ptr<GUILabel> mDemoLivesLabel;


	uint mLevel;
	uint mAsteroidCount;

	void ResetSpaceship();
	shared_ptr<GameObject> CreateSpaceship();
	void CreateGUI();
	void CreateAsteroids(const uint num_asteroids);
	void SaveScoresToFile(const std::string& playerName, int score);
	shared_ptr<GameObject> CreateExplosion();
	void InitialiseDemoMode();
	void DemoMode();

	const static uint SHOW_GAME_OVER = 0;
	const static uint START_NEXT_LEVEL = 1;
	const static uint CREATE_NEW_PLAYER = 2;
	const static uint SHOW_HIGH_SCORES = 3;
	const static uint SHOW_START_SCREEN = 4;


	ScoreKeeper mScoreKeeper;
	Player mPlayer;
	Button nextButton;
	Button highScoresButton;
	
};

#endif