#include "Asteroid.h"
#include "Asteroids.h"
#include "Animation.h"
#include "AnimationManager.h"
#include "GameUtil.h"
#include "GameWindow.h"
#include "GameWorld.h"
#include "GameDisplay.h"
#include "Spaceship.h"
#include "BoundingShape.h"
#include "BoundingSphere.h"
#include "GUILabel.h"
#include "Explosion.h"

// PUBLIC INSTANCE CONSTRUCTORS ///////////////////////////////////////////////

/** Constructor. Takes arguments from command line, just in case. */
Asteroids::Asteroids(int argc, char* argv[])
	: GameSession(argc, argv), playerName("")
{
	gameState = START_SCREEN;
	mLevel = 0;
	mAsteroidCount = 0;    
	isEnteringName = true;
}

void Asteroids::Update(int t) {
	// Example update method
	switch (gameState) {
	case START_SCREEN:
		DrawStartScreen()
			; break;
	case GAME_ACTIVE:
		if (demoModeActive) {
			DemoMode();
		}
		else {
			OnKeyPressed(' ', 0, 0); // Simulate shooting bullets
		}
		// Normal game update logic
		break;
	case GAME_OVER:
		DrawGameOverScreen();
		mDemoModeLabel->SetVisible(false);
		break;
	}
}


/** Destructor. */
Asteroids::~Asteroids(void)
{
}

// PUBLIC INSTANCE METHODS ////////////////////////////////////////////////////

/** Start an asteroids game. */
void Asteroids::Start()
{
	// Create a shared pointer for the Asteroids game object - DO NOT REMOVE
	shared_ptr<Asteroids> thisPtr = shared_ptr<Asteroids>(this);

	// Add this class as a listener of the game world
	mGameWorld->AddListener(thisPtr.get());

	// Add this as a listener to the world and the keyboard
	mGameWindow->AddKeyboardListener(thisPtr);

	// Add a score keeper to the game world
	mGameWorld->AddListener(&mScoreKeeper);

	// Add this class as a listener of the score keeper
	mScoreKeeper.AddListener(thisPtr);

	// Add a player (watcher) to the game world
	mGameWorld->AddListener(&mPlayer);

	// Add this class as a listener of the player
	mPlayer.AddListener(thisPtr);
	
	//Create the GUI
	CreateGUI();


	// Start the game
	GameSession::Start();
}

/** Stop the current game. */
void Asteroids::Stop()
{
	// Stop the game
	GameSession::Stop();
}

// PUBLIC INSTANCE METHODS IMPLEMENTING IKeyboardListener /////////////////////

