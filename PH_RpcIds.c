const int PH_RPC_BASE=777500;
const int PH_RPC_SNAPSHOT=PH_RPC_BASE+1;
const int PH_RPC_CMD=PH_RPC_BASE+2;
const int PH_RPC_OPEN_MANAGER=PH_RPC_BASE+3;
const int PH_RPC_OPEN_INV=PH_RPC_BASE+4;
const int PH_RPC_LIGHTS=PH_RPC_BASE+5;
const int PH_RPC_ADMIN_OPEN=PH_RPC_BASE+6;
const int PH_RPC_ADMIN_SNAPSHOT=PH_RPC_BASE+7;
const int PH_RPC_ADMIN_CMD=PH_RPC_BASE+8;
const int PH_RPC_ADMIN_NOTE=PH_RPC_BASE+9; // server->client admin note/toast
const int PH_MENU_ID=555050;
const int PH_MENU_ADMIN_ID=555051;

// Commands
const int PH_CMD_BUY=1;
const int PH_CMD_RENT=2;
const int PH_CMD_EXTEND=3;
const int PH_CMD_LOCK=4;
const int PH_CMD_UNLOCK=5;
const int PH_CMD_LIGHT_ON=6;
const int PH_CMD_LIGHT_OFF=7;
const int PH_CMD_SET_ACTIVE=8;
const int PH_CMD_RENAME=9;
const int PH_CMD_ADD_MEMBER=10;
const int PH_CMD_REM_MEMBER=11;
const int PH_CMD_ADD_COOWNER=12;
const int PH_CMD_REM_COOWNER=13;
const int PH_CMD_OPEN_VAULT=14;
const int PH_CMD_BELL=15;

// Admin
const int PH_ADMIN_RELOAD=101;
const int PH_ADMIN_DELETE_HOME=102;
const int PH_ADMIN_TRANSFER_HOME=103;
const int PH_ADMIN_GIVE_COINS=104; // nur virtual
const int PH_ADMIN_QUERY_HOMES=105; // arg: "near:500;owner:UID;mode:rent;name:foo"
const int PH_ADMIN_GET_ONLINE=106;  // no arg
