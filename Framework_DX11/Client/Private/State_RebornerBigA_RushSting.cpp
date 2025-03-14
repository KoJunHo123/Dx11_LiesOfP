#include "stdafx.h"
#include "State_RebornerBigA_RushSting.h"
#include "GameInstance.h"
#include "Model.h"
#include "Monster.h"
#include "Camera.h"

#include "RebornerBigA.h"

CState_RebornerBigA_RushSting::CState_RebornerBigA_RushSting(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_RebornerBigA_RushSting::Initialize(_uint iStateNum, void* pArg)
{
    //FSM_INIT_DESC* pDesc = static_cast<FSM_INIT_DESC*>(pArg);

    m_iStateNum = iStateNum;

    return S_OK;
}

HRESULT CState_RebornerBigA_RushSting::Start_State(void* pArg)
{
    m_pMonster->Change_Animation(AN_LINKED_START, false, 0.1f, 0, true);
    m_iRouteTrack = 0;

    m_bSwingSound = false;
    m_bSwing = false;

    return S_OK;
}

void CState_RebornerBigA_RushSting::Update(_float fTimeDelta)
{
    _double CurTrackPos = m_pMonster->Get_CurrentTrackPos();

    if (m_iRouteTrack == 0)
    {
        if (CurTrackPos >= 110)
        {
            if (m_pMonster->Get_TargetDead())
            {
                m_pMonster->Change_Animation(51, true, 0.5f, 0, true);
                m_pMonster->Change_State(CMonster::IDLE);
                return;
            }

            ++m_iRouteTrack;
            m_bSwingSound = false;
            m_bSwing = false;

            m_pMonster->SetUp_Animation(AN_LINKED_LAST, false, 0, true);
            return;
        }
        if ((CurTrackPos >= 51.f && CurTrackPos <= 88.f) ||
            (CurTrackPos >= 146.f && CurTrackPos <= 177.f))
        {
            m_pMonster->Get_Transform()->LookAt_Lerp_NoHeight(m_pMonster->Get_TargetDir(), 1, fTimeDelta);
        }
    }
    else
    {
        if (End_Check())
        {
            m_pMonster->Change_State(CMonster::IDLE);
            return;
        }

        if ((CurTrackPos <= 58.f))
        {
            m_pMonster->Get_Transform()->LookAt_Lerp_NoHeight(m_pMonster->Get_TargetDir(), 1, fTimeDelta);
        }
    }



    Collider_Check(CurTrackPos);
    Sound_Check(CurTrackPos);
    Effect_Check(CurTrackPos);

}

void CState_RebornerBigA_RushSting::End_State()
{
    m_pMonster->DeActive_CurrentWeaponCollider(0);
}

_bool CState_RebornerBigA_RushSting::End_Check()
{

    return m_pMonster->Get_EndAnim(AN_LINKED_LAST);;
}

void CState_RebornerBigA_RushSting::Collider_Check(_double CurTrackPos)
{
    if (m_iRouteTrack == 0)
    {
        if (CurTrackPos >= 87.f && CurTrackPos <= 95.f)
        {
            m_pMonster->Active_CurrentWeaponCollider(1.2f, 0, HIT_TYPE::HIT_METAL, ATTACK_STRENGTH::ATK_WEAK);
        }
        else
        {
            m_pMonster->DeActive_CurrentWeaponCollider(0);
        }
    }
    else
    {
        if (CurTrackPos >= 45.f && CurTrackPos <= 57.f)
        {
            m_pMonster->Active_CurrentWeaponCollider(1.3f, 0, HIT_TYPE::HIT_METAL, ATTACK_STRENGTH::ATK_NORMAL);
        }
        else
        {
            m_pMonster->DeActive_CurrentWeaponCollider(0);
        }
    }

}

void CState_RebornerBigA_RushSting::Sound_Check(_double CurTrackPos)
{
    if (m_iRouteTrack == 0)
    {
        if (CurTrackPos >= 87.f && CurTrackPos <= 95.f)
        {
            if (!m_bSwingSound)
            {
                m_pMonster->Play_Sound(CPawn::PAWN_SOUND_EFFECT1, TEXT("SE_NPC_SK_WS_Staff_03.wav"), false);
                m_bSwingSound = true;
            }
        }
    }
    else
    {
        if (CurTrackPos >= 45.f && CurTrackPos <= 57.f)
        {
            if (!m_bSwingSound)
            {
                m_pMonster->Play_Sound(CPawn::PAWN_SOUND_EFFECT1, TEXT("SE_NPC_SK_WS_Staff_03.wav"), false);
                m_bSwingSound = true;
            }
        }
    }
}

void CState_RebornerBigA_RushSting::Effect_Check(_double CurTrackPos)
{
    if (m_iRouteTrack == 0)
    {
        if (CurTrackPos >= 87.f && CurTrackPos <= 95.f)
        {
            if (!m_bSwing)
            {
                m_pMonster->Active_Effect(0, true);
                m_bSwing = true;
            }
        }
        else
        {
            m_pMonster->DeActive_Effect(0);
        }
    }
    else
    {
        if (CurTrackPos >= 45.f && CurTrackPos <= 57.f)
        {
            if (!m_bSwing)
            {
                m_pMonster->Active_Effect(0, true);
                m_bSwing = true;
            }
        }
        else
        {
            m_pMonster->DeActive_Effect(0);
        }
    }
}

CState_RebornerBigA_RushSting* CState_RebornerBigA_RushSting::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_RebornerBigA_RushSting* pInstance = new CState_RebornerBigA_RushSting(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_RebornerBigA_RushSting"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_RebornerBigA_RushSting::Free()
{
    __super::Free();
}