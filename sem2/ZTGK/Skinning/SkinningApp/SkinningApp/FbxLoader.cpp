#include "FbxLoader.h"

namespace SkinningApp
{
	std::tuple<std::vector<Mesh*>, Skeleton*> FbxLoader::LoadMesh(std::string path, DxDevice device)
	{
		FbxManager* sdk_manager = FbxManager::Create();
		FbxIOSettings *ios = FbxIOSettings::Create(sdk_manager, IOSROOT);

		sdk_manager->SetIOSettings(ios);

		FbxImporter* importer = FbxImporter::Create(sdk_manager, "");

		if (!importer->Initialize(path.c_str(), -1, sdk_manager->GetIOSettings())) {
			printf("Call to FbxImporter::Initialize() failed.\n");
			printf("Error returned: %s\n\n", importer->GetStatus().GetErrorString());
			exit(-1);
		}

		FbxScene* scene = FbxScene::Create(sdk_manager, "myScene");

		importer->Import(scene);
		importer->Destroy();

		std::vector<Mesh*> meshes;
		std::vector<Bone> bones;
		std::vector<FbxNode*> nodes;
		Skeleton* skeleton;

		FbxNode* root_node = scene->GetRootNode();
		if (root_node)
		{
			int count = root_node->GetChildCount();

			for (int node_index = 0; node_index < count; node_index++)
			{
				FbxNode* node = root_node->GetChild(node_index);

				if (node->GetNodeAttribute() == NULL)
					continue;

				FbxNodeAttribute::EType attribute_type = node->GetNodeAttribute()->GetAttributeType();


				if (attribute_type == FbxNodeAttribute::eSkeleton)
				{
					XMFLOAT4X4 matrix;
					XMStoreFloat4x4(&matrix, XMMatrixIdentity());
					Bone bone(node->GetName(), -1, matrix);
					bone.transform = GetTransform(node);
					bones.push_back(bone);
					nodes.push_back(node);

					if (node->GetChildCount() > 0)
						LoadSkeleton(node, bones, nodes, 0);
					break;
				}
			}

			for (int node_index = 0; node_index < count; node_index++)
			{
				FbxNode* node = root_node->GetChild(node_index);

				if (node->GetNodeAttribute() == NULL)
					continue;

				FbxNodeAttribute::EType attribute_type = node->GetNodeAttribute()->GetAttributeType();

				if (attribute_type == FbxNodeAttribute::eMesh)
				{
					std::vector<VertexA> mesh_vertices;
					std::vector<UINT> mesh_indices;
					std::vector<UINT> mesh_groups;
					FbxMesh* mesh = (FbxMesh*)node->GetNodeAttribute();

					tie(mesh_vertices, mesh_indices, mesh_groups) = LoadMesh(mesh, bones, nodes);
					auto tmp = device.CreateMesh(mesh_indices, mesh_vertices);
					meshes.push_back(new Mesh(std::move(std::get<0>(tmp)), sizeof(VertexA), 0U, std::move(std::get<1>(tmp)), mesh_groups));
				}
			}

			skeleton = new Skeleton(bones);
		}

		/*		if(sdk_manager!=nullptr)
					sdk_manager->Destroy();*/


		return { meshes, skeleton };
	}

