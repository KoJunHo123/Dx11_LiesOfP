#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_Player_OH_Jump : public CState
{
public:
    CState_Player_OH_Jump(class CFsm* pFsm, class CPlayer* pPlayer);
    virtual ~CState_Player_OH_Jump() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum, void* pArg) override;
    virtual HRESULT Start_State(void* pArg = nullptr) override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CPlayer* m_pPlayer = { nullptr };

    _uint               m_iAnimation_JumpUp = {};
    _uint               m_iAnimation_JumpFall = {};
    _uint               m_iAnimation_JumpLand = {};

public:
    static CState_Player_OH_Jump* Create(class CFsm* pFsm, class CPlayer* pPlayer, _uint iStateNum, void* pArg = nullptr);
    virtual void Free() override;

};

END