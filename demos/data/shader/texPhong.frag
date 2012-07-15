#version 130

out vec4 vFragColor;

uniform vec4 ambient;
uniform vec4 diffuse;
uniform vec4 specular;
uniform sampler2D texSampler;

smooth in vec3 vVaryingNormal;
smooth in vec3 vVaryingLightDir;
smooth in vec2 vVaryingTexCoord;

void main(void)
{  
    // dot product gives us diffuse intensity
    float diff = max(0.0, dot(normalize(vVaryingNormal), 
                              normalize(vVaryingLightDir)));

    // multiply intensity by diffuse color
    vFragColor = diff * diffuse;

    // add ambient
    vFragColor += ambient;

    // modulate in texture
    vFragColor *= texture(texSampler, vVaryingTexCoord);

    // calculate specular highlight
    vec3 vReflection = normalize(reflect(-normalize(vVaryingLightDir), 
                                          normalize(vVaryingNormal)));
    float spec = max(0.0, dot(normalize(vVaryingNormal), vReflection));

    // if diffuse is zero, dont bother with calculation
    if (diff != 0) {
        float fSpec = pow(spec, 128);
        vFragColor.rgb += vec3(fSpec, fSpec, fSpec) * specular.rgb;
    }
}
