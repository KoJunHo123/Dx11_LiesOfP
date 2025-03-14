#pragma once

#include "Tools_Defines.h"
#include "Base.h"


#include "GameInstance.h"

BEGIN(Engine)
class CGameObject;
END

BEGIN(Tools)
class CImGui_Manager : public CBase
{
	DECLARE_SINGLETON(CImGui_Manager)

private:
	CImGui_Manager();
	virtual ~CImGui_Manager() = default;

public:
	HRESULT Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	void Render_ImGui();

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

private:
	class CGameInstance*				m_pGameInstance = { nullptr };

	class CController_MapTool*			m_pController_MapTool = { nullptr };
	class CController_EffectTool*		m_pController_EffectTool = { nullptr };
	class CController_UITool*			m_pController_UITool = { nullptr };
	class CController_AnimationTool*	m_pController_AnimationTool = { nullptr };
	class CCamera_Manager*				m_pController_PostProcess = { nullptr };
	class CController_Cutscene*			m_pController_Cutscene = { nullptr };

private:
	void Update_ImGui();

private:
	void Tool_Cutscene();

private:
	void Tool_Map();

private:
	void Tool_Effect();

private:
	void Tool_UI();

private:
	void Tool_Animation();

	_bool	bAnimCtr{false};

private:
	void Tool_PostProcess();

private:
	void ImGui_Spacing(_uint iSpaceCount = 1);

	HRESULT Ready_Controllers();

	//�׽�Ʈ 
	_float m_fTest = 0.f;

public:
	virtual void Free() override;
};

END