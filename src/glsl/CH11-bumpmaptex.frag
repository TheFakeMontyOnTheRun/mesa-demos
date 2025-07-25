//
// Fragment shader for procedural bumps
//
// Authors: John Kessenich, Randi Rost
//
// Copyright (c) 2002-2006 3Dlabs Inc. Ltd.
//
// See 3Dlabs-License.txt for license information
//
// Texture mapping/modulation added by Brian Paul
//

varying vec3 LightDir;
varying vec3 EyeDir;

uniform float BumpDensity;     // = 16.0
uniform float BumpSize;        // = 0.15
uniform float SpecularFactor;  // = 0.5

uniform sampler2D Tex;

void main()
{
    vec3 ambient = vec3(0.25);
    vec3 litColor;
    vec2 c = BumpDensity * gl_TexCoord[0].st;
    vec2 p = fract(c) - vec2(0.5);

    float d, f;
    d = p.x * p.x + p.y * p.y;
    f = inversesqrt(d + 1.0);

    if (d >= BumpSize)
        { p = vec2(0.0); f = 1.0; }

    vec3 SurfaceColor = texture2D(Tex, gl_TexCoord[0].st).xyz;

    vec3 normDelta = vec3(p.x, p.y, 1.0) * f;
    litColor = SurfaceColor * (ambient + max(dot(normDelta, LightDir), 0.0));
    vec3 reflectDir = reflect(LightDir, normDelta);

    float spec = max(dot(EyeDir, reflectDir), 0.0);
    spec *= SpecularFactor;
    litColor = min(litColor + spec, vec3(1.0));

    gl_FragColor = vec4(litColor, 1.0);
}
