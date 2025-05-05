// lighting.fs
// phong : https://learnopengl.com/Lighting/Basic-Lighting
#version 330

in vec3 fragPosition;
in vec2 fragTexCoord;
in vec4 fragColor;
in vec3 fragNormal;

uniform vec3 viewPos;

uniform vec3 lightPos;
uniform vec3 lightColor;

uniform float ambientStr;
uniform float specularStr;
uniform int shininess;

uniform bool useTexture;
uniform vec4 materialColor;
uniform sampler2D texture0;

out vec4 finalColor;

void main() {
    
    // Ambient
    vec4 texColor = texture(texture0, fragTexCoord);
    vec3 ambient = ambientStr * lightColor;

    // Diffuse
    vec3 norm = normalize(fragNormal);
    vec3 lightDir = normalize(lightPos - fragPosition);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
    
    // Specular
    vec3 viewDir = normalize(viewPos - fragPosition);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    vec3 specular = specularStr * spec * lightColor;
    
    // Kombination
    vec3 result = (ambient + diffuse + specular);
    vec4 baseColor = useTexture ? texture(texture0, fragTexCoord) : materialColor;
    finalColor = vec4(baseColor.rgb * result, baseColor.a);
    //finalColor = vec4(texColor.rgb * result, 1);//vec4(result, 1.0);
}