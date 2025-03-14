#pragma once

#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Client)
class CEffect_Base abstract : public CGameObject
{
public:
	typedef struct
	{
		_int			iRenderGroup = { 0 };
		_int			iPpState = { PP_END };
	}RENDER_DESC;

	typedef struct : public CGameObject::GAMEOBJECT_DESC
	{
		_Matrix* pParentMatrix = { nullptr };
		RENDER_DESC		RenderDesc = {};
	} EFFECT_BASE_DESC;

	enum EFFECT_TYPE { TYPE_PARTICLE, TYPE_TEXTURE, TYPE_MESH, TYPE_TRAIL_OP, TYPE_TRAIL_TP, TYPE_TRAIL_MP, TYPE_END };

	enum GEOM_STATE
	{
		PS_GROW = 0x0001,
		PS_SHRINK = 0x0002,
		PS_ROTATION = 0x0004,
		PS_END
	};

	enum EFFECT_POSTPROCESSING
	{
		PP_NONE = 0x0001,
		PP_DISTORTION = 0x0002,
		PP_BLUR = 0x0004,
		PP_END = 0xFFFF
	};

	enum EFFECT_TEXTURE
	{
		TEXTURE_DIFFUSE,
		TEXTURE_MASK_1,
		TEXTURE_MASK_2,
		TEXTURE_NORMAL,
		TEXTURE_END
	};

protected:
	CEffect_Base(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEffect_Base(const CEffect_Base& Prototype);
	virtual ~CEffect_Base() = default;

public:
	void Set_ParentMatrix_Ptr(const _Matrix* pParentMatrix) {
		m_pParentMatrix = pParentMatrix;
	}

	void Set_WorldMatrix();

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg = nullptr);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

public:
	virtual void Reset();
	virtual void Set_Loop(_bool bLoop);

protected:
	HRESULT Bind_WorldMatrix(class CShader* pShader, const _char* pConstantName);

protected:
	EFFECT_TYPE		m_eEffectType = { TYPE_END };
	RENDER_DESC		m_RenderDesc = {};

	_Matrix			m_WorldMatrix = XMMatrixIdentity();
	const _Matrix*	m_pParentMatrix = { nullptr };

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};
END
