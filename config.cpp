class CfgPatches
{
    class PlayersHome
    {
        units[] = {"PlayersHome_ClaimDeed","PlayersHome_Key","PlayersHome_AdminTool","PlayersHome_Vault"};
        weapons[] = {};
        requiredVersion = 0.1;
        requiredAddons[] = {"DZ_Data","DZ_Scripts"};
    };
};

class CfgMods
{
    class PlayersHome
    {
        dir = "PlayersHome";
        name = "PlayersHome (Full Pack v4.2 — Admin UI Plus)";
        version = "4.2.0";
        type = "mod";
        dependencies[] = {"Game","World","Mission"};
        class defs
        {
            class gameScriptModule { value = ""; files[] = {"PlayersHome/scripts/3_Game"}; };
            class worldScriptModule { value = ""; files[] = {"PlayersHome/scripts/4_World"}; };
            class missionScriptModule { value = ""; files[] = {"PlayersHome/scripts/5_Mission"}; };
        };
    };
};

class CfgVehicles
{
    class Paper;
    class Inventory_Base;
    class Container_Base;

    class PlayersHome_ClaimDeed: Paper
    {
        scope = 2;
        displayName = "Hausurkunde";
        descriptionShort = "PlayersHome: Kaufen/Mieten via Tür-Menü.";
        model = "\dz\gear\consumables\Paper.p3d";
        weight = 5;
        itemSize[] = {1,1};
        varQuantityInit = 1;
        varQuantityMin = 0;
        varQuantityMax = 1;
    };

    class PlayersHome_Key: Inventory_Base
    {
        scope = 2;
        displayName = "PlayersHome Schlüssel";
        descriptionShort = "Öffnet Türen des Zuhauses (Home-ID gebunden; optional).";
        model = "\dz\gear\tools\lockpick.p3d";
        weight = 10;
        itemSize[] = {1,1};
        varQuantityInit = 1;
        varQuantityMin = 0;
        varQuantityMax = 1;
        rotationFlags = 16;
    };

    class PlayersHome_AdminTool: Inventory_Base
    {
        scope = 2;
        displayName = "PlayersHome Admin-Tool";
        descriptionShort = "Admin-Funktionen (Panel öffnen / Reload / Reports).";
        model = "\dz\gear\tools\screwdriver.p3d";
        weight = 300;
        itemSize[] = {1,2};
        varQuantityInit = 1;
        varQuantityMin = 0;
        varQuantityMax = 1;
    };

    class PlayersHome_Vault: Container_Base
    {
        scope = 2;
        displayName = "PlayersHome Tresor";
        descriptionShort = "Home-Depot (5.000 Slots) – via Menü zugänglich (optional permanent).";
        model = "\dz\gear\containers\sea_chest.p3d";
        itemSize[] = {10,10};
        weight = 15000;
        itemsCargoSize[] = {100,50};
        varQuantityInit = 0;
        varQuantityMin = 0;
        varQuantityMax = 0;
        rotationFlags = 16;
        physLayer = "item_large";
    };
};
