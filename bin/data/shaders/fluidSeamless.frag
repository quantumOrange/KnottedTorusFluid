#version 150
//
//  fluidSeamless.frag
//
//  Created by David Crooks on 20/08/2016.
//
//
uniform float time;
uniform float advectionRate;
uniform vec2 size;
uniform vec2 inverseSize;
uniform vec2 torusRadii;

uniform sampler2D tex0;
in vec2 texCoordVarying;
in vec2 normalizedCoordVarying;


out vec4 outputColor;

#define TWO_PI 6.283185

//The following notice applies to the functions permute, taylorInvSqrt , grad4, and simplexNoise
//
// Description : Array and textureless GLSL 2D/3D/4D simplex
//               noise functions.
//      Author : Ian McEwan, Ashima Arts.
//  Maintainer : ijm
//     Lastmod : 20110403 (stegu)
//     License : Copyright (C) 2011 Ashima Arts. All rights reserved.
//               Distributed under the MIT License. See LICENSE file.
//

vec4 permute(vec4 x)
{
    return mod(((x*34.0)+1.0)*x, 289.0);
}

float permute(float x)
{
    return floor(mod(((x*34.0)+1.0)*x, 289.0));
}

vec4 taylorInvSqrt(vec4 r)
{
    return 1.79284291400159 - 0.85373472095314 * r;
}

float taylorInvSqrt(float r)
{
    return 1.79284291400159 - 0.85373472095314 * r;
}

vec4 grad4(float j, vec4 ip)
{
    const vec4 ones = vec4(1.0, 1.0, 1.0, -1.0);
    vec4 p,s;
    
    p.xyz = floor( fract (vec3(j) * ip.xyz) * 7.0) * ip.z - 1.0;
    p.w = 1.5 - dot(abs(p.xyz), ones.xyz);
    s = vec4(lessThan(p, vec4(0.0)));
    p.xyz = p.xyz + (s.xyz*2.0 - 1.0) * s.www;
    
    return p;
}

float simplexNoise(vec4 v)
{
    const vec2  C = vec2( 0.138196601125010504,  // (5 - sqrt(5))/20  G4
                         0.309016994374947451); // (sqrt(5) - 1)/4   F4
    // First corner
    vec4 i  = floor(v + dot(v, C.yyyy) );
    vec4 x0 = v -   i + dot(i, C.xxxx);
    
    // Other corners
    
    // Rank sorting originally contributed by Bill Licea-Kane, AMD (formerly ATI)
    vec4 i0;
    
    vec3 isX = step( x0.yzw, x0.xxx );
    vec3 isYZ = step( x0.zww, x0.yyz );
    //  i0.x = dot( isX, vec3( 1.0 ) );
    i0.x = isX.x + isX.y + isX.z;
    i0.yzw = 1.0 - isX;
    
    //  i0.y += dot( isYZ.xy, vec2( 1.0 ) );
    i0.y += isYZ.x + isYZ.y;
    i0.zw += 1.0 - isYZ.xy;
    
    i0.z += isYZ.z;
    i0.w += 1.0 - isYZ.z;
    
    // i0 now contains the unique values 0,1,2,3 in each channel
    vec4 i3 = clamp( i0, 0.0, 1.0 );
    vec4 i2 = clamp( i0-1.0, 0.0, 1.0 );
    vec4 i1 = clamp( i0-2.0, 0.0, 1.0 );
    
    //  x0 = x0 - 0.0 + 0.0 * C
    vec4 x1 = x0 - i1 + 1.0 * C.xxxx;
    vec4 x2 = x0 - i2 + 2.0 * C.xxxx;
    vec4 x3 = x0 - i3 + 3.0 * C.xxxx;
    vec4 x4 = x0 - 1.0 + 4.0 * C.xxxx;
    
    // Permutations
    i = mod(i, 289.0);
    float j0 = permute( permute( permute( permute(i.w) + i.z) + i.y) + i.x);
    vec4 j1 = permute( permute( permute( permute (
                                                  i.w + vec4(i1.w, i2.w, i3.w, 1.0 ))
                                        + i.z + vec4(i1.z, i2.z, i3.z, 1.0 ))
                               + i.y + vec4(i1.y, i2.y, i3.y, 1.0 ))
                      + i.x + vec4(i1.x, i2.x, i3.x, 1.0 ));
    // Gradients
    // ( 7*7*6 points uniformly over a cube, mapped onto a 4-octahedron.)
    // 7*7*6 = 294, which is close to the ring size 17*17 = 289.
    
    vec4 ip = vec4(1.0/294.0, 1.0/49.0, 1.0/7.0, 0.0) ;
    
    vec4 p0 = grad4(j0,   ip);
    vec4 p1 = grad4(j1.x, ip);
    vec4 p2 = grad4(j1.y, ip);
    vec4 p3 = grad4(j1.z, ip);
    vec4 p4 = grad4(j1.w, ip);
    
    // Normalise gradients
    vec4 norm = taylorInvSqrt(vec4(dot(p0,p0), dot(p1,p1), dot(p2, p2), dot(p3,p3)));
    p0 *= norm.x;
    p1 *= norm.y;
    p2 *= norm.z;
    p3 *= norm.w;
    p4 *= taylorInvSqrt(dot(p4,p4));
    
    // Mix contributions from the five corners
    vec3 m0 = max(0.6 - vec3(dot(x0,x0), dot(x1,x1), dot(x2,x2)), 0.0);
    vec2 m1 = max(0.6 - vec2(dot(x3,x3), dot(x4,x4)            ), 0.0);
    m0 = m0 * m0;
    m1 = m1 * m1;
    return 49.0 * ( dot(m0*m0, vec3( dot( p0, x0 ), dot( p1, x1 ), dot( p2, x2 )))
                   + dot(m1*m1, vec2( dot( p3, x3 ), dot( p4, x4 ) ) ) );
    
}

