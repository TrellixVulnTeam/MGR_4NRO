#include "VSOut.hlsli"

float4 main(VSOut i) : SV_TARGET
{
	return i.col;
}