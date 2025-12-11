
modded class MissionServer
{
    override void OnInit(){ super.OnInit(); PH.Get(); }
    override void OnUpdate(float timeslice){ super.OnUpdate(timeslice); PH_Service svc = PH.Get(); if(svc) svc.Tick(); }

    override void OnRPC(PlayerIdentity sender, Object target, int rpc_type, ParamsReadContext ctx)
    {
        super.OnRPC(sender, target, rpc_type, ctx);
        if(!sender) return;
        PlayerBase p = PlayerBase.Cast(GetGame().GetPlayerByIdentity(sender));
        PH_Service s = PH.Get(); if(!s) return;

        if(rpc_type == PH_RPC_CMD)
        {
            Param2<int, string> data; if(!ctx.Read(data)) return; int cmd=data.param1; string arg=data.param2;
            Building b; int di; s.GetDoorContext(p, b, di);
            string msg="";
            if(cmd==PH_CMD_BUY) s.BuyHome(p, b, msg);
            else if(cmd==PH_CMD_RENT) s.RentHome(p, b, msg);
            else if(cmd==PH_CMD_EXTEND){ PH_Home h; if(s.GetHomeByBuilding(b,h)) s.ExtendRent(p, h, msg); }
            else if(cmd==PH_CMD_LOCK){ PH_Home h; if(s.GetHomeByBuilding(b,h)) s.LockDoor(h, di); }
            else if(cmd==PH_CMD_UNLOCK){ PH_Home h; if(s.GetHomeByBuilding(b,h)) s.UnlockDoor(h, di); }
            else if(cmd==PH_CMD_LIGHT_ON){ PH_Home h; if(s.GetHomeByBuilding(b,h)) s.SetLights(h,true); }
            else if(cmd==PH_CMD_LIGHT_OFF){ PH_Home h; if(s.GetHomeByBuilding(b,h)) s.SetLights(h,false); }
            else if(cmd==PH_CMD_OPEN_VAULT){ PH_Home h; if(s.GetHomeByBuilding(b,h)) s.OpenVault(p,h); }
            s.MakeSnapshot(p);
        }
        else if(rpc_type == PH_RPC_ADMIN_CMD)
        {
            if(!s.IsAdmin(sender.GetId())) return;
            Param2<int, string> data; if(!ctx.Read(data)) return; int cmd=data.param1; string arg=data.param2;
            if(cmd==PH_ADMIN_RELOAD){ s = new PH_Service(); s.AdminBroadcast("[PlayersHome] Configs neu geladen."); }
            else if(cmd==PH_ADMIN_DELETE_HOME){ s.AdminDeleteHome(arg); }
            else if(cmd==PH_ADMIN_TRANSFER_HOME){ array<string> parts = new array<string>(); arg.Split("|", parts); if(parts.Count()>=2) s.AdminTransferHome(parts[0], parts[1]); }
            else if(cmd==PH_ADMIN_GIVE_COINS){ array<string> parts = new array<string>(); arg.Split("|", parts); if(parts.Count()>=2) s.AdminGiveCoins(parts[0], parts[1].ToInt()); }
            else if(cmd==PH_ADMIN_QUERY_HOMES){ s.AdminQueryHomes(p, arg); }
            else if(cmd==PH_ADMIN_GET_ONLINE){ s.AdminQueryHomes(p, ""); } // sends snapshot with current players & (no filter) all homes (could be heavy; client usually filters near)
        }
    }
};
