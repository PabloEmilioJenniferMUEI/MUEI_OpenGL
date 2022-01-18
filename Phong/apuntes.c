#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>

in vec3 v_pos;
in vec3 v_normal;
in vec2 text_coord;

out vec3 frag_3Dpos;
out vec3 normal;

uniform mat4 model;
uniform mat4 view;
uniform projection;
uniform mat3 normal_matrix; 
//vectores normales: local space -> global space (3x3)
//matriz normal: trasposta da inversa da submatriz 3x3 superior esquerda da matriz modelo
uniform Material material;
uniform Light light; 
uniform vec3 view pos:

struct Material { //ata diapo 11
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
};

// struct Material { //diapo 13
// sampler2D diffuse;
// sampler2D specular;
// float shininess;
// };

// struct Material { //diapo 12
// sampler2D diffuse;
// vec3 specular;
// float shininess;
// }; 

struct Light {
	vec3 position;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

void main(){
	gl_Position = projection * view * model * vec4(v_pos, 1.0f);
	frag_3Dpos = vec3(model * vec4(v_pos, 1.0);
	//normal_matrix = glm:: transpose(glm :: inverse(glm :: mat3(model_matrix)));
	normal_matrix = glm :: inverseTranspose(gl m:: mat3(model_matrix));
	normal = normalize(normal_matrix * v_normal);

	//Luz ambiente
	vec3 ambient = light.ambient * material.ambient;
	vec3 light_dir = normalize(light.position - frag_3Dpos);

	//Luz difusa
	// Cálculo da comp.difusa usando light_pos e frag_3Dpos
	float diff = max(dot(normal, light_dir), 0.0);
	vec3 diffuse = light.diffuse * diff * material.diffuse;//ata diapo 11
	//vec3 diffuse = light.diffuse * max(dot(norm, light_dir), 0.0) * vec3(texture(material.diffuse, text_coord)); //diapo 12
	
	//Luz especular 
	// Cálculo da comp.especular con view_pos, light_pos e frag_3Dpos
	vec3 view_dir = normalize(view_pos - frag_3Dpos);
	vec3 reflect_dir = reflect(-light_dir, normal);
	float spec = pow(max(dot(view_dir, reflect_dir), 0.0), material.shininess);
	vec3 specular = light.specular * spec * material.specular; //ata diapo 11
	//vec3 specular = light.specular * spec * vec3(texture(material.specular,text_coord));	//diapo 13
	
	vec3 result = ambient + diffuse + specular; 
	frag_col = vec4(result, 1.0);
}
