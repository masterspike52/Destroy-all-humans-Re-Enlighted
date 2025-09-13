#include "movie_common.hlsli"

Interpolators shaderMain(in VertexShaderInput In)
{
    Interpolators Out;
    Out.ProjPos = In.ObjPos;
    Out.ProjPos.xy += g_HalfPixelOffset * Out.ProjPos.w;
    Out.UV = In.UV;
    return Out;
}
