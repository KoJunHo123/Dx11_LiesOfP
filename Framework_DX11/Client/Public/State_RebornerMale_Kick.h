#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_RebornerMale_Kick : public CState
{
private:
    typedef enum {
        AN_KICK = 4
    }ANIMNUM;
public:
    CState_RebornerMale_Kick(class CFsm* pFsm, class CMonster* pMonster);
    virtual ~CState_RebornerMale_Kick() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum, void* pArg) override;
    virtual HRESULT Start_State(void* pArg = nullptr) override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CMonster* m_pMonster = { nullptr };

    _bool           m_bFrameDelay = { false };

private:
    _bool               End_Check();
    void                Collider_Check(_double CurTrackPos);

public:
    static CState_RebornerMale_Kick* Create(class CFsm* pFsm, class CMonster* pMonster, _uint iStateNum, void* pArg = nullptr);
    virtual void Free() override;

};

END