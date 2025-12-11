
class PH_Context { string BType; vector BPos; int DoorIndex; int ExpiresAt; };

class PH_Home
{
    string HomeId; string PlayerId; float X; float Y; float Z; float Yaw; string WorldName; string HouseType;
    ref array<int> LockedDoors; ref array<string> CoOwners; ref array<string> Members;
    int CreatedAt; int ExpireAt; string Mode; string Name; int LightsOn; vector VaultPos;
    vector GetPos(){ return Vector(X,Y,Z);} void SetPos(vector p){ X=p[0]; Y=p[1]; Z=p[2]; }
};
class PH_PlayerData{ string PlayerId; ref array<ref PH_Home> Homes; int ActiveIndex; int LastSetAt; void PH_PlayerData(){ Homes=new array<ref PH_Home>(); } };
class PH_Persist{ ref array<ref PH_PlayerData> Players; };

class PH_Service
{
    protected ref map<string, ref PH_PlayerData> m_PlayerMap;
    protected ref map<string, ref PH_Home> m_HomeById;
    protected ref map<string, ref PH_Context> m_CtxByPid;
    protected string m_SaveDir; protected string m_SaveFile; protected ref PH_Config m_Cfg;
    protected ref PH_EconBase m_Econ; protected ref PH_Zones m_Zones; protected ref PH_PriceTable m_Price;
    protected int m_LastTick;

    void PH_Service()
    {
        m_PlayerMap=new map<string, ref PH_PlayerData>(); m_HomeById=new map<string, ref PH_Home>(); m_CtxByPid=new map<string, ref PH_Context>();
        m_SaveDir="$profile:PlayersHome"; m_SaveFile=m_SaveDir+"/playershome.json"; m_Cfg=PH_ConfigIO.LoadOrCreate(); if(!FileExist(m_SaveDir)) MakeDirectory(m_SaveDir);
        m_Econ = (m_Cfg.EconomyProvider=="item") ? new PH_EconItem() : new PH_EconVirtual();
        m_Zones = new PH_Zones(); m_Price = PH_EconTableIO.LoadOrCreate(); Load(); m_LastTick=0;
    }
    PH_Config GetCfg(){ return m_Cfg; } PH_EconBase Econ(){ return m_Econ; }

    protected void Load()
    {
        if(FileExist(m_SaveFile))
        {
            PH_Persist d; JsonFileLoader<PH_Persist>.JsonLoadFile(m_SaveFile, d);
            if(d && d.Players)
            {
                foreach(PH_PlayerData pd: d.Players)
                {
                    if(!pd || pd.PlayerId=="") continue; if(!pd.Homes) pd.Homes=new array<ref PH_Home>();
                    m_PlayerMap.Insert(pd.PlayerId, pd);
                    foreach(PH_Home h: pd.Homes){ if(!h) continue; if(!h.LockedDoors) h.LockedDoors=new array<int>(); if(!h.CoOwners) h.CoOwners=new array<string>(); if(!h.Members) h.Members=new array<string>(); m_HomeById.Insert(h.HomeId,h); }
                }
            }
        }
    }
    protected void Save(){ PH_Persist d=new PH_Persist(); d.Players=new array<ref PH_PlayerData>(); foreach(string pid, PH_PlayerData pd: m_PlayerMap) d.Players.Insert(pd); JsonFileLoader<PH_Persist>.JsonSaveFile(m_SaveFile,d); }

    protected string NewGuid(){ Math.Randomize(-1); return Math.RandomInt(0,int.MAX).ToString()+"-"+Math.RandomInt(0,int.MAX).ToString()+"-"+Math.RandomInt(0,int.MAX).ToString(); }
    protected PH_PlayerData GetOrCreatePD(string pid){ PH_PlayerData pd; if(!m_PlayerMap.Find(pid,pd)){ pd=new PH_PlayerData(); pd.PlayerId=pid; m_PlayerMap.Insert(pid,pd);} return pd; }
    protected int Now(){ return GetGame().GetTime()/1000; }
    bool IsAdmin(string pid){ if(pid==""||!m_Cfg.Admins) return false; foreach(string a: m_Cfg.Admins) if(a==pid) return true; return false; }

