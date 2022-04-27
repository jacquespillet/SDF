#version 430 core
layout(location = 0) out vec4 outputColor; 

in vec2 fragUv;

uniform vec3 camPosition;
uniform mat4 cameraMatrix;

uniform float camFov;
uniform vec2 resolution;

const float MAX_TRACE_DISTANCE = 100.0;           // max trace distance
const float INTERSECTION_PRECISION = 0.001;        // precision of the intersection
const int NUM_OF_TRACE_STEPS = 100;



struct Ray {
	vec3 origin;
    vec3 direction;
};

struct Hit
{
    float distance;
    vec3 color;
};

struct Material
{
    vec3 color;
    float padding;
};

#define SphereType 0
#define BoxType 1

#define SIMPLE_RENDER_OPTYPE 0
#define SMOOTH_RENDER_OPTYPE 1

struct Primitive
{
    vec4 Data1;
    vec2 Data2;
    int matID;
    int Type;
    mat4 transform;
};

struct RenderGroup
{
    int renderOpType;
    int numPrimitives;
    vec2 padding;
    mat4 transform;
    int primitives[256];
};


uniform int numPrimitives;
uniform int numGroups;

layout (std430, binding = 0) buffer PrimitivesBuffer {
   Primitive Primitives [];
};

layout (std430, binding = 2) buffer MaterialsBuffer {
   Material Materials [];
};

layout (std430, binding = 3) buffer renderGroupsBuffer {
   RenderGroup RenderGroups [];
};


// checks to see which intersection is closer
Hit opU( Hit d1, Hit d2 ){
	return (d1.distance<d2.distance) ? d1 : d2;
}

//Operations
float opTest( float d1, float d2, float k ){
    float h = clamp( 0.5 + 0.5*(d2-d1)/k, 0.0, 1.0 );
    return mix( d2, d1, h ) - k*h*(1.0-h); 
}

vec2 opSmoothUnion( float a, float b, float k )
{
    float h = max( k-abs(a-b), 0.0 )/k;
    float m = h*h*0.5;
    float s = m*k*(1.0/2.0);
    return (a<b) ? vec2(a-s,m) : vec2(b-s,1.0-m);    
}

//Shapes
float sdBox( vec3 p, vec3 b )
{
  vec3 d = abs(p) - b;
  return min(max(d.x,max(d.y,d.z)),0.0) +
         length(max(d,0.0));
}

float sdSphere( vec3 p, float s )
{
  return length(p)-s;
}

float sdRoundBox( vec3 p, vec3 b, float r )
{
  vec3 q = abs(p) - b;
  return length(max(q,0.0)) + min(max(q.x,max(q.y,q.z)),0.0) - r;
}




//--------------------------------
// Modelling 
//--------------------------------
Hit RayMarch( vec3 pos ){  
    Hit res = Hit(MAX_TRACE_DISTANCE, vec3(0,0,0));
    
    for(int i=0; i<numGroups; i++)
    {
        int groupOpType = RenderGroups[i].renderOpType;

        for(int j=0; j<RenderGroups[i].numPrimitives; j++)
        {
            int primInx = RenderGroups[i].primitives[j];
            Material primMat = Materials[Primitives[primInx].matID];
            if(Primitives[primInx].Type == SphereType)
            {
                vec3 transformedPos = (Primitives[primInx].transform * vec4(pos,1)).xyz;
                if(groupOpType == SIMPLE_RENDER_OPTYPE)
                {
                    res = opU(
                        Hit(sdSphere( transformedPos, Primitives[primInx].Data1.x), primMat.color), 
                        res);
                }
                else if(groupOpType == SMOOTH_RENDER_OPTYPE)
                {
                    vec2 smoothOp= opSmoothUnion (
                        sdSphere( transformedPos, Primitives[primInx].Data1.x), 
                        res.distance, 
                        1);
                    res.distance = smoothOp.x;
                    res.color =  mix(primMat.color, res.color, smoothOp.y);
                }
            }
            else if(Primitives[primInx].Type == BoxType)
            {
                vec3 transformedPos = (Primitives[primInx].transform * vec4(pos,1)).xyz;

                if(groupOpType == SIMPLE_RENDER_OPTYPE)
                {
                    res = opU(
                        Hit(sdBox( transformedPos, Primitives[primInx].Data1.xyz),  primMat.color), 
                        res);
                }
                else if(groupOpType == SMOOTH_RENDER_OPTYPE)
                {
                    vec2 smoothOp= opSmoothUnion (
                        sdBox( transformedPos, Primitives[primInx].Data1.xyz), 
                        res.distance, 
                        1);
                    res.distance = smoothOp.x;
                    res.color =  mix(primMat.color, res.color, smoothOp.y);
                }                    
            }
        }
    }
    return res;
}



