# Code-Driven Cluster Conversion PRs

Tracking list of pull requests that convert clusters from the legacy Ember
pattern to the code-driven pattern (identified by the presence of a
`CodegenIntegration.cpp` file in the cluster directory).

Status legend: **merged** = closed & merged, **open** = in review, **closed** =
closed without merge (draft/superseded), 🟡 = open or in-progress.

Data gathered on 2026-04-21. See also:
- [`code_driven_cluster_dos_and_donts.md`](./code_driven_cluster_dos_and_donts.md)
- [`writing_clusters.md`](./writing_clusters.md)
- [`migrating_ember_cluster_to_code_driven.md`](./migrating_ember_cluster_to_code_driven.md)

---

## Cluster Conversion PRs

58 clusters currently have `CodegenIntegration.cpp` files. The PRs below
drove those conversions.

| Cluster | PR | State | Author | Description |
|---|---|---|---|---|
| Access Control | [#41064](https://github.com/project-chip/connectedhomeip/pull/41064) | merged | zaid-google | Migrate Access Control Cluster to be Code Driven |
| Actions | [#43222](https://github.com/project-chip/connectedhomeip/pull/43222) | merged | Elen777300 | PR#1 Rename Actions Cluster |
| Actions | [#43349](https://github.com/project-chip/connectedhomeip/pull/43349) | closed | Elen777300 | [draft] Convert Actions to code-driven |
| Actions | [#43471](https://github.com/project-chip/connectedhomeip/pull/43471) | merged | Elen777300 | Convert Actions cluster to be code-driven |
| Actions | [#43652](https://github.com/project-chip/connectedhomeip/pull/43652) | merged | Elen777300 | Move only ActionsServer class to validate the API |
| Administrator Commissioning | [#39470](https://github.com/project-chip/connectedhomeip/pull/39470) | merged | andy31415 | Migrate Administrator Commissioning cluster |
| Air Quality | [#43546](https://github.com/project-chip/connectedhomeip/pull/43546) | merged | shubhamdp | Rename air-quality-server cpp/h to AirQualityCluster |
| Air Quality | [#43618](https://github.com/project-chip/connectedhomeip/pull/43618) | closed | shubhamdp | Migrate Air Quality cluster |
| Air Quality | [#43630](https://github.com/project-chip/connectedhomeip/pull/43630) | merged | shubhamdp | Migrate Air Quality cluster to code-driven pattern |
| AVSUM (Camera AV Settings User-Level) | [#42279](https://github.com/project-chip/connectedhomeip/pull/42279) | merged | marktrayer | Initial rename for conversion |
| AVSUM | [#42334](https://github.com/project-chip/connectedhomeip/pull/42334) | merged | marktrayer | Migrate to DefaultServerCluster |
| AVSUM | [#42776](https://github.com/project-chip/connectedhomeip/pull/42776) | merged | marktrayer | Cherry-pick rename |
| AVSUM | [#42787](https://github.com/project-chip/connectedhomeip/pull/42787) | merged | marktrayer | Cherry-pick migrate |
| Basic Information | [#39624](https://github.com/project-chip/connectedhomeip/pull/39624) | closed | andy31415 | Migrate Basic Information Cluster (early attempt) |
| Basic Information | [#40422](https://github.com/project-chip/connectedhomeip/pull/40422) | merged | andy31415 | Code driven basic information cluster |
| Binding | [#40607](https://github.com/project-chip/connectedhomeip/pull/40607) | merged | wqx6 | Migrate binding cluster to be code driven |
| Boolean State Configuration | [#41771](https://github.com/project-chip/connectedhomeip/pull/41771) | merged | andy31415 | Rename, prepare for conversion |
| Boolean State Configuration | [#41849](https://github.com/project-chip/connectedhomeip/pull/41849) | merged | andy31415 | Convert Boolean State Configuration |
| Bridged Device Basic Information | [#43084](https://github.com/project-chip/connectedhomeip/pull/43084) | merged | andy31415 | PR#1 Rename |
| Bridged Device Basic Information | [#43142](https://github.com/project-chip/connectedhomeip/pull/43142) | merged | andy31415 | Implement BridgedDeviceBasicInformation cluster |
| Bridged Device Basic Information | [#43338](https://github.com/project-chip/connectedhomeip/pull/43338) | merged | andy31415 | Linux Bridge switch to code-driven |
| Camera AVSM | [#42224](https://github.com/project-chip/connectedhomeip/pull/42224) | merged | pidarped | Migration of CameraAVSM cluster |
| Camera AVSM | [#42311](https://github.com/project-chip/connectedhomeip/pull/42311) | merged | pidarped | Rename towards code-driven model |
| Camera AVSM | [#42767](https://github.com/project-chip/connectedhomeip/pull/42767) | merged | pidarped | Cherry-pick to 1.5.1 |
| Chime | [#42305](https://github.com/project-chip/connectedhomeip/pull/42305) | merged | soares-sergio | PR#1 Rename |
| Chime | [#42331](https://github.com/project-chip/connectedhomeip/pull/42331) | merged | soares-sergio | Implement Chime cluster as Code driven |
| Chime | [#42785](https://github.com/project-chip/connectedhomeip/pull/42785) | merged | marktrayer | Cherry-pick |
| Closure Control | [#43195](https://github.com/project-chip/connectedhomeip/pull/43195) | merged | pimpalemahesh | PR#1 Rename |
| Closure Dimension | [#43203](https://github.com/project-chip/connectedhomeip/pull/43203) | merged | pimpalemahesh | PR#2 Move Cluster Headers to code-driven style |
| Closure Dimension | [#43573](https://github.com/project-chip/connectedhomeip/pull/43573) | merged | pimpalemahesh | Rename closure-dimension cluster files |
| Closure Dimension | [#43633](https://github.com/project-chip/connectedhomeip/pull/43633) | merged | pimpalemahesh | Merge Closure Dimension Logic with Cluster |
| Closure Dimension | [#43720](https://github.com/project-chip/connectedhomeip/pull/43720) | merged | pimpalemahesh | Decoupling of Closure Dimension Cluster |
| Commissioner Control | [#43577](https://github.com/project-chip/connectedhomeip/pull/43577) | closed | arielsz71 | Part 1 (superseded) |
| Commissioner Control | [#43581](https://github.com/project-chip/connectedhomeip/pull/43581) | merged | arielsz71 | Commissioner control cluster part 1 |
| Descriptor | [#40935](https://github.com/project-chip/connectedhomeip/pull/40935) | merged | zaid-google | Migrate Descriptor Cluster |
| Device Energy Management | [#42072](https://github.com/project-chip/connectedhomeip/pull/42072) | merged | lpbeliveau-silabs | First migration: rename server → Cluster |
| Device Energy Management | [#42081](https://github.com/project-chip/connectedhomeip/pull/42081) | merged | lpbeliveau-silabs | PR#2 |
| Device Energy Management | [#42192](https://github.com/project-chip/connectedhomeip/pull/42192) | merged | lpbeliveau-silabs | PR#3 |
| Electrical Energy Measurement | [#41654](https://github.com/project-chip/connectedhomeip/pull/41654) | merged | lpbeliveau-silabs | PR#2 |
| Electrical Energy Measurement | [#41797](https://github.com/project-chip/connectedhomeip/pull/41797) | merged | lpbeliveau-silabs | PR#3 |
| Electrical Power Measurement | [#42061](https://github.com/project-chip/connectedhomeip/pull/42061) | merged | lpbeliveau-silabs | PR#1 |
| Electrical Power Measurement | [#42071](https://github.com/project-chip/connectedhomeip/pull/42071) | merged | lpbeliveau-silabs | PR#2 |
| Electrical Power Measurement | [#42308](https://github.com/project-chip/connectedhomeip/pull/42308) | merged | malbert-silabs | PR#3 |
| Energy EVSE | [#42876](https://github.com/project-chip/connectedhomeip/pull/42876) | merged | lpbeliveau-silabs | #1 |
| Energy EVSE | [#42879](https://github.com/project-chip/connectedhomeip/pull/42879) | merged | lpbeliveau-silabs | #2 |
| Energy EVSE | [#42934](https://github.com/project-chip/connectedhomeip/pull/42934) | merged | lpbeliveau-silabs | #3 |
| Ethernet Network Diagnostics | [#39698](https://github.com/project-chip/connectedhomeip/pull/39698) | merged | yufengwangca | Migrate Ethernet Network Diagnostics |
| Fan Control | [#43179](https://github.com/project-chip/connectedhomeip/pull/43179) | merged | LyudmilaKostanyan | Decouple Fan Cluster part 1 |
| Fan Control 🟡 | [#43408](https://github.com/project-chip/connectedhomeip/pull/43408) | open | LyudmilaKostanyan | Decouple Fan Cluster part 2 |
| Fan Control | [#43718](https://github.com/project-chip/connectedhomeip/pull/43718) | closed | Elen777300 | Implement FanControl code-driven |
| Flow Measurement | [#71552](https://github.com/project-chip/connectedhomeip/pull/71552) | merged | shubhamdp | Cluster server implementation for flow measurement |
| General Commissioning | [#40788](https://github.com/project-chip/connectedhomeip/pull/40788) | merged | shripad621git | Migrate GeneralCommissioningCluster |
| General Commissioning | [#41367](https://github.com/project-chip/connectedhomeip/pull/41367) | merged | shripad621git | Post-merge review fixes |
| General Diagnostics | [#39729](https://github.com/project-chip/connectedhomeip/pull/39729) | merged | zaid-google | Migrate General Diagnostics |
| Group Key Management | [#40504](https://github.com/project-chip/connectedhomeip/pull/40504) | merged | zaid-google | Migrate Group Key Management Cluster |
| Groups | [#42851](https://github.com/project-chip/connectedhomeip/pull/42851) | merged | andy31415 | Part 1 — file renames |
| Groups | [#42886](https://github.com/project-chip/connectedhomeip/pull/42886) | merged | andy31415 | Migrate groups cluster |
| ICD Management | [#41499](https://github.com/project-chip/connectedhomeip/pull/41499) | merged | jadhavrohit924 | Part 3 — Decouple ICD Management |
| Identify | [#41232](https://github.com/project-chip/connectedhomeip/pull/41232) | merged | soares-sergio | Re-implement Identify as CodeDriven |
| Illuminance Measurement | [#42884](https://github.com/project-chip/connectedhomeip/pull/42884) | merged | arielsz71 | Migrate illuminance measurement |
| Illuminance Measurement | [#43307](https://github.com/project-chip/connectedhomeip/pull/43307) | merged | arielsz71 | Improvement |
| Level Control | [#42748](https://github.com/project-chip/connectedhomeip/pull/42748) | merged | soares-sergio | Implement Level Control Cluster + Speaker Device |
| Localization Configuration | [#40717](https://github.com/project-chip/connectedhomeip/pull/40717) | merged | jadhavrohit924 | Migrate Localization Configuration |
| Microwave Oven Control | [#71444](https://github.com/project-chip/connectedhomeip/pull/71444) | merged | arielsz71 | Part 1 |
| Microwave Oven Control 🟡 | [#71598](https://github.com/project-chip/connectedhomeip/pull/71598) | open | arielsz71 | Part 2 |
| Network Commissioning | [#39289](https://github.com/project-chip/connectedhomeip/pull/39289) | merged | andy31415 | Convert Network Commissioning Cluster |
| Occupancy Sensing | [#41954](https://github.com/project-chip/connectedhomeip/pull/41954) | merged | soares-sergio | Re-implement Occupancy Sensing |
| On/Off | [#42634](https://github.com/project-chip/connectedhomeip/pull/42634) | merged | andy31415 | Implement code-driven On/Off Cluster |
| OTA Requestor | [#42878](https://github.com/project-chip/connectedhomeip/pull/42878) | merged | harimau-qirex | File renames |
| OTA Requestor | [#42908](https://github.com/project-chip/connectedhomeip/pull/42908) | merged | harimau-qirex | Code moves |
| OTA Requestor | [#41970](https://github.com/project-chip/connectedhomeip/pull/41970) | merged | harimau-qirex | Convert OTA requestor to code-driven |
| Power Source | [#43135](https://github.com/project-chip/connectedhomeip/pull/43135) | merged | Hayk10002 | PR #1 (file rename) |
| Power Source 🟡 | [#43626](https://github.com/project-chip/connectedhomeip/pull/43626) | open | Hayk10002 | PR #2 |
| Power Topology | [#42063](https://github.com/project-chip/connectedhomeip/pull/42063) | merged | malbert-silabs | PR#1 |
| Power Topology | [#42073](https://github.com/project-chip/connectedhomeip/pull/42073) | merged | malbert-silabs | PR#2 |
| Power Topology | [#42222](https://github.com/project-chip/connectedhomeip/pull/42222) | merged | malbert-silabs | PR#3 |
| Relative Humidity Measurement | [#71424](https://github.com/project-chip/connectedhomeip/pull/71424) | merged | Elen777300 | Migrate Relative Humidity Measurement |
| Resource Monitoring | [#41255](https://github.com/project-chip/connectedhomeip/pull/41255) | closed | gd-mauri | Code-driven resource monitoring update |
| Scenes Management | [#42304](https://github.com/project-chip/connectedhomeip/pull/42304) | merged | andy31415 | PR#1 rename |
| Scenes Management | [#42475](https://github.com/project-chip/connectedhomeip/pull/42475) | merged | andy31415 | Migrate Scenes Management |
| Soil Measurement | [#40442](https://github.com/project-chip/connectedhomeip/pull/40442) | merged | arielsz71 | Convert soil measurement to code driven |
| Software Diagnostics | [#39036](https://github.com/project-chip/connectedhomeip/pull/39036) | merged | andy31415 | Migrate SoftwareDiagnosticsCluster |
| Switch | [#42968](https://github.com/project-chip/connectedhomeip/pull/42968) | merged | arielsz71 | Switch cluster conversion |
| Temperature Control | [#43394](https://github.com/project-chip/connectedhomeip/pull/43394) | merged | arielsz71 | Temperature control cluster |
| Temperature Measurement | [#43204](https://github.com/project-chip/connectedhomeip/pull/43204) | merged | arielsz71 | Temperature measurement cluster |
| Temperature Measurement | [#43337](https://github.com/project-chip/connectedhomeip/pull/43337) | merged | arielsz71 | Update |
| Thermostat 🟡 | [#42325](https://github.com/project-chip/connectedhomeip/pull/42325) | open | hasty | [Draft] Conversion of Thermostat |
| Time Synchronization | [#71461](https://github.com/project-chip/connectedhomeip/pull/71461) | merged | arielsz71 | Decouple dependencies |
| TLS Certificate Management | [#42381](https://github.com/project-chip/connectedhomeip/pull/42381) | merged | yufengwangca | Initial rename |
| TLS Certificate Management | [#42455](https://github.com/project-chip/connectedhomeip/pull/42455) | merged | yufengwangca | Migrate TLS Certificate Management |
| TLS Certificate Management | [#42739](https://github.com/project-chip/connectedhomeip/pull/42739) | closed | yufengwangca | 1.5-branch cherry-pick |
| TLS Certificate Management | [#42740](https://github.com/project-chip/connectedhomeip/pull/42740) | merged | yufengwangca | 1.5-branch cherry-pick |
| TLS Client Management | [#42377](https://github.com/project-chip/connectedhomeip/pull/42377) | merged | yufengwangca | Initial rename |
| TLS Client Management | [#42395](https://github.com/project-chip/connectedhomeip/pull/42395) | merged | yufengwangca | Migrate TLS Client Management |
| TLS Client Management | [#42662](https://github.com/project-chip/connectedhomeip/pull/42662) | merged | yufengwangca | 1.5-branch cherry-pick |
| Unit Localization | [#41331](https://github.com/project-chip/connectedhomeip/pull/41331) | closed | ratgr | With AttributePersistenceMigration |
| Unit Localization | [#42506](https://github.com/project-chip/connectedhomeip/pull/42506) | closed | ratgr | Make codedriven (3/5) |
| Unit Localization | [#42507](https://github.com/project-chip/connectedhomeip/pull/42507) | merged | ratgr | Make codedriven (3/5) |
| Valve Configuration and Control | [#42134](https://github.com/project-chip/connectedhomeip/pull/42134) | closed | tersal | [WIP] code-driven migration |
| WiFi Network Diagnostics | [#39898](https://github.com/project-chip/connectedhomeip/pull/39898) | merged | jadhavrohit924 | Migrate WiFi Network Diagnostics |
| WiFi Network Management | [#43071](https://github.com/project-chip/connectedhomeip/pull/43071) | merged | ksperling-apple | Part 1 |
| WiFi Network Management | [#43085](https://github.com/project-chip/connectedhomeip/pull/43085) | merged | ksperling-apple | Part 2 |
| Window Covering | [#43223](https://github.com/project-chip/connectedhomeip/pull/43223) | merged | AniDashyan | PR#1 rename |
| Window Covering 🟡 | [#71421](https://github.com/project-chip/connectedhomeip/pull/71421) | open | AniDashyan | PR#2 migrate |
| Zone Management | [#43423](https://github.com/project-chip/connectedhomeip/pull/43423) | merged | marybadalyan | ZoneMgmtCluster PR#2 |

---

## Supporting / Framework PRs

Not cluster-specific but part of the broader code-driven effort.

| PR | State | Author | Description |
|---|---|---|---|
| [#40405](https://github.com/project-chip/connectedhomeip/pull/40405) | merged | zaid-google | `IsAttributeEnabled` for dynamic/static endpoints |
| [#40487](https://github.com/project-chip/connectedhomeip/pull/40487) | merged | andy31415 | Create `OptionalAttributeSet` and `AttributeSet` classes |
| [#41006](https://github.com/project-chip/connectedhomeip/pull/41006) | merged | zaid-google | Code Driven Shutdown and Init Callbacks |
| [#41102](https://github.com/project-chip/connectedhomeip/pull/41102) | merged | andy31415 | Add/update documentation on code-driven clusters |
| [#41116](https://github.com/project-chip/connectedhomeip/pull/41116) | merged | andy31415 | Additional code-driven cluster docs |
| [#41400](https://github.com/project-chip/connectedhomeip/pull/41400) | merged | andy31415 | Allow `constexpr` for `DefaultServerCluster` |
| [#41409](https://github.com/project-chip/connectedhomeip/pull/41409) | merged | andy31415 | Update `writing_clusters` guide |
| [#41607](https://github.com/project-chip/connectedhomeip/pull/41607) | merged | zaid-google | Code Driven All Devices Example App |
| [#41675](https://github.com/project-chip/connectedhomeip/pull/41675) | open 🟡 | zaid-google | Lint rules for Init/Shutdown callbacks |
| [#41971](https://github.com/project-chip/connectedhomeip/pull/41971) | merged | harimau-qirex | Doc: mention another array for cluster migration |
| [#42131](https://github.com/project-chip/connectedhomeip/pull/42131) | merged | ksperling-apple | Call code-driven inits before emberAf callbacks |
| [#42144](https://github.com/project-chip/connectedhomeip/pull/42144) | merged | andy31415 | v1.5 cherry-pick of #42131 |
| [#42375](https://github.com/project-chip/connectedhomeip/pull/42375) | merged | jadhavrohit924 | `all-devices-app/esp32` variant |
| [#42439](https://github.com/project-chip/connectedhomeip/pull/42439) | closed | andy31415 | [Draft] Auto command-decoding helper |
| [#42738](https://github.com/project-chip/connectedhomeip/pull/42738) | merged | andy31415 | `set attribute and notify` helper |
| [#42982](https://github.com/project-chip/connectedhomeip/pull/42982) | merged | andy31415 | Remove `Accessors::Set` for code-driven clusters |
| [#43710](https://github.com/project-chip/connectedhomeip/pull/43710) | merged | jepenven-silabs | [Silabs] Code driven ground work |
| [#71543](https://github.com/project-chip/connectedhomeip/pull/71543) | merged | andy31415 | `DataModel::Nullable` support for `SetAttributeValue` |
| [#71676](https://github.com/project-chip/connectedhomeip/pull/71676) | open 🟡 | andy31415 | [AI Skill] Code-driven cluster development skill |

---

## Not-Yet-Migrated Clusters

Cluster directories present under `src/app/clusters/` that do **not** yet have
a `CodegenIntegration.cpp` file:

- account-login-server
- application-basic-server
- application-launcher-server
- audio-output-server
- channel-server
- color-control-server
- commodity-metering-server
- commodity-price-server
- commodity-tariff-server
- concentration-measurement-server
- content-app-observer
- content-control-server
- content-launch-server
- dishwasher-alarm-server
- door-lock-server
- ecosystem-information-server
- electrical-grid-conditions-server
- energy-preference-server
- fan-control-server
- fault-injection-server
- ias-zone-client
- ias-zone-server
- joint-fabric-administrator-server
- joint-fabric-datastore-server
- keypad-input-server
- laundry-dryer-controls-server
- laundry-washer-controls-server
- level-control (verify — conversion may be partial)
- low-power-server
- media-input-server
- media-playback-server
- messages-server
- meter-identification-server
- mode-base-server
- mode-select-server
- network-commissioning (verify — conversion may be partial)
- on-off-server (verify — conversion may be partial)
- pump-configuration-and-control-server
- refrigerator-alarm-server
- sample-mei-server
- service-area-server
- smoke-co-alarm-server
- target-navigator-server
- test-cluster-server
- thermostat-server
- thermostat-user-interface-configuration-server
- thread-border-router-management-server
- window-covering-server (conversion in progress — #71421)
