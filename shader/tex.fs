#version 330

// Inputs vom Vertex Shader
in vec3 fragPosition;
in vec2 fragTexCoord;
in vec4 fragColor;
in vec3 fragNormal;

// Uniforms
uniform sampler2D texture0;
uniform vec4 colDiffuse;
uniform vec3 viewPos;
uniform vec3 lightPos;

// Output
out vec4 finalColor;

void main()
{
    // Basis-Farbe (Textur oder Farbe)
    vec4 texColor = texture(texture0, fragTexCoord);
    vec4 baseColor = texColor.a > 0.1 ? texColor : colDiffuse * fragColor;

    // Lichtberechnung (Directional Light)
    vec3 lightDir = normalize(lightPos - fragPosition);
    vec3 normal = normalize(fragNormal);
    vec3 viewDir = normalize(viewPos - fragPosition);
    
    // Diffuse Komponente
    float diff = max(dot(normal, lightDir), 0.0);
    
    // Spekulare Komponente (Blinn-Phong)
    vec3 halfwayDir = normalize(lightDir + viewDir); 
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);
    
    // Ambient Light
    float ambient = 0.4;
    
    // Kombination
    vec3 result = (ambient + diff + spec) * baseColor.rgb;
    
    finalColor = vec4(result, baseColor.a);
}