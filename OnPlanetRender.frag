#ifdef GL_ES
precision highp float;
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
uniform vec2 playerPos;
uniform vec2 headPos;
uniform vec2 facePos;
uniform vec2 lightPos;

vec4 getTextureWithRect(vec2 uv, vec2 pos)
{
    vec2 ratio = spriteSheetUnit / spriteSheetRes;
    return texture2D(spriteSheet, pos * ratio + uv * ratio);
}

vec4 getRectangle(vec2 uv, vec2 pos, vec2 size, vec2 spriteSheetPos)
{
    vec2 fixed_uv = (uv - pos) / size;
    vec2 spriteSheetRatio = spriteSheetUnit / spriteSheetRes;
    if(uv.x > pos.x && uv.x < pos.x + size.x && uv.y > pos.y && uv.y < pos.y + size.y)
        return texture2D(spriteSheet, spriteSheetPos * spriteSheetRatio + fixed_uv * spriteSheetRatio);
    return vec4(0.0);
}

int[10] getNeighbours(vec2 pos)
{
    int[10] output = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};// output 8 = count, 9 = axis count
    float pixel = 1.0 / WORLD_SIZE;
    if(texture2D(planet, vec2(pos.x+pixel,pos.y)) == vec4(1.0)) // right
    {
        output[0] = 1;
        output[8] += 1;
        output[9] += 1;
    }
    if(texture2D(planet, vec2(pos.x+pixel,pos.y-pixel)) == vec4(1.0)) // top right
    {
        output[1] = 1;
        output[8] += 1;
    }
    if(texture2D(planet, vec2(pos.x,pos.y-pixel)) == vec4(1.0)) // top
    {
        output[2] = 1;
        output[8] += 1;
        output[9] += 1;
    }
    if(texture2D(planet, vec2(pos.x-pixel,pos.y-pixel)) == vec4(1.0)) // top left
    {
        output[3] = 1;
        output[8] += 1;
    }
    if(texture2D(planet, vec2(pos.x-pixel,pos.y)) == vec4(1.0)) // left
    {
        output[4] = 1;
        output[8] += 1;
        output[9] += 1;
    }
    if(texture2D(planet, vec2(pos.x-pixel,pos.y+pixel)) == vec4(1.0)) // bottom left
    {
        output[5] = 1;
        output[8] += 1;
    }
    if(texture2D(planet, vec2(pos.x,pos.y+pixel)) == vec4(1.0)) // bottom
    {
        output[6] = 1;
        output[8] += 1;
        output[9] += 1;
    }
    if(texture2D(planet, vec2(pos.x+pixel,pos.y+pixel)) == vec4(1.0)) // bottom right
    {
        output[7] = 1;
        output[8] += 1;
    }
    return output;
}

float light(vec2 uv, vec2 position, float radius){
    return  1.0- length(uv - vec2(position)) * 1.0/radius;
}

float DDA(vec2 lightPos, vec2 endPos, float maxdist)
{   
    vec2 dir = endPos - lightPos;
    dir = normalize(dir);
    vec2 dist;
    float distmag;
    int wallFound = 0;
    while(wallFound == 0 && distmag < maxdist)
    {
        
    }
    
/*    float dist = 0.0;
    dir = normalize(dir);
    float checkX = floor(lightPos.x);
    float checkY = floor(lightPos.y);
    float deltaX =  dir.x == 0.0 ? (1.0 / 1e30.0) : abs(1.0 / dir.x);
    float deltaY =  dir.y == 0.0 ? (1.0 / 1e30.0) : abs(1.0 / dir.y);
    float distX;
    float distY;
    float stepX;
    float stepY;
    float side = 0.0;
    float[2] output;
    if(dir.x > 0.0)
    {
        stepX = 1.0;
        distX = (1.0 - (lightPos.x - floor(lightPos.x))) * deltaX;
    }
    else
    {
        stepX = -1.0;
        distX = (lightPos.x - floor(lightPos.x)) * deltaX;
    }
    if(dir.y > 0.0)
    {
        distY = (1.0 - (lightPos.y - floor(lightPos.y))) * deltaY;
        stepY = 1.0;
    }
    else
    {
        distY = (lightPos.y - floor(lightPos.y)) * deltaY;
        stepY = -1.0;
    }

    bool wallFound = false;

    while(!wallFound)
    {
        if(distX < distY)
        {
            distX += deltaX;
            checkX += stepX;
            side = 0.0;
            if(checkX >= 0.0 and checkX < len(mapArr) and mapArr[checkY][checkX] == 1.0)
            {
                wallFound = True;
                dist = distX - deltaX;
                output[0.0] = dist;
                output[1.0] = side;
                return output;
            }
        }
        else{
            distY += deltaY;
            checkY += stepY;
            side = 1;

            if(checkY >= 0.0 and checkY < len(mapArr) and mapArr[checkY][checkX] == 1.0)
            {
                wallFound = true
                dist = distY - deltaY
                output[0.0] = dist;
                output[1.0] = side;
                return output;
            }
        }
    }*/
}

