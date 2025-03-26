#pragma once
#include "spaceship.hpp"
#include "obstacle.hpp"
#include "alien.hpp"
#include "mysteryShip.hpp"

class Game{
    public:
        Game();
        ~Game();
        void Draw();
        void Update();
        void HandleInput();
        bool run;
        int lives;
        int score;
        int highscore;
        Music music;
    private:
        void DeleteInactiveLasers();
        Spaceship spaceship;
        vector<Obstacle> obstacles;
        vector<Obstacle> CreateObstacle();
        vector<Alien> aliens;
        vector<Alien> CreateAliens();
        void MoveAliens();
        int aliensDirection;
        void MoveDownAliens(int distance);
        vector<Laser> alienLasers;
        void AlienShootLaser();
        constexpr static float alienLaserShootInterval = 0.35;
        float timeLastAlienFired;
        MysteryShip mysteryShip;
        float mysteryShipSpawnInterval;
        float timeLastSpawnMysteryShip;
        void CheckForCollisions();
        void GameOver();
        void Reset();
        void InitGame();
        void CheckForHighscore();
        void SaveHighscoreToFile(int highscore);
        int LoadHighscoreFromFile();
        Sound ExplosionSound;
     

};