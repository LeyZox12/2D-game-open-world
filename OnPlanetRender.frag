#version 330

#ifdef GL_ES
precision mediump float;
#endif


uniform vec2 resolution;
uniform sampler2D planet;
uniform sampler2D spriteSheet;
uniform vec2 spriteSheetRes;
uniform vec2 spriteSheetUnit;
uniform vec2 topLeftPos;
uniform float BLOCK_SIZE;
uniform float theta;
uniform float WORLD_SIZE;

vec4 getTextureWithRect(vec2 uv, vec2 pos)
{
    vec2 ratio = spriteSheetUnit / spriteSheetRes;
    return texture2D(spriteSheet, pos * ratio + uv * ratio);
}

void main()
{
    vec4 c;
    
    //float theta = 3.1415926;
    
    
    vec2 fp = vec2(gl_FragCoord.x, resolution.y - gl_FragCoord.y);
    vec2 index = (topLeftPos.xy + fp.xy) / BLOCK_SIZE / WORLD_SIZE;
    vec2 uv = vec2(mod(index.x * BLOCK_SIZE * WORLD_SIZE, BLOCK_SIZE) / BLOCK_SIZE, mod(index.y * BLOCK_SIZE * WORLD_SIZE, BLOCK_SIZE) / BLOCK_SIZE);

    if(texture2D(planet, vec2(index.x,index.y)) == vec4(1.0, 1.0, 1.0, 1.0))
        c = getTextureWithRect(uv, vec2(7, 0));
    else
        c = vec4(0);
    gl_FragColor = vec4(c);
}