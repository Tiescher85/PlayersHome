
class ActionPH_AdminOpen: ActionSingleUseBase
{
    void ActionPH_AdminOpen(){ m_Text="PlayersHome Admin"; }
    override void CreateConditionComponents(){ m_ConditionItem = new CCINonRuined(); m_ConditionTarget = new CCTNone(); }
    override bool HasTarget(){ return false; }
    override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
    {
        if(!item) return false;
        if(item.GetType() != "PlayersHome_AdminTool") return false;
        PlayerIdentity id = player.GetIdentity(); if(!id) return false;
        return true;
    }
    override void OnStartServer(ActionData action_data)
    {
        super.OnStartServer(action_data);
        PlayerBase p = action_data.m_Player; if(!p) return;
        PlayerIdentity id = p.GetIdentity(); if(!id) return;
        PH_Service s=PH.Get(); if(!s) return;
        if(!s.IsAdmin(id.GetId())) return;
        GetGame().RPCSingleParam(p, PH_RPC_ADMIN_OPEN, null, true, id);
    }
};
