#include "stdafx.h"
#include "State_SimonManusP1_SwingDown_Swing_L.h"
#include "GameInstance.h"
#include "Model.h"
#include "SimonManus.h"

#include "EffectObject.h"

CState_SimonManusP1_SwingDown_Swing_L::CState_SimonManusP1_SwingDown_Swing_L(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_SimonManusP1_SwingDown_Swing_L::Initialize(_uint iStateNum, void* pArg)
{
    m_iStateNum = iStateNum;
    //FSM_INIT_DESC* pDesc = static_cast<FSM_INIT_DESC*>(pArg);

    return S_OK;
}

HRESULT CState_SimonManusP1_SwingDown_Swing_L::Start_State(void* pArg)
{
    m_pMonster->Change_Animation(AN_ROUTE_FIRST, false, 0.2f, 0);

    m_isSwing = true;
    m_bStampEffect = false;
    m_bSwing_Sound = false;
    m_bSwing = false;

    return S_OK;
}

void CState_SimonManusP1_SwingDown_Swing_L::Update(_float fTimeDelta)
{
    if (m_isSwing)
    {
        if (100.f <= m_pMonster->Get_CurrentTrackPos())
        {
            if (m_pMonster->Get_TargetDead())
            {
                m_pMonster->Change_Animation(8, true, 0.5f, 0, true);
                m_pMonster->Change_State(CMonster::IDLE);
                return;
            }

            m_pMonster->Change_Animation(AN_ROUTE_LAST, false, 0, 0);
            ++m_iRouteTrack;
            m_isSwing = false;
        }
    }
    
    if (End_Check())
    {
        m_pMonster->Change_State(CSimonManus::IDLE);
        return;
    }

    _double CurTrackPos = m_pMonster->Get_CurrentTrackPos();

    Collider_Check(CurTrackPos);
    Control_Sound(CurTrackPos);
    Effect_Check(CurTrackPos);
}

void CState_SimonManusP1_SwingDown_Swing_L::End_State()
{
    m_pMonster->DeActive_CurrentWeaponCollider(0);
    m_iRouteTrack = 0;
}

_bool CState_SimonManusP1_SwingDown_Swing_L::End_Check()
{
    _uint iCurAnim = m_pMonster->Get_CurrentAnimIndex();
    _bool bEndCheck{ false };
    switch (m_iRouteTrack)
    {
    case 0:
        if ((AN_ROUTE_FIRST) == iCurAnim)
        {
            bEndCheck = m_pMonster->Get_EndAnim(AN_ROUTE_FIRST);
        }
        break;

    case 1:
        if ((AN_ROUTE_LAST) == iCurAnim)
        {
            bEndCheck = m_pMonster->Get_EndAnim(AN_ROUTE_LAST);
        }
        break;

    default:
        break;
    }

    return bEndCheck;
}

void CState_SimonManusP1_SwingDown_Swing_L::Collider_Check(_double CurTrackPos)
{
    if (m_iRouteTrack == 0)
    {
        if (CurTrackPos >= 65.f && CurTrackPos <= 75.f)
        {
            m_pMonster->Active_CurrentWeaponCollider(1.3f, 0, HIT_TYPE::HIT_METAL, ATTACK_STRENGTH::ATK_NORMAL);
        }
        else
        {
            m_pMonster->DeActive_CurrentWeaponCollider(0);
        }
    }
    else
    {
        if (CurTrackPos >= 40.f && CurTrackPos <= 85.f)
        {
            m_pMonster->Active_CurrentWeaponCollider(1.2f, 0, HIT_TYPE::HIT_METAL, ATTACK_STRENGTH::ATK_NORMAL);
        }
        else
        {
            m_pMonster->DeActive_CurrentWeaponCollider(0);
        }
    }
}

void CState_SimonManusP1_SwingDown_Swing_L::Effect_Check(_double CurTrackPos)
{
    if (m_iRouteTrack == 0)
    {
        if (!m_bStampEffect)
        {
            if ((CurTrackPos >= 75.f))
            {
                CEffectObject::EFFECTOBJ_DESC Desc{};
                Desc.fLifeDuration = 1.5f;
                Desc.strEffectTag = TEXT("SimonManus_Attack_Stamp");
                _float4x4 WorldMat{};

                XMStoreFloat4x4(&WorldMat, (*m_pMonster->Get_WeaponBoneCombinedMat(6) * (*m_pMonster->Get_WeaponWorldMat())));
                Desc.vPos = _Vec3{ WorldMat._41, WorldMat._42, WorldMat._43 };

                m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Monster_Attack"), TEXT("Prototype_GameObject_SpotEffect"), &Desc);

                m_bStampEffect = true;
            }
        }
    }
    if (m_iRouteTrack == 1)
    {
        if ((CurTrackPos >= 40.f && CurTrackPos <= 85.f))
        {
            if (!m_bSwing)
            {
                m_pMonster->Active_Effect(CSimonManus::P1_TRAIL);
                m_pMonster->Active_Effect(CSimonManus::SWING_DRAG_REVERSE);
                m_bSwing = true;
            }
        }
        else
        {
            m_pMonster->DeActive_Effect(CSimonManus::P1_TRAIL);
            m_pMonster->DeActive_Effect(CSimonManus::SWING_DRAG_REVERSE);
        }
    }
}

void CState_SimonManusP1_SwingDown_Swing_L::Control_Sound(_double CurTrackPos)
{
    if (m_iRouteTrack == 0)
    {
        if (!m_bStampEffect)
        {
            if (CurTrackPos >= 75.f)
            {
                m_pMonster->Play_Sound(CPawn::PAWN_SOUND_EFFECT1, TEXT("SE_NPC_SK_FX_Ground_Exp_L_03.wav"));
            }
        }
    }
    else
    {
        if (!m_bSwing_Sound)
        {
            if (CurTrackPos >= 40.f && CurTrackPos <= 45.f)
            {
                m_pMonster->Play_Sound(CPawn::PAWN_SOUND_EFFECT1, TEXT("SE_NPC_SK_FX_Ground_Exp_M_02.wav"), true);
                m_bSwing_Sound = true;
            }
            if (CurTrackPos >= 70.f)
            {
                m_pMonster->Play_Sound(CPawn::PAWN_SOUND_EFFECT1, TEXT("SE_PC_SK_Smash_Crystal_Stone_H_03.wav"));
                m_bSwing_Sound = true;
            }
        }
        else
        {
            if (CurTrackPos > 45.f && CurTrackPos <= 50.f)
            {
                m_bSwing_Sound = false;
            }
        }
    }
}

CState_SimonManusP1_SwingDown_Swing_L* CState_SimonManusP1_SwingDown_Swing_L::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_SimonManusP1_SwingDown_Swing_L* pInstance = new CState_SimonManusP1_SwingDown_Swing_L(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_SimonManusP1_SwingDown_Swing_L"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_SimonManusP1_SwingDown_Swing_L::Free()
{
    __super::Free();
}