	AnimationClip* FbxLoader::LoadAnimationClip(std::string path)
	{
		FbxManager* sdk_manager = FbxManager::Create();
		FbxIOSettings *ios = FbxIOSettings::Create(sdk_manager, IOSROOT);

		sdk_manager->SetIOSettings(ios);

		FbxImporter* importer = FbxImporter::Create(sdk_manager, "");

		if (!importer->Initialize(path.c_str(), -1, sdk_manager->GetIOSettings())) {
			printf("Call to FbxImporter::Initialize() failed.\n");
			printf("Error returned: %s\n\n", importer->GetStatus().GetErrorString());
			exit(-1);
		}

		FbxScene* scene = FbxScene::Create(sdk_manager, "myScene");

		importer->Import(scene);
		importer->Destroy();

		std::vector<Bone> bones;
		std::vector<FbxNode*> nodes;

		FbxNode* root_node = scene->GetRootNode();
		if (root_node)
		{
			int count = root_node->GetChildCount();
			for (int i = 0; i < count; i++)
			{
				FbxNode* node = root_node->GetChild(i);

				if (node->GetNodeAttribute() == NULL)
					continue;

				FbxNodeAttribute::EType attribute_type = node->GetNodeAttribute()->GetAttributeType();

				if (attribute_type == FbxNodeAttribute::eNull && node->GetChildCount() > 0)
				{
					for (int index = 0; index < node->GetChildCount(); index++)
					{
						node = node->GetChild(index);

						if (node->GetNodeAttribute() == NULL)
							continue;

						attribute_type = node->GetNodeAttribute()->GetAttributeType();

						if (attribute_type == FbxNodeAttribute::eSkeleton)
							break;
					}
				}

				if (attribute_type == FbxNodeAttribute::eSkeleton)
				{
					XMFLOAT4X4 matrix;
					XMStoreFloat4x4(&matrix, XMMatrixIdentity());
					bones.push_back(Bone(node->GetName(), -1, matrix));
					nodes.push_back(node);
					if (node->GetChildCount() > 0)
						LoadSkeleton(node, bones, nodes, 0);
				}
			}

			int anim_stack_count = scene->GetSrcObjectCount<FbxAnimStack>();
			for (int i = 0; i < anim_stack_count; i++)
			{
				FbxAnimStack* anim_stack = scene->GetSrcObject<FbxAnimStack>(i);
				if (anim_stack != nullptr)
				{
					FbxString name = anim_stack->GetName();
					std::vector<BoneAnimation> bone_animations(bones.size());
					int layer_count = anim_stack->GetMemberCount<FbxAnimLayer>();
					FbxTakeInfo* take_info = scene->GetTakeInfo(name);

					FbxLongLong start_index = take_info->mLocalTimeSpan.GetStart().GetFrameCount(FbxTime::eFrames60);
					FbxLongLong end_index = take_info->mLocalTimeSpan.GetStop().GetFrameCount(FbxTime::eFrames60);

					for (int j = 0; j < layer_count; j++)
					{
						FbxAnimLayer* layer = anim_stack->GetMember<FbxAnimLayer>();

						for (int k = 0; k < bones.size(); k++)
						{
							FbxTime currTime;
							std::vector<Keyframe> frames;
							FbxNode* node = nodes[k];

							for (FbxLongLong i = start_index; i <= end_index; i++)
							{
								currTime.SetFrame(i, FbxTime::eFrames60);

								FbxVector4 tmp;
								FbxAMatrix localMatrix = node->EvaluateLocalTransform(currTime);

								tmp = localMatrix.GetT();
								XMFLOAT3 translation = XMFLOAT3((float)tmp.mData[0], (float)tmp.mData[1], (float)tmp.mData[2]);

								FbxQuaternion quat = localMatrix.GetQ();
								XMFLOAT4 rotation = XMFLOAT4((float)quat.mData[0], (float)quat.mData[1], (float)quat.mData[2], (float)quat.mData[3]);

								tmp = localMatrix.GetS();
								XMFLOAT3 scaling = XMFLOAT3((float)tmp.mData[0], (float)tmp.mData[1], (float)tmp.mData[2]);

								frames.push_back(Keyframe(static_cast<float>(currTime.GetSecondDouble()), translation, rotation, scaling));
							}

							bone_animations[k] = BoneAnimation(frames);
						}
					}
					return new AnimationClip(bone_animations, (int)bones.size());
				}
			}

		}

		/*	if(sdk_manager!=nullptr)
				sdk_manager->Destroy();
	*/

		return nullptr;
	}