void Asteroids::OnKeyPressed(uchar key, int x, int y)
{
	if (gameState == START_SCREEN && isEnteringName) {
		if (key == 13) { // Enter key
			if (!playerName.empty()) {
				isEnteringName = false;
				gameState = GAME_ACTIVE;
				mScoreLabel->SetVisible(true);
				mLivesLabel->SetVisible(true);
				mStartScreenLabel->SetVisible(false);
				mStartScreenLabel2->SetVisible(false);
				mStartScreenLabel3->SetVisible(false);
				mNameEntryLabel->SetVisible(false);
				mDemoModeLabel->SetVisible(false);
				InitialiseGame();
			}
		}
		else if (key == 8 && !playerName.empty()) { // Backspace key
			playerName.pop_back();
		}
		else if (isalnum(key) && playerName.length() < 20) { // Alphanumeric keys
			playerName += key;
		}
		else if (key == 9) { // Tab key
			demoModeActive = !demoModeActive; // Toggle demo mode
			gameState = DEMO_MODE; // Change to GAME_ACTIVE to avoid redrawing start screen
			isEnteringName = false;
			mScoreLabel->SetVisible(true);
			mLivesLabel->SetVisible(false);
			mDemoLivesLabel->SetVisible(true);
			mDemoModeLabel->SetVisible(true);
			mStartScreenLabel->SetVisible(false);
			mStartScreenLabel2->SetVisible(false);
			mStartScreenLabel3->SetVisible(false);
			mNameEntryLabel->SetVisible(false);
			DemoMode();
		}
		mNameEntryLabel->SetText("Enter Name: " + playerName); // Update the label text
		DrawStartScreen(); // Redraw start screen with updated name
	}
	else if (gameState == GAME_ACTIVE || gameState == DEMO_MODE) {
		// Game active key controls
		switch (key) {
		case ' ':
			std::cout << "Spacebar pressed" << std::endl; // Add a print statement
			mSpaceship->Shoot();
			break;
		}
	}
}

	void Asteroids::InitialiseGame() 
	{

		// Create an ambient light to show sprite textures
		GLfloat ambient_light[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		GLfloat diffuse_light[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		glLightfv(GL_LIGHT0, GL_AMBIENT, ambient_light);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse_light);
		glEnable(GL_LIGHT0);

		Animation* explosion_anim = AnimationManager::GetInstance().CreateAnimationFromFile("explosion", 64, 1024, 64, 64, "explosion_fs.png");
		Animation* asteroid1_anim = AnimationManager::GetInstance().CreateAnimationFromFile("asteroid1", 128, 8192, 128, 128, "asteroid1_fs.png");
		Animation* spaceship_anim = AnimationManager::GetInstance().CreateAnimationFromFile("spaceship", 128, 128, 128, 128, "spaceship_fs.png");

		// Create a spaceship and add it to the world
		mGameWorld->AddObject(CreateSpaceship());
		// Create some asteroids and add them to the world
		CreateAsteroids(10);

			
		// Reset or set initial game states here
		CreateSpaceship();
		CreateAsteroids(10); // for example, start with 10 asteroids
		// Possibly reset score and lives
		//mScoreKeeper.ResetScore();
		//mPlayer.ResetLives();

	}
	
	void Asteroids::DrawStartScreen()
	{
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		gluOrtho2D(0.0, windowWidth, 0.0, windowHeight);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		glDisable(GL_LIGHTING);

		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
		glEnable(GL_LIGHTING);
	}

	void Asteroids::InitialiseDemoMode() {
		// Reset demoLives to 3
		mDemoLivesLabel->SetText("Lives: 3");
		// Reset score to 0
		mScoreLabel->SetText("Score: 0");

		// Create an ambient light to show sprite textures
		GLfloat ambient_light[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		GLfloat diffuse_light[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		glLightfv(GL_LIGHT0, GL_AMBIENT, ambient_light);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse_light);
		glEnable(GL_LIGHT0);

		Animation* explosion_anim = AnimationManager::GetInstance().CreateAnimationFromFile("explosion", 64, 1024, 64, 64, "explosion_fs.png");
		Animation* asteroid1_anim = AnimationManager::GetInstance().CreateAnimationFromFile("asteroid1", 128, 8192, 128, 128, "asteroid1_fs.png");
		Animation* spaceship_anim = AnimationManager::GetInstance().CreateAnimationFromFile("spaceship", 128, 128, 128, 128, "spaceship_fs.png");

		// Create a spaceship and add it to the world
		mGameWorld->AddObject(CreateSpaceship());
		// Create some asteroids and add them to the world
		CreateAsteroids(10);


		// Reset or set initial game states here
		CreateSpaceship();
		CreateAsteroids(10); // for example, start with 10 asteroids
		// Possibly reset score and lives
		//mScoreKeeper.ResetScore();
		//mPlayer.ResetLives();
	}


	
void Asteroids::DemoMode() {
    InitialiseDemoMode(); // Setup for demo mode

    // Simulate key presses to control the spaceship
    // For example, simulate pressing the spacebar to shoot bullets
	OnKeyPressed(' ', 0, 0); // Simulate shooting bullets

    // Simulate pressing the up arrow key to apply forward thrust
    OnSpecialKeyPressed(GLUT_KEY_UP, 0, 0); // Simulate applying forward thrust

    // Simulate pressing the left arrow key to rotate anti-clockwise
    OnSpecialKeyPressed(GLUT_KEY_LEFT, 0, 0); // Simulate rotating anti-clockwise

    // Simulate pressing the right arrow key to rotate clockwise
    OnSpecialKeyPressed(GLUT_KEY_RIGHT, 0, 0); // Simulate rotating clockwise
}


void Asteroids::DrawGameOverScreen()
{
	// Check if the player already exists in the high score table
	bool playerExists = false;
	int existingPlayerIndex = -1; // Track the index of the existing player if found
	for (size_t i = 0; i < highScores.size(); ++i) {
		if (highScores[i].playerName == playerName) {
			playerExists = true;
			existingPlayerIndex = i;
			break;
		}
	}

	if (playerExists) {
		// If the player exists, compare scores
		if (playerScore > highScores[existingPlayerIndex].score) {
			// If the current score is higher, update the existing score
			highScores[existingPlayerIndex].score = playerScore;
			// You may also update the timestamp here if needed
		}
	}
	else {
		// If the player does not already exist, add them to the high score table
		HighScore newScore{ playerName, playerScore };
		highScores.push_back(newScore);
	}

	// Sort high scores by score and timestamp
	std::sort(highScores.begin(), highScores.end(), [](const HighScore& a, const HighScore& b) {
		if (a.score == b.score) {
			// If scores are equal, sort by timestamp in descending order (latest first)
			return a.timestamp > b.timestamp;
		}
		else {
			// Otherwise, sort by score in descending order
			return a.score > b.score;
		}
		});

	// Keep only the top 5 scores
	if (highScores.size() > 5) {
		highScores.resize(5);
	}


	// Clear the screen with black background
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Set up orthographic projection for 2D text rendering
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0.0, windowWidth, 0.0, windowHeight);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Disable lighting for text rendering
	glDisable(GL_LIGHTING);

	// Set text color to white
	glColor3f(1.0f, 1.0f, 1.0f);

	// Restore the previous projection matrix
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);

	// Re-enable lighting if it was enabled before
	glEnable(GL_LIGHTING);

	// Show high scores
	mGameOverLabel->SetVisible(true);
	SetTimer(2000, SHOW_HIGH_SCORES);

	// Remove the spaceship from the game world if it exists
	if (mSpaceship) {
		mGameWorld->RemoveObject(mSpaceship);
	}
}


	void Asteroids::DrawHighScoresScreen()
	{

    LoadHighScoresFromFile();

    HighScore newScore{ playerName, playerScore };
    highScores.push_back(newScore);
    // Sort high scores by score and timestamp
    std::sort(highScores.begin(), highScores.end(), [](const HighScore& a, const HighScore& b) {
        if (a.score == b.score) {
            // If scores are equal, sort by timestamp in descending order (latest first)
            return a.timestamp > b.timestamp;
        }
        else {
            // Otherwise, sort by score in descending order
            return a.score > b.score;
        }
    });

    if (highScores.size() > 5) {
        highScores.resize(5);
    }

		// Clear the screen with black background
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Set up orthographic projection for 2D text rendering
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		gluOrtho2D(0.0, windowWidth, 0.0, windowHeight);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		// Disable lighting for text rendering
		glDisable(GL_LIGHTING);

		// Set text color to white
		glColor3f(1.0f, 1.0f, 1.0f);

		// Render the high scores
		DisplayHighScores();

		// Restore the previous projection matrix
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);

		// Re-enable lighting if it was enabled before
		glEnable(GL_LIGHTING);
	}
	
	void Asteroids::DisplayHighScores()
	{
		LoadHighScoresFromFile();  // Ensure the high scores are loaded
		mHighScoresLabel->SetText("High Scores:");  // Set the main high scores label

		int i = 0;
		for (auto& score : highScores) {
			if (i < mHighScoreEntries.size()) {
				std::ostringstream ss;
				ss << (i + 1) << ". " << score.playerName << " - " << score.score;
				mHighScoreEntries[i]->SetText(ss.str());
				mHighScoreEntries[i]->SetVisible(true);
			}
			else {
				break;
			}
			++i;
		}

		// Hide any unused score labels
		for (; i < mHighScoreEntries.size(); ++i) {
			mHighScoreEntries[i]->SetVisible(false);
		}
	}
	
	void Asteroids::LoadHighScoresFromFile() {
		std::ifstream file(highScoresFileName);
		std::string line;

		// Clear existing high scores
		highScores.clear();

		// Load scores from file
		while (getline(file, line)) {
			std::istringstream iss(line);
			std::string name;
			int score;
			if (iss >> name >> score) {
				HighScore hs{ name, score };
				highScores.push_back(hs);
			}
		}
		file.close();

		// Sort high scores by score in descending order
		std::sort(highScores.begin(), highScores.end(), [](const HighScore& a, const HighScore& b) {
			return a.score > b.score;
			});
	}



	void Asteroids::SaveHighScoresToFile() {
		std::ofstream file(highScoresFileName, std::ios_base::app); // Open file in append mode
		if (file.is_open()) {
			std::cout << "Writing to file: " << playerName << " " << playerScore << std::endl;  // Debugging output
			file << playerName << " " << playerScore << "\n";  // Make sure to write a newline character
			file.close(); // Close the file after writing
		}
		else {
			std::cerr << "Unable to open file " << highScoresFileName << " for writing." << std::endl;  // Error message if file cannot be opened
		}
	}


	void Asteroids::OnKeyReleased(uchar key, int x, int y) {}

	void Asteroids::OnSpecialKeyPressed(int key, int x, int y)
	{
		switch (key)
		{
			// If up arrow key is pressed start applying forward thrust
		case GLUT_KEY_UP: mSpaceship->Thrust(10); break;
			// If left arrow key is pressed start rotating anti-clockwise
		case GLUT_KEY_LEFT: mSpaceship->Rotate(90); break;
			// If right arrow key is pressed start rotating clockwise
		case GLUT_KEY_RIGHT: mSpaceship->Rotate(-90); break;
			// Default case - do nothing
		default: break;
		}
	}

	void Asteroids::OnSpecialKeyReleased(int key, int x, int y)
	{
		switch (key)
		{
			// If up arrow key is released stop applying forward thrust
		case GLUT_KEY_UP: mSpaceship->Thrust(0); break;
			// If left arrow key is released stop rotating
		case GLUT_KEY_LEFT: mSpaceship->Rotate(0); break;
			// If right arrow key is released stop rotating
		case GLUT_KEY_RIGHT: mSpaceship->Rotate(0); break;
			// Default case - do nothing
		default: break;
		}
	}


	// PUBLIC INSTANCE METHODS IMPLEMENTING IGameWorldListener ////////////////////

	void Asteroids::OnObjectRemoved(GameWorld * world, shared_ptr<GameObject> object)
	{
		if (object->GetType() == GameObjectType("Asteroid"))
		{
			shared_ptr<GameObject> explosion = CreateExplosion();
			explosion->SetPosition(object->GetPosition());
			explosion->SetRotation(object->GetRotation());
			mGameWorld->AddObject(explosion);
			mAsteroidCount--;
			if (mAsteroidCount <= 0)
			{
				SetTimer(500, START_NEXT_LEVEL);
			}
		}
	}

	// PUBLIC INSTANCE METHODS IMPLEMENTING ITimerListener ////////////////////////

	void Asteroids::OnTimer(int value)
	{
		if (value == CREATE_NEW_PLAYER)
		{
			mSpaceship->Reset();
			mGameWorld->AddObject(mSpaceship);
		}

		if (value == START_NEXT_LEVEL)
		{
			mLevel++;
			int num_asteroids = 10 + 2 * mLevel;
			CreateAsteroids(num_asteroids);
		}

		if (value == SHOW_GAME_OVER)
		{
			mGameOverLabel->SetVisible(true);
		}

		if (value == SHOW_HIGH_SCORES)
		{
			mHighScoresLabel->SetVisible(true);
			mGameOverLabel->SetVisible(false);
			DrawHighScoresScreen();
		}
	}

	// PROTECTED INSTANCE METHODS /////////////////////////////////////////////////
	shared_ptr<GameObject> Asteroids::CreateSpaceship()
	{
		// Create a raw pointer to a spaceship that can be converted to
		// shared_ptrs of different types because GameWorld implements IRefCount
		mSpaceship = make_shared<Spaceship>();
		mSpaceship->SetBoundingShape(make_shared<BoundingSphere>(mSpaceship->GetThisPtr(), 4.0f));
		shared_ptr<Shape> bullet_shape = make_shared<Shape>("bullet.shape");
		mSpaceship->SetBulletShape(bullet_shape);
		Animation* anim_ptr = AnimationManager::GetInstance().GetAnimationByName("spaceship");
		shared_ptr<Sprite> spaceship_sprite =
			make_shared<Sprite>(anim_ptr->GetWidth(), anim_ptr->GetHeight(), anim_ptr);
		mSpaceship->SetSprite(spaceship_sprite);
		mSpaceship->SetScale(0.1f);
		// Reset spaceship back to centre of the world
		mSpaceship->Reset();
		// Return the spaceship so it can be added to the world
		return mSpaceship;

	}

	void Asteroids::CreateAsteroids(const uint num_asteroids)
	{
		mAsteroidCount = num_asteroids;
		for (uint i = 0; i < num_asteroids; i++)
		{
			Animation* anim_ptr = AnimationManager::GetInstance().GetAnimationByName("asteroid1");
			shared_ptr<Sprite> asteroid_sprite
				= make_shared<Sprite>(anim_ptr->GetWidth(), anim_ptr->GetHeight(), anim_ptr);
			asteroid_sprite->SetLoopAnimation(true);
			shared_ptr<GameObject> asteroid = make_shared<Asteroid>();
			asteroid->SetBoundingShape(make_shared<BoundingSphere>(asteroid->GetThisPtr(), 10.0f));
			asteroid->SetSprite(asteroid_sprite);
			asteroid->SetScale(0.2f);
			mGameWorld->AddObject(asteroid);
		}
	}

	void Asteroids::CreateGUI()
	{
		// Add a (transparent) border around the edge of the game display
		mGameDisplay->GetContainer()->SetBorder(GLVector2i(10, 10));
		// Create a new GUILabel and wrap it up in a shared_ptr
		mScoreLabel = make_shared<GUILabel>("Score: 0");
		// Set the vertical alignment of the label to GUI_VALIGN_TOP
		mScoreLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_TOP);
		mScoreLabel->SetVisible(gameState == GAME_ACTIVE);

		// Add the GUILabel to the GUIComponent  
		shared_ptr<GUIComponent> score_component
			= static_pointer_cast<GUIComponent>(mScoreLabel);
		mGameDisplay->GetContainer()->AddComponent(score_component, GLVector2f(0.0f, 1.0f));

		// Create a new GUILabel and wrap it up in a shared_ptr
		mLivesLabel = make_shared<GUILabel>("Lives: 3");
		// Set the vertical alignment of the label to GUI_VALIGN_BOTTOM
		mLivesLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_BOTTOM);
		// Add the GUILabel to the GUIComponent  
		shared_ptr<GUIComponent> lives_component = static_pointer_cast<GUIComponent>(mLivesLabel);
		mGameDisplay->GetContainer()->AddComponent(lives_component, GLVector2f(0.0f, 0.0f));
		mLivesLabel->SetVisible(gameState == GAME_ACTIVE);

		// Create a new GUILabel and wrap it up in a shared_ptr
		mDemoLivesLabel = make_shared<GUILabel>("Lives: 3");
		// Set the vertical alignment of the label to GUI_VALIGN_BOTTOM
		mDemoLivesLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_BOTTOM);
		// Add the GUILabel to the GUIComponent  
		shared_ptr<GUIComponent> demolives_component = static_pointer_cast<GUIComponent>(mDemoLivesLabel);
		mGameDisplay->GetContainer()->AddComponent(demolives_component, GLVector2f(0.0f, 0.0f));
		mDemoLivesLabel->SetVisible(gameState == DEMO_MODE);


		// Create a new GUILabel and wrap it up in a shared_ptr
		mGameOverLabel = shared_ptr<GUILabel>(new GUILabel("GAME OVER"));
		// Set the horizontal alignment of the label to GUI_HALIGN_CENTER
		mGameOverLabel->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_CENTER);
		// Set the vertical alignment of the label to GUI_VALIGN_MIDDLE
		mGameOverLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_MIDDLE);
		// Set the visibility of the label to false (hidden)
		mGameOverLabel->SetVisible(gameState == GAME_OVER);
		// Add the GUILabel to the GUIContainer  
		shared_ptr<GUIComponent> game_over_component
			= static_pointer_cast<GUIComponent>(mGameOverLabel);
		mGameDisplay->GetContainer()->AddComponent(game_over_component, GLVector2f(0.5f, 0.5f));
		
		mStartScreenLabel = make_shared<GUILabel>("Press Enter to Start the Game");
		mStartScreenLabel->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_CENTER);
		mStartScreenLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_MIDDLE);
		mStartScreenLabel->SetVisible(gameState == START_SCREEN);
		mGameDisplay->GetContainer()->AddComponent(static_pointer_cast<GUIComponent>(mStartScreenLabel), GLVector2f(0.5f, 0.85f));

		mStartScreenLabel2 = make_shared<GUILabel>("Press Tab to Start Demo Mode");
		mStartScreenLabel2->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_CENTER);
		mStartScreenLabel2->SetVerticalAlignment(GUIComponent::GUI_VALIGN_MIDDLE);
		mStartScreenLabel2->SetVisible(gameState == START_SCREEN);
		mGameDisplay->GetContainer()->AddComponent(static_pointer_cast<GUIComponent>(mStartScreenLabel2), GLVector2f(0.5f, 0.8f));

		mStartScreenLabel3 = make_shared<GUILabel>("Press Escape To Leave");
		mStartScreenLabel3->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_CENTER);
		mStartScreenLabel3->SetVerticalAlignment(GUIComponent::GUI_VALIGN_MIDDLE);
		mStartScreenLabel3->SetVisible(gameState == START_SCREEN);
		mGameDisplay->GetContainer()->AddComponent(static_pointer_cast<GUIComponent>(mStartScreenLabel3), GLVector2f(0.5f, 0.75f));


		mNameEntryLabel = make_shared<GUILabel>("Enter Name: " + playerName);
		mNameEntryLabel->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_CENTER);
		mNameEntryLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_MIDDLE);
		mNameEntryLabel->SetVisible(gameState == START_SCREEN);
		mGameDisplay->GetContainer()->AddComponent(static_pointer_cast<GUIComponent>(mNameEntryLabel), GLVector2f(0.5f, 0.4f));

		// Create a new GUILabel for "DEMO MODE"
		mDemoModeLabel = make_shared<GUILabel>("DEMO MODE");
		// Set the horizontal alignment of the label to GUI_HALIGN_CENTER
		mDemoModeLabel->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_CENTER);
		// Set the vertical alignment of the label to GUI_VALIGN_MIDDLE
		mDemoModeLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_MIDDLE);
		// Set the text color to red
		mDemoModeLabel->SetColor(GLVector3f(1.0f, 0.0f, 0.0f));
		// Set the visibility of the label to false (hidden)
		mDemoModeLabel->SetVisible(gameState == DEMO_MODE);
		// Add the GUILabel to the GUIContainer
		shared_ptr<GUIComponent> demo_mode_component = static_pointer_cast<GUIComponent>(mDemoModeLabel);
		mGameDisplay->GetContainer()->AddComponent(demo_mode_component, GLVector2f(0.5f, 0.9f));

		mHighScoresLabel = make_shared<GUILabel>("High Scores:");
		mHighScoresLabel->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_CENTER);
		mHighScoresLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_TOP);
		mHighScoresLabel->SetVisible(gameState == GAME_OVER || gameState == START_SCREEN);
		mGameDisplay->GetContainer()->AddComponent(static_pointer_cast<GUIComponent>(mHighScoresLabel), GLVector2f(0.5f, 0.80f));
		mHighScoresLabel->SetVisible(false);

		
		for (int i = 0; i < 5; ++i) {
			shared_ptr<GUILabel> label = make_shared<GUILabel>("");
			label->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_CENTER);
			label->SetVerticalAlignment(GUIComponent::GUI_VALIGN_TOP);
			label->SetVisible(gameState == GAME_OVER || gameState == START_SCREEN);
			mHighScoreEntries.push_back(label);
			mGameDisplay->GetContainer()->AddComponent(static_pointer_cast<GUIComponent>(label), GLVector2f(0.5f, 0.75f - i * 0.05f));
		}
	}


	void Asteroids::OnScoreChanged(int score)
	{
		playerScore = score;
		// Format the score message using an string-based stream
		std::ostringstream msg_stream;
		msg_stream << "Score: " << playerScore;
		// Get the score message as a string
		std::string score_msg = msg_stream.str();
		mScoreLabel->SetText(score_msg);
	}



	void Asteroids::OnPlayerKilled(int lives_left)
	{
		// Check if the game is already in GAME_OVER state
		if (gameState == GAME_OVER) {
			// Do nothing if the game is already over
			return;
		}

		shared_ptr<GameObject> explosion = CreateExplosion();
		explosion->SetPosition(mSpaceship->GetPosition());
		explosion->SetRotation(mSpaceship->GetRotation());
		mGameWorld->AddObject(explosion);

		// Format the lives left message using a string-based stream
		std::ostringstream msg_stream;
		msg_stream << "Lives: " << lives_left;
		// Get the lives left message as a string
		std::string lives_msg = msg_stream.str();
		mLivesLabel->SetText(lives_msg);

		// Update demo lives label only if demo mode is active
		if (demoModeActive) {
			mDemoLivesLabel->SetText(lives_msg);
		}
		std::cout << "Player Killed. Lives left: " << lives_left << std::endl;  // Debug output
		if ((gameState == GAME_ACTIVE || gameState == DEMO_MODE) && lives_left > 0) {
			SetTimer(1000, CREATE_NEW_PLAYER);
		}

		if ((gameState == GAME_ACTIVE) && lives_left <= 0)
		{
			std::cout << "Game Over. Final score: " << playerScore << std::endl;  // Debug output
			gameState = GAME_OVER;
			mGameWorld->ClearObjects();
			SaveHighScoresToFile();
			DrawGameOverScreen();
		}
		else if (gameState == DEMO_MODE && lives_left <= 0)
		{
			mGameWorld->ClearObjects();
			mDemoModeLabel->SetVisible(false);
			mScoreLabel->SetVisible(false);
			mDemoLivesLabel->SetVisible(false);
			isEnteringName = true;
			demoModeActive = false;
			gameState = START_SCREEN;
			CreateGUI();
			DrawStartScreen();
		}
		else
		{
			// Additional logic here
		}
	}




	shared_ptr<GameObject> Asteroids::CreateExplosion()
	{
		Animation* anim_ptr = AnimationManager::GetInstance().GetAnimationByName("explosion");
		shared_ptr<Sprite> explosion_sprite =
			make_shared<Sprite>(anim_ptr->GetWidth(), anim_ptr->GetHeight(), anim_ptr);
		explosion_sprite->SetLoopAnimation(false);
		shared_ptr<GameObject> explosion = make_shared<Explosion>();
		explosion->SetSprite(explosion_sprite);
		explosion->Reset();
		return explosion;
	}
