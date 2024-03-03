
// Most of this just copied from 
// https://landelare.github.io/2023/02/18/dxr-tutorial.html#shader.hlsl
struct Payload
{
    float16_t3 color;
    uint16_t layer;
};

/*float4 frag(v2f IN, uint primID : SV_PrimitiveID) : SV_Target
{
// Crappy "random"
    float randPerPrim = frac((float) primID * 113.171);
    return randPerPrim;
}*/

struct Vert
{
    float3 pos;
    float3 norm;
};
struct Tringle
{
    Vert verts[3];
};

RaytracingAccelerationStructure scene : register(t0);
StructuredBuffer<Tringle> geomdata : register(t1);
RWTexture2D<float16_t4> colorOut : register(u0);

[shader("raygeneration")]
void RayGeneration()
{

    float3 camera = float3(0, 0, 30);

    uint2 idx = DispatchRaysIndex().xy;
    float2 size = DispatchRaysDimensions().xy;

    float2 uv = idx / size;
    float3 target = float3((uv.x * 2 - 1) * 1.8 * (size.x / size.y),
                           (1 - uv.y) * 4 - 2 + camera.y,
                           0);
    target *= 1.5f;

    RayDesc ray;
    ray.Origin = camera;
    ray.Direction = normalize(target - camera);
    ray.TMin = 0.001;
    ray.TMax = 100;

    Payload payload;
    payload.layer = 0;

    TraceRay(scene, RAY_FLAG_FORCE_OPAQUE, 0xFF, 0, 0, 0, ray, payload);

    colorOut[idx] = float16_t4(payload.color.x, payload.color.y, payload.color.z, 1.0);
}

[shader("miss")]
void Miss(inout Payload payload)
{
    payload.color = float16_t3(0.1, 0.1, 0.1);
}

#define IOR 1.2

void RefractHit(inout Payload payload, in float3 normal)
{
    bool inside = dot(WorldRayDirection(), normal) > 0;
        
    float3 pos = WorldRayOrigin() + WorldRayDirection() * RayTCurrent();
    float3 refracted = refract(normalize(WorldRayDirection()), normal, inside ? IOR : 1.0 / IOR);
    
    RayDesc ray;
    ray.Origin = pos - (normal * 0.0001);
    ray.Direction = refracted;
    ray.TMin = 0.001;
    ray.TMax = 100;
    TraceRay(scene, RAY_FLAG_NONE, 0xFF, 0, 0, 0, ray, payload);
}

void MirrorHit(inout Payload payload, in float3 normal)
{
    float3 pos = WorldRayOrigin() + WorldRayDirection() * RayTCurrent();
    
    float3 reflected = reflect(normalize(WorldRayDirection()), normal);
    
    RayDesc mirrorRay;
    mirrorRay.Origin = pos + (normal * 0.0001);
    mirrorRay.Direction = reflected;
    mirrorRay.TMin = 0.001;
    mirrorRay.TMax = 100;
    
    //TraceRay(scene, RAY_FLAG_NONE, 0xFF, 0, 0, 0, mirrorRay, payload);
}


[shader("closesthit")]
void ClosestHit(inout Payload payload, in BuiltInTriangleIntersectionAttributes attrib)
{
    payload.layer++;
    Tringle tri = geomdata[PrimitiveIndex()];
    //float3 pos = WorldRayOrigin() + WorldRayDirection() * RayTCurrent();
    float3 normal = tri.verts[0].norm;//    (normalize(mul(float3(0, 1, 0), (float3x3) ObjectToWorld4x3())));
    
    float3 color = abs(normal);
    payload.color = float16_t3(color.x, color.y, color.z);
    return;
    
    if(payload.layer > 3)
    {
        payload.color = float16_t3(1.0, 0.0, 0.0);
        return;
    }
    //return;
    switch (InstanceID())
    {
        case 0: RefractHit(payload, normal); break;
        default: payload.color = float16_t3(color.x, color.y, color.z); break;
    }
}
