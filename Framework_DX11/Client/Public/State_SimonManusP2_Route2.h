#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)       //���̺� �ֵθ���

class CState_SimonManusP2_Route2 :
    public CState
{
private:
    typedef enum {
        AN_ROUTE_FIRST = 54,        //SlideSwing
        AN_ROUTE_LAST = 23,       //HighJump
    }ANIMNUM;

public:
    CState_SimonManusP2_Route2(class CFsm* pFsm, class CMonster* pMonster);
    virtual ~CState_SimonManusP2_Route2() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum, void* pArg) override;
    virtual HRESULT Start_State(void* pArg = nullptr) override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CMonster* m_pMonster = { nullptr };

    _Vec4               m_vTargetDir{};
    _Vec3               m_vFlyMoveStack{};

    _Vec4               m_vRimLightColor = {};
    _bool               m_isRimLight = { false };

    _bool               m_isJump = { false };
    _bool               m_bStartSpot = { true };

    _bool               m_bSwing = { false };

    _bool               m_bSwingSound = { false };
    _bool               m_bLandSound = { false };
    
    _uint               m_iRouteTrack = {};

private:
    _bool               End_Check();
    void                Collider_Check(_double CurTrackPos);
    void                Effect_Check(_double CurTrackPos);
    void                Update_Rimlight(_float fTimeDelta, _double CurTrackPos);
    void                Control_Sound(_double CurTrackPos);

public:
    static CState_SimonManusP2_Route2* Create(class CFsm* pFsm, class CMonster* pMonster, _uint iStateNum, void* pArg = nullptr);
    virtual void Free() override;

};

END