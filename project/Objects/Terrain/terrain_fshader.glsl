#version 330

in vec2 uv;
in vec3 pos_3d;
in float height;
in vec2 displacement_vector;

uniform sampler2D tex0; // perlin texture
uniform sampler2D tex1; // perlin normalmap
uniform sampler2D tex2; // water lighting
uniform sampler2D tex3; // sand 
uniform sampler2D tex4; // grass
uniform sampler2D tex5; // rock
uniform sampler2D tex6; // snow


uniform float time;
uniform vec3 lightDirection;
uniform vec3 lightPosition;
uniform vec3 cameraPosition;
uniform vec3 La, Ld, Ls;
uniform vec3 Ma, Md, Ms;

out vec3 color;

float shineDumper = 2.0f;
float reflectivity = 0.6;

vec3 BLEND(vec3 sand, float a1 ,vec3 grass, float a2, vec3 rock, float a3, vec3 snow, float a4){
    return sand.rgb * a1 + grass.rgb * a2 + rock.rgb * a3 + snow.rgb * a4;
}

void main() {
    
    vec3 height_color;
    vec3 underwater_fix = vec3(0.0f);
    vec3 sand;
    vec3 grass;
    vec3 rock;
    vec3 snow;
    vec3 color1;
    vec3 color2;
    vec3 color3;
    vec3 color12;
    //getting the textures
    sand = texture(tex3, uv + displacement_vector).rgb;
    grass = texture(tex4, uv + displacement_vector).rgb;
    rock = texture(tex5, uv + displacement_vector).rgb;
    snow = texture(tex6, uv + displacement_vector).rgb;
    
   
    // normal caculation according to normalmap
    vec4 normalMapColor = texture(tex1, uv + displacement_vector);
    // alpha values - max height is 0.4?
    float a1 = 0.0f;
    float a2 = 0.0f;
    float a3 = 0.0f;
    float a4 = 0.0f;
    float temp_height = 0.0f; // norm height

    if (height >= 0.10f && height < 0.40f){
        //grass
        //first
        temp_height = height-0.1f;
        //interval
        a2 = exp(1.0f-(temp_height/0.30f))-1;
        a3 = exp(temp_height/0.30f)-1;

    }else if(height > 0.40f && height <= 0.6f){
        //rock
        temp_height = height-0.40f;
        a3 = exp(1.0f - (temp_height/0.20f))-1;
        a4 = exp(temp_height/0.20f)-1;
    } else if(height > 0.6f){
        //snow
        a4 = exp(1.0f)-1;

    }else{
        //sand
        a1 = exp(1.0f - (height/0.1f))-1;
        a2 = exp(height/0.1f)-1;
    }

    height_color = BLEND(sand,a1,grass,a2,rock,a3,snow,a4);    // normal caculation according to normalmap
    
    vec3 n = vec3(normalMapColor.r * 2.0f - 1.0f, normalMapColor.b, normalMapColor.g * 2.0f - 1.0f);

    vec3 l = normalize(lightDirection);

    //Now we do the difuse shading
    float temp;
    float nl = ((temp = dot(n,l)) < 0) ? 0.0f : temp;
        
    if(height < 0.0f){   
        height_color = vec3(0.9f, 0.9f, 0.0f) * pow((1.0f-gl_FragCoord.z), 0.8f);

        vec4 normalMapColor = texture(tex2, uv + displacement_vector + time/200.0f);
        vec3 normal = vec3(normalMapColor.r * 2.0f - 1.0f, normalMapColor.b, normalMapColor.g * 2.0f - 1.0f);
        normal = normalize(normal);

        vec3 reflectedLight = reflect(normalize(lightDirection), normal);
        float specular = max(dot(reflectedLight, normalize(pos_3d.xyz - cameraPosition)), 0.0f);
        specular = pow(specular, shineDumper);
        vec3 specularHighlights = Ls * specular * reflectivity;

        underwater_fix = mix(height_color, specularHighlights * (1.0f-gl_FragCoord.z), vec3(0.35));
    }
    vec3 diffuse = Md * nl * Ld * height_color + underwater_fix;
    color = diffuse.xyz;
}