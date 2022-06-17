sampler samp;
textureCUBE envMap;
matrix vpInv;
matrix viewInvMtx;
float3 cam_holePos;
float holePos_e;
float mass;
float mass_e;

struct PSInput
{
	float4 pos : SV_POSITION;
	float3 tex : TEXCOORD0;
};

float fun(float w, float M, float b) {
	return 1 - w * w * (1 - w * 2 * M / b);
}

float dfun(float w, float M, float b) {
	return -2 * w * (b - 3 * M * w) / b;
}

float fun2(float w, float M, float b) {
	return pow(fun(w, M, b), -0.5f);
}

float FindW1(float M, float b)
{
	float f0 = fun(0, M, b);

	float w = 0.5f;
	float fw = fun(w, M, b);
	float diff = 0.5f;

	int i = 0;
	while (f0 * fw > 0.0f)
	{
		if (i == 100)
			return -1.0f;
		i++;

		w = w + diff;
		fw = fun(w, M, b);
		if (i % 10 == 0) diff = diff * 2;
	}

	i = 0;

	while (abs(fw) > 0.0001f)
	{
		if (i == 100)
			return -1.0f;
		i++;

		w = w - fw / dfun(w, M, b);
		fw = fun(w, M, b);
	}

	return w;
}

float Integrate(float w1, float M, float b) {
	int N = 500;
	float h = w1 / N;
	float sum = 0.0f;
	float podstawa_a = fun2(0.0f, M, b), podstawa_b = 0.0f;

	for (int i = 1; i < N; i++)
	{
		podstawa_b = fun2(i * h, M, b);
		sum += (podstawa_a + podstawa_b);
		podstawa_a = podstawa_b;
	}

	return 0.5f * sum * h;
}

matrix RotationMatrix(float3 axis, float angle) {
	axis = normalize(axis);
	float s = sin(angle);
	float c = cos(angle);
	float oc = 1.0 - c;

	return matrix(oc * axis.x * axis.x + c, oc * axis.x * axis.y - axis.z * s, oc * axis.z * axis.x + axis.y * s, 0.0,
		oc * axis.x * axis.y + axis.z * s, oc * axis.y * axis.y + c, oc * axis.y * axis.z - axis.x * s, 0.0,
		oc * axis.z * axis.x - axis.y * s, oc * axis.y * axis.z + axis.x * s, oc * axis.z * axis.z + c, 0.0,
		0.0, 0.0, 0.0, 1.0);
}

float3 Rotate(float3 vec, float3 axis, float angle) {
	matrix rot = RotationMatrix(axis, angle);
	return mul(rot, float4(vec, 0.0f));
}

float dist(float3 a, float3 dir, float3 p)
{
	float d = length(cross(p - a, dir));
	return d;
}

float4 main(PSInput i) : SV_TARGET
{
	//convert screenPos to range [-1;1]
	float x_res = 1366.0f;
	float y_res = 768.0f;
	float x = i.pos.x / x_res * 2.0f - 1.0f;
	float y = i.pos.y / y_res * 2.0f - 1.0f;

	//compute ray in world coords
	float4 to = mul(vpInv,float4(x, -y, 1.0f, 1.0f));
	float4 from = mul(vpInv,float4(x, -y, 0.0f, 1.0f));
	to /= to.w;
	from /= from.w;
	float3 dir = normalize(to.xyz - from.xyz);

	//find (world coords) hole position based on cam position
	float3 camX = float3(viewInvMtx._11, viewInvMtx._21, viewInvMtx._31);
	float3 camY = float3(viewInvMtx._12, viewInvMtx._22, viewInvMtx._32);
	float3 camDir = float3(viewInvMtx._13, viewInvMtx._23, viewInvMtx._33);
	float3 camPos = float3(viewInvMtx._14, viewInvMtx._24, viewInvMtx._34);
	float3 cam_holePos_t = cam_holePos * pow(10.0f, holePos_e);
	float3 holePos = camPos
		+ cam_holePos_t.x * normalize(camX)
		+ cam_holePos_t.y * normalize(camY)
		+ cam_holePos_t.z * normalize(camDir)
		;

	//ray to hole dist
	float d = dist(from, dir, holePos.xyz);

	//test
	//if (d < 1e7f)
	//	return float4(0.0f, 0.0f, 1.0f, 1.0f);

	float m = mass * pow(10.0f, mass_e);
	float w1 = FindW1(m, d);
	if (w1 < 0.0f) return float4(0.0f,0.0f,0.0f, 1.0f);
	float integr = Integrate(w1, m, d);
	float Pi = 3.14159265358979323846f;
	float angle = 2 * integr - Pi;
	if (abs(angle) > Pi || isnan(angle) || isinf(angle)) {
		return float4(0.0f, 0.0f, 0.0f, 1.0f);
	}

	float3 axis = cross(normalize(holePos - camPos), dir);
	float3 newDir = normalize(Rotate(dir, axis, angle));
	float3 color = envMap.Sample(samp, newDir).rgb;

	return float4(color, 1.0f);
}
