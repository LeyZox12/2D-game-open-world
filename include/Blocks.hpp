#include "BlockRegistry.hpp"
#include "GameManager.hpp"
#include "globals.hpp"


void import(BlockRegistry& registry)
{
    registry.registerBlock(
    Block(
        "Stone",
        Vector3i(1, 0, 0),
        vec2(5, 0),
        [](BlockContext ctx) -> vector<any>
        {
            return ctx.args;
        },
        [](BlockContext ctx) -> vector<any>
        {
            return ctx.args;
        },
        true
    ));
    registry.registerBlock(
    Block(
        "Air",
        Vector3i(0, 0, 0),
        vec2(0, 0),
        [](BlockContext ctx) -> vector<any>
        {
            return ctx.args;
        },
        [](BlockContext ctx) -> vector<any>
        {
            return ctx.args;
        },
        false
    ));
    registry.registerBlock(
    Block(
        "Water",
        Vector3i(0, 0, 0),
        vec2(0, 0),
        [](BlockContext ctx) -> vector<any>
        {
            return ctx.args;
        },
        [](BlockContext ctx) -> vector<any>
        {
            float spacing = BLOCK_SIZE / (float)LIQUID_PER_BOX_ROW / 2.0f;
            for(int i = -LIQUID_PER_BOX_ROW; i < LIQUID_PER_BOX_ROW; i++)
            {
                for(int j = -LIQUID_PER_BOX_ROW; j < LIQUID_PER_BOX_ROW; j++)
                {
                    ctx.gm.water[ctx.gm.waterCount] = ctx.mousepos + vec2(j * spacing, i * spacing);
                    ctx.gm.waterCount++;
                }
            }

            return ctx.args;
        },
        false
    ));
    registry.registerBlock(
    Block(
        "Torch",
        Vector3i(2, 0, 0),
        vec2(0, 5),
        [](BlockContext ctx) -> vector<any>
        {
            return ctx.args;
        },
        [](BlockContext ctx) -> vector<any>
        {
            return ctx.args;
        },
        false,
        vec2(0.1875, 0),
        Vector3f(255, 0, 0)
    ));
    registry.registerBlock(
    Block(
        "Blue Torch",
        Vector3i(3, 0, 0),
        vec2(0, 5),
        [](BlockContext ctx) -> vector<any>
        {
            return ctx.args;
        },
        [](BlockContext ctx) -> vector<any>
        {
            return ctx.args;
        },
        false,
        vec2(0.1875, 0),
        Vector3f(171,219,227)
    ));
}
