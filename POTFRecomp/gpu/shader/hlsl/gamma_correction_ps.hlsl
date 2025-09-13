#include "../../../../tools/XenosRecomp/XenosRecomp/shader_common.h"

#ifdef __spirv__

#define g_Gamma vk::RawBufferLoad<float>(g_PushConstants.SharedConstants + 0)
#define g_TextureDescriptorIndex vk::RawBufferLoad<uint>(g_PushConstants.SharedConstants + 4)

#define g_ViewportOffset vk::RawBufferLoad<int2>(g_PushConstants.SharedConstants + 8)
#define g_ViewportSize vk::RawBufferLoad<int2>(g_PushConstants.SharedConstants + 16)

#else

cbuffer SharedConstants : register(b2, space4)
{
    float g_Gamma;
    uint g_TextureDescriptorIndex;
    int2 g_ViewportOffset;
    int2 g_ViewportSize;
};

#endif

float4 shaderMain(in float4 position : SV_Position) : SV_Target
{
    Texture2D<float4> texture = g_Texture2DDescriptorHeap[g_TextureDescriptorIndex];
    
    int2 movedPosition = int2(position.xy) - g_ViewportOffset;
    bool boxed = any(movedPosition < 0) || any(movedPosition >= g_ViewportSize);
    if (boxed) movedPosition = 0;
    
    float4 color = boxed ? 0.0 : texture.Load(int3(movedPosition, 0));
    color.rgb = pow(color.rgb, g_Gamma);
    return color;
}
