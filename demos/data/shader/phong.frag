#version 130
 
out vec4 vFragColor;

uniform vec4 ambientColor;
uniform vec4 diffuseColor;
uniform vec4 specularColor;
uniform sampler2D colorMap;

smooth in vec3 vVaryingNormal;
smooth in vec3 vVaryingLightDir;
smooth in vec2 vTexCoords;

void main(void)
{  
    // dot product gives us diffuse intensity
    float diff = max(0.0, dot(normalize(vVaryingNormal), 
                              normalize(vVaryingLightDir)));

    // multiply intensity by diffuse color
    vFragColor = diff * diffuseColor;

    // add ambient
    vFragColor += ambientColor;

    // modulate in texture
    vFragColor *= texture(colorMap, vTexCoords);

    // calculate specular highlight
    vec3 vReflection = normalize(reflect(-normalize(vVaryingLightDir), 
                                          normalize(vVaryingNormal)));
    float spec = max(0.0, dot(normalize(vVaryingNormal), vReflection));

    // if diffuse is zero, dont bother with calculation
    if (diff != 0) {
        float fSpec = pow(spec, 128.0);
        vFragColor.rgb += vec3(fSpec, fSpec, fSpec);
    }
}