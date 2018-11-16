#version 430

in vec3 fragPosition;
in vec3 fragNormal;
in vec2 fragCoord;
in vec4 testOut;

uniform mat4 MV;

struct LightInfo
{
  vec4 Position;
  vec3 La;
  vec3 Ld;
  vec3 Ls;
};


uniform LightInfo Light = LightInfo(vec4(10.0, 10.0, 10.0, 1.0),
                                    vec3(0.2, 0.2, 0.2),
                                    vec3(1.0, 1.0, 1.0),
                                    vec3(0.0, 0.0, 0.0));


struct MaterialInfo
{
  vec3 Ka;
  vec3 Kd;
  vec3 Ks;
  float Shininess;
};


uniform MaterialInfo Material = MaterialInfo(vec3(0.1, 0.1, 0.1),
                                             vec3(1.0, 1.0, 1.0),
                                             vec3(1.0, 1.0, 1.0),
                                             1.0);

out vec4 FragColor;

void main()
{
  vec3 n = normalize( fragNormal );
  vec3 transformedLight = vec4(MV * Light.Position).xyz;
  vec3 s = normalize( vec3(transformedLight) - fragPosition );
  vec3 v = normalize(vec3(-fragPosition));
  vec3 r = reflect( -s, n );
  vec3 lightColor = (Light.La * Material.Ka +
                     Light.Ld * Material.Kd * max( dot(s, n), 0.0 ) +
                     Light.Ls * Material.Ks * pow( max( dot(r,v), 0.0 ), Material.Shininess ));
  FragColor = vec4(lightColor, 1.0);

}
