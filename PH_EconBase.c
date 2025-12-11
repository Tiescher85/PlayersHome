
class PH_EconBase
{
    int GetDisplayBalance(string pid, PlayerBase p) { return 0; }
    void EnsureStarting(string pid, int startCoins) {}
    bool TryPay(string pid, int amount, PlayerBase p, out string why) { why=""; return true; }
    void Add(string pid, int amount) {}
};
