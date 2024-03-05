#version 430 core

in vec2 fragTexCoord;
out vec4 finalColor;

uniform sampler2D textureSampler;

void main() {
    float alpha = texture(textureSampler, fragTexCoord).a;
    vec4 baseColor = texture(textureSampler, fragTexCoord);

    // Get the size of the texture
    ivec2 texSize = textureSize(textureSampler, 0); // 0 is the mipmap level, assuming no mipmaps

    // Sample surrounding texels
    float alphaSide = ((fragTexCoord.x > 0.0) ? texture(textureSampler, fragTexCoord + vec2(-1.0 / texSize.x, 0.0)).a : 0.0)
                    + ((fragTexCoord.x < 1.0) ? texture(textureSampler, fragTexCoord + vec2(1.0 / texSize.x, 0.0)).a : 0.0)
                    + ((fragTexCoord.y < 1.0) ? texture(textureSampler, fragTexCoord + vec2(0.0, 1.0 / texSize.y)).a : 0.0)
                    + ((fragTexCoord.y > 0.0) ? texture(textureSampler, fragTexCoord + vec2(0.0, -1.0 / texSize.y)).a : 0.0);

    // If the center pixel is transparent and there's at least one opaque neighboring pixel, set it to white
    finalColor = (alpha == 0.0 && alphaSide > 0.0) ? vec4(1.0) : baseColor;
}

/*
uniform vec2 textureSize;

void main()
{
    vec2 texelSize = 1.0 / textureSize;
    vec4 centerColor = texture(textureSampler, fragTexCoord);
    
    // Check if the center texel is transparent
    if (centerColor.a == 0.0)
    {
        // Iterate over the surrounding texels to find non-transparent texels
        for (int x = -1; x <= 1; x++)
        {
            for (int y = -1; y <= 1; y++)
            {
                if (x != 0 || y != 0)
                {
                    vec2 offset = vec2(x, y) * texelSize;
                    vec4 surroundingColor = texture(textureSampler, fragTexCoord + offset);
                    
                    // If a non-transparent texel is found, set finalColor to white and break the loop
                    if (surroundingColor.a > 0.0)
                    {
                        finalColor = vec4(1.0);
                        return;
                    }
                }
            }
        }
    }
    
    // If no non-transparent texels are found surrounding the center texel, keep the original color
    finalColor = centerColor;
}
*/
