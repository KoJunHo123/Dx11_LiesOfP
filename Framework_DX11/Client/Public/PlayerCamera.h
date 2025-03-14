#pragma once
#include "Client_Defines.h"
#include "Camera.h"

BEGIN(Client)

class CPlayerCamera :
	public CCamera
{
public:
	enum CAMERA_MODE { MODE_LERP, MODE_STATIC, MODE_END };

	typedef struct : public CCamera::CAMERA_DESC {
		class CPlayer* pPlayer = { nullptr };
		_float			fSpeed = {};
	}CAMERA_PLAYER_DESC;

private:
	CPlayerCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayerCamera(const CCamera& Prototype);
	virtual ~CPlayerCamera() = default;

public:
	_bool	Get_IsLerpEnd() { return m_isLerpEnd; }
	void	Change_Mode(CAMERA_MODE eMode) { m_eCameraMode = eMode; }

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	void PlayerMove(_float fTimeDelta);
	void PlayerLockOn(_float fTimeDelta);
	void PlayerInitMove(_float fTimeDelta);

public:
	void Move_PlayerBackPos();

private:
	HRESULT					Ready_Components();

private:
	class CPlayer*			m_pPlayer = { nullptr };

	_float					m_fSpeed = {};
	_float					m_fRadian = {};

	_float					m_fCurrentRotationX = {};
	_float					m_fCurrentRotationY = {};

	_Vec3					m_vTargetPos = {};
	_Vec3					m_vOffset = {};

	POINT					m_ptOldMousePos = {};

private:
	_bool					m_isMoveInitPos = { false };
	_Vec4					m_vInitMovePos = {};

private:
	_bool					m_isControl = { false };
	_bool					m_isLerp = { false };
	_bool					m_isLerpEnd = { false };
	_bool					m_isKeepAt = { false };

	CAMERA_MODE				m_eCameraMode = { MODE_LERP };

private:
	void CameraShaking(_matrix WorldMatrix);

public:
	static CPlayerCamera* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END