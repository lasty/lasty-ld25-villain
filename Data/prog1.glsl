
#version 120

attribute vec3 position;
attribute vec2 texcoord;
attribute vec3 normal;

uniform mat4 model_matrix;
uniform mat4 projection_view_matrix;

varying vec2 uv;

void main()
{
	gl_Position = projection_view_matrix * model_matrix * vec4(position, 1.0);

	uv = texcoord;
}

%%%%

#version 120

uniform sampler2D tex;

varying vec2 uv;

void main()
{
	gl_FragColor = texture2D(tex, uv);
}