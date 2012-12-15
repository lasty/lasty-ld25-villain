
#version 120

attribute vec3 position;
attribute vec2 texcoord;
attribute vec3 normal;

uniform mat4 model_matrix;
uniform mat4 projection_matrix;
uniform mat4 view_matrix;
uniform mat4 projection_view_matrix;
uniform mat3 normal_matrix;

uniform vec3 light1_pos;
uniform vec3 light1_col;
varying vec3 light1_dir;

uniform vec3 light2_pos;
uniform vec3 light2_col;
varying vec3 light2_dir;

uniform vec3 light3_pos;
uniform vec3 light3_col;
varying vec3 light3_dir;


varying vec2 uv;
varying vec3 normals;

//varying float light_dist1;
//varying float light_dist2;
//varying float light_dist3;

varying vec4 world_pos;

void main()
{
	world_pos = model_matrix * vec4(position, 1.0);
	
	gl_Position = projection_matrix * view_matrix * model_matrix * vec4(position, 1.0); 

	mat3 nm = mat3( model_matrix);
	//normals = normal_matrix * normal;
	normals = nm * normal;
	uv = texcoord;
	
	light1_dir = normalize(light1_pos - world_pos.xyz);
	light2_dir = normalize(light2_pos - world_pos.xyz);
	light3_dir = normalize(light3_pos - world_pos.xyz);
}

%%%%

#version 120

uniform sampler2D tex;

varying vec2 uv;
varying vec3 normals;

//varying float light_dist1;
//varying float light_dist2;
//varying float light_dist3;

uniform vec3 light1_pos;
uniform vec3 light1_col;
varying vec3 light1_dir;

uniform vec3 light2_pos;
uniform vec3 light2_col;
varying vec3 light2_dir;

uniform vec3 light3_pos;
uniform vec3 light3_col;
varying vec3 light3_dir;


varying vec4 world_pos;


void main()
{
	float light_dist1 = 2.0 / (pow(distance(world_pos.xyz, light1_pos)/2, 2.0) );
	float light_dist2 = 2.0 / (pow(distance(world_pos.xyz, light2_pos)/2, 2.0) ); 
	float light_dist3 = 2.0 / (pow(distance(world_pos.xyz, light3_pos)/2, 2.0) );

	float dot1 = max(0.1, dot(normalize(light1_dir), normalize(normals)));
	float dot2 = max(0.1, dot(normalize(light2_dir), normalize(normals)));
	float dot3 = max(0.1, dot(normalize(light3_dir), normalize(normals)));
	
	light_dist1 *= dot1;
	light_dist2 *= dot2;
	light_dist3 *= dot3;

	vec3 light = (light1_col * light_dist1) + (light2_col * light_dist2) + (light3_col * light_dist3);
	gl_FragColor = texture2D(tex, uv) * vec4(light, 1.0);
	//gl_FragColor =  vec4(light, 1.0);
	
}