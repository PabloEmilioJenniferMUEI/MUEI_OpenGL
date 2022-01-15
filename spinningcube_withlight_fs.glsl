#version 130

struct Material {
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
  float shininess;
};

struct Light {
  vec3 position;
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
};

out vec4 frag_col;

in vec3 frag_3Dpos;
in vec3 vs_normal;
in vec2 vs_tex_coord;

uniform Material material;
uniform Light light;
uniform vec3 view_pos;

void main() {
 //Luz ambiente
  vec3 ambient = light.ambient * material.ambient;
  vec3 light_dir = normalize(light.position - frag_3Dpos);

  //Luz difusa
  // Cálculo da comp.difusa usando light_pos e frag_3Dpos
  float diff = max(dot(vs_normal, light_dir), 0.0);
  vec3 diffuse = light.diffuse * diff * material.diffuse; //ata diapo 11
  //vec3 diffuse = light.diffuse * max(dot(norm, light_dir), 0.0) * vec3(texture(material.diffuse, text_coord));

  //Luz especular 
  // Cálculo da comp.especular con view_pos, light_pos e frag_3Dpos
  vec3 view_dir = normalize(view_pos - frag_3Dpos);
  vec3 reflect_dir = reflect(-light_dir, vs_normal);
  float spec = pow(max(dot(view_dir, reflect_dir), 0.0), material.shininess);
  vec3 specular = light.specular * spec * material.specular; //ata diapo 11
  //vec3 specular = light.specular * spec * vec3(texture(material.specular,text_coord));	

  vec3 result = ambient + diffuse + specular;
  frag_col = vec4(result, 1.0);
}
