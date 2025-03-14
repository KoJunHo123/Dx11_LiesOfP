#include "stdafx.h"
#include "..\Public\CarcassTail.h"

#include "GameInstance.h"
#include "Player.h"
#include "Fsm.h"

#include "GameInterface_Controller.h"

#include "State_CarcassTail_Idle.h"
#include "State_CarcassTail_Die.h"
#include "State_CarcassTail_Grogy.h"
#include "State_CarcassTail_HitFatal.h"
#include "State_CarcassTail_KnockBack.h"

#include "State_CarcassTail_Heading.h"
#include "State_CarcassTail_Leap.h"
#include "State_CarcassTail_MultyHittingDown.h"
#include "State_CarcassTail_Scratching.h"
#include "State_CarcassTail_TailSwingDown.h"
#include "State_CarcassTail_TailSwip.h"
#include "State_CarcassTail_TailSwipMultiple.h"

#include "State_CarcassTail_HeadingMultiple.h"
#include "State_CarcassTail_LeapAttack.h"
#include "State_CarcassTail_LeapToAttack.h"
#include "State_CarcassTail_ScratchingMultiple.h"
#include "State_CarcassTail_ScratchingToSwip.h"

#include "Dissolve_PowerAttack.h"

CCarcassTail::CCarcassTail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMonster{ pDevice, pContext }
{
}

CCarcassTail::CCarcassTail(const CCarcassTail& Prototype)
	: CMonster{ Prototype }
{
}

HRESULT CCarcassTail::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCarcassTail::Initialize(void* pArg)
{
	OBJECT_DEFAULT_DESC* pDefaultDesc = static_cast<OBJECT_DEFAULT_DESC*>(pArg);

	CGameObject::GAMEOBJECT_DESC		Desc{};
	Desc.fSpeedPerSec = 1.5f;
	Desc.fRotationPerSec = 30.f;

	if (FAILED(__super::Initialize(&Desc)))
		return E_FAIL;

	m_eStat.fHp = 900.f;
	m_eStat.fMaxHp = 900.f;
	m_eStat.fAtk = 210.f;
	//m_eStat.fDefence = 2.f;

	m_eStat.fGrogyPoint = 0.f;
	m_eStat.fMaxGrogyPoint = 210.f;

	m_iKnockBackResist = 5;
	m_iErgoPoint = 400;

	m_pTransformCom->LookAt(_vector{ 0, 0, -1, 0 });
	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_iOriginCellNum = pDefaultDesc->iCurrentCellNum;

	m_pNavigationCom->Move_to_Cell(m_pRigidBodyCom, m_iOriginCellNum);
	m_iInitRoomNum = m_pNavigationCom->Get_Cell_AreaNum(m_iOriginCellNum);

	m_pModelCom->SetUp_Animation(rand() % 20, true);

	if (FAILED(Ready_FSM()))
		return E_FAIL;

	if (FAILED(Ready_Effect()))
		return E_FAIL;

	m_strObjectTag = TEXT("Monster");

	m_vRimLightColor = { 0.f, 0.f, 0.f, 0.f };

	m_vCenterOffset = _Vec3{ 0.f, 0.91f, 0.f };

	m_bDiscover = false;


	GET_GAMEINTERFACE->Register_Pointer_Into_OrthoUIPage(UI_ORTHO_OBJ_TYPE::ORTHO_CARCASS_TAIL, this);

	GET_GAMEINTERFACE->Set_OnOff_OrthoUI(false, this);

	return S_OK;
}

void CCarcassTail::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);

	for (_uint i = 0; i < TYPE_END; ++i)
	{
		m_pColliderObject[i]->Priority_Update(fTimeDelta);
	}

	for (auto& Effect : m_SurfaceEffect)
		Effect->Priority_Update(fTimeDelta);

	m_pPowerAttackEffect->Priority_Update(fTimeDelta);
}

