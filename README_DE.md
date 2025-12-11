# PlayersHome — Full Pack v4.2 (Admin UI Plus)

**Neu in v4.2**
- **Homes-Suche (Admin)**: Filter `Name enthält`, `Owner UID`, `Mode buy/rent`, `Nähe (m)`; Button „Suche nahe mir“.
- **Autocomplete/Listen**: Online-Spieler (UID|Name) + Admins aus `config.json`; per Klick in die Eingabefelder übernehmbar.
- **Broadcast/Toasts**: Admin-Aktionen werden als kurzer Hinweis (Toast) an **Admins** gesendet (optional an alle – `AdminBroadcast.ToAllPlayers=true`).

**Konfiguration (`$profile:/PlayersHome/config.json`)**
```json
{
  "Admins": ["7656119..."],
  "AdminBroadcast": { "Enabled": true, "ToAllPlayers": false }
}
```

**Tipp:** Panel öffnen mit Item `PlayersHome_AdminTool` → Aktion „PlayersHome Admin“.
