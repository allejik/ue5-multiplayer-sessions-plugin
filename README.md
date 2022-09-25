# Multiplayer Sessions Plugin UE5

- Clone the content of repository into Plugins/MultiplayerSessions, e.g. `git clone git@github.com:allejik/ue5-multiplayer-sessions-plugin.git .`

- Add to Config/DefaultGame.ini:

```ini
; Fix Network Rubberbanding Lag
[/Script/Engine.GameNetworkManager]
TotalNetBandwidth=500000
MaxDynamicBandwidth=80000
MinDynamicBandwidth=20000
```

- Add to Config/DefaultEngine.ini:

```ini
; Fix Network Rubberbanding Lag
[/Script/Engine.Player]
ConfiguredInternetSpeed=500000
ConfiguredLanSpeed=500000

[/Script/OnlineSubsystemUtils.IpNetDriver]
MaxClientRate=800000
MaxInternetClientRate=800000
```
- Add to level blueprint:
  - Add node "Create Widget"
  - Select WB_MenuUserWidget
  - Connect white pin to Event Begin Play
  - Create node "Menu Setup" from Return Value pin
  - Set the number of public connections e.g. 4
  - Set the type of match "FreeForAll"
  - Set the paty to your level in Lobby Path e.g. "/Game/Levels/Lobby"
  - Connect white pin to MenuUserWidget
