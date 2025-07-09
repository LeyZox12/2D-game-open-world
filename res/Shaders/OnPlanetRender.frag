#version 300 es

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
uniform vec2 lightPos[100];
uniform int lightCount;
uniform vec2 mousepos;
out vec4 fragColor;




vec4 getRectangle(vec2 uv, vec2 pos, vec2 size, vec2 spriteSheetPos)
{
    vec2 fixed_uv = (uv - pos) / size;
    vec2 spriteSheetRatio = spriteSheetUnit / spriteSheetRes;
    if(uv.x > pos.x && uv.x < pos.x + size.x && uv.y > pos.y && uv.y < pos.y + size.y)
        return texture(spriteSheet, spriteSheetPos * spriteSheetRatio + fixed_uv * spriteSheetRatio);
    else return vec4(0.0);

}

void getNeighbours(vec2 pos, out int n[10])
{
    
    float pixel = 1.0 / WORLD_SIZE;
    if(texture(planet, vec2(pos.x+pixel,pos.y)).x == 1.0) // right
    {
        n[0] = 1;
        n[8] += 1;
        n[9] += 1;
    }
    if(texture(planet, vec2(pos.x+pixel,pos.y-pixel)).x == 1.0) // top right
    {
        n[1] = 1;
        n[8] += 1;
    }
    if(texture(planet, vec2(pos.x,pos.y-pixel)).x == 1.0) // top
    {
        n[2] = 1;
        n[8] += 1;
        n[9] += 1;
    }
    if(texture(planet, vec2(pos.x-pixel,pos.y-pixel)).x == 1.0) // top left
    {
        n[3] = 1;
        n[8] += 1;
    }
    if(texture(planet, vec2(pos.x-pixel,pos.y)).x == 1.0) // left
    {
        n[4] = 1;
        n[8] += 1;
        n[9] += 1;
    }
    if(texture(planet, vec2(pos.x-pixel,pos.y+pixel)).x == 1.0) // bottom left
    {
        n[5] = 1;
        n[8] += 1;
    }
    if(texture(planet, vec2(pos.x,pos.y+pixel)).x == 1.0) // bottom
    {
        n[6] = 1;
        n[8] += 1;
        n[9] += 1;
    }
    if(texture(planet, vec2(pos.x+pixel,pos.y+pixel)).x == 1.0) // bottom right
    {
        n[7] = 1;
        n[8] += 1;
    }

}

float light(vec2 uv, vec2 position, float radius){
    return  1.0- length(uv - vec2(position)) * 1.0/radius;
}


float DDA(vec2 origin, vec2 endPos, float maxdist, float power)
{   

    
    vec2 origin_old = origin;
    vec2 endPos_old = endPos; 
    origin /= BLOCK_SIZE;
    endPos /= BLOCK_SIZE;
    vec2 direction = endPos - origin;
    float distTotal = 0.0;
    float dirVecDist = sqrt(direction.x * direction.x + direction.y * direction.y);

    if(dirVecDist == 0.0) dirVecDist = 1e-30;
    direction.x = direction.x / dirVecDist;
    direction.y = direction.y / dirVecDist;
    float checkX = origin.x;
    float checkY = origin.y;
    float side = 0.0;
    vec2 steps;
    vec2 dist;
    vec2 delta;
    delta = abs(vec2(1.0) / direction);
    if(direction.x > 0.0)
    {
        steps.x = 1.0;
        dist.x = (1.0 - (origin.x - floor(origin.x))) * delta.x;
    }
    else
    {
        steps.x = -1.0;
        dist.x = (origin.x - floor(origin.x)) * delta.x;
    }
    if(direction.y > 0.0)
    {
        steps.y = 1.0;
        dist.y = (1.0 - (origin.y - floor(origin.y))) * delta.y;
    }
    else
    {
        dist.y = (origin.y - floor(origin.y)) * delta.y;
        steps.y = -1.0;
    }
    int wallFound = 0;
    float percent;
    if(length(origin - endPos) > maxdist) return 0.0;
    while(wallFound != 1 && distTotal < maxdist)
    {
        if(dist.x < dist.y)
        {
            dist.x += delta.x;
            checkX += steps.x;
            side = steps.x > 0.0 ? 0.0 : 1.0;
            distTotal = dist.x - delta.x;

            if(texture(planet, vec2((checkX)/WORLD_SIZE, (checkY)/WORLD_SIZE)).x == 1.0)
            {
                wallFound = 1;
                vec2 hit = origin + direction * distTotal;
                percent = hit.y - floor(hit.y);
                   
            }
        }
        else
        {
            dist.y += delta.y;
            checkY += steps.y;
            side = steps.y > 0.0 ? 2.0 : 3.0;
            distTotal = dist.y - delta.y;
            if(wallFound == -1) wallFound = 1;
            if(texture(planet, vec2((checkX)/WORLD_SIZE, (checkY)/WORLD_SIZE)).x == 1.0)
            {
                wallFound = 1;
                vec2 hit = origin + direction * distTotal;
                percent = hit.x - floor(hit.x);
            }
        }
    }
    float l = (1.0 - length(origin - endPos) * 1.0/power);
    l = clamp(l, 0.0, 1.0);
    if(wallFound == 1) return length(origin - endPos) < distTotal ? l : 0.0;
    else return l;

}

