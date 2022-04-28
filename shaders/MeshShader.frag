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

#define SPHERE_TYPE 0
#define BOX_TYPE 1
#define ROUNDBOX_TYPE 2
#define BOXFRAME_TYPE 3
#define TORUS_TYPE 4
#define CAPPEDTORRUS_TYPE 5
#define LINK_TYPE 6
#define INFINITECYLINDER_TYPE 7
#define CONE_TYPE 8
#define INFINITECONE_TYPE 9
#define PLANE_TYPE 10
#define HEXAPRISM_TYPE 11
#define TRIPRISM_TYPE 12
#define CAPSULE_TYPE 13
#define CAPPEDCYLINDER_TYPE 14
#define ROUNDCYLINDER_TYPE 15
#define CAPPEDCONE_TYPE 16
#define SOLIDANGLE_TYPE 17
#define CUTSPHERE_TYPE 18
#define CUTHOLLOWSPHERE_TYPE 19
#define DEATHSTAR_TYPE 20
#define ROUNDCONE_TYPE 21
#define ELLIPSOID_TYPE 22
#define RHOMBUS_TYPE 23
#define OCTAHEDRON_TYPE 24
#define PYRAMID_TYPE 25
#define TRIANGLE_TYPE 26
#define QUAD_TYPE 27

#define SIMPLE_RENDER_OPTYPE 0
#define SMOOTH_RENDER_OPTYPE 1
#define SUBTRACT_RENDER_OPTYPE 2
#define SMOOTH_SUBTRACT_RENDER_OPTYPE 3
#define INTERSECT_RENDER_OPTYPE 4
#define SMOOTH_INTERSECT_RENDER_OPTYPE 5

struct Primitive
{
    vec4 Data1;
    vec4 Data2;

    vec3 elongation;
    float rounding;    
    
    int matID;
    int type;
    int opType;
    int symmetry;

    vec4 repetitionData;    
    vec4 repetitionBounds;    
    
    mat4 transform;
};