void main()
{
    vec4 c;
    vec2 fp = vec2(gl_FragCoord.x, resolution.y - gl_FragCoord.y);
    vec2 index = (topLeftPos.xy + fp.xy) / BLOCK_SIZE / WORLD_SIZE;
    vec2 block_uv = vec2(mod(index.x * BLOCK_SIZE * WORLD_SIZE, BLOCK_SIZE) / BLOCK_SIZE, mod(index.y * BLOCK_SIZE * WORLD_SIZE, BLOCK_SIZE) / BLOCK_SIZE);
    vec2 uv = gl_FragCoord.xy / resolution.xy;
    uv.y = 1.0 - uv.y;

    

    if(texture2D(planet, vec2(index.x,index.y)) == vec4(1.0, 1.0, 1.0, 1.0))
    {
        int[10] neighbours = getNeighbours(index);
        int count = neighbours[8];
        int axisCount = neighbours[9];
        vec2 texPos;
        if(axisCount == 0) texPos = vec2(5, 0);
        else if(axisCount == 4) texPos = vec2(4, 4);
        else if(axisCount == 1)
        {
            if(neighbours[0] == 1) texPos = vec2(6, 0);
            else if(neighbours[2]) texPos = vec2(0, 1);
            else if(neighbours[4] == 1) texPos = vec2(6, 1);
            else if(neighbours[6]) texPos = vec2(0, 0);
        }
        else if(axisCount == 2)
        {
            if(neighbours[6] == 1 && neighbours[0] == 1) texPos = vec2(1, 0);
            else if(neighbours[2] == 1 && neighbours[4] == 1) texPos = vec2(2, 1);
            else if(neighbours[2] == 1 && neighbours[0] == 1) texPos = vec2(1, 1);
            else if(neighbours[4] == 1 && neighbours[6] == 1) texPos = vec2(2, 0);
            else if(neighbours[6] == 1 && neighbours[2] == 1) texPos = vec2(3, 0);
            else if(neighbours[0] == 1 && neighbours[4] == 1) texPos = vec2(7, 0);
        }
        else if(axisCount == 3)
        {
            if(neighbours[0] == 1 && neighbours[4] == 1)
            {   
                if(neighbours[6] == 1)
                    texPos = vec2(0,2);
                else
                    texPos = vec2(0,3);
            }
            else if(neighbours[2] == 1 && neighbours[6] == 1)
            {
                if(neighbours[0] == 1)
                    texPos = vec2(4, 0);
                else
                    texPos = vec2(4, 1);
            }
        }

        c = getTextureWithRect(block_uv, texPos);
    }
    else
        c = vec4(0);
    c += getRectangle(fp + topLeftPos.xy, playerPos, vec2(BLOCK_SIZE, BLOCK_SIZE * 2.0), vec2(2, 2));
    c += getRectangle(fp + topLeftPos.xy, headPos, vec2(BLOCK_SIZE, BLOCK_SIZE), vec2(2, 2));
    c += getRectangle(fp + topLeftPos.xy, facePos, vec2(BLOCK_SIZE, BLOCK_SIZE), vec2(3, 2));
    c += light(fp, lightPos, 400.0f);
    gl_FragColor = vec4(c);
}