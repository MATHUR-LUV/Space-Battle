#pragma once
#include <raylib.h>
#include "laser.hpp"
#include <vector>
using namespace std;


class Spaceship{
    public:
        Spaceship();
        ~Spaceship();
        void Draw();
        void MoveLeft();
        void MoveRight();
        void FireLaser();
        vector<Laser> lasers;
        void DeleteInactiveLasers();
        Rectangle getRect();
        void Reset();
    private:
        Texture2D image;
        Vector2 position;
        double lastFireTime;
        Sound laserSound;
};