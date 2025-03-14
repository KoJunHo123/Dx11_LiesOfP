#pragma once

#include "Effect_Base.h"

BEGIN(Engine)
class CShader;
class CModel;
class CTexture;
END

BEGIN(Client)
class CMesh_Effect final : public CEffect_Base
{
public:
	typedef struct
	{
		_uint		iShaderIndex = { 0 };
		_float		fDuration = { 10.f };
		_Vec4		vColor = { 0.f, 0.f, 0.f, 1.f };
		_Vec2		vTileRepeat = { 1.f, 1.f };
		_Vec2		vTileMoveDir = {};
		_float		fTileMoveSpeed = { 0.f };
		_Vec3		vPos = {};
		_Vec3		vStartRotation = {};
		_Vec4		vRotationAxis = { 0.f, 1.f, 0.f, 0.f };
		_float		fRotationSpeed = { 0.f };
		_Vec3		vStartScale = { 1.f, 1.f, 1.f };
		_Vec3		vScalingSpeed = {};
		_float		fAlpha = { 1.f };
		_float		fAlphaSpeed = { 0.f };
		_bool		bLoop = { false };

	}DEFAULT_DESC;

	typedef struct
	{
		_tchar		szModelTag[MAX_PATH] = L"";

		_tchar		szDiffuseTexturTag[MAX_PATH] = L"";
		_tchar		szMaskTextureTag_1[MAX_PATH] = L"";
		_tchar		szMaskTextureTag_2[MAX_PATH] = L"";
		_tchar		szNormalTextureTag[MAX_PATH] = L"";
	}TEXT_DESC;

	typedef struct : CEffect_Base::EFFECT_BASE_DESC
	{
		DEFAULT_DESC DefaultDesc = {};
		TEXT_DESC TextDesc = {};
	}MESH_EFFECT_DESC;

private:
	CMesh_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMesh_Effect(const CMesh_Effect& Prototype);
	virtual ~CMesh_Effect() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg = nullptr);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

public:
	virtual void Reset();
	virtual void Set_Loop(_bool bLoop) override {
		m_DefaultDesc.bLoop = bLoop;
		m_InitDesc.bLoop = bLoop;
		if (true == bLoop)
			Reset();
	}

private:
	class CShader* m_pShaderCom = { nullptr };
	class CModel* m_pModelCom = { nullptr };
	class CTexture* m_pTextureCom[TEXTURE_END] = { nullptr, nullptr, nullptr, nullptr };

private:
	DEFAULT_DESC m_DefaultDesc = {};
	DEFAULT_DESC m_InitDesc = {};

	_Vec2	m_vCurrentTileMove = {};
	_float	m_fAccumulateTime = { 0.f };
	_bool	m_isReset = { false };

private:
	HRESULT Ready_Components(const TEXT_DESC& Desc);

public:
	static CMesh_Effect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};
END
