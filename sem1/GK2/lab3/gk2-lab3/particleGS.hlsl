cbuffer cbProj : register(b0) //Geometry Shader constant buffer slot 0
{
	matrix projMatrix;
};

struct GSInput
{
	float4 pos : POSITION;
	float age : TEXCOORD0;
	float angle : TEXCOORD1;
	float size : TEXCOORD2;
};

struct PSInput
{
	float4 pos : SV_POSITION;
	float2 tex1: TEXCOORD0;
	float2 tex2: TEXCOORD1;
};

static const float TimeToLive = 4.0f;

[maxvertexcount(4)]
void main(point GSInput inArray[1], inout TriangleStream<PSInput> ostream)
{
	GSInput i = inArray[0];
	float sina, cosa;
	sincos(i.angle, sina, cosa);
	float dx = (cosa - sina) * 0.5 * i.size;
	float dy = (cosa + sina) * 0.5 * i.size;

	//lewy górny
	PSInput lg = (PSInput)0;
	lg.pos = i.pos;
	lg.pos.x += -dy;
	lg.pos.y += dx;
	lg.pos = mul(projMatrix, lg.pos);
	lg.tex1.x = 0.0f;
	lg.tex1.y = 0.0f;
	lg.tex2.x = i.age/TimeToLive;
	lg.tex2.y = 0.5f;
	
	//lewy dolny
	PSInput ld = (PSInput)0;
	ld.pos = i.pos;
	ld.pos.x += -dx;
	ld.pos.y += -dy;
	ld.pos = mul(projMatrix, ld.pos);
	ld.tex1.x = 0.0f;
	ld.tex1.y = 1.0f;
	ld.tex2.x = i.age/TimeToLive;
	ld.tex2.y = 0.5f;

	//prawy górny
	PSInput pg = (PSInput)0;
	pg.pos = i.pos;
	pg.pos.x += dx;
	pg.pos = mul(projMatrix, pg.pos);
	pg.pos.y += dy;
	pg.tex1.x = 1.0f;
	pg.tex1.y = 0.0f;
	pg.tex2.x = i.age / TimeToLive;
	pg.tex2.y = 0.5f;

	//prawy dolny
	PSInput pd = (PSInput)0;
	pd.pos = i.pos;
	pd.pos.x += dy;
	pd.pos.y -= dx;
	pd.pos = mul(projMatrix, pd.pos);
	pd.tex1.x = 1.0f;
	pd.tex1.y = 1.0f;
	pd.tex2.x = i.age / TimeToLive;
	pd.tex2.y = 0.5f;

	// TODO : 1.30 Initialize 4 vertices to make a bilboard and append them to the ostream

	ostream.Append(ld);
	ostream.Append(lg);
	ostream.Append(pd);
	ostream.Append(pg);

	ostream.RestartStrip();
}