void CCarcassTail::Update(_float fTimeDelta)
{
	if (m_pGameInstance->Get_Player_AreaNum() == m_iInitRoomNum)
		m_vCurRootMove = XMVector3TransformNormal(m_pModelCom->Play_Animation(fTimeDelta), m_pTransformCom->Get_WorldMatrix());
	else
		m_vCurRootMove = _Vec3(0.f, 0.f, 0.f);

	if (m_bDebuffed[SURFACE_ELECTRIC])
	{
		m_vCurRootMove *= 0.8f;
	}

	m_pRigidBodyCom->Set_Velocity(m_vCurRootMove / fTimeDelta);

	m_pFsmCom->Update(fTimeDelta);

	for (auto& Effect : m_SurfaceEffect)
		Effect->Update(fTimeDelta);
	m_pPowerAttackEffect->Update(fTimeDelta);

	_float4x4 WorldMat{};
	XMStoreFloat4x4(&WorldMat , m_pModelCom->Get_BoneCombindTransformationMatrix(7) * XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix_Ptr()));

	Update_Collider();
	Update_Debuff(fTimeDelta);

	for (_uint i = 0; i < TYPE_END; ++i)
	{
		m_pColliderObject[i]->Update(fTimeDelta);
	}
}

void CCarcassTail::Late_Update(_float fTimeDelta)
{
	if (m_pGameInstance->Get_Player_AreaNum() == m_iInitRoomNum)
	{
		if (m_pGameInstance->isIn_Frustum_WorldSpace(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 50.f))
		{
			__super::Late_Update(fTimeDelta);

			m_pRigidBodyCom->Update(fTimeDelta);

			for(auto& Effect : m_SurfaceEffect)
				Effect->Late_Update(fTimeDelta);
			m_pPowerAttackEffect->Late_Update(fTimeDelta);

			m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);

			for (_uint i = 0; i < TYPE_END; ++i)
			{
				m_pColliderObject[i]->Late_Update(fTimeDelta);
			}


			for (_uint i = 0; i < CT_END - 1; ++i)
			{
				m_pGameInstance->Add_ColliderList(m_EXCollider[i]);
			}
			m_pGameInstance->Add_ColliderList(m_pColliderCom);
		}
	}
}

HRESULT CCarcassTail::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

#ifdef _DEBUG
	m_pGameInstance->Add_DebugObject(m_pColliderCom);

	for (_uint i = 0; i <CT_END - 1; ++i)
	{
		m_pGameInstance->Add_DebugObject(m_EXCollider[i]);
	}
#endif
	return S_OK;
}

void CCarcassTail::Resetting()
{
	m_vRimLightColor = { 0.f, 0.f, 0.f, 0.f };

	m_eStat.fHp = 900.f;
	m_eStat.fMaxHp = 900.f;
	m_eStat.fAtk = 210.f;

	m_eStat.fGrogyPoint = 0.f;
	m_eStat.fMaxGrogyPoint = 210.f;

	m_eStat.bWeakness = false;
	m_eStat.bFatalAttack = false;

	m_bDieState = false;

	m_bDiscover = false;
	m_bFirstMeetCheck = false;

	m_bFatalAttacked = false;

	m_pNavigationCom->Move_to_Cell(m_pRigidBodyCom, m_iOriginCellNum);
	m_iInitRoomNum = m_pNavigationCom->Get_Cell_AreaNum(m_iOriginCellNum);

	Reset_Debuff();

	Change_State(CMonster::IDLE);

	GET_GAMEINTERFACE->Set_OnOff_OrthoUI(false, this);

	m_pRigidBodyCom->Add_Actor();
}

void CCarcassTail::On_PowerAttack(_bool bOn)
{
	if (bOn != m_pPowerAttackEffect->Get_On())
		m_pPowerAttackEffect->Set_On(bOn);
}

void CCarcassTail::Active_CurrentWeaponCollider(_float fDamageRatio, _uint iCollIndex, HIT_TYPE eHitType, ATTACK_STRENGTH eAtkStrength)
{
	m_pColliderObject[iCollIndex]->Active_Collider(fDamageRatio, iCollIndex, eHitType, eAtkStrength);
}

void CCarcassTail::DeActive_CurrentWeaponCollider(_uint iCollIndex)
{
	m_pColliderObject[iCollIndex]->DeActive_Collider();
}

void CCarcassTail::Active_Debuff(_int iIndex, _float fDebuffRatio)
{
	m_pColliderObject[iIndex]->Active_Debuff(fDebuffRatio);
}

void CCarcassTail::DeActive_Debuff(_int iIndex)
{
	m_pColliderObject[iIndex]->DeActive_Debuff();
}

