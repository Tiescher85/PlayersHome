
class PH_EconItemCfgDenom { string Class; int Value; };
class PH_EconItemCfg
{
    string Provider = "item";
    bool AutoDetect = true;
    int MinDenoms = 2;
    ref array<string> Patterns;
    ref array<string> NameHints;
    ref array<ref PH_EconItemCfgDenom> Denominations;
    ref array<string> Scan;
    void PH_EconItemCfg(){ Denominations=new array<ref PH_EconItemCfgDenom>(); Scan=new array<string>(); Patterns=new array<string>(); NameHints=new array<string>(); }
};

class PH_EconItem : PH_EconBase
{
    protected string m_File;
    protected ref PH_EconItemCfg m_Cfg;
    protected ref map<string,int> m_Map;

    void PH_EconItem()
    {
        m_File = "$profile:PlayersHome/Economy/items.json";
        LoadOrCreate();
    }

    void LoadOrCreate()
    {
        string dirEconomy = "$profile:PlayersHome/Economy";
        if(!FileExist("$profile:PlayersHome")) MakeDirectory("$profile:PlayersHome");
        if(!FileExist(dirEconomy)) MakeDirectory(dirEconomy);

        PH_Config g = PH_ConfigIO.LoadOrCreate();

        if(FileExist(m_File)) JsonFileLoader<PH_EconItemCfg>.JsonLoadFile(m_File, m_Cfg);
        if(!m_Cfg)
        {
            m_Cfg = new PH_EconItemCfg();
            m_Cfg.AutoDetect = g.AutoDetectCurrency;
            m_Cfg.MinDenoms  = g.AutoDetectMinDenoms;
            foreach(string p: g.AutoDetectPatterns) m_Cfg.Patterns.Insert(p);
            foreach(string h: g.AutoDetectNameHints) m_Cfg.NameHints.Insert(h);
            m_Cfg.Scan.Insert("player");
            JsonFileLoader<PH_EconItemCfg>.JsonSaveFile(m_File, m_Cfg);
        }

        m_Map = new map<string,int>();
        foreach(PH_EconItemCfgDenom d: m_Cfg.Denominations) if(d) m_Map.Insert(d.Class, d.Value);
    }

    protected void SaveCfg(){ JsonFileLoader<PH_EconItemCfg>.JsonSaveFile(m_File, m_Cfg); }

    protected int ExtractValueFromClass(string cls)
    {
        for(int i=0;i<m_Cfg.Patterns.Count();i++)
        {
            string pat = m_Cfg.Patterns[i];
            int brace = pat.IndexOf("{value}");
            if (brace < 0) continue;
            string pre = pat.Substring(0, brace);
            string post = pat.Substring(brace+7, pat.Length() - (brace+7));
            if (cls.IndexOf(pre) == 0 && cls.Length() > pre.Length())
            {
                string rest = cls.Substring(pre.Length(), cls.Length() - pre.Length());
                int end = rest.IndexOf(post);
                string mid = (end>=0) ? rest.Substring(0, end) : rest;
                string num=""; for(int k=0;k<mid.Length();k++){ string ch=mid[k]; if (ch >= "0" && ch <= "9") num += ch; }
                if (num.Length()>0) return num.ToInt();
            }
        }
        return 0;
    }

    protected int ExtractValueFromDisplay(string cls)
    {
        string name; bool ok = GetGame().ConfigGetText("CfgVehicles " + cls + " displayName", name);
        if (!ok) return 0;
        string lower = name; lower.ToLower();
        bool hint=false;
        for(int i=0;i<m_Cfg.NameHints.Count();i++) if(lower.Contains(m_Cfg.NameHints[i])) { hint=true; break; }
        if(!hint) return 0;
        string num=""; for(int k=0;k<name.Length();k++){ string ch=name[k]; if (ch >= "0" && ch <= "9") num += ch; else if(num.Length()>0) break; }
        if (num.Length()>0) return num.ToInt();
        return 0;
    }

    protected void TryLearnFromItem(EntityAI e)
    {
        if(!e) return;
        string cls = e.GetType();
        if (m_Map.Contains(cls)) return;
        int v = ExtractValueFromClass(cls);
        if (v<=0) v = ExtractValueFromDisplay(cls);
        if (v<=0) return;
        PH_EconItemCfgDenom d = new PH_EconItemCfgDenom();
        d.Class = cls; d.Value = v;
        m_Cfg.Denominations.Insert(d);
        m_Map.Insert(cls, v);
        SaveCfg();
        PH_Log.Info("AutoCurrency: learned "+cls+" => "+v.ToString());
    }

    protected void CollectRecursive(EntityAI e, out array<EntityAI> outItems, out array<int> outVals, bool learn=true)
    {
        if(!outItems) outItems = new array<EntityAI>();
        if(!outVals) outVals = new array<int>();

        if(e)
        {
            string t = e.GetType();
            int val=0;
            if(m_Map.Find(t, val))
            {
                outItems.Insert(e);
                outVals.Insert(val);
            }
            else if (learn && m_Cfg && m_Cfg.AutoDetect)
            {
                TryLearnFromItem(e);
                if(m_Map.Find(t, val))
                {
                    outItems.Insert(e);
                    outVals.Insert(val);
                }
            }

            if(e.GetInventory())
            {
                CargoBase cb = e.GetInventory().GetCargo();
                if(cb) for(int i=0;i<cb.GetItemCount();i++) CollectRecursive(cb.GetItem(i), outItems, outVals, learn);
                for(int ai=0; ai<e.GetInventory().AttachmentCount(); ai++) CollectRecursive(e.GetInventory().GetAttachmentFromIndex(ai), outItems, outVals, learn);
            }
        }
    }

    int UniqueDenoms(){ return m_Map.Count(); }
    protected int ComputeBalance(PlayerBase p){ array<EntityAI> items = new array<EntityAI>(); array<int> vals = new array<int>(); CollectRecursive(p, items, vals, true); int sum=0; for(int i=0;i<vals.Count();i++) sum += vals[i]; return sum; }
    override int GetDisplayBalance(string pid, PlayerBase p){ if(!p) return 0; return ComputeBalance(p); }

    override bool TryPay(string pid, int amount, PlayerBase p, out string why)
    {
        why=""; if(amount<=0) return true; if(!p){ why="Kein Spieler."; return false; }
        if (m_Cfg && m_Cfg.MinDenoms > 0 && UniqueDenoms() < m_Cfg.MinDenoms){ ComputeBalance(p); if (UniqueDenoms() < m_Cfg.MinDenoms){ why="Währung noch nicht erkannt (mind. 2 Stückelungen benötigt)."; return false; } }
        array<EntityAI> items = new array<EntityAI>(); array<int> vals = new array<int>(); CollectRecursive(p, items, vals, false);
        for(int i=0;i<vals.Count();i++){ int maxI=i; for(int j=i+1;j<vals.Count();j++) if(vals[j]>vals[maxI]) maxI=j; 
            if(maxI!=i){ int tv=vals[i]; vals[i]=vals[maxI]; vals[maxI]=tv; EntityAI ti=items[i]; items[i]=items[maxI]; items[maxI]=ti; } }
        int need = amount; array<EntityAI> toDelete = new array<EntityAI>();
        for(int k=0;k<items.Count(); k++){ if(need<=0) break; toDelete.Insert(items[k]); need -= vals[k]; }
        if(need > 0){ why = "Nicht genug Währung im Inventar."; return false; }
        for(int r=0; r<toDelete.Count(); r++){ GetGame().ObjectDelete(toDelete[r]); }
        return true;
    }
};
