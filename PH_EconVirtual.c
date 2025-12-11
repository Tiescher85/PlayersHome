
class PH_EconVirtual : PH_EconBase
{
    protected string m_File;
    protected ref map<string, int> m_Balances;
    void PH_EconVirtual() { m_File="$profile:PlayersHome/coins.json"; m_Balances=new map<string,int>(); Load(); }
    void Load(){ if (FileExist(m_File)) JsonFileLoader<ref map<string,int>>.JsonLoadFile(m_File, m_Balances); }
    void Save(){ JsonFileLoader<ref map<string,int>>.JsonSaveFile(m_File, m_Balances); }
    override int GetDisplayBalance(string pid, PlayerBase p){ int v=0; m_Balances.Find(pid,v); return v; }
    override void EnsureStarting(string pid, int startCoins){ int v=0; if(!m_Balances.Find(pid,v)){ m_Balances.Insert(pid,startCoins); Save(); } }
    override bool TryPay(string pid, int amount, PlayerBase p, out string why)
    { why=""; if(amount<=0) return true; int bal=0; m_Balances.Find(pid, bal); if (bal<amount){ why="Nicht genug Coins."; return false; } m_Balances.Set(pid, bal-amount); Save(); return true; }
    override void Add(string pid, int amount){ int bal=0; m_Balances.Find(pid, bal); m_Balances.Set(pid, bal+amount); Save(); }
};