Hit calcIntersection( in Ray ray ){
    float h =  INTERSECTION_PRECISION*2.0;
    float t = 0.0;
	float res = -1.0;
    vec3 color = vec3(0,0,0);
    
    for( int i=0; i< NUM_OF_TRACE_STEPS ; i++ ){
        if( h < INTERSECTION_PRECISION || t > MAX_TRACE_DISTANCE ) break;
	   	
        //Calculate closest distance from the shapes
        Hit m = RayMarch( ray.origin+ray.direction*t );
        
        //Set marching distance 
        h = m.distance;

        //March forward by this distance
        t += h;

        //Sets out color
        color = m.color;
    }

    if( t < MAX_TRACE_DISTANCE ) res = t;
    if( t > MAX_TRACE_DISTANCE ) color =vec3(0,0,0);
    
    return Hit(res , color );
}



// Calculates the normal by taking a very small distance,
// remapping the function, and getting normal for that
vec3 calcNormal( in vec3 pos ){
    
	vec3 eps = vec3( 0.001, 0.0, 0.0 );
	vec3 nor = vec3(
	    RayMarch(pos+eps.xyy).distance - RayMarch(pos-eps.xyy).distance,
	    RayMarch(pos+eps.yxy).distance - RayMarch(pos-eps.yxy).distance,
	    RayMarch(pos+eps.yyx).distance - RayMarch(pos-eps.yyx).distance );
	return normalize(nor);
}




vec3 render( Hit res , Ray ray ){
//   vec3 color =texture(iChannel0, ray.direction).rgb;
  vec3 color =vec3(1,1,1);
  vec3 lightPos = vec3( 5 , 5. , 5. );
    
    
  if( length(res.color) > 0.0){
    vec3 pos = ray.origin + ray.direction * res.distance;
    vec3 norm = calcNormal( pos );
      
    vec3 lightDir = normalize( lightPos - pos );
    vec3 fragToCam = normalize( pos - ray.origin );
    
    float diffuse = max( 0. , dot( lightDir , norm ));
      
    vec3 halfwayVec = normalize(lightDir + fragToCam);
    
    // vec4 tex = texture(iChannel0, norm);
    vec4 tex = vec4(1,1,1,1);
    vec3 specular =  vec3(tex) * pow(max(dot(norm.xyz, halfwayVec),0.0), 1.0);
	
    
    vec3 matColor = res.color;

    color = matColor * diffuse + specular;
  }
   
  return color;
}



void main()
{
    vec2 d = (2.0 * fragUv - 1.0);
    
    // float scale = tan(camFov * 0.5);
    // d.y *= resolution.y / resolution.x * scale;
    // d.x *= scale;
    // vec3 rayDir = normalize(d.x * camRight + d.y * camUp - camForward);


    float imageAspectRatio = resolution.x / resolution.y; // assuming width > height 
    float camLength = 1.0 / tan(camFov / 2); 
    vec4 rayDir = normalize(vec4(d.x * imageAspectRatio, d.y, -camLength, 0));
    rayDir = (cameraMatrix * rayDir);    

    // float Px = (2 * ((fragUv.x + 0.5) / resolution.x) - 1) * tan(camFov / 2) * imageAspectRatio; 
    // float Py = (1 - 2 * ((fragUv.y + 0.5) / resolution.y) * tan(camFov / 2)); 
    // vec3 rayDir = normalize(Px * camRight + Py * camUp - camForward);
    // rayDir = normalize(rayDir); // it's a direction so don't forget to normalize 
    

    Ray ray = Ray(camPosition, rayDir.xyz);
    Hit res = calcIntersection(ray);    
	
    vec3 color = render( res ,ray );
    
	outputColor = vec4(color,1.0);
}