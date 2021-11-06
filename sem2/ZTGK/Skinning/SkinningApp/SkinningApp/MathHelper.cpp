#include "MathHelper.h"


namespace SkinningApp
{
	XMMATRIX MathHelper::GetAffineTransformationMatrix(Transform transform)
	{
		const auto scale = transform.GetScaling();
		const auto rotation = EulerToQuaternion(transform.GetRotation());
		const auto position = transform.GetPosition();

		return XMMatrixAffineTransformation(
			XMLoadFloat3(&scale),
			XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f),
			XMLoadFloat4(&rotation),
			XMLoadFloat3(&position)
		);
	}

	XMFLOAT4 MathHelper::EulerToQuaternion(XMFLOAT3 euler)
	{
		float cy = cosf(euler.z * 0.5f);
		float sy = sinf(euler.z * 0.5f);
		float cp = cosf(euler.y * 0.5f);
		float sp = sinf(euler.y * 0.5f);
		float cr = cosf(euler.x * 0.5f);
		float sr = sinf(euler.x * 0.5f);

		return XMFLOAT4(/*x*/cy * cp * sr - sy * sp * cr,
			/*y*/sy * cp * sr + cy * sp * cr,
			/*z*/sy * cp * cr - cy * sp * sr,
			/*w*/cy * cp * cr + sy * sp * sr);
	}
}


