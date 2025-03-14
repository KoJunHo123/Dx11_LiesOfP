#pragma once
#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Cube;
END

BEGIN(Client)
class CDecal_Blood :
    public CGameObject
{
public:
	enum BLOOD_TYPE{TYPE1, TYPE2, TYPE3, TYPE4, TYPE_TRAIL, TYPE_END};

	typedef struct : public CGameObject::GAMEOBJECT_DESC
	{
		_Vec3 vPos = {};
	}BLOOD_DECAL_DESC;

private:
	CDecal_Blood(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDecal_Blood(const CDecal_Blood& Prototype);
	virtual ~CDecal_Blood() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void	Active_Random(_Vec3 vPos);
	void	Active_Trail(_Vec3 vPos);

private:
	CShader* m_pShaderCom = { nullptr };
	CVIBuffer_Cube* m_pVIBufferCom = { nullptr };

	CTexture* m_pTextureCom_Diffuse[TYPE_END] = {nullptr};
	CTexture* m_pTextureCom_Normal[TYPE_END] = {nullptr};

private:
	_bool	m_isNormal = { false };
	_bool	m_bUseWorldColor = { false };

	_float	m_fRenderTime = { 0.f };
	_float  m_fRenderTimer = { 0.f };
	_float  m_fCurrentSize = { 0.f };
	_float  m_fShrinkSpeed= { 0.f };

	_tchar m_szTextureTag_Diffuse[MAX_PATH];
	_tchar m_szTextureTag_Normal[MAX_PATH];

	_Vec3 m_vColor = { 0.f ,0.05f,0.1f };

	BLOOD_TYPE	m_eType = { TYPE_END };

	_bool		m_isTrailDecal = {};

private:
	HRESULT Ready_Components();

public:
	static CDecal_Blood* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END