	void FbxLoader::LoadSkeleton(FbxNode* parent_node, std::vector<Bone>& bones, std::vector<FbxNode*>& nodes, int parent)
	{
		int offset = (int)bones.size();
		int count = parent_node->GetChildCount();

		for (int i = 0; i < count; i++)
		{
			FbxNode* node = parent_node->GetChild(i);

			if (node->GetNodeAttribute() == NULL && node->GetNodeAttribute()->GetAttributeType() != FbxNodeAttribute::eSkeleton)
				continue;

			XMFLOAT4X4 matrix;
			XMStoreFloat4x4(&matrix, XMMatrixIdentity());
			Bone bone(node->GetName(), parent, matrix);
			bone.transform = GetTransform(node);
			bones.push_back(bone);
			nodes.push_back(node);
			LoadSkeleton(parent_node->GetChild(i), bones, nodes, (int)bones.size() - 1);
		}
	}

	std::tuple<std::vector<VertexA>, std::vector<UINT>, std::vector<UINT>> FbxLoader::LoadMesh(FbxMesh* mesh, std::vector<Bone>& bones, std::vector<FbxNode*>& nodes)
	{
		const int N = 4;
		std::vector<VertexA> vertices;
		std::vector<UINT> indices;
		std::vector<UINT> groups;

		FbxSkin* skin = nullptr;
		int deformer_count = mesh->GetDeformerCount();

		for (int i = 0; i < deformer_count; i++)
		{
			skin = reinterpret_cast<FbxSkin*>(mesh->GetDeformer(i, FbxDeformer::eSkin));

			if (skin != nullptr)
				break;
		}

		FbxGeometryElementNormal* normal_element = mesh->GetElementNormal();
		auto mode = normal_element->GetMappingMode();
		auto direct_array = normal_element->GetDirectArray();

		int* blend_count = nullptr;
		int** blend_indices = nullptr;
		float** blend_weights = nullptr;

		FbxVector4* p_vertices = mesh->GetControlPoints();

		int cp_count = mesh->GetControlPointsCount();


		if (skin != nullptr)
		{
			blend_count = new int[cp_count];
			blend_indices = new int*[cp_count];
			blend_weights = new float*[cp_count];

			for (int i = 0; i < cp_count; i++)
			{
				blend_count[i] = 0;
				blend_indices[i] = new int[N];
				blend_weights[i] = new float[N];
				for (int j = 0; j < N; j++)
				{
					blend_indices[i][j] = 0;
					blend_weights[i][j] = 0;
				}
			}

			int cluster_count = skin->GetClusterCount();

			for (int i = 0; i < cluster_count; i++)
			{
				FbxCluster* cluster = skin->GetCluster(i);
				std::string curr_joint_name = cluster->GetLink()->GetName();

				int index = -1;
				for (int j = 0; j < bones.size(); j++)
					if (curr_joint_name == bones[j].name)
					{
						index = j;
						break;
					}

				FbxAMatrix transformMatrix;
				FbxAMatrix transformLinkMatrix;
				FbxAMatrix offsetMatrix;

				cluster->GetTransformMatrix(transformMatrix);
				cluster->GetTransformLinkMatrix(transformLinkMatrix);
				offsetMatrix = transformLinkMatrix.Inverse() * transformMatrix * GetGeometryTransformation(nodes[index]);

				bones[index].offset = XMFLOAT4X4(
					(float)offsetMatrix.mData[0][0], (float)offsetMatrix.mData[0][1], (float)offsetMatrix.mData[0][2], (float)offsetMatrix.mData[0][3],
					(float)offsetMatrix.mData[1][0], (float)offsetMatrix.mData[1][1], (float)offsetMatrix.mData[1][2], (float)offsetMatrix.mData[1][3],
					(float)offsetMatrix.mData[2][0], (float)offsetMatrix.mData[2][1], (float)offsetMatrix.mData[2][2], (float)offsetMatrix.mData[2][3],
					(float)offsetMatrix.mData[3][0], (float)offsetMatrix.mData[3][1], (float)offsetMatrix.mData[3][2], (float)offsetMatrix.mData[3][3]
				);

				if (index >= 0)
				{
					int cp_count = cluster->GetControlPointIndicesCount();
					int* cp_indices = cluster->GetControlPointIndices();
					double* cp_weights = cluster->GetControlPointWeights();

					for (int j = 0; j < cp_count; j++)
					{
						int cp_index = cp_indices[j];
						int indices_count = blend_count[cp_index];

						if (indices_count < N)
						{
							blend_indices[cp_index][indices_count] = index;
							blend_weights[cp_index][indices_count] = static_cast<float>(cp_weights[j]);
							blend_count[cp_index]++;
						}
						else
						{
							float weight = static_cast<float>(cp_weights[j]) / N;

							for (int k = 0; k < N; k++)
								blend_weights[cp_index][k] += weight;
						}


					}

				}
				else
					throw "Brak szkieletu";
			}
		}
		else
			return tie(vertices, indices, groups);

		FbxGeometryElementMaterial* material_element = mesh->GetElementMaterial();
		int polygon_count = mesh->GetPolygonCount();
		int material_count = mesh->GetNode()->GetMaterialCount();

		std::vector<int>* materials_polygons_indices = new std::vector<int>[material_count];

		if (material_element->GetMappingMode() == FbxGeometryElement::eAllSame)
		{
			materials_polygons_indices[0].resize(polygon_count);
			for (int polygon_index = 0; polygon_index < polygon_count; polygon_index++)
				materials_polygons_indices[0][polygon_index] = polygon_index;
		}
		else
		{
			auto index_array = material_element->GetIndexArray();

			for (int polygon_index = 0; polygon_index < polygon_count; polygon_index++)
			{
				int material_index = index_array.GetAt(polygon_index);
				materials_polygons_indices[material_index].push_back(polygon_index);
			}
		}

		switch (mode)
		{
		case FbxLayerElement::eByControlPoint:
		{

			for (int i = 0; i < cp_count; i++)
			{
				FbxVector4 vertex = p_vertices[i];
				FbxVector4 normal = direct_array.GetAt(i);
				int* blend_ind = blend_indices[i];
				float* blend_w = blend_weights[i];

				VertexA tmp = VertexA{
					XMFLOAT3((float)vertex.mData[0], (float)vertex.mData[1], (float)vertex.mData[2]),
					XMFLOAT3((float)normal.mData[0], (float)normal.mData[1], (float)normal.mData[2]),
					XMFLOAT3((float)blend_w[0], (float)blend_w[1], (float)blend_w[2])
				};

				for (int j = 0; j < N; j++)
					tmp.indices[j] = static_cast<BYTE>(blend_ind[j]);

				vertices.push_back(tmp);
			}

			for (int material_index = 0; material_index < material_count; material_index++)
			{
				groups.push_back((UINT)indices.size());
				polygon_count = (int)materials_polygons_indices[material_index].size();

				for (int j = 0; j < polygon_count; j++)
				{
					int polygon_index = materials_polygons_indices[material_index][j];
					int size = mesh->GetPolygonSize(polygon_index);

					if (size == 3)
					{
						indices.push_back(mesh->GetPolygonVertex(polygon_index, 0));
						indices.push_back(mesh->GetPolygonVertex(polygon_index, 1));
						indices.push_back(mesh->GetPolygonVertex(polygon_index, 2));
					}
					else
						if (size == 4)
						{
							int i0, i2;
							indices.push_back(i0 = mesh->GetPolygonVertex(polygon_index, 0));
							indices.push_back(mesh->GetPolygonVertex(polygon_index, 1));
							indices.push_back(i2 = mesh->GetPolygonVertex(polygon_index, 2));

							indices.push_back(i0);
							indices.push_back(i2);
							indices.push_back(mesh->GetPolygonVertex(polygon_index, 3));
						}
						else
							throw "Not implemented size of polygon";
				}
			}

			groups.push_back((UINT)indices.size());
			break;
		}
		case FbxLayerElement::eByPolygonVertex:
		{
			UINT index = 0;
			for (int material_index = 0; material_index < material_count; material_index++)
			{
				groups.push_back((UINT)indices.size());
				polygon_count = (int)materials_polygons_indices[material_index].size();

				for (int j = 0; j < polygon_count; j++)
				{
					int size = mesh->GetPolygonSize(j);

					for (int i = 0; i < size; i++)
					{
						int pi = mesh->GetPolygonVertex(j, i);
						FbxVector4 vertex = p_vertices[pi];
						FbxVector4 normal = direct_array.GetAt(index + i);
						int* blend_ind = blend_indices[pi];
						float* blend_w = blend_weights[pi];

						VertexA tmp = VertexA{
							XMFLOAT3((float)vertex.mData[0], (float)vertex.mData[1], (float)vertex.mData[2]),
							XMFLOAT3((float)normal.mData[0], (float)normal.mData[1], (float)normal.mData[2]),
							XMFLOAT3((float)blend_w[0], (float)blend_w[1], (float)blend_w[2])
						};

						for (int j = 0; j < N; j++)
							tmp.indices[j] = static_cast<BYTE>(blend_ind[j]);


						vertices.push_back(tmp);
					}

					if (size == 3)
					{
						indices.push_back(index);
						indices.push_back(index + 1);
						indices.push_back(index + 2);
						index += 3;
					}
					else
						if (size == 4)
						{
							indices.push_back(index);
							indices.push_back(index + 1);
							indices.push_back(index + 2);

							indices.push_back(index);
							indices.push_back(index + 2);
							indices.push_back(index + 3);
							index += 4;
						}
						else
						{
							throw "Not implemented size of polygon";
						}
				}
			}

			groups.push_back((UINT)indices.size());
			break;
		}
		default:
			throw "Not implemented mapping mode";
		}


		if (skin != nullptr)
		{
			polygon_count = mesh->GetControlPointsCount();
			for (int i = 0; i < polygon_count; i++)
			{
				delete[] blend_indices[i];
				delete[] blend_weights[i];
			}

			delete[] blend_count;
			delete[] blend_indices;
			delete[] blend_weights;
		}

		return tie(vertices, indices, groups);
	}

