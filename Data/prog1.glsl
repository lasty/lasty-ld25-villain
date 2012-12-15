
#version 120

attribute vec3 position;
attribute vec2 texcoord;
attribute vec3 normal;

uniform mat4 model_matrix;
uniform mat4 projection_view_matrix;

varying vec2 uv;

void main()
{
	gl_Position = model_matrix * projection_view_matrix	* vec4(position, 1.0);

	uv = texcoord;
}

%%%%

#version 120

uniform sampler2D tex;

varying vec2 uv;

void main()
{
	gl_FragColor = //vec4(1.0, 1.0, 0.0, 1.0); 
	texture2D(tex, uv);
}