    string ZoneOf(vector pos){ return (new PH_Zones()).ZoneOf(pos); }
    float ClassPrefixMul(string cls){ float m=1.0; array<string> parts = new array<string>(); cls.Split("_", parts); string pref = parts.Count()>1 ? parts[0]+"_"+parts[1] : cls; m_Price.MulClassPrefix.Find(pref, m); return m; }
    int CalcBuyPrice(PlayerBase p, Building b){ int baseP = m_Cfg.BuyCostCoins; string zone = ZoneOf(b.GetPosition()); float mul = (new PH_Zones()).MultForZone(zone, m_Price) * ClassPrefixMul(b.GetType()); return Math.Round(baseP * mul); }
    int CalcRentPrice(PlayerBase p, Building b){ int baseP = m_Cfg.RentCostCoinsPer7Days; string zone = ZoneOf(b.GetPosition()); float mul = (new PH_Zones()).MultForZone(zone, m_Price) * ClassPrefixMul(b.GetType()); return Math.Round(baseP * mul); }

    bool IsBuildingAllowed(Building b)
    {
        if(!b) return false; if(!m_Cfg || !m_Cfg.AllowOnlyResidential) return true; string t = b.GetType();
        foreach(string pfx: m_Cfg.DeniedBuildingClassPrefixes){ if(pfx!="" && t.IndexOf(pfx)==0) return false; }
        if(m_Cfg.AllowedBuildingClassPrefixes.Count()==0) return true;
        foreach(string ap: m_Cfg.AllowedBuildingClassPrefixes){ if(ap!="" && t.IndexOf(ap)==0) return true; }
        return false;
    }

    bool CanAddHome(PH_PlayerData pd){ return pd.Homes.Count()<m_Cfg.MaxHomesPerPlayer; }
    bool IsOnCooldown(PH_PlayerData pd){ return (Now()-pd.LastSetAt)<m_Cfg.CooldownSeconds; }
    void TouchCooldown(PH_PlayerData pd){ pd.LastSetAt=Now(); }

    bool Pay(PlayerBase p, int amount, out string why)
    { why=""; if(!p){ why="Kein Spieler."; return false; } string pid = p.GetIdentity() ? p.GetIdentity().GetId() : ""; if(amount<=0) return true; if (m_Cfg.EconomyProvider=="virtual") m_Econ.EnsureStarting(pid, m_Cfg.StartingCoins); return m_Econ.TryPay(pid, amount, p, why); }

    void EnsureVaultSpawned(PH_Home h)
    {
        if(!h) return; vector basePos = h.GetPos() + m_Cfg.VaultSpawnOffset;
        array<Object> objs = new array<Object>(); GetGame().GetObjectsAtPosition(basePos, 5.0, objs, NULL);
        foreach(Object o: objs){ if(o && o.IsInherited(PlayersHome_Vault)) return; }
        EntityAI chest = EntityAI.Cast( GetGame().CreateObject("PlayersHome_Vault", basePos, true, m_Cfg.PlacePermanentVault, true) );
        if(chest) chest.SetOrientation(Vector(h.Yaw,0,0));
    }

    bool GetHomeByBuilding(Building b, out PH_Home outH)
    {
        outH=null; if(!b) return false; string t = b.GetType(); vector p = b.GetPosition(); float best=999999;
        foreach(string pid, PH_PlayerData pd: m_PlayerMap){ foreach(PH_Home h: pd.Homes){ if(h.HouseType != t) continue; float dx=h.X-p[0]; float dz=h.Z-p[2]; float d = dx*dx + dz*dz; if(d < best){ best=d; outH=h; } } }
        return outH!=null && best < 400;
    }