HRESULT CCarcassTail::Ready_Components()
{
	if (FAILED(__super::Ready_Components()))
		return E_FAIL;

	/* FOR.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_CarcassTail"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* FOR.Com_Collider */		//Body
	CBounding_OBB::BOUNDING_OBB_DESC			ColliderDesc{};
	ColliderDesc.vExtents = _float3(0.45f, 0.45f, 0.9f);
	ColliderDesc.vCenter = _float3(0.5f, 0.f, 0.f);
	ColliderDesc.vAngles = _float3(0.f, 0.f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
		return E_FAIL;
	m_pColliderCom->Set_Owner(this);
	m_pColliderBindMatrix[CT_BODY_UPPER] = m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr(8);

	//MiddleBody
	ColliderDesc.vExtents = _float3(0.6f, 0.55f, 0.4f);
	ColliderDesc.vCenter = _float3(0.f, 0.f, 0.f);
	ColliderDesc.vAngles = _float3(0.f, 0.f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider_BodyMiddle"), reinterpret_cast<CComponent**>(&m_EXCollider[CT_BODY_MIDDLE]), &ColliderDesc)))
		return E_FAIL;
	m_pColliderBindMatrix[CT_BODY_MIDDLE] = m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr(7);


	//LowerBody
	ColliderDesc.vExtents = _float3(0.35f, 0.6f, 0.85f);
	ColliderDesc.vCenter = _float3(0.0f, -0.2f, 0.f);
	ColliderDesc.vAngles = _float3(0.f, 0.f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider_BodyLower"), reinterpret_cast<CComponent**>(&m_EXCollider[CT_BODY_LOWER]), &ColliderDesc)))
		return E_FAIL;
	m_pColliderBindMatrix[CT_BODY_LOWER] = m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr(5);

	//Head
	ColliderDesc.vExtents = _float3(0.4f, 0.5f, 0.5f);
	ColliderDesc.vCenter = _float3(0.2f, 0.1f, 0.f);
	ColliderDesc.vAngles = _float3(0.f, 0.f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider_Head"), reinterpret_cast<CComponent**>(&m_EXCollider[CT_HEAD]), &ColliderDesc)))
		return E_FAIL;
	m_pColliderBindMatrix[CT_HEAD] = m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr(m_pModelCom->Get_UFBIndices(UFB_HEAD));


	//LOWERArmLeft
	ColliderDesc.vExtents = _float3(0.8f, 0.3f, 0.3f);
	ColliderDesc.vCenter = _float3(0.3f, 0.f, 0.f);
	ColliderDesc.vAngles = _float3(0.f, 0.f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider_ArmLeft"), reinterpret_cast<CComponent**>(&m_EXCollider[CT_ARM_LEFT]), &ColliderDesc)))
		return E_FAIL;
	m_pColliderBindMatrix[CT_ARM_LEFT] = m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr(m_pModelCom->Get_UFBIndices(UFB_HAND_LEFT) - 1);


	//LOWERArmRight
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider_ArmRight"), reinterpret_cast<CComponent**>(&m_EXCollider[CT_ARM_RIGHT]), &ColliderDesc)))
		return E_FAIL;
	m_pColliderBindMatrix[CT_ARM_RIGHT] = m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr(m_pModelCom->Get_UFBIndices(UFB_HAND_RIGHT) - 1);



	//LegLeft
	ColliderDesc.vExtents = _float3(0.9f, 0.5f, 0.35f);
	ColliderDesc.vCenter = _float3(-0.25f, 0.3f, 0.f);
	ColliderDesc.vAngles = _float3(0.f, 0.f, -0.6f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider_UpperLegLeft"), reinterpret_cast<CComponent**>(&m_EXCollider[CT_LEG_LEFT]), &ColliderDesc)))
		return E_FAIL;
	m_pColliderBindMatrix[CT_LEG_LEFT] = m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr(m_pModelCom->Get_UFBIndices(UFB_FOOT_LEFT) - 1);


	//LegRight
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider_LegRight"), reinterpret_cast<CComponent**>(&m_EXCollider[CT_LEG_RIGHT]), &ColliderDesc)))
		return E_FAIL;
	m_pColliderBindMatrix[CT_LEG_RIGHT] = m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr(m_pModelCom->Get_UFBIndices(UFB_FOOT_RIGHT) - 1);


	for (_int i = 0; i < CT_END - 1; ++i)
	{
		m_EXCollider[i]->Set_Owner(this);
	}

	/* FOR.Com_Collider_OBB */
	CBounding_OBB::BOUNDING_OBB_DESC			ColliderOBBDesc_Obj{};

	ColliderOBBDesc_Obj.vExtents = _float3(1.2f, 0.7f, 0.7f);
	ColliderOBBDesc_Obj.vCenter = _float3(0.3f, 0.f, 0.f);
	ColliderOBBDesc_Obj.vAngles = _float3(0.f, 0.f, 0.f);
	
	CColliderObject::COLIDEROBJECT_DESC Desc{};

	Desc.pBoundingDesc = &ColliderOBBDesc_Obj;
	Desc.eType = CCollider::TYPE_OBB;
	Desc.pSocketBoneMatrix = m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr(m_pModelCom->Get_UFBIndices(UFB_HAND_LEFT) - 1);
	Desc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
	Desc.pSocketBoneMatrix2 = m_pTransformCom->Get_WorldMatrix_Ptr();
	Desc.fDamageAmount = m_eStat.fAtk;
	Desc.pOWner = this;

	m_pColliderObject[TYPE_LEFTHAND] = dynamic_cast<CColliderObject*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_ColliderObj"), &Desc));

	//
	//ColliderOBBDesc_Obj.vExtents = _float3(0.5f, 0.5f, 0.5f);
	//ColliderOBBDesc_Obj.vCenter = _float3(0.f, 0.f, 0.f);
	//ColliderOBBDesc_Obj.vAngles = _float3(0.f, 0.f, 0.f);

	Desc.pSocketBoneMatrix = m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr(m_pModelCom->Get_UFBIndices(UFB_HAND_RIGHT) - 1);

	m_pColliderObject[TYPE_RIGHTHAND] = dynamic_cast<CColliderObject*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_ColliderObj"), &Desc));

	//
	ColliderOBBDesc_Obj.vExtents = _float3(1.2f, 0.4f, 0.4f);
	ColliderOBBDesc_Obj.vCenter = _float3(0.4f, 0.f, 0.f);
	ColliderOBBDesc_Obj.vAngles = _float3(0.f, 0.f, 0.f);

	Desc.pSocketBoneMatrix = m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr(m_pModelCom->Get_UFBIndices(UFB_WEAPON));
	Desc.fDamageAmount = m_eStat.fAtk;

	Desc.iDebuffType = CPlayer::DEBUFF_ACID;
	Desc.fDebuffAmount = 15.f;

	m_pColliderObject[TYPE_TAIL] = dynamic_cast<CColliderObject*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_ColliderObj"), &Desc));


	//
	ColliderOBBDesc_Obj.vExtents = _float3(0.5f, 0.7f, 1.4f);
	ColliderOBBDesc_Obj.vCenter = _float3(0.8f, 0.f, 0.f);
	ColliderOBBDesc_Obj.vAngles = _float3(0.f, 0.f, 0.f);

	Desc.pSocketBoneMatrix = m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr(8);
	Desc.fDamageAmount = m_eStat.fAtk;

	Desc.iDebuffType = CPlayer::DEBUFF_END;
	Desc.fDebuffAmount = 0.f;

	m_pColliderObject[TYPE_IMPACT] = dynamic_cast<CColliderObject*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_ColliderObj"), &Desc));

	//167
	ColliderOBBDesc_Obj.vExtents = _float3(1.0f, 0.4f, 0.4f);
	ColliderOBBDesc_Obj.vCenter = _float3(0.4f, 0.f, 0.f);
	ColliderOBBDesc_Obj.vAngles = _float3(0.f, 0.f, 0.f);

	Desc.pSocketBoneMatrix = m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr(167);
	Desc.fDamageAmount = m_eStat.fAtk;

	Desc.iDebuffType = CPlayer::DEBUFF_ACID;
	Desc.fDebuffAmount = 5.f;

	m_pColliderObject[TYPE_TAIL_2] = dynamic_cast<CColliderObject*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_ColliderObj"), &Desc));

	

	for (_int i = 0; i < TYPE_END; ++i)
	{
		m_pColliderObject[i]->DeActive_Collider();
	}

	// 항상 마지막에 생성하기
	CRigidBody::RIGIDBODY_DESC RigidBodyDesc{};
	RigidBodyDesc.isStatic = false;
	RigidBodyDesc.isGravity = false;
	RigidBodyDesc.pOwnerTransform = m_pTransformCom;
	RigidBodyDesc.pOwnerNavigation = m_pNavigationCom;
	RigidBodyDesc.isCapsule = true;

	RigidBodyDesc.pOwner = this;
	RigidBodyDesc.fStaticFriction = 1.0f;
	RigidBodyDesc.fDynamicFriction = 0.0f;
	RigidBodyDesc.fRestituion = 0.0f;
	RigidBodyDesc.PxLockFlags = PxRigidDynamicLockFlag::eLOCK_ANGULAR_X |
		PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y |
		PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z;

	MyPhysX::GeometryCapsule CapsuleDesc;
	CapsuleDesc.fHeight = 1.f;
	CapsuleDesc.fRadius = 1.f;
	RigidBodyDesc.pGeometryDesc = &CapsuleDesc;

	/* FOR.Com_RigidBody */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_RigidBody"),
		TEXT("Com_RigidBody"), reinterpret_cast<CComponent**>(&m_pRigidBodyCom), &RigidBodyDesc)))
		return E_FAIL;


	return S_OK;
}

HRESULT CCarcassTail::Ready_FSM()
{
	if (FAILED(__super::Ready_FSM()))
		return E_FAIL;

	FSM_INIT_DESC Desc{};
	

	m_pFsmCom->Add_State(CState_CarcassTail_Idle::Create(m_pFsmCom, this, IDLE, &Desc));
	m_pFsmCom->Add_State(CState_CarcassTail_Die::Create(m_pFsmCom, this, DIE, &Desc));
	m_pFsmCom->Add_State(CState_CarcassTail_Grogy::Create(m_pFsmCom, this, GROGY, &Desc));
	m_pFsmCom->Add_State(CState_CarcassTail_HitFatal::Create(m_pFsmCom, this, HITFATAL, &Desc));
	m_pFsmCom->Add_State(CState_CarcassTail_KnockBack::Create(m_pFsmCom, this, KNOCKBACK, &Desc));

	m_pFsmCom->Add_State(CState_CarcassTail_Heading::Create(m_pFsmCom, this, HEADING, &Desc));
	m_pFsmCom->Add_State(CState_CarcassTail_Leap::Create(m_pFsmCom, this, LEAP, &Desc));
	m_pFsmCom->Add_State(CState_CarcassTail_MultyHittingDown::Create(m_pFsmCom, this, MULTYHITTINGDOWN, &Desc));
	m_pFsmCom->Add_State(CState_CarcassTail_Scratching::Create(m_pFsmCom, this, SCRATCHING, &Desc));
	m_pFsmCom->Add_State(CState_CarcassTail_TailSwingDown::Create(m_pFsmCom, this, TAILSWINGDOWN, &Desc));
	m_pFsmCom->Add_State(CState_CarcassTail_TailSwip::Create(m_pFsmCom, this, TAILSWIP, &Desc));
	m_pFsmCom->Add_State(CState_CarcassTail_TailSwipMultiple::Create(m_pFsmCom, this, TAILSWIPMULTIPLE, &Desc));

	m_pFsmCom->Add_State(CState_CarcassTail_HeadingMultiple::Create(m_pFsmCom, this, HEADINGMULTIPLE, &Desc));
	m_pFsmCom->Add_State(CState_CarcassTail_LeapAttack::Create(m_pFsmCom, this, LEAPATTACK, &Desc));
	m_pFsmCom->Add_State(CState_CarcassTail_LeapToAttack::Create(m_pFsmCom, this, LEAPTOATTACK, &Desc));
	m_pFsmCom->Add_State(CState_CarcassTail_ScratchingMultiple::Create(m_pFsmCom, this, SCRATCHINGMULTIPLE, &Desc));
	m_pFsmCom->Add_State(CState_CarcassTail_ScratchingToSwip::Create(m_pFsmCom, this, SCRATCHINGTOWIP, &Desc));
	
	
	m_pFsmCom->Set_State(IDLE);


	return S_OK;

}

HRESULT CCarcassTail::Ready_Effect()
{
	CDissolve_Effect::DISSOLVE_EFFECT_DESC DissolveDesc = {};
	DissolveDesc.fRotationPerSec = XMConvertToRadians(90.f);
	DissolveDesc.fSpeedPerSec = 1.f;
	DissolveDesc.iLevelIndex = LEVEL_GAMEPLAY;
	DissolveDesc.pTarget_ModelCom = m_pModelCom;
	DissolveDesc.pTarget_TransformCom = m_pTransformCom;
	DissolveDesc.strVIBufferTag = TEXT("Prototype_Component_VIBuffer_Dissolve_CarcassTail_PowerAttack");

	m_pPowerAttackEffect = static_cast<CDissolve_PowerAttack*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Effect_Dissolve_PowerAttack"), &DissolveDesc));
	if (nullptr == m_pPowerAttackEffect)
		return E_FAIL;

	m_SurfaceEffect.resize(SURFACE_END);

	DissolveDesc.strVIBufferTag = TEXT("Prototype_Component_VIBuffer_Dissolve_CarcassTail_Fire");
	m_SurfaceEffect[SURFACE_FIRE] = static_cast<CDissolve_Effect*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Effect_Dissolve_Fire"), &DissolveDesc));
	if (nullptr == m_SurfaceEffect[SURFACE_FIRE])
		return E_FAIL;

	DissolveDesc.strVIBufferTag = TEXT("Prototype_Component_VIBuffer_Dissolve_CarcassTail_Electric");
	m_SurfaceEffect[SURFACE_ELECTRIC] = static_cast<CDissolve_Effect*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Effect_Dissolve_Electric"), &DissolveDesc));
	if (nullptr == m_SurfaceEffect[SURFACE_ELECTRIC])
		return E_FAIL;

	//On_PowerAttack(true);
	//On_SurfaceEffect(SURFACE_FIRE, true);
	//On_SurfaceEffect(SURFACE_ELECTRIC, true);

	return S_OK;
}

void CCarcassTail::Update_Collider()
{
	_float4x4 UpdateMat{};

	_Matrix WorldMat = XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix_Ptr());
	XMStoreFloat4x4(&UpdateMat
		, *(m_pColliderBindMatrix[CT_BODY_UPPER]) * WorldMat);
	m_pColliderCom->Update(&UpdateMat);

	XMStoreFloat4x4(&UpdateMat
		, *(m_pColliderBindMatrix[CT_BODY_MIDDLE]) * WorldMat);
	m_EXCollider[CT_BODY_MIDDLE]->Update(&UpdateMat);

	XMStoreFloat4x4(&UpdateMat
		, *(m_pColliderBindMatrix[CT_BODY_LOWER]) * WorldMat);
	m_EXCollider[CT_BODY_LOWER]->Update(&UpdateMat);

	XMStoreFloat4x4(&UpdateMat
		, *(m_pColliderBindMatrix[CT_HEAD]) * WorldMat);
	m_EXCollider[CT_HEAD]->Update(&UpdateMat);


	XMStoreFloat4x4(&UpdateMat
		, *(m_pColliderBindMatrix[CT_ARM_LEFT]) * WorldMat);
	m_EXCollider[CT_ARM_LEFT]->Update(&UpdateMat);
	XMStoreFloat4x4(&UpdateMat
		, *(m_pColliderBindMatrix[CT_ARM_RIGHT]) * WorldMat);
	m_EXCollider[CT_ARM_RIGHT]->Update(&UpdateMat);


	XMStoreFloat4x4(&UpdateMat
		, *(m_pColliderBindMatrix[CT_LEG_LEFT]) * WorldMat);
	m_EXCollider[CT_LEG_LEFT]->Update(&UpdateMat);
	XMStoreFloat4x4(&UpdateMat
		, *(m_pColliderBindMatrix[CT_LEG_RIGHT]) * WorldMat);
	m_EXCollider[CT_LEG_RIGHT]->Update(&UpdateMat);

}

CCarcassTail* CCarcassTail::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCarcassTail* pInstance = new CCarcassTail(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CCarcassTail"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CPawn* CCarcassTail::Clone(void* pArg)
{
	CCarcassTail* pInstance = new CCarcassTail(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CCarcassTail"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCarcassTail::Free()
{
	for (_uint i = 0; i < TYPE_END; ++i)
	{
		Safe_Release(m_pColliderObject[i]);
	}

	for (_uint i = 0; i < CT_END - 1; ++i)
	{
		Safe_Release(m_EXCollider[i]);
	}

	Safe_Release(m_pPowerAttackEffect);

	__super::Free();

}