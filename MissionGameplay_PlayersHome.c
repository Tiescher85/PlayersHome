
const int MENU_PH_MANAGER = PH_MENU_ID;
const int MENU_PH_ADMIN   = PH_MENU_ADMIN_ID;

ref PH_Manager g_PH_Manager;
ref PH_AdminMenu g_PH_Admin;

modded class MissionGameplay
{
    override void OnRPC(PlayerIdentity sender, Object target, int rpc_type, ParamsReadContext ctx)
    {
        super.OnRPC(sender, target, rpc_type, ctx);
        if (rpc_type == PH_RPC_OPEN_MANAGER)
        {
            GetGame().GetUIManager().EnterScriptedMenu(MENU_PH_MANAGER, null);
        }
        else if (rpc_type == PH_RPC_SNAPSHOT)
        {
            Param1<ref map<string, ref Param>> snap; if(!ctx.Read(snap)) return;
            if(g_PH_Manager) g_PH_Manager.ApplySnapshot(snap.param1);
        }
        else if (rpc_type == PH_RPC_ADMIN_OPEN)
        {
            GetGame().GetUIManager().EnterScriptedMenu(MENU_PH_ADMIN, null);
            // auto query near 300m
            GetGame().RPCSingleParam(null, PH_RPC_ADMIN_CMD, new Param2<int,string>(PH_ADMIN_QUERY_HOMES, "near:300"), true);
        }
        else if (rpc_type == PH_RPC_ADMIN_SNAPSHOT)
        {
            Param3<ref array<string>, ref array<string>, ref array<string>> data;
            if(!ctx.Read(data)) return;
            if(g_PH_Admin) g_PH_Admin.ApplySnapshot(data.param1, data.param2, data.param3);
        }
        else if (rpc_type == PH_RPC_ADMIN_NOTE)
        {
            Param1<string> msg; if(!ctx.Read(msg)) return;
            PH_Toast.Show(msg.param1);
        }
    }
};
