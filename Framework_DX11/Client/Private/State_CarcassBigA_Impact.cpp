#include "stdafx.h"
#include "State_CarcassBigA_Impact.h"
#include "GameInstance.h"
#include "Model.h"
#include "CarcassBigA.h"

CState_CarcassBigA_Impact::CState_CarcassBigA_Impact(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_CarcassBigA_Impact::Initialize(_uint iStateNum, void* pArg)
{
    m_iStateNum = iStateNum;
    //FSM_INIT_DESC* pDesc = static_cast<FSM_INIT_DESC*>(pArg);

    return S_OK;
}

HRESULT CState_CarcassBigA_Impact::Start_State(void* pArg)
{
    m_pMonster->Change_Animation(AN_IMPACT, false, 0.1f, 0, true);

    return S_OK;
}

void CState_CarcassBigA_Impact::Update(_float fTimeDelta)
{
    if (End_Check())
        m_pMonster->Change_State(CCarcassBigA::IDLE);
    
    
    _double CurTrackPos = m_pMonster->Get_CurrentTrackPos();
    
    if (CurTrackPos >= 55.f && CurTrackPos <= 135.f)
    {
        _Vec3 vDir = m_pMonster->Get_Transform()->Get_State(CTransform::STATE_LOOK);

        m_pMonster->Get_RigidBody()->Set_Velocity(XMVector3Normalize(vDir) * m_fImpactSpeed);

    }
   

    Collider_Check(CurTrackPos);
    Sound_Check(CurTrackPos);
}

void CState_CarcassBigA_Impact::End_State()
{
    m_pMonster->DeActive_CurrentWeaponCollider(2);
}

_bool CState_CarcassBigA_Impact::End_Check()
{
    return m_pMonster->Get_EndAnim(AN_IMPACT);
}

void CState_CarcassBigA_Impact::Collider_Check(_double CurTrackPos)
{
    if (CurTrackPos >= 55.f && CurTrackPos <= 140.f)
    {
        m_pMonster->Active_CurrentWeaponCollider(1.4f, 2, HIT_TYPE::HIT_CARCASS, ATTACK_STRENGTH::ATK_NORMAL);
    }
    else
    {
        m_pMonster->DeActive_CurrentWeaponCollider(2);
    }
}

void CState_CarcassBigA_Impact::Sound_Check(_double CurTrackPos)
{
    if ((CurTrackPos >= 27.f && CurTrackPos <= 30.f) || 
        (CurTrackPos >= 76.f && CurTrackPos <= 80.f) ||
        (CurTrackPos >= 108.f && CurTrackPos <= 115.f) || 
        (CurTrackPos >= 126.f && CurTrackPos <= 130.f))
    {
        if(!m_bStepSound)
        {
            m_pMonster->Play_Sound(CPawn::PAWN_SOUND_EFFECT1, TEXT("SE_NPC_FS_Carcass_OneArmed_Stone_04.wav"), false);
            m_bStepSound = true;
        }
    }
    else
    {
        m_bStepSound = false;
    }
}

CState_CarcassBigA_Impact* CState_CarcassBigA_Impact::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_CarcassBigA_Impact* pInstance = new CState_CarcassBigA_Impact(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_CarcassBigA_Impact"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_CarcassBigA_Impact::Free()
{
    __super::Free();
}