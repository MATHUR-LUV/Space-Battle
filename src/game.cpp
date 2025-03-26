#include "game.hpp"
#include <iostream>
#include <fstream>

Game::Game()
{
    music = LoadMusicStream("Sounds/music.ogg");
    ExplosionSound = LoadSound("Sounds/explosion.ogg");
    PlayMusicStream(music);
    InitGame();
}

Game::~Game()
{
    Alien::UnloadImages();
    UnloadMusicStream(music);
    UnloadSound(ExplosionSound);
}
void Game::Update()
{

    if (run)
    {
        double currentTime = GetTime();
        if (currentTime - timeLastSpawnMysteryShip > mysteryShipSpawnInterval)
        {
            mysteryShip.Spawn();
            timeLastSpawnMysteryShip = currentTime;
            mysteryShipSpawnInterval = GetRandomValue(10, 20);
        }
        for (auto &laser : spaceship.lasers)
        {
            laser.update();
        }

        MoveAliens();
        AlienShootLaser();
        for (auto &laser : alienLasers)
        {
            laser.update();
        }
        DeleteInactiveLasers();
        mysteryShip.Update();

        CheckForCollisions();
    }
    else
    {
        if(IsKeyDown(KEY_ENTER)){
            Reset();
            InitGame();
        }
    }
}
void Game::Draw()
{
    spaceship.Draw();
    for (auto &laser : spaceship.lasers)
    {
        laser.Draw();
    }
    for (auto &obstacle : obstacles)
    {
        obstacle.Draw();
    }
    for (auto &alien : aliens)
    {
        alien.Draw();
    }
    for (auto &laser : alienLasers)
    {
        laser.Draw();
    }
    mysteryShip.Draw();
}

void Game::HandleInput()
{
    if (run)
    {
        if (IsKeyDown(KEY_A))
        {
            spaceship.MoveLeft();
        }
        else if (IsKeyDown(KEY_D))
        {
            spaceship.MoveRight();
        }
        else if (IsKeyDown(KEY_LEFT))
        {
            spaceship.MoveLeft();
        }
        else if (IsKeyDown(KEY_RIGHT))
        {
            spaceship.MoveRight();
        }
        else if (IsKeyDown(KEY_SPACE))
        {
            spaceship.FireLaser();
        }
    }
}