    bool BuyHome(PlayerBase p, Building b, out string msg)
    {
        msg=""; if(!p||!b){msg="Ungültig."; return false;} if(!IsBuildingAllowed(b)){ msg="Dieses Gebäude kann nicht als Zuhause genutzt werden."; return false; }
        PlayerIdentity id=p.GetIdentity(); if(!id){ msg="Keine Identität."; return false; } string pid=id.GetId(); PH_PlayerData pd=GetOrCreatePD(pid);
        if(IsOnCooldown(pd)){ msg="Cooldown aktiv."; return false; } if(!CanAddHome(pd)){ msg="Maximale Anzahl an Homes erreicht."; return false; }
        int price = CalcBuyPrice(p,b); string why=""; if(!Pay(p, price, why)){ msg=why; return false; }
        PH_Home h=new PH_Home(); h.HomeId=NewGuid(); h.PlayerId=pid; h.WorldName=GetGame().GetWorldName(); h.HouseType=b.GetType();
        vector pos=p.GetPosition(); pos[1]=GetGame().SurfaceY(pos[0],pos[2])+0.05; h.SetPos(pos); h.Yaw=p.GetOrientation()[0];
        h.CreatedAt=Now(); h.ExpireAt=0; h.Mode="buy"; h.Name="Home-"+h.HomeId; h.LockedDoors=new array<int>(); h.CoOwners=new array<string>(); h.Members=new array<string>(); h.LightsOn=0; h.VaultPos=h.GetPos()+m_Cfg.VaultSpawnOffset;
        pd.Homes.Insert(h); m_HomeById.Insert(h.HomeId,h); pd.ActiveIndex=pd.Homes.Count()-1; TouchCooldown(pd); if(m_Cfg.PlacePermanentVault) EnsureVaultSpawned(h); Save(); msg="Zuhause gekauft."; return true;
    }
    bool RentHome(PlayerBase p, Building b, out string msg)
    {
        msg=""; if(!p||!b){msg="Ungültig."; return false;} if(!IsBuildingAllowed(b)){ msg="Dieses Gebäude kann nicht als Zuhause genutzt werden."; return false; }
        PlayerIdentity id=p.GetIdentity(); if(!id){ msg="Keine Identität."; return false; } string pid=id.GetId(); PH_PlayerData pd=GetOrCreatePD(pid);
        if(IsOnCooldown(pd)){ msg="Cooldown aktiv."; return false; } if(!CanAddHome(pd)){ msg="Maximale Anzahl an Homes erreicht."; return false; }
        int price = CalcRentPrice(p,b); string why=""; if(!Pay(p, price, why)){ msg=why; return false; }
        PH_Home h=new PH_Home(); h.HomeId=NewGuid(); h.PlayerId=pid; h.WorldName=GetGame().GetWorldName(); h.HouseType=b.GetType();
        vector pos=p.GetPosition(); pos[1]=GetGame().SurfaceY(pos[0],pos[2])+0.05; h.SetPos(pos); h.Yaw=p.GetOrientation()[0];
        h.CreatedAt=Now(); h.ExpireAt=h.CreatedAt+(m_Cfg.ExtendRentDays*24*3600); h.Mode="rent"; h.Name="Miet-Home-"+h.HomeId; h.LockedDoors=new array<int>(); h.CoOwners=new array<string>(); h.Members=new array<string>(); h.LightsOn=0; h.VaultPos=h.GetPos()+m_Cfg.VaultSpawnOffset;
        pd.Homes.Insert(h); m_HomeById.Insert(h.HomeId,h); pd.ActiveIndex=pd.Homes.Count()-1; TouchCooldown(pd); if(m_Cfg.PlacePermanentVault) EnsureVaultSpawned(h); Save(); msg="Zuhause gemietet."; return true;
    }
    bool ExtendRent(PlayerBase p, PH_Home h, out string msg){ msg=""; if(!p||!h){msg="Ungültig."; return false;} string why=""; if(!Pay(p, m_Cfg.ExtendRentCostCoinsPerPeriod, why)){ msg=why; return false; } h.ExpireAt += m_Cfg.ExtendRentDays*24*3600; Save(); msg="Miete verlängert."; return true; }

    bool LockDoor(PH_Home h, int di){ if(!h) return false; if(h.LockedDoors.Find(di)==-1) h.LockedDoors.Insert(di); Save(); return true; }
    bool UnlockDoor(PH_Home h, int di){ if(!h) return false; int ix=h.LockedDoors.Find(di); if(ix>-1) h.LockedDoors.Remove(ix); Save(); return true; }
    bool IsDoorLocked(PH_Home h, int di){ return h && h.LockedDoors && h.LockedDoors.Find(di)>-1; }
    void SetLights(PH_Home h, bool on){ if(!h) return; h.LightsOn = on ? 1 : 0; Save(); }
    void OpenVault(PlayerBase p, PH_Home h){ if(!p||!h) return; if(m_Cfg.PlacePermanentVault){ EnsureVaultSpawned(h); return; } EntityAI chest = EntityAI.Cast( GetGame().CreateObject("PlayersHome_Vault", p.GetPosition()+"0 0.1 0", true, false, true) ); }

