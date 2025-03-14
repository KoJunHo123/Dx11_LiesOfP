#include "stdafx.h"
#include "State_Raxasia_CutScene_Die.h"
#include "GameInstance.h"
#include "Model.h"
#include "Raxasia.h"
#include "CutScene.h"

#include "GameInterface_Controller.h"
#include "Effect_Manager.h"

CState_Raxasia_CutScene_Die::CState_Raxasia_CutScene_Die(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_Raxasia_CutScene_Die::Initialize(_uint iStateNum, void* pArg)
{

    m_iStateNum = iStateNum;
    //CSimonManus::FSMSTATE_DESC* pDesc = static_cast<CSimonManus::FSMSTATE_DESC*>(pArg);

    return S_OK;
}

HRESULT CState_Raxasia_CutScene_Die::Start_State(void* pArg)
{
    // 모델이 달라서 여기서 해주기
    m_iAnimation_KneeDown = m_pMonster->Get_Model()->Find_AnimationIndex("AS_Raxasia_knee_down", 1.f);
    m_iAnimation_Die = m_pMonster->Get_Model()->Find_AnimationIndex("AS_Raxasia_Raxasia_Dead_C00_CINE", 1.f);
    m_pMonster->Get_Model()->Set_SpeedPerSec(m_iAnimation_KneeDown, 38);

    m_pMonster->Change_Animation(m_iAnimation_KneeDown, false, 0.f, 0);

    m_isStartCutScene = false;
    m_fDelay = 0.f;

    m_pCutSceneWeapon = dynamic_cast<CRaxasia*>(m_pMonster)->Get_CutSceneWeapon();

    return S_OK;
}

void CState_Raxasia_CutScene_Die::Update(_float fTimeDelta)
{
    _double TrackPosition = m_pMonster->Get_CurrentTrackPos();
    _int iFrame = m_pMonster->Get_Frame();
    _uint iCurAnim = m_pMonster->Get_CurrentAnimIndex();

    if (!m_isBoom)
        m_fBoomTime += fTimeDelta;
    //if (iFrame > 50)
    //{
    //    m_pMonster->Stop_Animation();
    //}

    //m_fDelay += fTimeDelta;
    //if (m_fDelay >= 0.1f && !m_isStartCutScene)
    //{
    //    m_pMonster->Play_Animation();
    //    m_isStartCutScene = true;
    //}
    //else if (m_fDelay < 16.f)
    //{
    //    m_pMonster->Stop_Animation();
    //}


    End_Check(fTimeDelta);

    _Vec3 vMove = m_pMonster->Get_Model()->Get_BoneCombindTransformationMatrix_Ptr(3)->Translation();
    _float4x4 TransMat;
    XMStoreFloat4x4(&TransMat, m_pMonster->Get_Model()->Get_Bones()[3]->Get_TransformationMatrix());
    TransMat._41 = TransMat._42 = TransMat._43 = 0.f;

    m_pMonster->Get_Model()->Get_Bones()[3]->Set_TransformationMatrix(TransMat);;

    m_pMonster->Get_Model()->Update_Bone();

    vMove = XMVector3TransformNormal(vMove, m_pMonster->Get_Transform()->Get_WorldMatrix()) * 0.8f;
    vMove.x = vMove.z = 0.f;


    m_pMonster->Get_RigidBody()->Set_Velocity((vMove - m_vRootMoveStack) / fTimeDelta);

    m_vRootMoveStack = vMove;

    if (!m_isBoom && m_fBoomTime > 8.f)
    {
        CEffect_Manager::Get_Instance()->Add_Effect_ToLayer_Rot(LEVEL_GAMEPLAY, TEXT("Raxasia_CutScene_Death_Top"));
        m_isBoom = true;
    }

    if (!m_isKneeDown && iFrame > 50)
    {
        _Vec3 vCurrentPos = m_pMonster->Get_Transform()->Get_State(CTransform::STATE_POSITION);
        vCurrentPos.y -= 1.2f;

        CEffect_Manager::Get_Instance()->Add_Effect_ToLayer_Rot(LEVEL_GAMEPLAY, TEXT("Raxasia_CutScene_Weapon_DriveGround"), vCurrentPos);
        m_isKneeDown = true;
    }


    Control_Dialog(iFrame);
}

void CState_Raxasia_CutScene_Die::End_State()
{
    m_pCutSceneWeapon = nullptr;
}

void CState_Raxasia_CutScene_Die::End_Check(_float fTimeDelta)
{
    if (m_pMonster->Get_EndAnim(m_iAnimation_KneeDown))
    {
        m_fDelay += fTimeDelta;

        if (m_fDelay < 5.f)
            return;

        dynamic_cast<CRaxasia*>(m_pMonster)->Get_ShieldWeapon()->Set_IsUpdatePos(false);
        m_pMonster->Get_Transform()->Rotation(_vector{ 0, 1, 0, 0 }, XMConvertToRadians(150.f));
        m_pMonster->Change_Animation(m_iAnimation_Die, false, 0.f);
        m_pMonster->Get_Model()->Set_SpeedPerSec(m_iAnimation_Die, 31.5);

    }
    else if (m_pMonster->Get_EndAnim(m_iAnimation_Die))
    {
        // 죽이기
    }
}

void CState_Raxasia_CutScene_Die::Control_Dialog(_int iFrame)
{
    if (m_pMonster->Get_CurrentAnimIndex() == m_iAnimation_Die)
    {
        if (!m_isShowDialog && iFrame >= 20)
        {
            GET_GAMEINTERFACE->Show_Script(TEXT("아아, 시몬님. 당신이야말로 제 유일한..."), TEXT("none"), 9.35f);
            m_isShowDialog = true;
        }
    }
}

CState_Raxasia_CutScene_Die* CState_Raxasia_CutScene_Die::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_Raxasia_CutScene_Die* pInstance = new CState_Raxasia_CutScene_Die(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_Raxasia_CutScene_Die"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_Raxasia_CutScene_Die::Free()
{
    __super::Free();
}