#include "SkinningApplication.h"
#include "vertexDef.h"

#include <shobjidl.h> 
#include <strsafe.h>
#include <iostream>
#include <string>
#include <direct.h>

#include "FbxLoader.h"
#include "MathHelper.h"

#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_win32.h"
#include "ImGui/imgui_impl_dx11.h"

namespace SkinningApp
{
	SkinningApplication::SkinningApplication(HINSTANCE hInstance, UINT width, UINT height, std::wstring title) :
		DxApplication(hInstance, width, height, title), m_inputDevice(hInstance),
		m_mouse(m_inputDevice.CreateMouseDevice(m_window.getHandle())),
		m_keyboard(m_inputDevice.CreateKeyboardDevice(m_window.getHandle())),
		m_camera(XMFLOAT3(0, 4, 0), 0.01f, 50, 15),
		m_state(AppState::GUI),
		next_state(AppState::GUI),
		time_pos(0)
	{
		/*ImGui*/
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		ImGui_ImplWin32_Init(this->m_window.getHandle());
		ImGui_ImplDX11_Init(this->m_device, this->m_device.context().get());
		ImGui::StyleColorsLight();

		XMMATRIX proj_matrix = XMMatrixPerspectiveFovLH(5 * XM_PI / 12.0f, width / (float)height, 0.01f, 100.0f);

		m_effect = SkinningEffect(m_device.LoadEffect(L"SkinningEffect.fxo"), SkinningEffect::SKINNING_TECH, proj_matrix);

		//TODO: 04. Stworzyć Input Layout
		auto desc = m_effect.GetPassDesc();
		m_inputLayout = m_device.CreateInputLayout(VertexA::Layout, 4, desc.pIAInputSignature, desc.IAInputSignatureSize);



		//m_inputLayout =;

		tie(meshes, skeleton) = FbxLoader::LoadMesh("Character.fbx", m_device);

		//TODO: 18. Odkomentować
		animations.resize(4);
		animations[AppState::ANIMATION_1] = FbxLoader::LoadAnimationClip("Jump.fbx");
		animations[AppState::ANIMATION_2] = FbxLoader::LoadAnimationClip("Running.fbx");
		animations[AppState::GUI] = GetAnimationClipFromDefaultValues();
		animations[AppState::BLEND] = new AnimationClip(skeleton->GetBoneCount());

		std::vector<DirectionalLight> light(2);
		light[0].Ambient = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
		light[0].Diffuse = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
		light[0].Specular = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
		light[0].Direction = XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);

		light[1].Ambient = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
		light[1].Diffuse = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
		light[1].Specular = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
		light[1].Direction = XMFLOAT4(0.0f, 0.0f, -1.0f, 1.0f);

		Material material;
		material.Ambient = XMFLOAT4(0.78f, 0, 0, 1);
		material.Diffuse = XMFLOAT4(1, 0, 0, 1);
		material.Specular = XMFLOAT4(0.8f, 0.8f, 0.8f, 16.0f);

		XMMATRIX world_matrix = XMMatrixScaling(0.05f, 0.05f, 0.05f) * XMMatrixRotationY(-XM_PI);

		//TODO: 05. Zaktualizować materiał
		m_effect.UpdateMaterial(material);

		//TODO: 06. Zaktualizować światło
		m_effect.UpdateDirectionalLight(light);

		//TODO: 07. Zaktualizować macierz świata
		m_effect.UpdateWorldMatrix(world_matrix);

