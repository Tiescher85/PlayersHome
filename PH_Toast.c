
class PH_Toast : UIScriptedMenu
{
    protected TextWidget m_Text;
    static void Show(string text)
    {
        PH_Toast t = PH_Toast.Cast( GetGame().GetUIManager().EnterScriptedMenu(999900, null) );
        if(t) t.SetText(text);
    }

    void SetText(string t){ if(m_Text) m_Text.SetText(t); GetGame().GetCallQueue(CALL_CATEGORY_GUI).CallLater(this.CloseSelf, 4000, false); }
    void CloseSelf(){ GetGame().GetUIManager().Back(); }

    override Widget Init()
    {
        layoutRoot = GetGame().GetWorkspace().CreateWidgets("PlayersHome/gui/layouts/playershome/PH_Toast.layout", null);
        m_Text = TextWidget.Cast(layoutRoot.FindAnyWidget("txt"));
        return layoutRoot;
    }
};
