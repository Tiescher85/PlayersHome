
class PH_Zone { string Name; string Shape; ref array<vector> Vertices; float X; float Z; float Radius; float PriceMultiplier; int MaxHomesPerPlayerHere; };
class PH_RentCfg { bool AutoPay=true; int WarnDays=2; int MaxAutoPayPeriods=8; };
class PH_RespawnCfg { bool Enabled=false; int CooldownSec=7200; int Radius=30; };
class PH_AdminBroadcastCfg { bool Enabled=true; bool ToAllPlayers=false; };

class PH_Config
{
    int MaxHomesPerPlayer = 5;
    int MaxCoOwnersPerHome = 4;
    int MaxMembersPerHome = 8;
    int CooldownSeconds = 1200;
    bool MembersCanOpenInventory = true;
    bool MembersCanLockDoors = false;
    string EconomyProvider = "item";
    bool AutoDetectCurrency = true;
    ref array<string> AutoDetectPatterns;
    ref array<string> AutoDetectNameHints;
    int AutoDetectMinDenoms = 2;
    int BuyCostCoins = 1000;
    int RentCostCoinsPer7Days = 250;
    int ExtendRentDays = 7;
    int ExtendRentCostCoinsPerPeriod = 250;
    int KeyCopyCostCoins = 50;
    int StartingCoins = 0;
    ref PH_RentCfg Rent;
    vector VaultSpawnOffset = "0 -0.75 0";
    int TempVaultNearPlayerSeconds = 15;
    bool PlacePermanentVault = true;
    ref array<string> Admins;
    ref PH_AdminBroadcastCfg AdminBroadcast;
    string ZonesMode = "allow";
    ref array<ref PH_Zone> Zones;
    ref PH_RespawnCfg RespawnAtHome;
    bool AllowOnlyResidential = true;
    ref array<string> AllowedBuildingClasses;
    ref array<string> AllowedBuildingClassPrefixes;
    ref array<string> DeniedBuildingClasses;
    ref array<string> DeniedBuildingClassPrefixes;

    void PH_Config()
    {
        Rent = new PH_RentCfg();
        RespawnAtHome = new PH_RespawnCfg();
        Admins = new array<string>;
        AdminBroadcast = new PH_AdminBroadcastCfg();
        Zones = new array<ref PH_Zone>;
        AutoDetectPatterns = new array<string>;
        AutoDetectPatterns.Insert("Money{value}");
        AutoDetectPatterns.Insert("MoneyRuble{value}");
        AutoDetectPatterns.Insert("Banknote_{value}");
        AutoDetectPatterns.Insert("Cash{value}");
        AutoDetectNameHints = new array<string>;
        AutoDetectNameHints.Insert("money"); AutoDetectNameHints.Insert("cash"); AutoDetectNameHints.Insert("coin");
        AutoDetectNameHints.Insert("banknote"); AutoDetectNameHints.Insert("note"); AutoDetectNameHints.Insert("ruble");
        AutoDetectNameHints.Insert("euro"); AutoDetectNameHints.Insert("dollar");

        AllowedBuildingClasses = new array<string>;
        AllowedBuildingClassPrefixes = new array<string>;
        AllowedBuildingClassPrefixes.Insert("Land_House");
        AllowedBuildingClassPrefixes.Insert("Land_Panelak");
        AllowedBuildingClassPrefixes.Insert("Land_HouseBlock");
        AllowedBuildingClassPrefixes.Insert("Land_Tenement");
        AllowedBuildingClassPrefixes.Insert("Land_Apartment");
        AllowedBuildingClassPrefixes.Insert("Land_TownHouse");
        AllowedBuildingClassPrefixes.Insert("Land_Villa");

        DeniedBuildingClasses = new array<string>;
        DeniedBuildingClassPrefixes = new array<string>;
        string deny[] = {"Land_Police","Land_Hospital","Land_Prison","Land_Mil","Land_Barracks","Land_Bunker","Land_Checkpoint",
            "Land_Fire","Land_FireStation","Land_Shop","Land_Store","Land_Market","Land_Supermarket","Land_Office",
            "Land_Factory","Land_Industrial","Land_Warehouse","Land_Rail","Land_Train","Land_Station","Land_Hangar",
            "Land_Garage","Land_Barn","Land_Shed","Land_Farm","Land_School","Land_Kindergarten","Land_CityHall","Land_TownHall",
            "Land_Church","Land_Castle","Land_Lighthouse","Land_BusStation","Land_BusStop","Land_Power"};
        for (int i=0;i<deny.Count();i++) DeniedBuildingClassPrefixes.Insert(deny[i]);
    }
};

class PH_ConfigIO
{
    protected static string GetDir(){ return "$profile:PlayersHome"; }
    protected static string GetFile(){ return GetDir()+"/config.json"; }
    static PH_Config LoadOrCreate()
    {
        if(!FileExist(GetDir())) MakeDirectory(GetDir());
        PH_Config cfg;
        if(FileExist(GetFile())){ JsonFileLoader<PH_Config>.JsonLoadFile(GetFile(), cfg); if(cfg) return cfg; }
        cfg = new PH_Config(); JsonFileLoader<PH_Config>.JsonSaveFile(GetFile(), cfg); return cfg;
    }
    static void Save(PH_Config cfg){ if(!cfg) return; if(!FileExist(GetDir())) MakeDirectory(GetDir()); JsonFileLoader<PH_Config>.JsonSaveFile(GetFile(), cfg); }
};