		this->m_timer.Tick();

	}

	SkinningApplication::~SkinningApplication()
	{
		for (int index = 0; index < meshes.size(); index++)
			delete meshes[index];

		for (int index = 0; index < animations.size(); index++)
			delete animations[index];

		if (skeleton != nullptr)
			delete skeleton;
	}

	void SkinningApplication::Update(const float dt)
	{
		time_pos += dt;

		while (m_state != AppState::GUI && animations[m_state]->GetEndTime() < time_pos)
		{
			time_pos -= animations[m_state]->GetEndTime();
			if (m_state == AppState::BLEND)
			{
				m_state = next_state;
				time_pos = 0;
			}
		}

		this->HandleInput(dt);

		//TODO: 21. Dodać blendowanie pomiędzy zmieniającymi się animacjami


		int bone_count = skeleton->GetBoneCount();
		XMMATRIX* bone_matrix = new XMMATRIX[bone_count];

		//TODO: 19. Podmienić wyznaczanie przekształcenia dziecko-rodzic na funkcję Interpolate dla aktualnego stanu
		animations[m_state]->Interpolate(time_pos, bone_matrix);

		//TODO: 11. Wyznaczyć przekształcenie dla każdej kości

		for (int i = 0; i < bone_count; ++i)
		{
			/*
			auto& transform = skeleton->GetBone(i).transform;
			auto rotation = MathHelper::EulerToQuaternion(transform.GetRotation());
			auto scale = transform.GetScaling();
			auto position = transform.GetPosition();

			bone_matrix[i] = XMMatrixAffineTransformation(
				XMLoadFloat3(&scale),
				XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f),
				XMLoadFloat4(&rotation),
				XMLoadFloat3(&position)
			);
			*/

			if (i >= 1)
			{
				int parent = skeleton->GetBone(i).parent_index;
				XMMATRIX matrix = bone_matrix[i];
				XMMATRIX pMatrix = bone_matrix[parent];

				bone_matrix[i] = XMMatrixMultiply(matrix, pMatrix);
			}

		}

		for (int i = 0; i < bone_count; ++i)
		{
			bone_matrix[i] = XMMatrixMultiply(XMLoadFloat4x4(&skeleton->GetBone(i).offset), bone_matrix[i]);
		}

		//TODO: 12. Zaktualizować macierze kości
		m_effect.UpdateBoneMatrix(bone_matrix, bone_count);
		delete[] bone_matrix;
	}

	void SkinningApplication::Render()
	{
		DxApplication::Render();
		m_device.BeginRender(m_renderTargetView.get(), m_depthStencilView.get());

		m_device.context()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		m_device.context()->IASetInputLayout(m_inputLayout.get());


		//TODO: 08. Zaktualizować macierz widoku
		m_effect.UpdateViewMatrix(m_camera.getViewMatrix());
		//TODO: 09. Zaktualizować pozycje kamery
		m_effect.UpdateEyePosition(m_camera.getCameraPosition());
		//TODO: 10. Przesłać zaktualizowane dane na GPU
		m_effect.Apply(m_device.context());

		for (int index = 0; index < meshes.size(); index++)
		{
			meshes[index]->SetDrawBuffers(m_device.context());
			meshes[index]->Draw(m_device.context());
		}

		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		this->RenderImGui();

		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	}

	void SkinningApplication::RenderImGui()
	{
		ImGui::SetNextWindowPos({ 0.0f, 0.0f }, ImGuiSetCond_Once);
		ImGui::SetNextWindowSize({ 275.0f, 500.0f }, ImGuiSetCond_Once);
		ImGui::Begin("Menu", NULL, ImGuiWindowFlags_NoMove | /*ImGuiWindowFlags_NoResize |*/ ImGuiWindowFlags_NoSavedSettings);

		//TODO: 14. Stworzyć proste gui do edycji: pozycji, skali i rotacji kości

		//TODO: 20. Dodać aktualizowanie klatek kluczowych dla stanu AppState::GUI

		ImGui::End();
	}

	bool SkinningApplication::HandleInput(double dt)
	{
		ImGuiIO& io = ImGui::GetIO(); if (io.WantCaptureMouse || io.WantCaptureKeyboard) return true;

		MouseState mstate;
		if (m_mouse.GetState(mstate))
		{
			auto d = mstate.getMousePositionChange();
			if (mstate.isButtonDown(0))
			{
				float tmp = d.y * ROTATION_SPEED;
				float angle = m_camera.getYAngle();
				if (m_camera.getXAngle() + tmp < -XM_PIDIV2)
					tmp = -XM_PIDIV2 - m_camera.getXAngle();
				else
					if (m_camera.getXAngle() + tmp > XM_PIDIV2)
						tmp = XM_PIDIV2 - m_camera.getXAngle();
				m_camera.Rotate(tmp, d.x * ROTATION_SPEED);
			}
			else if (mstate.isButtonDown(1))
				m_camera.Zoom(d.y * ZOOM_SPEED);
		}
		KeyboardState kstate;
		if (!m_keyboard.GetState(kstate))
			return false;

		float distance = (float)(3.0f * dt);
		if (kstate.isKeyDown(DIK_A))
			m_camera.MoveTarget(XMFLOAT3{ -distance, 0, 0 });
		if (kstate.isKeyDown(DIK_D))
			m_camera.MoveTarget(XMFLOAT3{ distance, 0, 0 });
		if (kstate.isKeyDown(DIK_W))
			m_camera.MoveTarget(XMFLOAT3{ 0, distance, 0 });
		if (kstate.isKeyDown(DIK_S))
			m_camera.MoveTarget(XMFLOAT3{ 0, -distance, 0 });
		if (kstate.isKeyDown(DIK_Q))
			m_camera.MoveTarget(XMFLOAT3{ 0, 0, distance });
		if (kstate.isKeyDown(DIK_E))
			m_camera.MoveTarget(XMFLOAT3{ 0, 0, -distance });

		if (kstate.isKeyDown(DIK_1) && m_state != AppState::ANIMATION_1 && next_state != AppState::ANIMATION_1)
		{
			BlendAnimation(time_pos, m_state, 0, AppState::ANIMATION_1, 0.2f);
		}

		if (kstate.isKeyDown(DIK_2) && m_state != AppState::ANIMATION_2 && next_state != AppState::ANIMATION_2)
		{
			BlendAnimation(time_pos, m_state, 0, AppState::ANIMATION_2, 0.2f);
		}

		if (kstate.isKeyDown(DIK_3) && m_state != AppState::GUI && next_state != AppState::GUI)
		{
			BlendAnimation(time_pos, m_state, 0, AppState::GUI, 0.2f);
		}

		return true;
	}


	AnimationClip* SkinningApplication::GetAnimationClipFromDefaultValues()
	{
		int bone_count = skeleton->GetBoneCount();
		AnimationClip* gui_clip = new AnimationClip(bone_count);

		for (int index = 0; index < bone_count; index++)
		{
			Transform transform = skeleton->GetBone(index).transform;
			std::vector<Keyframe> frames;

			frames.push_back(Keyframe(0, transform.GetPosition(), MathHelper::EulerToQuaternion(transform.GetRotation()), transform.GetScaling()));

			frames[0].time_pos = 0;

			gui_clip->bone_animations[index].frames = frames;
		}

		gui_clip->SetEndTime(0);

		return gui_clip;
	}

	void SkinningApplication::BlendAnimation(float time_pos1, AppState state1, float time_pos2, AppState state2, float duration)
	{
		auto a1 = animations[state1]->bone_animations;
		auto a2 = animations[state2]->bone_animations;

		AnimationClip* a = animations[AppState::BLEND];

		for (int i = 0; i < skeleton->GetBoneCount(); ++i)
		{
			auto& frames = a->bone_animations[i].frames;
			frames.clear();
			frames.push_back(a1[i].GetKeyframe(time_pos1));
			frames[0].time_pos = 0;
			frames.push_back(a2[i].GetKeyframe(time_pos2));
			frames[1].time_pos = duration;

		}

		a->SetEndTime(duration);

		time_pos = 0;
		next_state = state2;
		m_state = AppState::BLEND;
	}
}
