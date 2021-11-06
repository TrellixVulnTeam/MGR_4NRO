#pragma once
#include <DirectXMath.h>
#include <vector>
#include "Structures.h"

using namespace DirectX;

namespace SkinningApp
{
	struct Bone
	{
		Bone(std::string _name, int _parent_index, XMFLOAT4X4 _offset) :
			name(_name), parent_index(_parent_index), offset(_offset) {}

		std::string name;
		int parent_index;
		XMFLOAT4X4 offset;
		Transform transform;
	};

	struct Skeleton
	{
		Skeleton(std::vector<Bone>& _bones) : bones(_bones) {}

		int GetBoneCount() { return (int)bones.size(); }
		Bone& GetBone(int index) { return bones[index]; }

	private:
		std::vector<Bone> bones;
	};
}