    void MakeSnapshot(PlayerBase p){ PlayerIdentity id=p.GetIdentity(); if(!id) return; string pid=id.GetId(); int bal = m_Econ.GetDisplayBalance(pid, p); ref map<string, ref Param> snap = new map<string, ref Param>(); snap.Insert("Balance", new Param1<int>(bal)); GetGame().RPCSingleParam(p, PH_RPC_SNAPSHOT, new Param1<ref map<string, ref Param>>(snap), true, id); }

    void SetDoorContext(PlayerBase p, Building b, int di){ if(!p||!b) return; PlayerIdentity id=p.GetIdentity(); if(!id) return; string pid=id.GetId(); PH_Context c=new PH_Context(); c.BType=b.GetType(); c.BPos=b.GetPosition(); c.DoorIndex=di; c.ExpiresAt=Now()+20; m_CtxByPid.Set(pid,c); }
    bool GetDoorContext(PlayerBase p, out Building outB, out int outDoorIndex)
    {
        outB=null; outDoorIndex=-1; if(!p) return false; PlayerIdentity id=p.GetIdentity(); if(!id) return false; string pid=id.GetId();
        PH_Context c; if(!m_CtxByPid.Find(pid, c)) return false; if(Now() > c.ExpiresAt) return false;
        Building nearest; array<Object> objs=new array<Object>(); GetGame().GetObjectsAtPosition(c.BPos, 10.0, objs, null);
        foreach(Object o: objs){ Building bb; if(Class.CastTo(bb,o) && bb.GetType()==c.BType){ nearest=bb; break; } }
        if(!nearest) return false; outB=nearest; outDoorIndex=c.DoorIndex; return true;
    }

    // --- Admin helpers ---
    void AdminNote(PlayerBase toP, string text){ if(!toP) return; PlayerIdentity id=toP.GetIdentity(); if(!id) return; GetGame().RPCSingleParam(toP, PH_RPC_ADMIN_NOTE, new Param1<string>(text), true, id); }
    void AdminBroadcast(string text)
    {
        if(!m_Cfg.AdminBroadcast || !m_Cfg.AdminBroadcast.Enabled) return;
        array<Man> players=new array<Man>(); GetGame().GetPlayers(players);
        for(int i=0;i<players.Count();i++)
        {
            PlayerBase pb; if(!Class.CastTo(pb, players[i])) continue;
            PlayerIdentity id=pb.GetIdentity(); if(!id) continue;
            if(!m_Cfg.AdminBroadcast.ToAllPlayers && !IsAdmin(id.GetId())) continue;
            GetGame().RPCSingleParam(pb, PH_RPC_ADMIN_NOTE, new Param1<string>(text), true, id);
        }
    }

