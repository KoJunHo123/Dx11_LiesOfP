#include "stdafx.h"
#include "AObj_LightningImpact.h"

#include "GameInstance.h"
#include "Player.h"

#include "Effect_Manager.h"

CAObj_LightningImpact::CAObj_LightningImpact(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CAttackObject{ pDevice, pContext }
{
}

CAObj_LightningImpact::CAObj_LightningImpact(const CGameObject& Prototype)
    : CAttackObject{ Prototype }
{
}

HRESULT CAObj_LightningImpact::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CAObj_LightningImpact::Initialize(void* pArg)
{
    GAMEOBJECT_DESC GameObjDesc{};
    if (FAILED(__super::Initialize(&GameObjDesc)))
        return E_FAIL;

    ATKOBJ_DESC* pDesc = static_cast<ATKOBJ_DESC*>(pArg);

    m_pTransformCom->Set_State(CTransform::STATE_POSITION, pDesc->vPos);

    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_pOwner = pDesc->pOwner;
    Safe_AddRef(m_pOwner);

    m_fDamageAmount = 250.f;
    m_fLifeDuration = 0.2f;

    m_pColliderCom->IsActive(true);

    m_pColliderCom->Set_Owner(this);

    m_strObjectTag = TEXT("MonsterWeapon");

    //if (m_pSoundCom[EFF_SOUND_EFFECT1] != nullptr)       //번개 사운드 넣기
    //{
    //    m_pSoundCom[EFF_SOUND_EFFECT1]->Play2D(TEXT("SE_NPC_Raxasia_SK_PJ_Thunder_Explo_05.wav"), &g_fEffectVolume, false);
    //}

    return S_OK;
}

void CAObj_LightningImpact::Priority_Update(_float fTimeDelta)
{
    m_pEffect->Priority_Update(fTimeDelta);
}

void CAObj_LightningImpact::Update(_float fTimeDelta)
{
    if (m_pEffect->Get_Dead())
    {
        m_isDead = true;
    }
    m_fLifeTime += fTimeDelta;
    m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix_Ptr());

    m_pEffect->Update(fTimeDelta);
}

void CAObj_LightningImpact::Late_Update(_float fTimeDelta)
{
    if (m_pOwner->Get_Dead())
    {
        m_isDead = true;
    }

    m_pEffect->Late_Update(fTimeDelta);
    if (m_fLifeTime < m_fLifeDuration)
    {
        m_pGameInstance->Add_ColliderList(m_pColliderCom);
#ifdef DEBUG
        m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
        m_pGameInstance->Add_DebugObject(m_pColliderCom);
#endif // DEBUG
    }
}

HRESULT CAObj_LightningImpact::Render()
{
#ifdef DEBUG
    m_pColliderCom->Render();
#endif // DEBUG
    return S_OK;
}

HRESULT CAObj_LightningImpact::Render_LightDepth()
{
    return S_OK;
}

void CAObj_LightningImpact::OnCollisionEnter(CGameObject* pOther)
{
    if (m_pOwner->Get_IsDieState())
    {
        return;
    }

    //pOther check
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
            _bool bHitCheck = pOther->Calc_DamageGain(m_fDamageAmount * m_fDamageRatio, _Vec3{}, HIT_TYPE::HIT_METAL, ATTACK_STRENGTH::ATK_NORMAL);

            CPlayer* pPlayer = static_cast<CPlayer*>(pOther);

            if (bHitCheck && !pPlayer->Get_IsInvicible())
            {
                _Vec3 vDir = {};
                vDir = _Vec3{ pPlayer->Get_Transform()->Get_State(CTransform::STATE_POSITION) - m_pTransformCom->Get_State(CTransform::STATE_POSITION) };

                CEffect_Manager::Get_Instance()->Add_Effect_ToLayer(LEVEL_GAMEPLAY, TEXT("Player_Impact"),
                    _Vec3{ pOther->Get_Transform()->Get_State(CTransform::STATE_POSITION) + _Vec3{0.f, 1.f, 0.f} }, vDir);
            }
        }
    }
}

void CAObj_LightningImpact::OnCollisionStay(CGameObject* pOther)
{
}

void CAObj_LightningImpact::OnCollisionExit(CGameObject* pOther)
{
}

HRESULT CAObj_LightningImpact::Ready_Components()
{
    if (FAILED(__super::Ready_Components()))
        return E_FAIL;

    /* FOR.Com_Collider */
    CBounding_Sphere::BOUNDING_SPHERE_DESC      ColliderDesc{};
    ColliderDesc.vCenter = _float3(0.f, 0.f, 0.f);
    ColliderDesc.fRadius = 1.5f;

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
        TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
        return E_FAIL;
    m_pColliderCom->Set_Owner(this);

    const _Matrix* pParetnMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();

    m_pEffect = CEffect_Manager::Get_Instance()->Clone_Effect(TEXT("SimonManus_Attack_LightningImpact"), pParetnMatrix,
        nullptr, _Vec3(0.f, 0.f, 0.f), _Vec3(0.f, 0.f, 1.f));

    m_pEffect->Reset_Effects();

    return S_OK;
}

CAObj_LightningImpact* CAObj_LightningImpact::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CAObj_LightningImpact* pInstance = new CAObj_LightningImpact(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CAObj_LightningImpact"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CAObj_LightningImpact::Clone(void* pArg)
{
    CAObj_LightningImpact* pInstance = new CAObj_LightningImpact(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CAObj_LightningImpact"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CAObj_LightningImpact::Free()
{
    Safe_Release(m_pOwner);

    __super::Free();

    if (true == m_isCloned)
    {
        m_pEffect->Set_Cloned(false);
        Safe_Release(m_pEffect);
    }
}
