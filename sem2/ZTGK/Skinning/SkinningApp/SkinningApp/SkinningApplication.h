#include "dxApplication.h"
#include <DirectXMath.h>
#include "diDevice.h"
#include "keyboard.h"
#include "mouse.h"
#include "effect.h"
#include "Camera.h"
#include "SkinningEffect.h"
#include "mesh.h"
#include <vector>
#include "Skeleton.h"
#include "AnimationClip.h"

#include <d3d11.h>

using namespace Real_Engine;

namespace SkinningApp
{
	enum AppState
	{
		ANIMATION_1 = 0,
		ANIMATION_2 = 1,
		GUI = 2,
		BLEND = 3,
	};

    class SkinningApplication : public DxApplication
    {
    public:
		SkinningApplication(HINSTANCE hInstance, UINT width = Window::m_defaultWindowWidth, UINT height = Window::m_defaultWindowHeight, std::wstring title = L"Skinning Application");
		~SkinningApplication();

    protected:
        void Update(const float dt) override;
        void Render() override;
        bool HandleInput(double dt);

    private:
		static constexpr float ROTATION_SPEED = 0.01f;
		static constexpr float ZOOM_SPEED = 0.02f;

		DiDevice m_inputDevice;
		Mouse m_mouse;
		Keyboard m_keyboard;

		OrbitCamera m_camera;
		SkinningEffect m_effect;
		dx_ptr<ID3D11InputLayout> m_inputLayout;
		AppState m_state;

		std::vector<Mesh*> meshes;
		Skeleton* skeleton;
		std::vector<AnimationClip*> animations;
		float time_pos;
		AppState next_state;

        void RenderImGui();
		AnimationClip* GetAnimationClipFromDefaultValues();

		void BlendAnimation(float time_pos1, AppState state1, float time_pos2, AppState state2, float duration);
    };
}
