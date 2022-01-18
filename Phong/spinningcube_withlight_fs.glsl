#version 130

/*struct Material {
 
  vec3 diffuse;
  vec3 specular;
  float shininess;
};*/

struct Material {  // diapo 12
	sampler2D diffuse;
	vec3 specular;
	float shininess;
}; 

struct Light {
  vec3 position;
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
};

vec3 CalcPointLight(Light light, vec3 view_dir);

out vec4 frag_col;

in vec3 frag_3Dpos;
in vec3 vs_normal;
in vec2 vs_tex_coord;

uniform Material material;
#define N_POINT_LIGHTS 2 
uniform Light pointLights[N_POINT_LIGHTS];
uniform vec3 view_pos;

void main() {
  vec3 view_dir = normalize(view_pos - frag_3Dpos);
  vec3 result = CalcPointLight(pointLights[0], view_dir);
  for(int i = 1; i < N_POINT_LIGHTS; i++){
    result += CalcPointLight(pointLights[i], view_dir);
  }
  frag_col = vec4(result, 1.0);
}

vec3 CalcPointLight(Light light, vec3 view_dir){
	//Luz ambiente
  vec3 ambient = light.ambient * vec3(texture(material.diffuse, vs_tex_coord));
  vec3 light_dir = normalize(light.position - frag_3Dpos);

  //Luz difusa
  // Cálculo da comp.difusa usando light_pos e frag_3Dpos
  float diff = max(dot(vs_normal, light_dir), 0.0);
  //vec3 diffuse = light.diffuse * diff * material.diffuse; //ata diapo 11
  vec3 diffuse = light.diffuse * max(dot(vs_normal, light_dir), 0.0) * vec3(texture(material.diffuse, vs_tex_coord)); //diapo 12

  //Luz especular 
  // Cálculo da comp.especular con view_pos, light_pos e frag_3Dpos
  vec3 reflect_dir = reflect(-light_dir, vs_normal);
  float spec = pow(max(dot(view_dir, reflect_dir), 0.0), material.shininess);
  vec3 specular = light.specular * spec * material.specular; //ata diapo 11
  //vec3 specular = light.specular * spec * vec3(texture(material.specular,vs_tex_coord));	

  return (ambient + diffuse + specular);
}