void Game::DeleteInactiveLasers()
{
    for (auto it = spaceship.lasers.begin(); it != spaceship.lasers.end();)
    {
        if (!it->active)
        {
            it = spaceship.lasers.erase(it);
        }
        else
        {
            ++it;
        }
    }

    for (auto it = alienLasers.begin(); it != alienLasers.end();)
    {
        if (!it->active)
        {
            it = alienLasers.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

vector<Obstacle> Game::CreateObstacle()
{
    int obstacleWidth = Obstacle::grid[0].size() * 3;
    float gap = (GetScreenWidth() - (4 * obstacleWidth)) / 5;

    for (int i = 0; i < 4; i++)
    {
        float offsetX = (i + 1) * gap + i * obstacleWidth;

        obstacles.push_back(Obstacle({offsetX, float(GetScreenHeight() - 200)}));
    }

    return obstacles;
}

vector<Alien> Game::CreateAliens()
{
    vector<Alien> aliens;
    for (int row = 0; row < 5; row++)
    {
        for (int col = 0; col < 11; col++)
        {

            int alienType;
            if (row == 0)
            {
                alienType = 3;
            }
            else if (row == 1 || row == 2)
            {
                alienType = 1;
            }
            else
            {
                alienType = 2;
            }

            float x = 75 + col * 55;
            float y = 110 + row * 55;

            aliens.push_back(Alien(alienType, {x, y}));
        }
    }
    return aliens;
}

void Game::MoveAliens() {
    for(auto& alien: aliens) {
        if(alien.position.x + alien.alienImages[alien.type - 1].width > GetScreenWidth() - 25) {
            aliensDirection = -1;
            MoveDownAliens(4);
        }
        if(alien.position.x < 25) {
            aliensDirection = 1;
            MoveDownAliens(4);
        }

        alien.Update(aliensDirection);
    }
}

void Game::MoveDownAliens(int distance)
{
    for (auto &alien : aliens)
    {
        alien.position.y += distance;
    }
}

void Game::AlienShootLaser()
{
    double currentTime = GetTime();
    if (currentTime - timeLastAlienFired >= alienLaserShootInterval && !aliens.empty())
    {
        int randomIndex = GetRandomValue(0, aliens.size() - 1);
        Alien &alien = aliens[randomIndex];
        alienLasers.push_back(Laser({alien.position.x + alien.alienImages[alien.type - 1].width / 2,
                                     alien.position.y + alien.alienImages[alien.type - 1].height},
                                    6));
        timeLastAlienFired = GetTime();
    }
}

void Game::CheckForCollisions()
{
    // Spaceship lasers
    for (auto &laser : spaceship.lasers)
    {
        auto it = aliens.begin();
        while (it != aliens.end())
        {
            if (CheckCollisionRecs(laser.getRect(), it->getRect()))
            {   
                PlaySound(ExplosionSound);
                if(it->type == 2){
                    score += 100;
                }else if(it -> type == 1){
                    score += 200;
                }else if(it -> type == 3){
                    score += 300;
                }

                CheckForHighscore();
                it = aliens.erase(it);
                laser.active = false;
            }
            else
            {
                ++it;
            }
        }

        for (auto &obstacle : obstacles)
        {
            auto it = obstacle.blocks.begin();
            while (it != obstacle.blocks.end())
            {
                if (CheckCollisionRecs(laser.getRect(), it->getRect()))
                {
                    it = obstacle.blocks.erase(it);
                    laser.active = false;
                }
                else
                {
                    ++it;
                }
            }
        }

        if (CheckCollisionRecs(mysteryShip.getRect(), laser.getRect()))
        {
            score += 500;
            CheckForHighscore();
            mysteryShip.alive = false;
            laser.active = false;
            PlaySound(ExplosionSound);
        }
    }

    // Alien lasers
    for (auto &laser : alienLasers)
    {
        if (CheckCollisionRecs(laser.getRect(), spaceship.getRect()))
        {
            laser.active = false;
            lives--;
            PlaySound(ExplosionSound);
            
            if (lives == 0)
            {
                GameOver();
            }
        }

        for (auto &obstacle : obstacles)
        {
            auto it = obstacle.blocks.begin();
            while (it != obstacle.blocks.end())
            {
                if (CheckCollisionRecs(laser.getRect(), it->getRect()))
                {
                    it = obstacle.blocks.erase(it);
                    laser.active = false;
                }
                else
                {
                    ++it;
                }
            }
        }
    }

    // Alien Collision with obstacles
    for (auto &alien : aliens)
    {
        for (auto &obstacle : obstacles)
        {
            auto it = obstacle.blocks.begin();
            while (it != obstacle.blocks.end())
            {
                if (CheckCollisionRecs(it->getRect(), alien.getRect()))
                {
                    it = obstacle.blocks.erase(it);
                }
                else
                {
                    it++;
                }
            }
        }
        if (CheckCollisionRecs(alien.getRect(), spaceship.getRect()))
        {
            PlaySound(ExplosionSound);
            GameOver();
        }
    }
}

void Game::GameOver()
{
    run = false;
}
void Game::Reset() {
    spaceship.Reset();
    aliens.clear();
    alienLasers.clear();
    obstacles.clear();
}

void Game::InitGame(){
    obstacles = CreateObstacle();
    aliens = CreateAliens();
    aliensDirection = 1;
    timeLastAlienFired = 0.0;
    timeLastSpawnMysteryShip = 0.0;
    lives = 3;
    run = true;
    score = 0;
    highscore = LoadHighscoreFromFile();
    mysteryShipSpawnInterval = GetRandomValue(10, 20);
}

void Game::CheckForHighscore(){
    if(score > highscore){
        highscore = score;
        SaveHighscoreToFile(highscore);
    }
}

void Game::SaveHighscoreToFile(int highscore){

    ofstream highscoreFile("highscore.txt");
    if(highscoreFile.is_open()){
        highscoreFile << highscore;
        highscoreFile.close();
    }else{
        cerr<<"Failed to save highscore to file !!! "<<endl;
    }
}

int Game::LoadHighscoreFromFile(){
    int loadededHighscore = 0;
    ifstream highScoreFile("highscore.txt");
    if(highScoreFile.is_open()){
        highScoreFile >> loadededHighscore;
        highScoreFile.close();
    }else{
        cerr<<"Failed to save highscore to file !!! "<<endl;
    }
    return loadededHighscore;
}