struct RenderGroup
{
    int padding0;
    int numPrimitives;
    vec2 padding1;
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



float dot2( in vec2 v ) { return dot(v,v); }
float dot2( in vec3 v ) { return dot(v,v); }
float ndot( in vec2 a, in vec2 b ) { return a.x*b.x - a.y*b.y; }

//Render operations

Hit opU( Hit d1, Hit d2 ){
	return (d1.distance<d2.distance) ? d1 : d2;
}

vec2 opSmoothUnion( float a, float b, float k )
{
    float h = max( k-abs(a-b), 0.0 )/k;
    float m = h*h*0.5;
    float s = m*k*(1.0/2.0);
    return (a<b) ? vec2(a-s,m) : vec2(b-s,1.0-m);    
}

float opSubtract( float d1, float d2 ){
    return max(-d1,d2);
}


float opSmoothSubtraction( float d1, float d2, float k ) {
    float h = clamp( 0.5 - 0.5*(d2+d1)/k, 0.0, 1.0 );
    return mix( d2, -d1, h ) + k*h*(1.0-h);
}

float opIntersection( float d1, float d2 ) { 
    return max(d1,d2);
}

float opSmoothIntersection( float d1, float d2, float k ) {
    float h = clamp( 0.5 - 0.5*(d2-d1)/k, 0.0, 1.0 );
    return mix( d2, d1, h ) + k*h*(1.0-h); 
}

//Primitive operations

vec4 opElongate( in vec3 p, in vec3 h )
{    
    vec3 q = abs(p)-h;
    return vec4( max(q,0.0), min(max(q.x,max(q.y,q.z)),0.0) );
}

float rounding( in float d, in float h )
{
    return d - h;
}

vec3 opSymX( in vec3 p)
{
    p.x = abs(p.x);
    return p;
}

vec3 opSymXZ( in vec3 p)
{
    p.xz = abs(p.xz);
    return p;
}

vec3 opRep( in vec3 p, in vec3 c)
{
    vec3 q = mod(p+0.5*c,c)-0.5*c;
    return q;
}


vec3 opRepLim( in vec3 p, in float c, in vec3 l)
{
    vec3 q = p-c*clamp(round(p/c),-l,l);
    return q;
}




//SDFs
float sdBoxFrame( vec3 p, vec4 data1, vec4 data2 )
{
    vec3 b = data1.xyz;
    float e = data1.w;
        p = abs(p  )-b;
    vec3 q = abs(p+e)-e;
    return min(min(
        length(max(vec3(p.x,q.y,q.z),0.0))+min(max(p.x,max(q.y,q.z)),0.0),
        length(max(vec3(q.x,p.y,q.z),0.0))+min(max(q.x,max(p.y,q.z)),0.0)),
        length(max(vec3(q.x,q.y,p.z),0.0))+min(max(q.x,max(q.y,p.z)),0.0));
}

float sdTorus( vec3 p,  vec4 data1, vec4 data2 )
{
    vec2 t = data1.xy;
    vec2 q = vec2(length(p.xz)-t.x,p.y);
    return length(q)-t.y;
}

float sdCappedTorus(in vec3 p,  vec4 data1, vec4 data2 )
{
    vec2 sc = data1.xy;
    float ra = data1.z;
    float rb = data1.w;
    p.x = abs(p.x);
    float k = (sc.y*p.x>sc.x*p.y) ? dot(p.xy,sc) : length(p.xy);
    return sqrt( dot(p,p) + ra*ra - 2.0*ra*k ) - rb;
}


float sdLink( vec3 p,  vec4 data1, vec4 data2 )
{
    float le = data1.x;
    float r1 = data1.y;
    float r2 = data1.z;
    vec3 q = vec3( p.x, max(abs(p.y)-le,0.0), p.z );
    return length(vec2(length(q.xy)-r1,q.z)) - r2;
}


float sdCylinder( vec3 p, vec4 data1, vec4 data2 )
{
    vec3 c = data1.xyz;
    return length(p.xz-c.xy)-c.z;
}


float sdCone( vec3 p, vec4 data1, vec4 data2 )
{
    vec2 c = data1.xy;
    float h = data1.z;
    
    vec2 q = h*vec2(c.x/c.y,-1.0);
        
    vec2 w = vec2( length(p.xz), p.y );
    vec2 a = w - q*clamp( dot(w,q)/dot(q,q), 0.0, 1.0 );
    vec2 b = w - q*vec2( clamp( w.x/q.x, 0.0, 1.0 ), 1.0 );
    float k = sign( q.y );
    float d = min(dot( a, a ),dot(b, b));
    float s = max( k*(w.x*q.y-w.y*q.x),k*(w.y-q.y)  );
    return sqrt(d)*sign(s);    
}


float sdPlane( vec3 p, vec4 data1, vec4 data2 )
{
    vec3 n = data1.xyz;
    float h = data1.w;
    // n must be normalized
    return dot(p,n) + h;
}


float sdHexPrism( vec3 p, vec4 data1, vec4 data2 )
{
    vec2 h = data1.xy;
    const vec3 k = vec3(-0.8660254, 0.5, 0.57735);
    p = abs(p);
    p.xy -= 2.0*min(dot(k.xy, p.xy), 0.0)*k.xy;
    vec2 d = vec2(
        length(p.xy-vec2(clamp(p.x,-k.z*h.x,k.z*h.x), h.x))*sign(p.y-h.x),
        p.z-h.y );
    return min(max(d.x,d.y),0.0) + length(max(d,0.0));
}


float sdTriPrism( vec3 p, vec4 data1, vec4 data2 )
{
    vec2 h = data1.xy;
    vec3 q = abs(p);
    return max(q.z-h.y,max(q.x*0.866025+p.y*0.5,-p.y)-h.x*0.5);
}

//MISSING PARAMS
// float sdCapsule( vec3 p, vec3 a, vec3 b, float r )
// {
//     vec3 a = data1.xyz;
//     vec3 b = vec3(data1.w, data2.x, data2.y);
//   vec3 pa = p - a, ba = b - a;
//   float h = clamp( dot(pa,ba)/dot(ba,ba), 0.0, 1.0 );
//   return length( pa - ba*h ) - r;
// }


float sdVerticalCapsule( vec3 p, vec4 data1, vec4 data2 )
{
    float h = data1.x;
    float r = data1.y;
    p.y -= clamp( p.y, 0.0, h );
    return length( p ) - r;
}


float sdCappedCylinder( vec3 p, vec4 data1, vec4 data2 )
{
    float h = data1.x;
    float r = data1.y;
    vec2 d = abs(vec2(length(p.xz),p.y)) - vec2(h,r);
    return min(max(d.x,d.y),0.0) + length(max(d,0.0));
}

//MISSING PARAMS
// float sdCappedCylinder(vec3 p, vec3 a, vec3 b, float r)
// {
//   vec3  ba = b - a;
//   vec3  pa = p - a;
//   float baba = dot(ba,ba);
//   float paba = dot(pa,ba);
//   float x = length(pa*baba-ba*paba) - r*baba;
//   float y = abs(paba-baba*0.5)-baba*0.5;
//   float x2 = x*x;
//   float y2 = y*y*baba;
//   float d = (max(x,y)<0.0)?-min(x2,y2):(((x>0.0)?x2:0.0)+((y>0.0)?y2:0.0));
//   return sign(d)*sqrt(abs(d))/baba;
// }


float sdRoundedCylinder( vec3 p, vec4 data1, vec4 data2 )
{
    float ra = data1.x;
    float rb = data1.y;
    float h = data1.z;
    vec2 d = vec2( length(p.xz)-2.0*ra+rb, abs(p.y) - h );
    return min(max(d.x,d.y),0.0) + length(max(d,0.0)) - rb;
}


float sdCappedCone( vec3 p, vec4 data1, vec4 data2 )
{
    float h = data1.x;
    float r1 = data1.y;
    float r2 = data1.z;
    vec2 q = vec2( length(p.xz), p.y );
    vec2 k1 = vec2(r2,h);
    vec2 k2 = vec2(r2-r1,2.0*h);
    vec2 ca = vec2(q.x-min(q.x,(q.y<0.0)?r1:r2), abs(q.y)-h);
    vec2 cb = q - k1 + k2*clamp( dot(k1-q,k2)/dot2(k2), 0.0, 1.0 );
    float s = (cb.x<0.0 && ca.y<0.0) ? -1.0 : 1.0;
    return s*sqrt( min(dot2(ca),dot2(cb)) );
}

//MISSING PARAMS
// float sdCappedCone(vec3 p, vec3 a, vec3 b, float ra, float rb)
// {
//   float rba  = rb-ra;
//   float baba = dot(b-a,b-a);
//   float papa = dot(p-a,p-a);
//   float paba = dot(p-a,b-a)/baba;
//   float x = sqrt( papa - paba*paba*baba );
//   float cax = max(0.0,x-((paba<0.5)?ra:rb));
//   float cay = abs(paba-0.5)-0.5;
//   float k = rba*rba + baba;
//   float f = clamp( (rba*(x-ra)+paba*baba)/k, 0.0, 1.0 );
//   float cbx = x-ra - f*rba;
//   float cby = paba - f;
//   float s = (cbx<0.0 && cay<0.0) ? -1.0 : 1.0;
//   return s*sqrt( min(cax*cax + cay*cay*baba,
//                      cbx*cbx + cby*cby*baba) );
// }


float sdSolidAngle(vec3 p, vec4 data1, vec4 data2 )
{
    vec2 c = data1.xy;
    float ra = data1.z;
    // c is the sin/cos of the angle
    vec2 q = vec2( length(p.xz), p.y );
    float l = length(q) - ra;
    float m = length(q - c*clamp(dot(q,c),0.0,ra) );
    return max(l,m*sign(c.y*q.x-c.x*q.y));
}


float sdCutSphere( vec3 p, vec4 data1, vec4 data2 )
{
    float r = data1.x;
    float h = data1.y;
    // sampling independent computations (only depend on shape)
    float w = sqrt(r*r-h*h);

    // sampling dependant computations
    vec2 q = vec2( length(p.xz), p.y );
    float s = max( (h-r)*q.x*q.x+w*w*(h+r-2.0*q.y), h*q.x-w*q.y );
    return (s<0.0) ? length(q)-r :
            (q.x<w) ? h - q.y     :
                    length(q-vec2(w,h));
}


float sdCutHollowSphere( vec3 p,  vec4 data1, vec4 data2 )
{
    float r = data1.x;
    float h = data1.y;
    float t = data1.z;
    // sampling independent computations (only depend on shape)
    float w = sqrt(r*r-h*h);

    // sampling dependant computations
    vec2 q = vec2( length(p.xz), p.y );
    return ((h*q.x<w*q.y) ? length(q-vec2(w,h)) : 
                            abs(length(q)-r) ) - t;
}


float sdDeathStar( in vec3 p2, vec4 data1, vec4 data2 )
{
    float ra = data1.x;
    float rb = data1.y;
    float d = data1.z;
    // sampling independent computations (only depend on shape)
    float a = (ra*ra - rb*rb + d*d)/(2.0*d);
    float b = sqrt(max(ra*ra-a*a,0.0));

    // sampling dependant computations
    vec2 p = vec2( p2.x, length(p2.yz) );
    if( p.x*b-p.y*a > d*max(b-p.y,0.0) )
    return length(p-vec2(a,b));
    else
    return max( (length(p          )-ra),
                -(length(p-vec2(d,0))-rb));
}


float sdRoundCone( vec3 p, vec4 data1, vec4 data2 )
{
    float r1 = data1.x;
    float r2 = data1.y;
    float h = data1.z;
    // sampling independent computations (only depend on shape)
    float b = (r1-r2)/h;
    float a = sqrt(1.0-b*b);

    // sampling dependant computations
    vec2 q = vec2( length(p.xz), p.y );
    float k = dot(q,vec2(-b,a));
    if( k<0.0 ) return length(q) - r1;
    if( k>a*h ) return length(q-vec2(0.0,h)) - r2;
    return dot(q, vec2(a,b) ) - r1;
}


//MISSOING
// float sdRoundCone(vec3 p, vec3 a, vec3 b, float r1, float r2)
// {
//   // sampling independent computations (only depend on shape)
//   vec3  ba = b - a;
//   float l2 = dot(ba,ba);
//   float rr = r1 - r2;
//   float a2 = l2 - rr*rr;
//   float il2 = 1.0/l2;
    
//   // sampling dependant computations
//   vec3 pa = p - a;
//   float y = dot(pa,ba);
//   float z = y - l2;
//   float x2 = dot2( pa*l2 - ba*y );
//   float y2 = y*y*l2;
//   float z2 = z*z*l2;

//   // single square root!
//   float k = sign(rr)*rr*rr*x2;
//   if( sign(z)*a2*z2>k ) return  sqrt(x2 + z2)        *il2 - r2;
//   if( sign(y)*a2*y2<k ) return  sqrt(x2 + y2)        *il2 - r1;
//                         return (sqrt(x2*a2*il2)+y*rr)*il2 - r1;
// }


float sdEllipsoid( vec3 p,  vec4 data1, vec4 data2 )
{
    vec3 r = data1.xyz;
    float k0 = length(p/r);
    float k1 = length(p/(r*r));
    return k0*(k0-1.0)/k1;
}



float sdRhombus(vec3 p,  vec4 data1, vec4 data2 )
{
    float la = data1.x;
    float lb = data1.y;
    float h = data1.z;
    float ra = data1.w;
    p = abs(p);
    vec2 b = vec2(la,lb);
    float f = clamp( (ndot(b,b-2.0*p.xz))/dot(b,b), -1.0, 1.0 );
    vec2 q = vec2(length(p.xz-0.5*b*vec2(1.0-f,1.0+f))*sign(p.x*b.y+p.z*b.x-b.x*b.y)-ra, p.y-h);
    return min(max(q.x,q.y),0.0) + length(max(q,0.0));
}



float sdOctahedron( vec3 p,   vec4 data1, vec4 data2 )
{
    float s = data1.x;
    p = abs(p);
    return (p.x+p.y+p.z-s)*0.57735027;
}


float sdPyramid( vec3 p, vec4 data1, vec4 data2 )
{
    float h = data1.x;
    float m2 = h*h + 0.25;

    p.xz = abs(p.xz);
    p.xz = (p.z>p.x) ? p.zx : p.xz;
    p.xz -= 0.5;

    vec3 q = vec3( p.z, h*p.y - 0.5*p.x, h*p.x + 0.5*p.y);

    float s = max(-q.x,0.0);
    float t = clamp( (q.y-0.5*p.z)/(m2+0.25), 0.0, 1.0 );

    float a = m2*(q.x+s)*(q.x+s) + q.y*q.y;
    float b = m2*(q.x+0.5*t)*(q.x+0.5*t) + (q.y-m2*t)*(q.y-m2*t);

    float d2 = min(q.y,-q.x*m2-q.y*0.5) > 0.0 ? 0.0 : min(a,b);

    return sqrt( (d2+q.z*q.z)/m2 ) * sign(max(q.z,-p.y));
}


// float udTriangle( vec3 p, vec3 a, vec3 b, vec3 c )
// {
//   vec3 ba = b - a; vec3 pa = p - a;
//   vec3 cb = c - b; vec3 pb = p - b;
//   vec3 ac = a - c; vec3 pc = p - c;
//   vec3 nor = cross( ba, ac );

//   return sqrt(
//     (sign(dot(cross(ba,nor),pa)) +
//      sign(dot(cross(cb,nor),pb)) +
//      sign(dot(cross(ac,nor),pc))<2.0)
//      ?
//      min( min(
//      dot2(ba*clamp(dot(ba,pa)/dot2(ba),0.0,1.0)-pa),
//      dot2(cb*clamp(dot(cb,pb)/dot2(cb),0.0,1.0)-pb) ),
//      dot2(ac*clamp(dot(ac,pc)/dot2(ac),0.0,1.0)-pc) )
//      :
//      dot(nor,pa)*dot(nor,pa)/dot2(nor) );
// }


// float udQuad( vec3 p, vec3 a, vec3 b, vec3 c, vec3 d )
// {
//   vec3 ba = b - a; vec3 pa = p - a;
//   vec3 cb = c - b; vec3 pb = p - b;
//   vec3 dc = d - c; vec3 pc = p - c;
//   vec3 ad = a - d; vec3 pd = p - d;
//   vec3 nor = cross( ba, ad );

//   return sqrt(
//     (sign(dot(cross(ba,nor),pa)) +
//      sign(dot(cross(cb,nor),pb)) +
//      sign(dot(cross(dc,nor),pc)) +
//      sign(dot(cross(ad,nor),pd))<3.0)
//      ?
//      min( min( min(
//      dot2(ba*clamp(dot(ba,pa)/dot2(ba),0.0,1.0)-pa),
//      dot2(cb*clamp(dot(cb,pb)/dot2(cb),0.0,1.0)-pb) ),
//      dot2(dc*clamp(dot(dc,pc)/dot2(dc),0.0,1.0)-pc) ),
//      dot2(ad*clamp(dot(ad,pd)/dot2(ad),0.0,1.0)-pd) )
//      :
//      dot(nor,pa)*dot(nor,pa)/dot2(nor) );
// }

//Shapes
float sdBox( vec3 p, vec4 data1, vec4 data2 )
{
  vec3 d = abs(p) - data1.xyz;
  return min(max(d.x,max(d.y,d.z)),0.0) +
         length(max(d,0.0));
}

float sdSphere( vec3 p, vec4 data1, vec4 data2 )
{
  return length(p)-data1.x;
}

float sdRoundBox( vec3 p, vec4 data1, vec4 data2 )
{
  vec3 q = abs(p) - data1.xyz;
  return length(max(q,0.0)) + min(max(q.x,max(q.y,q.z)),0.0) - data1.w;
}


#define RAYMARCH_PRIM(pos, groupInx, primInx, res, sdFunc) \
{ \
    mat4 groupTransform = RenderGroups[groupInx].transform; \
    int primOpType = Primitives[primInx].opType; \
    Material primMat = Materials[Primitives[primInx].matID]; \
    \
    vec3 posTmp = pos;\
    if(Primitives[primInx].symmetry==1) posTmp.x = abs(posTmp.x);\
    if(Primitives[primInx].symmetry==2) posTmp.z = abs(posTmp.z);\
    if(Primitives[primInx].symmetry==3) posTmp.y = abs(posTmp.y);\
    if(Primitives[primInx].symmetry==4) posTmp.xz = abs(posTmp.xz);\
    if(Primitives[primInx].symmetry==5) posTmp.xy = abs(posTmp.xy);\
    if(Primitives[primInx].symmetry==6) posTmp.yz = abs(posTmp.yz);\
    if(Primitives[primInx].symmetry==7) posTmp = abs(posTmp);\
    \
    vec3 transformedPos = (groupTransform * Primitives[primInx].transform * vec4(posTmp,1)).xyz; \
    if(Primitives[primInx].repetitionData.w==1) transformedPos = opRepLim( transformedPos,Primitives[primInx].repetitionBounds.w, Primitives[primInx].repetitionBounds.xyz);\
    if(Primitives[primInx].repetitionData.w==2) transformedPos = opRep( transformedPos, Primitives[primInx].repetitionData.xyz);\
    \
    vec4 w = opElongate( transformedPos, Primitives[primInx].elongation );\
    transformedPos = w.xyz;\
    float addend=w.w;\
    float roundingFactor = Primitives[primInx].rounding;\
    \
    if(primOpType == SIMPLE_RENDER_OPTYPE) \
    { \
        res = opU( \
            Hit(addend+rounding(\
                sdFunc( transformedPos, Primitives[primInx].Data1, Primitives[primInx].Data2), \
                roundingFactor),\
                primMat.color),  \
            res); \
    } \
    else if(primOpType == SMOOTH_RENDER_OPTYPE) \
    { \
        vec2 smoothOp= opSmoothUnion ( \
            addend + rounding(\
                sdFunc( transformedPos, Primitives[primInx].Data1, Primitives[primInx].Data2), \
                roundingFactor),  \
            res.distance,  \
            1); \
        res.distance = smoothOp.x; \
        res.color =  mix(primMat.color, res.color, smoothOp.y); \
    } \
    else if(primOpType == SUBTRACT_RENDER_OPTYPE) \
    { \
        float subtractOp = opSubtract ( \
            addend + rounding(\
                sdFunc( transformedPos, Primitives[primInx].Data1, Primitives[primInx].Data2), \
                roundingFactor),  \
            res.distance); \
        res.distance = subtractOp; \
        res.color =  primMat.color; \
    } \
    else if(primOpType == SMOOTH_SUBTRACT_RENDER_OPTYPE) \
    { \
        float subtractOp = opSmoothSubtraction ( \
            addend + rounding(\
                sdFunc( transformedPos, Primitives[primInx].Data1, Primitives[primInx].Data2), \
                roundingFactor),  \
            res.distance, \
            1); \
        res.distance = subtractOp; \
        res.color =  primMat.color; \
    } \
    else if(primOpType == INTERSECT_RENDER_OPTYPE) \
    { \
        float intersectOp = opIntersection ( \
            addend + rounding(\
                sdFunc( transformedPos, Primitives[primInx].Data1, Primitives[primInx].Data2), \
                roundingFactor),  \
            res.distance); \
        res.distance = intersectOp; \
        res.color =  primMat.color; \
    } \
    else if(primOpType == SMOOTH_INTERSECT_RENDER_OPTYPE) \
    { \
        float intersectOp = opSmoothIntersection ( \
            addend + rounding(\
                sdFunc( transformedPos, Primitives[primInx].Data1, Primitives[primInx].Data2), \
                roundingFactor),  \
            res.distance, \
            1); \
        res.distance = intersectOp; \
        res.color =  primMat.color; \
    } \
} \

//--------------------------------
// Modelling 
//--------------------------------
Hit RayMarch( vec3 pos ){  
    Hit res = Hit(MAX_TRACE_DISTANCE, vec3(0,0,0));
    
    for(int i=0; i<numGroups; i++)
    {
        mat4 groupTransform = RenderGroups[i].transform;
        
        for(int j=0; j<RenderGroups[i].numPrimitives; j++)
        {
            int primInx = RenderGroups[i].primitives[j];
            Material primMat = Materials[Primitives[primInx].matID];
            if(Primitives[primInx].type == SPHERE_TYPE)             { 
                RAYMARCH_PRIM(pos, i, primInx,  res,  sdSphere); 
            }
            else if(Primitives[primInx].type == BOX_TYPE) { 
                RAYMARCH_PRIM(pos, i, primInx,  res,  sdBox);
            }
            else if(Primitives[primInx].type == ROUNDBOX_TYPE) { 
                RAYMARCH_PRIM(pos, i, primInx,  res,  sdRoundBox);             
            }
            else if(Primitives[primInx].type == BOXFRAME_TYPE) { 
                RAYMARCH_PRIM(pos, i, primInx,  res,  sdBoxFrame);
            }
            else if(Primitives[primInx].type == TORUS_TYPE) { 
                RAYMARCH_PRIM(pos, i, primInx,  res,  sdTorus);
            }
            else if(Primitives[primInx].type == CAPPEDTORRUS_TYPE) { 
                RAYMARCH_PRIM(pos, i, primInx,  res,  sdCappedTorus);
            }
            else if(Primitives[primInx].type == LINK_TYPE) { 
                RAYMARCH_PRIM(pos, i, primInx,  res,  sdLink);
            }
            else if(Primitives[primInx].type == INFINITECYLINDER_TYPE) { 
                RAYMARCH_PRIM(pos, i, primInx,  res,  sdCylinder);
            }
            else if(Primitives[primInx].type == CONE_TYPE) { 
                RAYMARCH_PRIM(pos, i, primInx,  res,  sdCone);
            }
            else if(Primitives[primInx].type == PLANE_TYPE) { 
                RAYMARCH_PRIM(pos, i, primInx,  res,  sdPlane);
            }
            else if(Primitives[primInx].type == HEXAPRISM_TYPE) { 
                RAYMARCH_PRIM(pos, i, primInx,  res,  sdHexPrism);
            }
            else if(Primitives[primInx].type == TRIPRISM_TYPE) { 
                RAYMARCH_PRIM(pos, i, primInx,  res,  sdTriPrism);
            }
            else if(Primitives[primInx].type == CAPPEDCYLINDER_TYPE) { 
                RAYMARCH_PRIM(pos, i, primInx,  res,  sdCappedCylinder);
            }
            else if(Primitives[primInx].type == ROUNDCYLINDER_TYPE) { 
                RAYMARCH_PRIM(pos, i, primInx,  res,  sdRoundedCylinder);
            }
            else if(Primitives[primInx].type == CAPPEDCONE_TYPE) { 
                RAYMARCH_PRIM(pos, i, primInx,  res,  sdCappedCone);
            }
            else if(Primitives[primInx].type == SOLIDANGLE_TYPE) { 
                RAYMARCH_PRIM(pos, i, primInx,  res,  sdSolidAngle);
            }
            else if(Primitives[primInx].type == CUTSPHERE_TYPE) { 
                RAYMARCH_PRIM(pos, i, primInx,  res,  sdCutSphere);
            }
            else if(Primitives[primInx].type == CUTHOLLOWSPHERE_TYPE) { 
                RAYMARCH_PRIM(pos, i, primInx,  res,  sdCutHollowSphere);
            }
            else if(Primitives[primInx].type == DEATHSTAR_TYPE) { 
                RAYMARCH_PRIM(pos, i, primInx,  res,  sdDeathStar);
            }
            else if(Primitives[primInx].type == ROUNDCONE_TYPE) { 
                RAYMARCH_PRIM(pos, i, primInx,  res,  sdRoundCone);
            }
            else if(Primitives[primInx].type == ELLIPSOID_TYPE) { 
                RAYMARCH_PRIM(pos, i, primInx,  res,  sdEllipsoid);
            }
            else if(Primitives[primInx].type == RHOMBUS_TYPE) { 
                RAYMARCH_PRIM(pos, i, primInx,  res,  sdRhombus);
            }
            else if(Primitives[primInx].type == OCTAHEDRON_TYPE) { 
                RAYMARCH_PRIM(pos, i, primInx,  res,  sdOctahedron);
            }
            else if(Primitives[primInx].type == PYRAMID_TYPE) { 
                RAYMARCH_PRIM(pos, i, primInx,  res,  sdPyramid);
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
    const float h = 0.0001; // replace by an appropriate value
    const vec2 k = vec2(1,-1);
    return normalize( k.xyy*RayMarch( pos + k.xyy*h ).distance + 
                      k.yyx*RayMarch( pos + k.yyx*h ).distance + 
                      k.yxy*RayMarch( pos + k.yxy*h ).distance + 
                      k.xxx*RayMarch( pos + k.xxx*h ).distance );
}

float shadow( in vec3 ro, in vec3 rd, float mint, float maxt, float k )
{
    float res = 1.0;
    for( float t=mint; t<maxt; )
    {
        float h = RayMarch(ro + rd*t).distance;
        if( h<0.001 )
            return 0.0;
        res = min( res, k*h/t );
        t += h;
    }
    return res;
}



vec3 render( Hit res , Ray ray ){
//   vec3 color =texture(iChannel0, ray.direction).rgb;
  vec3 color =vec3(1,1,1);
  vec3 lightPos = vec3( 50 , 50. , 50. );
    
    
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
	
    float sh = shadow( pos, lightDir, 0.01, 10, 32);
    
    vec3 matColor = res.color;

    // color = matColor * diffuse + specular;
    color = matColor * diffuse * sh;
  }
   
  return color;
}



void main()
{
    vec2 d = (2.0 * fragUv - 1.0);
    
    float imageAspectRatio = resolution.x / resolution.y; // assuming width > height 
    float camLength = 1.0 / tan(camFov / 2); 
    vec4 rayDir = normalize(vec4(d.x * imageAspectRatio, d.y, -camLength, 0));
    rayDir = (cameraMatrix * rayDir);    

    Ray ray = Ray(camPosition, rayDir.xyz);
    Hit res = calcIntersection(ray);    
	
    vec3 color = render( res ,ray );
    
	outputColor = vec4(color,1.0);
}