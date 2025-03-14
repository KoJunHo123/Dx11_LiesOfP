#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CTransform final : public CComponent
{
public:
	typedef struct
	{
		_float		fSpeedPerSec{}, fRotationPerSec{};
	}TRANSFORM_DESC;

	enum STATE { STATE_RIGHT, STATE_UP, STATE_LOOK, STATE_POSITION, STATE_END };

private:
	CTransform(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CTransform() = default;

public:
	_Vec4				Get_State(STATE eState) { return XMLoadFloat4x4(&m_WorldMatrix).r[eState]; }
	void				Set_State(STATE eState, _Vec3 vState) { XMStoreFloat3((_float3*)&m_WorldMatrix.m[eState][0], vState); }

	_Matrix				Get_WorldMatrix() const { return XMLoadFloat4x4(&m_WorldMatrix); }	//연산용
	void				Set_WorldMatrix(const _Matrix WorldMatrix) { XMStoreFloat4x4(&m_WorldMatrix, WorldMatrix); }
	const _Matrix*		Get_WorldMatrix_Ptr() { return &m_WorldMatrix; }	//저장용

	_Matrix				Get_WorldMatrix_Inverse() const { return m_WorldMatrix.Invert(); }

	_Quaternion			Get_Quaternion() {
		_Matrix MatQuat;
		MatQuat.Right(XMVector3Normalize(m_WorldMatrix.Right()));
		MatQuat.Up(XMVector3Normalize(m_WorldMatrix.Up()));
		MatQuat.Forward(XMVector3Normalize(m_WorldMatrix.Forward()));

		return XMQuaternionRotationMatrix(MatQuat);
	}

public:	
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	
public:
	_Vec3		Get_Scaled() const;
	void		Set_Scaled(_float fX, _float fY, _float fZ);

	void		LookAt(_Vec4 vAt);
	void		LookAt_NoHeight(_Vec4 vAt);
	void		LookAt_Lerp(_Vec4 vLook, _float fSpeed, _float fTimeDelta);
	_int		LookAt_Lerp_NoHeight(_Vec4 vLook, _float fSpeed, _float fTimeDelta);
	void		LookAt_Dir(_Vec4 vDir);
	void		Look_Dir(_Vec4 vDir);

	void		Go_Straight(_float fTimeDelta, _float fSpeed = 0.f, class CNavigation* pNavigation = nullptr);
	void		Go_Backward(_float fTimeDelta, _float fSpeed = 0.f, class CNavigation* pNavigation = nullptr);
	void		Go_Right(_float fTimeDelta, _float fSpeed = 0.f, class CNavigation* pNavigation = nullptr);
	void		Go_Left(_float fTimeDelta, _float fSpeed = 0.f, class CNavigation* pNavigation = nullptr);
	void		Go_Lerp(_Vec4 _CurrentPos, _Vec4 _TargetPos, _float _fSpeed, class CNavigation* pNavigation = nullptr);

	void		Turn(_Vec4 vAxis, _float fTimeDelta, _float fSpeed = 0.f);
	void		Turn(_bool isRotationX, _bool isRotationY, _bool isRotationZ, _float fTimeDelta, _float fSpeed = 0.f);
	void		Turn_Lerp(_fvector vDir, _float fTurnSpeed, _float fTimeDelta);
	void		Turn_RollPitchYaw_Lerp(_float fPitch, _float fYaw, _float fRoll, _float fSpeed, _float fTimeDelta); //x,y,z 회전

	void		Rotation(const _Vec4& vAxis, _float fRadian);
	void		Rotation(_float fX, _float fY, _float fZ);
	_Vec3		Get_CurrentRotation();

	void		BillBoard();
	void		BillBoard_NoHeight();

	void		Set_NewUp(_Vec3 vNewUp);
	void		Set_NewLook(_Vec3 vNewLook);

	void		Orbit(_vector vAxis, _vector vCenter, _float fLimit, _float fDistance, _float fTimeDelta);

	void		Set_isTurning(_bool bTurn) { m_isTurning = bTurn; }
	_bool		Get_isTurning() { return m_isTurning; }

public:
	HRESULT Bind_ShaderResource(class CShader* pShader, const _char* pConstantName);


private:
	_float			m_fSpeedPerSec{};
	_float			m_fRotationPerSec{};

	_Matrix			m_WorldMatrix = {};

	_Vec3			m_vCurrentRot = {};

	_bool			m_isTurning = { false };

public:
	static CTransform* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg);
	virtual CComponent* Clone(void* pArg) { return nullptr; }
	virtual void Free() override;
};

END