# Multiplayer Sessions Plugin UE5 Steam

Adds a simple menu online system to connect via Steam.

## Steps

- Add to your **YOUR_PROJECT_NAME.uproject** file:
```c
{
  ...
  "Modules": [
    {
      ...
      "AdditionalDependencies": [
        ...
        "UMG"
      ]	
    }
  ],
  "Plugins": [
    ...
    {
      "Name": "OnlineSubsystem",
      "Enabled": true
    },
    {
      "Name": "OnlineSubsystemSteam",
      "Enabled": true
    }
  ]
}
```

- Add to your **YOUR_PROJECT_NAME.Build.cs** file: **"UMG"**, **"OnlineSubsystem"**, **"OnlineSubsystemSteam"**, e.g.
```c
PublicDependencyModuleNames.AddRange(new string[] {
  ...
  "UMG",
  "OnlineSubsystem",
  "OnlineSubsystemSteam",
});
```

- Activate the following plugins in the editor:
  - Enable Online Subsystem
  - Enable Online Subsystem Steam

- Clone the content of repository into Plugins/MultiplayerSessions,<br>
e.g. `git clone git@github.com:allejik/ue5-multiplayer-sessions-plugin.git .`

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
