#pragma once
#include "block.hpp"
#include<vector>
using namespace std;

class Obstacle{
    public:
        Obstacle(Vector2 position);
        void Draw();
        vector<Block> blocks;
        static vector<vector<int>> grid;

    private:
        Vector2 position;

};