#ifndef BLOCKREGISTRY_H
#define BLOCKREGISTRY_H

#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <functional>
#include <any>
#include <BlockContext.hpp>
#include <Block.hpp>

using namespace sf;
using namespace std;


class blockRegistry
{
    public:
        blockRegistry();
        void registerBlock(Block block);

    private:
        vector<Block> blocks;
};

#endif // BLOCKREGISTRY_H
