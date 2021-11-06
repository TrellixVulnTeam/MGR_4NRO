#pragma once

#include <string>
#include <fbxsdk.h>
#include <vector>
#include "Skeleton.h"
#include "AnimationClip.h"
#include "Structures.h"
#include "mesh.h"
#include "dxDevice.h"
#include "vertexDef.h"

using namespace fbxsdk;
using namespace Real_Engine;

namespace SkinningApp
{
	class FbxLoader
	{
	public:
		static std::tuple<std::vector<Mesh*>, Skeleton*> LoadMesh(std::string path, DxDevice device);
		static AnimationClip* LoadAnimationClip(std::string path);

	private:
		static void LoadSkeleton(FbxNode* parent_node, std::vector<Bone>& bones, std::vector<FbxNode*>& nodes, int parent);
		static std::tuple<std::vector<VertexA>, std::vector<UINT>, std::vector<UINT>> LoadMesh(FbxMesh* mesh, std::vector<Bone>& bones, std::vector<FbxNode*>& nodes);
		static FbxAMatrix GetGeometryTransformation(FbxNode* inNode);
		static Transform GetTransform(FbxNode* node);
	};
}