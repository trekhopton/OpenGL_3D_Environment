#version 330

in vec4 fragPos;
in vec3 normal;
in vec2 texCoord;

out vec4 fragColour;

uniform vec3 viewPos;
uniform vec3 lightPos;
uniform vec3 lightDir;

uniform vec3 lightAmbient;     // Light ambient RGBA values
uniform vec3 lightDiffuse;     // Light diffuse RGBA values
uniform vec3 lightSpecular;    // Light specular RGBA values

uniform vec3 mtlAmbient;  // Ambient surface colour
uniform vec3 mtlDiffuse;  // Diffuse surface colour
uniform vec3 mtlSpecular; // Specular surface colour

uniform sampler2D texMap;

// uniform int numPoints;

const float shininess = 16;
const float specStrength = 0.5;

const float near = 1.0;
const float far = 4096.0;

vec3 point(){

	vec3 color = texture(texMap, vec2(texCoord.x, 1.0-texCoord.y)).rgb;
	vec3 norm = normalize(normal);

    // diffuse
    vec3 lightDirec = normalize(lightPos - vec3(fragPos));
    float diff = max(dot(lightDirec, norm), 0.0);
    
    // specular
    vec3 viewDir = normalize(viewPos - vec3(fragPos));
    vec3 reflectDir = reflect(-lightDirec, norm);
	vec3 halfwayDir = normalize(lightDirec + viewDir);  
	float spec = pow(max(dot(norm, halfwayDir), 0.0), shininess);

	vec3 ambient = lightAmbient * mtlAmbient * color;
	vec3 diffuse = lightDiffuse * mtlDiffuse * color * diff;
    vec3 specular = lightSpecular * mtlSpecular * color * spec * specStrength;

	return ambient + diffuse + specular;
}

vec3 directional(){

	vec3 color = texture(texMap, vec2(texCoord.x, 1.0-texCoord.y)).rgb;
	vec3 norm = normalize(normal);

	vec3 lightDirec = normalize(-lightDir);
    // diffuse
    float diff = max(dot(norm, lightDirec), 0.0);
    // specular
	vec3 viewDir = normalize(viewPos - vec3(fragPos));
    vec3 reflectDir = reflect(-lightDirec, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);

	vec3 ambient = lightAmbient * mtlAmbient * color;
	vec3 diffuse = lightDiffuse * mtlDiffuse * color * diff;
    vec3 specular = lightSpecular * mtlSpecular * color * spec * specStrength;

    return (ambient + diffuse + specular);
}

// vec3 spot(){
	
// }

float linearDepth(float depth){
    float z = depth * 2.0 - 1.0;
    return (2.0 * near * far) / (far + near - z * (far - near));	
}

// float fog(float depth){
    
// }

void main()
{
	vec3 outputColour;
	outputColour += directional();
	outputColour += point();
	//outputColour += spot();

	vec4 withAlpha = vec4(outputColour, texture(texMap, vec2(texCoord.x, 1.0-texCoord.y)).a);

	if(withAlpha.a < 0.5)
		discard;
	
	float depth = linearDepth(gl_FragCoord.z) / (1.3 * far);

	vec3 fog = (vec3(depth) * vec3(1.0)) + ((1 - vec3(depth)) * withAlpha.rgb);

	fragColour = vec4(fog, 1.0);
}  