
class PH_Manager : UIScriptedMenu
{
    protected TextWidget m_Title;
    protected TextWidget m_Bal;
    protected ButtonWidget m_Buy;
    protected ButtonWidget m_Rent;
    protected ButtonWidget m_Extend;
    protected ButtonWidget m_OpenVault;
    protected ButtonWidget m_Lock;
    protected ButtonWidget m_Unlock;
    protected ButtonWidget m_LightOn;
    protected ButtonWidget m_LightOff;

    override Widget Init()
    {
        layoutRoot = GetGame().GetWorkspace().CreateWidgets("PlayersHome/gui/layouts/playershome/PH_Manager.layout", null);
        m_Title = TextWidget.Cast(layoutRoot.FindAnyWidget("title"));
        m_Bal = TextWidget.Cast(layoutRoot.FindAnyWidget("txtCoins"));
        m_Buy = ButtonWidget.Cast(layoutRoot.FindAnyWidget("btnBuy"));
        m_Rent = ButtonWidget.Cast(layoutRoot.FindAnyWidget("btnRent"));
        m_Extend = ButtonWidget.Cast(layoutRoot.FindAnyWidget("btnExtend"));
        m_OpenVault = ButtonWidget.Cast(layoutRoot.FindAnyWidget("btnVault"));
        m_Lock = ButtonWidget.Cast(layoutRoot.FindAnyWidget("btnLock"));
        m_Unlock = ButtonWidget.Cast(layoutRoot.FindAnyWidget("btnUnlock"));
        m_LightOn = ButtonWidget.Cast(layoutRoot.FindAnyWidget("btnLightOn"));
        m_LightOff = ButtonWidget.Cast(layoutRoot.FindAnyWidget("btnLightOff"));
        g_PH_Manager = this;
        return layoutRoot;
    }
    override bool OnClick(Widget w, int x, int y, int button)
    {
        if(w == m_Buy){ SendCmd(PH_CMD_BUY); return true; }
        if(w == m_Rent){ SendCmd(PH_CMD_RENT); return true; }
        if(w == m_Extend){ SendCmd(PH_CMD_EXTEND); return true; }
        if(w == m_OpenVault){ SendCmd(PH_CMD_OPEN_VAULT); return true; }
        if(w == m_Lock){ SendCmd(PH_CMD_LOCK); return true; }
        if(w == m_Unlock){ SendCmd(PH_CMD_UNLOCK); return true; }
        if(w == m_LightOn){ SendCmd(PH_CMD_LIGHT_ON); return true; }
        if(w == m_LightOff){ SendCmd(PH_CMD_LIGHT_OFF); return true; }
        return false;
    }
    void SendCmd(int c){ GetGame().RPCSingleParam(null, PH_RPC_CMD, new Param2<int,string>(c,""), true); }
    void ApplySnapshot(ref map<string, ref Param> snap){ Param1<int> bal = Param1<int>.Cast(snap.Get("Balance")); if(bal && m_Bal) m_Bal.SetText("Balance: "+bal.param1.ToString()); }
    override void OnHide(){ g_PH_Manager = null; }
};