	FbxAMatrix FbxLoader::GetGeometryTransformation(FbxNode* inNode)
	{
		if (!inNode)
		{
			throw std::exception("Null for mesh geometry");
		}

		const FbxVector4 lT = inNode->GetGeometricTranslation(FbxNode::eSourcePivot);
		const FbxVector4 lR = inNode->GetGeometricRotation(FbxNode::eSourcePivot);
		const FbxVector4 lS = inNode->GetGeometricScaling(FbxNode::eSourcePivot);

		return FbxAMatrix(lT, lR, lS);
	}

	Transform FbxLoader::GetTransform(FbxNode* node)
	{
		FbxVector4 tmp;
		FbxAMatrix localMatrix = node->EvaluateLocalTransform();

		tmp = localMatrix.GetT();
		XMFLOAT3 position = XMFLOAT3((float)tmp.mData[0], (float)tmp.mData[1], (float)tmp.mData[2]);

		tmp = localMatrix.GetR();
		float x = XMConvertToRadians((float)tmp.mData[0]);
		float y = XMConvertToRadians((float)tmp.mData[1]);
		float z = XMConvertToRadians((float)tmp.mData[2]);
		XMFLOAT3 rotation = XMFLOAT3(x, y, z);

		tmp = localMatrix.GetS();
		XMFLOAT3 scaling = XMFLOAT3((float)tmp.mData[0], (float)tmp.mData[1], (float)tmp.mData[2]);

		return Transform(position, rotation, scaling);

		tmp = node->LclTranslation.Get();
	}
}