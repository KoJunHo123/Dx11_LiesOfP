#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_CarcassBigA_Grogy : public CState
{
private:
    typedef enum {
        AN_GROGY_START = 17, AN_GROGY_LOOP = 16, AN_GROGY_END = 15
    }ANIMNUM;
public:
    CState_CarcassBigA_Grogy(class CFsm* pFsm, class CMonster* pMonster);
    virtual ~CState_CarcassBigA_Grogy() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum, void* pArg) override;
    virtual HRESULT Start_State(void* pArg = nullptr) override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CMonster* m_pMonster = { nullptr };

    _float              m_fGrogyTime = { 0.f };
    _float              m_fGrogyDuration = { 3.f };

    _uint               m_iAnimCnt = {};

private:
    _bool               End_Check();

public:
    static CState_CarcassBigA_Grogy* Create(class CFsm* pFsm, class CMonster* pMonster, _uint iStateNum, void* pArg = nullptr);
    virtual void Free() override;

};

END