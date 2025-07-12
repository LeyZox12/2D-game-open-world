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
uniform float WORLD_SIZE;
uniform vec2 lightPos[100];
uniform int lightCount;
uniform vec3 lightColors[100];


out vec4 fragColor;



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

            if(texture(planet, vec2((checkX)/WORLD_SIZE, (checkY)/WORLD_SIZE)).x != 0.0)
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
            if(texture(planet, vec2((checkX)/WORLD_SIZE, (checkY)/WORLD_SIZE)).x != 0.0)
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

vec4 addColors(vec4 c1, vec4 c2)
{
    return c1 + c2 * c2.w;
}


void main()
{
    float c = 1.0;
    vec2 uv = vec2(gl_FragCoord.x, resolution.y - gl_FragCoord.y);
    vec4 ddaR = vec4(0.0);
    for(int i = 0; i< lightCount; i++)
    {
        vec3 col = lightColors[i] / 255.0; 
        float l = DDA(lightPos[i], uv + topLeftPos.xy, 20.0, 5.0);
        l = clamp(l, 0.0, 1.0);
        ddaR.x = mix(ddaR.x, col.x, l);
        ddaR.y = mix(ddaR.y, col.y, l);
        ddaR.z = mix(ddaR.z, col.z, l);

        ddaR.w = 1.0;

    }
    ddaR.w = ddaR.w;
    fragColor = ddaR;

}