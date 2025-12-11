
modded class PlayerBase
{
    override void SetActions()
    {
        super.SetActions();
        AddAction(ActionPH_DoorMenu);
        AddAction(ActionPH_AdminOpen);
    }
};
