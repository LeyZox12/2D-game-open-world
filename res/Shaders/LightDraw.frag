uniform vec2 resolution;
uniform vec2 topLeftPos;
uniform vec2 pos;
uniform vec2 texturePosStart;
uniform vec2 texturePosEnd;
uniform sampler2D lightChunks;
uniform vec2 chunkPos;
out vec4 fragColor;


void main(){
    vec2 uv = vec2(gl_FragCoord.x, resolution.y - gl_FragCoord.y);
    uv += topLeftPos;
    vec2 diff = texturePosEnd - texturePosStart;

    vec2 posOnTexture = (uv - texturePosStart)/ diff;
    fragColor = texture(lightChunks, posOnTexture);
}

