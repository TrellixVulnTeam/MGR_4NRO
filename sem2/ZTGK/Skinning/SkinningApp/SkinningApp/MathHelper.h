#pragma once
#include "Structures.h"


namespace SkinningApp
{
	class MathHelper
	{
	public:
		static XMMATRIX GetAffineTransformationMatrix(Transform transform);
		static XMFLOAT4 EulerToQuaternion(XMFLOAT3 euler);
	};
}


