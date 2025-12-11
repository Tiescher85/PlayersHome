
class ActionPH_DoorMenu: ActionInteractBase
{
    void ActionPH_DoorMenu(){ m_Text="PlayersHome"; m_CommandUID=DayZPlayerConstants.CMD_ACTIONMOD_OPENITEM; m_StanceMask=DayZPlayerConstants.STANCEMASK_ERECT|DayZPlayerConstants.STANCEMASK_CROUCH; }
    override void CreateConditionComponents(){ m_ConditionItem=new CCINone(); m_ConditionTarget=new CCTCursor(); }
    override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
    {
        Building b; if(!Class.CastTo(b, target.GetObject())) return false;
        int di = b.GetDoorIndexFromComponent(target.GetComponentIndex());
        return di >= 0;
    }
    override void OnStartServer(ActionData action_data)
    {
        super.OnStartServer(action_data);
        PlayerBase p=action_data.m_Player; Building b=Building.Cast(action_data.m_Target.GetObject());
        if(!p||!b) return;
        int di = b.GetDoorIndexFromComponent(action_data.m_Target.GetComponentIndex());
        PH_Service s=PH.Get(); if(!s) return;
        s.SetDoorContext(p, b, di);
        PlayerIdentity id=p.GetIdentity(); if(id) GetGame().RPCSingleParam(p, PH_RPC_OPEN_MANAGER, null, true, id);
    }
};