    // Query homes by filter string: "near:500;owner:UID;mode:rent;name:foo"
    void AdminQueryHomes(PlayerBase admin, string filter)
    {
        array<string> parts=new array<string>(); filter.Split(";", parts);
        float nearR=0; string owner=""; string mode=""; string name="";
        for(int i=0;i<parts.Count();i++)
        {
            string kv=parts[i]; int c=kv.IndexOf(":"); if(c<0) continue; string k=kv.Substring(0,c); string v=kv.Substring(c+1, kv.Length()-(c+1));
            k.ToLower(); v.ToLower();
            if(k=="near") nearR = v.ToFloat();
            else if(k=="owner") owner = v;
            else if(k=="mode") mode = v;
            else if(k=="name") name = v;
        }

        vector aPos = admin ? admin.GetPosition() : "0 0 0";
        float r2 = nearR>0 ? nearR*nearR : 0;

        ref array<string> rows = new array<string>();
        foreach(string pid, PH_PlayerData pd: m_PlayerMap)
        {
            foreach(PH_Home h: pd.Homes)
            {
                if(owner!="" && h.PlayerId != owner) continue;
                if(mode!="" && h.Mode != mode) continue;
                if(name!="" )
                {
                    string nlow = h.Name; nlow.ToLower();
                    if(nlow.IndexOf(name) < 0) continue;
                }
                if(nearR>0)
                {
                    float dx = h.X - aPos[0]; float dz = h.Z - aPos[2];
                    float d2 = dx*dx + dz*dz;
                    if(d2 > r2) continue;
                }
                // row: HomeId|Owner|Name|World|X|Z|Mode|Type
                rows.Insert(h.HomeId + "|" + h.PlayerId + "|" + h.Name + "|" + h.WorldName + "|" + h.X.ToString() + "|" + h.Z.ToString() + "|" + h.Mode + "|" + h.HouseType);
            }
        }

        // online players (UID|Name)
        ref array<string> plist = new array<string>();
        array<Man> players=new array<Man>(); GetGame().GetPlayers(players);
        for(int j=0;j<players.Count();j++)
        {
            PlayerBase pb; if(!Class.CastTo(pb, players[j])) continue;
            PlayerIdentity id=pb.GetIdentity(); if(!id) continue;
            plist.Insert(id.GetId()+"|"+id.GetName());
        }

        // admins from config
        ref array<string> admins = new array<string>();
        if(m_Cfg.Admins) for(int a=0;a<m_Cfg.Admins.Count();a++) admins.Insert(m_Cfg.Admins[a]);

        PlayerIdentity aid = admin ? admin.GetIdentity() : null;
        if(aid) GetGame().RPCSingleParam(admin, PH_RPC_ADMIN_SNAPSHOT, new Param3<ref array<string>, ref array<string>, ref array<string>>(rows, plist, admins), true, aid);
    }

    // --- Admin operations ---
    bool AdminDeleteHome(string homeId)
    {
        if(homeId=="") return false;
        PH_Home h; if(!m_HomeById.Find(homeId, h)) return false;
        PH_PlayerData pd; if(!m_PlayerMap.Find(h.PlayerId, pd)) return false;
        int idx=-1; for(int i=0;i<pd.Homes.Count();i++){ if(pd.Homes[i].HomeId==homeId){ idx=i; break; } }
        if(idx>-1) pd.Homes.Remove(idx);
        m_HomeById.Remove(homeId);
        array<Object> objs=new array<Object>(); GetGame().GetObjectsAtPosition(h.VaultPos, 5.0, objs, null);
        foreach(Object o: objs){ if(o && o.IsInherited(PlayersHome_Vault)) GetGame().ObjectDelete(o); }
        Save();
        AdminBroadcast("[PlayersHome] Home gelöscht: "+homeId);
        return true;
    }

    bool AdminTransferHome(string homeId, string newPid)
    {
        if(homeId==""||newPid=="") return false;
        PH_Home h; if(!m_HomeById.Find(homeId, h)) return false;
        PH_PlayerData oldpd; if(!m_PlayerMap.Find(h.PlayerId, oldpd)) return false;
        PH_PlayerData newpd = GetOrCreatePD(newPid);
        int idx=-1; for(int i=0;i<oldpd.Homes.Count();i++){ if(oldpd.Homes[i].HomeId==homeId){ idx=i; break; } }
        if(idx>-1) oldpd.Homes.Remove(idx);
        h.PlayerId = newPid; newpd.Homes.Insert(h);
        Save();
        AdminBroadcast("[PlayersHome] Home übertragen: "+homeId+" → "+newPid);
        return true;
    }

    bool AdminGiveCoins(string pid, int amount)
    {
        if(m_Cfg.EconomyProvider!="virtual") return false;
        m_Econ.Add(pid, amount);
        AdminBroadcast("[PlayersHome] Coins an "+pid+": "+amount.ToString());
        return true;
    }

    void Tick(){ int now=Now(); if(m_LastTick==0 || now-m_LastTick>=60){ m_LastTick=now; /* autopay/reservations optional */ } }
};

class PH{ protected static ref PH_Service s; static PH_Service Get(){ if(!GetGame()||!GetGame().IsServer()) return null; if(!s) s=new PH_Service(); return s; } };
