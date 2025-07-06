#ifndef BLOCKREGISTRY_H
#define BLOCKREGISTRY_H

#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <functional>
#include <any>
#include <BlockContext.hpp>
#include <Block.h>

using namespace sf;
using namespace std;


class blockRegistry
{
    public:
        blockRegistry();
        void registerBlock(IntRect spriteRect, function<vector<any>(BlockContext ctx)> OnInteract, Vector3i mapColor, bool isLightSource);
    protected:

    private:
        vector<Block> blocks;
};

#endif // BLOCKREGISTRY_H
