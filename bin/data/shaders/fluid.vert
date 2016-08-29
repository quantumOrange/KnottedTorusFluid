#version 150
//
//  fluid.vert
//
//  Created by David Crooks on 20/08/2016.
//
//
uniform mat4 modelViewProjectionMatrix;
in vec2 texcoord;

in vec4 position;
out vec2 texCoordVarying;


void main(){
    texCoordVarying = texcoord;
   
	gl_Position = modelViewProjectionMatrix * position;
}
