
class PH_AdminMenu : UIScriptedMenu
{
    protected EditBoxWidget m_EHomeId;
    protected EditBoxWidget m_EPlayerId;
    protected EditBoxWidget m_EAmount;
    protected ButtonWidget m_BReload;
    protected ButtonWidget m_BDelete;
    protected ButtonWidget m_BTransfer;
    protected ButtonWidget m_BGiveCoins;
    protected ButtonWidget m_BQuery;
    protected ButtonWidget m_BQueryNear;
    protected EditBoxWidget m_EFilterName;
    protected EditBoxWidget m_EFilterOwner;
    protected EditBoxWidget m_EFilterMode;
    protected EditBoxWidget m_EFilterNear;

    protected TextListboxWidget m_ListHomes;
    protected TextListboxWidget m_ListPlayers;
    protected TextListboxWidget m_ListAdmins;
    protected ButtonWidget m_BUseSelectedHome;
    protected ButtonWidget m_BUseSelectedPlayer;

    override Widget Init()
    {
        layoutRoot = GetGame().GetWorkspace().CreateWidgets("PlayersHome/gui/layouts/playershome/PH_Admin_Plus.layout", null);

        m_EHomeId   = EditBoxWidget.Cast(layoutRoot.FindAnyWidget("inpHomeId"));
        m_EPlayerId = EditBoxWidget.Cast(layoutRoot.FindAnyWidget("inpPlayerId"));
        m_EAmount   = EditBoxWidget.Cast(layoutRoot.FindAnyWidget("inpAmount"));

        m_BReload   = ButtonWidget.Cast(layoutRoot.FindAnyWidget("btnReload"));
        m_BDelete   = ButtonWidget.Cast(layoutRoot.FindAnyWidget("btnDelete"));
        m_BTransfer = ButtonWidget.Cast(layoutRoot.FindAnyWidget("btnTransfer"));
        m_BGiveCoins= ButtonWidget.Cast(layoutRoot.FindAnyWidget("btnGiveCoins"));

        m_BQuery    = ButtonWidget.Cast(layoutRoot.FindAnyWidget("btnQuery"));
        m_BQueryNear= ButtonWidget.Cast(layoutRoot.FindAnyWidget("btnQueryNear"));
        m_EFilterName = EditBoxWidget.Cast(layoutRoot.FindAnyWidget("inpNameFilter"));
        m_EFilterOwner= EditBoxWidget.Cast(layoutRoot.FindAnyWidget("inpOwnerFilter"));
        m_EFilterMode = EditBoxWidget.Cast(layoutRoot.FindAnyWidget("inpModeFilter"));
        m_EFilterNear = EditBoxWidget.Cast(layoutRoot.FindAnyWidget("inpNearFilter"));

        m_ListHomes   = TextListboxWidget.Cast(layoutRoot.FindAnyWidget("listHomes"));
        m_ListPlayers = TextListboxWidget.Cast(layoutRoot.FindAnyWidget("listPlayers"));
        m_ListAdmins  = TextListboxWidget.Cast(layoutRoot.FindAnyWidget("listAdmins"));

        m_BUseSelectedHome   = ButtonWidget.Cast(layoutRoot.FindAnyWidget("btnUseHome"));
        m_BUseSelectedPlayer = ButtonWidget.Cast(layoutRoot.FindAnyWidget("btnUsePlayer"));

        g_PH_Admin = this;
        return layoutRoot;
    }

    override bool OnClick(Widget w, int x, int y, int button)
    {
        if(w == m_BReload){ SendAdmin(PH_ADMIN_RELOAD, ""); return true; }
        if(w == m_BDelete){ SendAdmin(PH_ADMIN_DELETE_HOME, m_EHomeId.GetText()); return true; }
        if(w == m_BTransfer){ SendAdmin(PH_ADMIN_TRANSFER_HOME, m_EHomeId.GetText()+"|"+m_EPlayerId.GetText()); return true; }
        if(w == m_BGiveCoins){ SendAdmin(PH_ADMIN_GIVE_COINS, m_EPlayerId.GetText()+"|"+m_EAmount.GetText()); return true; }
        if(w == m_BQuery){ SendAdmin(PH_ADMIN_QUERY_HOMES, BuildFilterString()); return true; }
        if(w == m_BQueryNear){ string r = m_EFilterNear.GetText(); if(r=="") r="300"; SendAdmin(PH_ADMIN_QUERY_HOMES, "near:"+r); return true; }
        if(w == m_BUseSelectedHome){ UseSelectedHome(); return true; }
        if(w == m_BUseSelectedPlayer){ UseSelectedPlayer(); return true; }
        return false;
    }

    string BuildFilterString()
    {
        string f="";
        string n=m_EFilterName.GetText(); string o=m_EFilterOwner.GetText(); string m=m_EFilterMode.GetText(); string r=m_EFilterNear.GetText();
        if(n!="") f += (f!=""?";":"")+"name:"+n;
        if(o!="") f += (f!=""?";":"")+"owner:"+o;
        if(m!="") f += (f!=""?";":"")+"mode:"+m;
        if(r!="") f += (f!=""?";":"")+"near:"+r;
        return f;
    }

    void UseSelectedHome()
    {
        int row = m_ListHomes.GetSelectedRow();
        if(row < 0) return;
        string rec = m_ListHomes.GetItemText(row, 0);
        // rec is "HomeId | Name | Owner | World | X | Z | Mode"
        array<string> cols = new array<string>(); rec.Split("|", cols);
        if(cols.Count()>0) m_EHomeId.SetText(cols[0].Trim());
        if(cols.Count()>2) m_EPlayerId.SetText(cols[2].Trim());
    }

    void UseSelectedPlayer()
    {
        int row = m_ListPlayers.GetSelectedRow();
        if(row < 0) return;
        string rec = m_ListPlayers.GetItemText(row, 0);
        // "UID | Name"
        array<string> cols = new array<string>(); rec.Split("|", cols);
        if(cols.Count()>0) m_EPlayerId.SetText(cols[0].Trim());
    }

    void SendAdmin(int c, string arg){ GetGame().RPCSingleParam(null, PH_RPC_ADMIN_CMD, new Param2<int,string>(c,arg), true); }

    void ApplySnapshot(ref array<string> homes, ref array<string> players, ref array<string> admins)
    {
        if(m_ListHomes){ m_ListHomes.ClearItems(); for(int i=0;i<homes.Count();i++){ string s=homes[i]; array<string> p=new array<string>(); s.Split("|", p); string line=p[0]+" | "+p[2]+" | "+p[1]+" | "+p[3]+" | "+p[4]+" | "+p[5]+" | "+p[6]; m_ListHomes.AddItem(line, null, 0); } }
        if(m_ListPlayers){ m_ListPlayers.ClearItems(); for(int j=0;j<players.Count();j++){ m_ListPlayers.AddItem(players[j], null, 0); } }
        if(m_ListAdmins){ m_ListAdmins.ClearItems(); for(int k=0;k<admins.Count();k++){ m_ListAdmins.AddItem(admins[k], null, 0); } }
    }

    override void OnHide(){ g_PH_Admin = null; }
};
