#pragma once

#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <functional>
#include <any>
#include <BlockContext.hpp>
#include <Block.hpp>
#include <algorithm>

using namespace sf;
using namespace std;


struct BlockRegistry
{
        blockRegistry();
        void registerBlock(Block block)
        {
            blocks.emplace_back(block);
        }
        Block getBlockByName(string name)
        {
            auto it = find_if(blocks.begin(), blocks.end(), [name](const Block& b){return b.name == name;});
            int index = distance(blocks.begin(), it);
            return blocks[index];
        }
        Block getBlockById(int id)
        {
            return blocks[id];
        }
        vector<Block> blocks;
};