//--------------------------------------------------------------------------------------------------
/*
    Fractal noise.
*/
float fractalNoise(vec4 samplePoint, int octaves, float lacunarity, float persistence)
{
    vec4  p = samplePoint;
    float amplitude = 1.0;
    float sum = 0.0;
    
    for(int i = 0; i < octaves; i++)
    {
        amplitude *= persistence;
        sum += amplitude * simplexNoise(p);
        p *= lacunarity;
    }
    
    return sum;
}

/*
    Custom noise function periodic over range [0,1].
*/
float noise(vec2 v) {
    float lacunarity = 2.0;
    float persistence = 0.4;
    int octaves = 4;
    float r1 = torusRadii.x;
    float r2 = torusRadii.y;
    vec2 theta = TWO_PI*v;
    float t = time;
    //This is 4d torus: a circle in the x-y plane and a circle in the z-w plane. We shift the origin for time dependence.
    vec4 noiseCoord = vec4(r1*cos(theta.x)+t,r1*sin(theta.x)+t ,r2*cos(theta.y)+t,r2*sin(theta.y)+t);
    return fractalNoise(noiseCoord,octaves,lacunarity,persistence);
}

/*
    The function curlOfNoise() generates a random vector flow field from perlin (simplex) noise.
 
    The following brief explantion assumes some familarity with vector calculus. The intuive notion that a vector field is flowing smoothly
    corresponds to the mathematical condition that it is divergence free:
 
        Div F = 0
 
    We can satisfy this condtion by writing F = Curl H because Div Curl H = 0 for any H.  We also want our field to flow in 2d, so we require Fz = 0.
    Fz depends on the derivites of the x and y components, so setting these to any constant solves this conditon.
    Setting H = (0,0,E(x,y)) gives:
 
        F = (dE/dy, -dE/dx,0)
 
    for any scalar function of x and y.
    So setting  E(x,y) = noise(x,y) gives a random divergence free flow field, as required.
 
*/
vec2 curlOfNoise() {
    vec2 Tp0 =  vec2((gl_FragCoord.x +1) ,gl_FragCoord.y)*inverseSize;
    vec2 Tm0 =  vec2((gl_FragCoord.x -1) ,gl_FragCoord.y)*inverseSize;
    vec2 T0p =  vec2(gl_FragCoord.x ,(gl_FragCoord.y +1))*inverseSize;
    vec2 T0m =  vec2(gl_FragCoord.x ,(gl_FragCoord.y -1))*inverseSize;
    
    vec2 Np = vec2(noise(Tp0),noise(T0p));
    vec2 Nm = vec2(noise(Tm0),noise(T0m));
    
    vec2  dN = 0.5*(Np - Nm)*size;
    
    return vec2(dN.y,-dN.x);
}

void main()
{
    vec2 curlN = curlOfNoise();
    vec2 advection =  advectionRate * curlN ;
    float threashold = 0.5;
   
    vec4 density =  texture(tex0,texCoordVarying + advection*inverseSize);
    
    outputColor =  vec4(density.rgb, 1.0);
}