#include "stdafx.h"
#include "State_RaxasiaP2_TeleportAttack.h"
#include "GameInstance.h"
#include "Model.h"
#include "Raxasia.h"

#include "AttackObject.h"

CState_RaxasiaP2_TeleportAttack::CState_RaxasiaP2_TeleportAttack(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_RaxasiaP2_TeleportAttack::Initialize(_uint iStateNum, void* pArg)
{
    m_iStateNum = iStateNum;
    //CSimonManus::FSMSTATE_DESC* pDesc = static_cast<CSimonManus::FSMSTATE_DESC*>(pArg);

    return S_OK;
}

HRESULT CState_RaxasiaP2_TeleportAttack::Start_State(void* pArg)
{
    m_iRouteTrack = 0;
    m_pMonster->Change_Animation(AN_BACKJUMP, false, 0.1f, 0);
    
    m_bAccelSound = { false };
    m_bSwingSound = false;
    m_bShieldSwingSound = false;

    m_bTeleport = false;
    m_bSetDir = false;

    m_bSwing = false;
    m_bStart = false;
    m_bEnvelop = false;
    m_bAccel = false;
    m_bFire = false;

    return S_OK;
}

void CState_RaxasiaP2_TeleportAttack::Update(_float fTimeDelta)
{
    _double CurTrackPos = m_pMonster->Get_CurrentTrackPos();
    
    switch (m_iRouteTrack)
    {
    case 0:
        if (CurTrackPos >= 99.f)
        {
            if (m_pMonster->Get_TargetDead())
            {
                m_pMonster->Change_Animation(8, true, 0.5f, 0, true);
                m_pMonster->Change_State(CMonster::IDLE);
                return;
            }
            ++m_iRouteTrack;
            m_bTeleport = false;
            m_bAccel = false;
            m_pMonster->DeActive_Effect(CRaxasia::EFFECT_THUNDERENVELOP_SMALL);
            m_pMonster->Change_Animation(AN_JUMPATTACK, false, 0.2f, 50);
            return;
        }

        if (CurTrackPos >= 40.f && CurTrackPos <= 45.f)
        {
            if (!m_bSetDir)
            {
                _Vec3 vDir = m_pMonster->Get_Transform()->Get_State(CTransform::STATE_LOOK);

                vDir.Normalize();
                m_vDashDirection = -vDir;
                m_bSetDir = true;
            }
            m_pMonster->Get_RigidBody()->Set_Velocity((m_vDashDirection * 90.f));
        }
        else
        {
            m_pMonster->Get_Transform()->LookAt_Lerp_NoHeight(m_pMonster->Get_TargetDir(), 2.f, fTimeDelta);
        }
        break;

    case 1:
        if (End_Check())
        {
            if (m_pMonster->Get_TargetDead())
            {
                m_pMonster->Change_State(CMonster::IDLE);
                return;
            }
            m_pMonster->Change_State(CRaxasia::ATKP2_DECLAREWAR);//개전으로 연결
            return;
        }

        if (!m_bTeleport)
        {
            if (CurTrackPos >= 135.f)
            {
                m_bTeleport = true;
                // 텔
                _Vec3 vDir = m_pMonster->Get_TargetDir();
                _Vec3 vPos = m_pMonster->Get_TargetPos();

                vDir.Normalize();

                m_pMonster->Get_RigidBody()->Set_GloblePose(vPos - (vDir * 3.f));

            }
            else
                m_pMonster->Get_Transform()->LookAt_Lerp_NoHeight(m_pMonster->Get_TargetDir(), 2.f, fTimeDelta);
        }
        break;

    default:
        break;
    }

    Collider_Check(CurTrackPos);
    Effect_Check(CurTrackPos);
    Control_Sound(CurTrackPos);

}

void CState_RaxasiaP2_TeleportAttack::End_State()
{
    m_pMonster->DeActive_CurrentWeaponCollider(0);
    m_pMonster->DeActive_CurrentWeaponCollider(1);
}

_bool CState_RaxasiaP2_TeleportAttack::End_Check()
{
    _uint iCurAnim = m_pMonster->Get_CurrentAnimIndex();
    _bool bEndCheck{ false };
    switch (m_iRouteTrack)
    {
    case 0:
        if ((AN_BACKJUMP) == iCurAnim)
        {
            bEndCheck = m_pMonster->Get_EndAnim(AN_BACKJUMP);
        }
        break;

    case 1:
        if ((AN_JUMPATTACK) == iCurAnim)
        {
            bEndCheck = m_pMonster->Get_EndAnim(AN_JUMPATTACK);
        }
        break;

    default:
        break;
    }

    return bEndCheck;
}

void CState_RaxasiaP2_TeleportAttack::Collider_Check(_double CurTrackPos)
{
    if (m_iRouteTrack == 1)
    {
        if ((CurTrackPos >= 90.f && CurTrackPos <= 110.f) ||
            (CurTrackPos >= 140.f && CurTrackPos <= 165.f))
        {
            m_pMonster->Active_CurrentWeaponCollider(1.3f, 0, HIT_TYPE::HIT_METAL, ATTACK_STRENGTH::ATK_NORMAL);
            m_pMonster->Active_Debuff(0, 0.4f);
        }
        else
        {
            m_pMonster->DeActive_CurrentWeaponCollider(0);
            m_pMonster->DeActive_Debuff(0);
        }

        if ((CurTrackPos >= 75.f && CurTrackPos <= 100.f))
        {
            m_pMonster->Active_CurrentWeaponCollider(1.3f, 1, HIT_TYPE::HIT_METAL, ATTACK_STRENGTH::ATK_NORMAL);
            m_pMonster->Active_Debuff(0, 0.4f);
        }
        else
        {
            m_pMonster->DeActive_CurrentWeaponCollider(1);
            m_pMonster->DeActive_Debuff(0);
        }
    }
}

void CState_RaxasiaP2_TeleportAttack::Effect_Check(_double CurTrackPos)
{
    if (m_iRouteTrack == 0)
    {
        if (CurTrackPos >= 35.f && CurTrackPos <= 45.f)
        {
            if (!m_bAccel)
            {
                m_pMonster->Active_Effect(CRaxasia::EFFECT_THUNDERENVELOP_SMALL, true);
                m_pMonster->Active_Effect(CRaxasia::EFFECT_THUNDERACCEL, true);
                m_bAccel = true;
            }
        }
        else
        {
            m_pMonster->DeActive_Effect(CRaxasia::EFFECT_THUNDERENVELOP_SMALL);
            m_pMonster->DeActive_Effect(CRaxasia::EFFECT_THUNDERACCEL);
        }
    }
    else
    {
        if (CurTrackPos >= 130.f && CurTrackPos <= 140.f)
        {
            if (!m_bAccel)
            {
                m_pMonster->Active_Effect(CRaxasia::EFFECT_THUNDERENVELOP_SMALL, true);
                m_pMonster->Active_Effect(CRaxasia::EFFECT_THUNDERACCEL, true);
                m_bAccel = true;
            }
        }
        else
        {
            m_pMonster->DeActive_Effect(CRaxasia::EFFECT_THUNDERENVELOP_SMALL);
            m_pMonster->DeActive_Effect(CRaxasia::EFFECT_THUNDERACCEL);
        }
    }
}

void CState_RaxasiaP2_TeleportAttack::Control_Sound(_double CurTrackPos)
{
    if (m_iRouteTrack == 0)
    {
        if (CurTrackPos >= 35.f && CurTrackPos <= 45.f)
        {
            if (!m_bAccelSound)
            {
                m_pMonster->Play_Sound(CPawn::PAWN_SOUND_VOICE, TEXT("SE_NPC_Raxasia_SK_PJ_Spark_Electric_Loop_02.wav"), true);
                m_pMonster->Play_Sound(CPawn::PAWN_SOUND_EFFECT2, TEXT("SE_NPC_Raxasia_SK_FX_Blink_Spark_Foot_03.wav"), true);
                m_bAccel = true;
            }
        }
        else
        {
            m_pMonster->Stop_Sound(CPawn::PAWN_SOUND_VOICE);
            m_pMonster->Stop_Sound(CPawn::PAWN_SOUND_EFFECT2);
        }
    }
    else
    {
        if ((CurTrackPos >= 90.f && CurTrackPos <= 110.f) ||
            (CurTrackPos >= 140.f && CurTrackPos <= 165.f))
        {
            if (!m_bSwingSound)
            {
                m_pMonster->Play_Sound(CPawn::PAWN_SOUND_EFFECT1, TEXT("SE_NPC_SK_WS_BroadSword_06.wav"), false);
                m_bSwingSound = true;
            }
        }
        else
        {
            m_bSwingSound = false;
        }

        if ((CurTrackPos >= 75.f && CurTrackPos <= 100.f))
        {
            if (!m_bShieldSwingSound)
            {
                m_pMonster->Play_Sound(CPawn::PAWN_SOUND_EFFECT2, TEXT("SE_NPC_SK_WS_Blunt_L.wav"), false);
                m_bShieldSwingSound = true;
            }
        }

        if (CurTrackPos >= 130.f && CurTrackPos <= 140.f)
        {
            if (!m_bAccelSound)
            {
                m_pMonster->Play_Sound(CPawn::PAWN_SOUND_VOICE, TEXT("SE_NPC_Raxasia_SK_PJ_Spark_Electric_Loop_02.wav"), true);
                m_pMonster->Play_Sound(CPawn::PAWN_SOUND_EFFECT2, TEXT("SE_NPC_Raxasia_SK_FX_Blink_Spark_Foot_03.wav"), true);
                m_bAccel = true;
            }
        }
        else
        {
            m_pMonster->Stop_Sound(CPawn::PAWN_SOUND_VOICE);
            m_pMonster->Stop_Sound(CPawn::PAWN_SOUND_EFFECT2);
        }
    }
}

CState_RaxasiaP2_TeleportAttack* CState_RaxasiaP2_TeleportAttack::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_RaxasiaP2_TeleportAttack* pInstance = new CState_RaxasiaP2_TeleportAttack(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_RaxasiaP2_TeleportAttack"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_RaxasiaP2_TeleportAttack::Free()
{
    __super::Free();
}