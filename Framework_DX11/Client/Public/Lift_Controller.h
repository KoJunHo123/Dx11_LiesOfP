#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CCollider;
class CShader;
class CModel;
END

BEGIN(Client)

class CLift_Controller :
    public CGameObject
{
private:
	CLift_Controller(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CLift_Controller(const CLift_Controller& Prototype);
	virtual ~CLift_Controller() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	virtual void OnCollisionEnter(CGameObject* pOther) override;
	virtual void OnCollisionStay(CGameObject* pOther) override;
	virtual void OnCollisionExit(CGameObject* pOther) override;

private:
	CCollider* m_pColliderCom = { nullptr};
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
private:
	_bool m_bShadow = false;
	_bool m_bClose_with_Player = { false };
	_bool m_bMoveFloor = { false };					//상호작용 키를 눌러 바닥을 움직일 지 말지
	_bool m_bActiveFloor = { false };				//바닥 이동방향 계산 및 작동 시켰는지
	_bool m_bCollision = { false };

	_int  m_iAnim_Open = { 0 };
	_int  m_iAnim_Idle = { 0 };

	_float fActiveTimer = { 0.f };

	class CLift_Floor* m_pLift_Floor = { nullptr };

private:
	HRESULT Ready_Components();

	void Set_Floor_Dir();

	_bool Is_Close_Between_Lift_Floor();

public:
	static CLift_Controller* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END