vec4 getTextureWithRect(vec2 uv, vec2 pos)
{
    vec2 ratio = spriteSheetUnit / spriteSheetRes;
    return texture(spriteSheet, pos * ratio + uv * ratio);

}

float sdBox(in vec2 p, in vec2 b)
{
	vec2 d = abs(p) - b;
    return length(max(d, vec2(0))) + min(max(d.x, d.y), 0.0);
}

vec4 addColors(vec4 c1, vec4 c2)
{
    return c1 + c2 * c2.w;
}


void main()
{
    vec4 c = vec4(0.0);
    vec2 fp = vec2(gl_FragCoord.x, resolution.y - gl_FragCoord.y);
    vec2 index = (topLeftPos.xy + fp.xy) / BLOCK_SIZE / WORLD_SIZE;
    vec2 block_uv = vec2(mod(index.x * BLOCK_SIZE * WORLD_SIZE, BLOCK_SIZE) / BLOCK_SIZE, mod(index.y * BLOCK_SIZE * WORLD_SIZE, BLOCK_SIZE) / BLOCK_SIZE);
    vec2 uv = gl_FragCoord.xy / resolution.xy;
    uv.y = 1.0 - uv.y;

    vec4 ddaR = vec4(0.9647, 0.349, 0.0157, 0.0);
    for(int i = 0; i< lightCount; i++)
    {
        float l = DDA(lightPos[i], fp + topLeftPos.xy, 20.0, 5.0);

        ddaR.w += l;
    }
    ddaR.w = clamp(ddaR.w, 0.0, 1.0);
    
    c += getRectangle(fp + topLeftPos.xy, playerPos, vec2(BLOCK_SIZE, BLOCK_SIZE * 2.0), vec2(2, 2));
    c += getRectangle(fp + topLeftPos.xy, headPos, vec2(BLOCK_SIZE, BLOCK_SIZE), vec2(2, 2));
    c += getRectangle(fp + topLeftPos.xy, facePos, vec2(BLOCK_SIZE, BLOCK_SIZE), vec2(3, 2));
    c = addColors(c, ddaR);

    if(texture(planet, vec2(index.x,index.y)).x == 1.0)
    {
        int n[10];
        for(int i = 0; i < 10; i++)n[i] = 0;
        getNeighbours(index, n);
        int count = n[8];
        int axisCount = n[9];
        vec2 texPos;
        if(axisCount == 0) texPos = vec2(5, 0);
        else if(axisCount == 4) texPos = vec2(4, 4);
        else if(axisCount == 1)
        {
            if(n[0] == 1) texPos = vec2(6, 0);
            else if(n[2] == 1) texPos = vec2(0, 1);
            else if(n[4] == 1) texPos = vec2(6, 1);
            else if(n[6] == 1) texPos = vec2(0, 0);
        }
        else if(axisCount == 2)
        {
            if(n[6] == 1 && n[0] == 1) texPos = vec2(1, 0);
            else if(n[2] == 1 && n[4] == 1) texPos = vec2(2, 1);
            else if(n[2] == 1 && n[0] == 1) texPos = vec2(1, 1);
            else if(n[4] == 1 && n[6] == 1) texPos = vec2(2, 0);
            else if(n[6] == 1 && n[2] == 1) texPos = vec2(3, 0);
            else if(n[0] == 1 && n[4] == 1) texPos = vec2(7, 0);
        }
        else if(axisCount == 3)
        {
            if(n[0] == 1 && n[4] == 1)
            {   
                if(n[6] == 1)
                    texPos = vec2(0,2);
                else
                    texPos = vec2(0,3);
            }
            else if(n[2] == 1 && n[6] == 1)
            {
                if(n[0] == 1)
                    texPos = vec2(4, 0);
                else
                    texPos = vec2(4, 1);
            }
        }
            c += getTextureWithRect(block_uv, texPos) * 0.6;    
    }
    float b = sdBox(fp + topLeftPos - floor((mousepos) / BLOCK_SIZE)* BLOCK_SIZE  - vec2(BLOCK_SIZE * 0.5) , vec2(50.0));
    c = 1.0 - smoothstep(b, 5.0, 6.0) == 1.0 ? vec4(1.0) : c ;
    fragColor = texture(planet, uv);
    fragColor = vec4(c);
}