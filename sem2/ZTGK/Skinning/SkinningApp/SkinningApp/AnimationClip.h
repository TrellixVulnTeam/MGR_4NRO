#pragma once
#include <DirectXMath.h>
#include <vector>

using namespace DirectX;

namespace SkinningApp
{
	struct Keyframe
	{
		Keyframe(float _time_pos, XMFLOAT3 _position, XMFLOAT4 _rotation, XMFLOAT3 _scaling) :
			time_pos(_time_pos), position(_position), rotation(_rotation), scaling(_scaling) {}

		float time_pos;
		XMFLOAT3 position;
		XMFLOAT4 rotation;
		XMFLOAT3 scaling;
	};

	struct BoneAnimation
	{
		BoneAnimation() {};
		BoneAnimation(std::vector<Keyframe> _frames) : frames(_frames) {};

		void Interpolate(float time_pos, XMMATRIX& bone_transform);
		Keyframe GetKeyframe(float time_pos);

		float GetStartTime() const { return frames.size() == 0 ? 0 : frames.front().time_pos; };
		float GetEndTime() const { return frames.size() == 0 ? 0 : frames.back().time_pos; };

		std::vector<Keyframe> frames;
	};

	struct AnimationClip
	{
		AnimationClip() : bone_count(0) {};
		AnimationClip(int _bone_count);
		AnimationClip(std::vector<BoneAnimation> _bone_animations, int _bone_count);

		void Interpolate(float time_pos, XMMATRIX* bone_transforms);

		void SetEndTime(float value) { end_time = value; }
		float GetEndTime() const { return end_time; };

		std::vector<BoneAnimation> bone_animations;

	private:
		float end_time;
	protected:
		int bone_count;
	};
}

