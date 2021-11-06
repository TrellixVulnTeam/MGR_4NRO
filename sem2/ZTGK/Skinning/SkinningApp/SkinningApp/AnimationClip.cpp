#include "AnimationClip.h"



namespace SkinningApp
{
	void BoneAnimation::Interpolate(float time_pos, XMMATRIX& bone_transform)
	{
		//TODO: 16. Uzupełnić - metoda pobierą klatkę dla danej pozycji i wyznacza macierz przekształcenia

		auto frame = GetKeyframe(time_pos);

		bone_transform = XMMatrixAffineTransformation(
			XMLoadFloat3(&frame.scaling),
			XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f),
			XMLoadFloat4(&frame.rotation),
			XMLoadFloat3(&frame.position)
		);
	}

	Keyframe BoneAnimation::GetKeyframe(float time_pos)
	{
		//TODO: 15. Uzupełnić - metoda zwraca klatkę dla danej pozycji w czasie
		if (frames.size() == 0)
		{
			return Keyframe(time_pos, XMFLOAT3{ 0,0,0 }, XMFLOAT4{ 0,0,0,1 }, XMFLOAT3{ 1,1,1 });
		}

		if (frames.size() == 1 || time_pos <= GetStartTime())
		{
			return Keyframe(time_pos, frames[0].position, frames[0].rotation, frames[0].scaling);
		}

		if (time_pos >= GetEndTime()) {
			return Keyframe(time_pos, frames.back().position, frames.back().rotation, frames.back().scaling);
		}

		int ind = 0;
		while (frames[ind + 1].time_pos <= time_pos)
			ind++;

		auto f1 = frames[ind];
		auto f2 = frames[ind + 1];
		auto diff = f2.time_pos - f1.time_pos;
		auto diff1 = time_pos - f1.time_pos;
		float t = diff1 / diff;
		XMFLOAT3 pos;
		XMStoreFloat3(&pos, XMVectorLerp(XMLoadFloat3(&f1.position), XMLoadFloat3(&f2.position), t));
		XMFLOAT4 rot;
		XMStoreFloat4(&rot, XMQuaternionSlerp(XMLoadFloat4(&f1.rotation), XMLoadFloat4(&f2.rotation), t));
		XMFLOAT3 scale;
		XMStoreFloat3(&scale, XMVectorLerp(XMLoadFloat3(&f1.scaling), XMLoadFloat3(&f2.scaling), t));

		return Keyframe(time_pos, pos, rot, scale);

		// XMVectorLerp
		// XMQuaternionSlerp
	}

	AnimationClip::AnimationClip(int _bone_count) : bone_count(_bone_count)
	{
		bone_animations.resize(bone_count);
	}

	AnimationClip::AnimationClip(std::vector<BoneAnimation> _bone_animations, int _bone_count) : bone_animations(_bone_animations), bone_count(_bone_count)
	{
		end_time = bone_animations[0].GetEndTime();

		for (int i = 1; i < bone_count; i++)
		{
			float end_value = bone_animations[i].GetEndTime();

			if (end_value > end_time) end_time = end_value;
		}
	}

	void AnimationClip::Interpolate(float time_pos, XMMATRIX* bone_transforms)
	{
		for (int i = 0; i < bone_count; ++i)
		{
			bone_animations[i].Interpolate(time_pos, bone_transforms[i]);
		}
	}
}