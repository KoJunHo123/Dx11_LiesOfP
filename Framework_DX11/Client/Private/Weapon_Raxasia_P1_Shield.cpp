#include "stdafx.h"
#include "Weapon_Raxasia_P1_Shield.h"

#include "Monster.h"
#include "Player.h"

#include "GameInstance.h"

#include "Effect_Manager.h"

CWeapon_Raxasia_P1_Shield::CWeapon_Raxasia_P1_Shield(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CWeapon{ pDevice, pContext }
{
}

CWeapon_Raxasia_P1_Shield::CWeapon_Raxasia_P1_Shield(const CWeapon_Raxasia_P1_Shield& Prototype)
	: CWeapon{ Prototype }
{
}

HRESULT CWeapon_Raxasia_P1_Shield::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CWeapon_Raxasia_P1_Shield::Initialize(void* pArg)
{
	MONSTER_WAPON_DESC* pDesc = static_cast<MONSTER_WAPON_DESC*>(pArg);
	m_pMonster = pDesc->pMonster;
	if (nullptr == m_pMonster)
		return E_FAIL;

	m_pEffect_Manager = CEffect_Manager::Get_Instance();
	Safe_AddRef(m_pEffect_Manager);

	/* 직교퉁여을 위한 데이터들을 모두 셋하낟. */
	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;


	if (m_pParentAtk == nullptr)
	{
		return E_FAIL;
	}

	m_fDebuffAmount = pDesc->fDebuffAmount;
	m_iDebuffType = pDesc->iDebuffType;

	m_strObjectTag = TEXT("MonsterWeapon");

	m_fDamageAmount = *m_pParentAtk;
	
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _Vec3{0.f, 0.f, -0.1f});

	return S_OK;
}

void CWeapon_Raxasia_P1_Shield::Priority_Update(_float fTimeDelta)
{
	if (!m_isActive)
		return;

}

void CWeapon_Raxasia_P1_Shield::Update(_float fTimeDelta)
{
	if (!m_isActive)
		return;

	__super::Update(fTimeDelta);

	m_pColliderCom->Update(&m_WorldMatrix);
}

void CWeapon_Raxasia_P1_Shield::Late_Update(_float fTimeDelta)
{
	if (!m_isActive)
		return;

	/* 직교투영을 위한 월드행렬까지 셋팅하게 된다. */
	__super::Late_Update(fTimeDelta);

	m_pGameInstance->Add_ColliderList(m_pColliderCom);
	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
	m_pGameInstance->Add_RenderObject(CRenderer::RG_SHADOWOBJ, this);
}

HRESULT CWeapon_Raxasia_P1_Shield::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CWeapon_Raxasia_P1_Shield::Render_LightDepth()
{
	if (FAILED(__super::Render_LightDepth()))
		return E_FAIL;

	return S_OK;
}

void CWeapon_Raxasia_P1_Shield::OnCollisionEnter(CGameObject* pOther)
{
	if (m_pMonster->Get_IsDieState())
	{
		return;
	}

	if (pOther->Get_Tag() == TEXT("Player"))
	{
		_bool bOverlapCheck = false;
		for (auto& pObj : m_DamagedObjects)
		{
			if (pObj == pOther)
			{
				bOverlapCheck = true;
				break;
			}
		}

		if (!bOverlapCheck)
		{
			m_DamagedObjects.push_back(pOther);
			_Vec3 vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			_bool bHitCheck = pOther->Calc_DamageGain(m_fDamageAmount * m_fDamageRatio, vPos, HIT_METAL, m_eAttackStrength);

			CPlayer* pPlayer = static_cast<CPlayer*>(pOther);

			if (bHitCheck && !pPlayer->Get_IsInvicible())
			{
				m_pEffect_Manager->Add_Effect_ToLayer(LEVEL_GAMEPLAY, TEXT("Player_Impact"),
					_Vec3{ pOther->Get_Transform()->Get_State(CTransform::STATE_POSITION) + _Vec3{0.f, 1.f, 0.f} }, m_vAttackDir);
			}
			

			if (m_bDebuffAttack)
			{
				static_cast<CPlayer*>(pOther)->Calc_DebuffGain(m_iDebuffType, m_fDebuffRatio * m_fDebuffAmount);
			}

			CEffect_Manager::Get_Instance()->Add_Effect_ToLayer(LEVEL_GAMEPLAY, TEXT("Raxasia_Attack_Shield_Impact"),
				vPos, _Vec3{ m_pMonster->Get_TargetDir() });
		}
	}
}

void CWeapon_Raxasia_P1_Shield::OnCollisionStay(CGameObject* pOther)
{
}

void CWeapon_Raxasia_P1_Shield::OnCollisionExit(CGameObject* pOther)
{
}

void CWeapon_Raxasia_P1_Shield::ChangeAnimation(_int iAnimIndex, _bool isLoop, _float fChangeDuration, _int iStartFrame, _bool bEitherChange, _bool bSameChange)
{
}

_bool CWeapon_Raxasia_P1_Shield::is_EndAnim(_int iAnimIndex)
{
	return m_pModelCom->Get_IsEndAnimArray()[iAnimIndex];
}

HRESULT CWeapon_Raxasia_P1_Shield::Ready_Components()
{
	if (FAILED(__super::Ready_Components()))
		return E_FAIL;

	/* FOR.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_RaxasiaP1_Shield"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* FOR.Com_Collider */
	CBounding_OBB::BOUNDING_OBB_DESC			ColliderDesc{};
	ColliderDesc.vExtents = _float3(0.65f, 0.65f, 0.9f);
	ColliderDesc.vCenter = _float3(0.f, -0.65f, -0.1f);
	ColliderDesc.vAngles = _float3(0.f, 0.f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
		return E_FAIL;
	m_pColliderCom->Set_Owner(this);

	return S_OK;
}

CWeapon_Raxasia_P1_Shield* CWeapon_Raxasia_P1_Shield::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CWeapon_Raxasia_P1_Shield* pInstance = new CWeapon_Raxasia_P1_Shield(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CWeapon_Raxasia_P1_Shield"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject* CWeapon_Raxasia_P1_Shield::Clone(void* pArg)
{
	CWeapon_Raxasia_P1_Shield* pInstance = new CWeapon_Raxasia_P1_Shield(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CWeapon_Raxasia_P1_Shield"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CWeapon_Raxasia_P1_Shield::Free()
{
	__super::Free();
}
