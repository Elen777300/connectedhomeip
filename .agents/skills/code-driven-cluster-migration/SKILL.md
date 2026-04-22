---
name: code-driven-cluster-migration
description: >
    Step-by-step procedure for migrating a legacy Ember-based Matter server
    cluster under `src/app/clusters/` to the code-driven pattern
    (ServerClusterInterface + DefaultServerCluster + CodegenIntegration).
    Use this skill when the task is phrased as "migrate X cluster", "convert X
    to code-driven", or similar — i.e. you are starting from an existing
    Ember cluster, not writing a new one from scratch.
---

# Code-Driven Cluster Migration

## When to invoke this skill

Invoke when **all** of the following are true:

1. The task is about an existing cluster under `src/app/clusters/<name>-server/`.
2. That directory does **not** yet contain a `CodegenIntegration.cpp` file
   (i.e. it is still Ember-backed).
3. The goal is conversion / migration, not adding a feature to an
   already-code-driven cluster.

If `CodegenIntegration.cpp` already exists, use the
`code-driven-cluster-development` skill instead — that cluster is already
migrated.

If you are writing a brand-new cluster that never existed before, also use
`code-driven-cluster-development`.

---

## Overview — the three-PR pattern

Conversion **must** be split into at least three pull requests. Mixing these
phases is the single most common review rejection reason
(see PR #41849, #43633).

| PR | Contents | Reviewable via |
|----|----------|----------------|
| 1  | File renames and compat-stub creation **only** | `git diff --color-moved` |
| 2  | Code reordering / anonymous-namespace moves **only** (skip if nothing to reorder) | `git diff --color-moved=dimmed-zebra` |
| 3  | The substantive conversion: new class, removed Ember paths, tests, regen | Normal diff |

Never mix renames with logic changes. Never mix reorders with logic changes.

---

## Phase 0 — Discovery

Before writing any code, collect the following. Record them in your working
notes or TodoWrite tasks; you will reference them repeatedly.

### 0.1 Baseline the existing cluster

```bash
# Core files
ls src/app/clusters/<name>-server/
# Look for: <name>-server.h, <name>-server.cpp, BUILD.gn,
#           app_config_dependent_sources.{cmake,gni}, and any Delegate files

# What attributes does ZAP define?
# Open the cluster XML (source of truth) and the generated Metadata.h
cat src/app/zap-templates/zcl/data-model/chip/<name>-cluster.xml
cat zzz_generated/app-common/clusters/<Name>/Metadata.h    # after a regen
cat zzz_generated/app-common/clusters/<Name>/Attributes.h  # after a regen
```

Write down:
- **Mandatory attributes** (spec conformance `M`)
- **Optional attributes** (`O`), and which are gated by features
- **Fixed attributes** (spec quality `F`) — these go in `Config`, no setter
- **Writable attributes** — these need `WriteAttribute` logic
- **Nullable attributes** — check `Attributes.h` for `DataModel::Nullable<T>`
- **Commands** — `AcceptedCommands` set
- **Features** — the feature map bits
- **Whether it is singleton or multi-instance** (Scope: Node vs Endpoint in spec)

### 0.2 Map the blast radius in `examples/` — mandatory

Do **not** skip this step. Clusters are consumed by example apps via
sensor-manager / delegate shims, plugin callbacks, and direct API calls.
Any signature change ripples into those files. You must inventory the
consumers **before** you decide your migration strategy, because the
inventory determines whether backward compat is achievable (see §Backward
compatibility strategy, below).

```bash
# Legacy cluster header + server-class references in apps
grep -rn "<name>-server" examples/ --include="*.h" --include="*.cpp" --include="*.cmake" --include="*.gn"
grep -rn "class <Name>Server" examples/

# ZAP generated accessor usage (Set/Get) — these break when the code-driven
# cluster no longer reads/writes the Ember RAM buffer
grep -rn "<Name>::Attributes::.*::Set" examples/
grep -rn "<Name>::Attributes::.*::Get" examples/

# Plugin callbacks apps may rely on existing (even as empty stubs)
grep -rn "Matter<Name>ClusterServerInitCallback" examples/
grep -rn "Matter<Name>ClusterServerAttributeChangedCallback" examples/
grep -rn "Matter<Name>ClusterServerPreAttributeChangedCallback" examples/

# Application-supplied delegates
grep -rn "public <Name>::Delegate" examples/
grep -rn "public <Name>Delegate" examples/
```

For every hit, record: which app, which file, what API surface (ZAP
accessor / direct class / delegate / plugin callback), and how frequently
it is called. This is the input to §0.4.

### 0.3 Pick a reference PR

Find the most recently-merged conversion PR with a similar cluster shape in
[`docs/guides/code_driven_cluster_conversion_prs.md`](../../../docs/guides/code_driven_cluster_conversion_prs.md).

| Your cluster looks like… | Reference PR |
|---|---|
| Nullable measurement + min/max/tolerance | #71424 Relative Humidity |
| Mandatory list + delegate + commands | #43471 Actions |
| Multi-instance (per-endpoint state) | #43720 Closure Dimension |
| Singleton, node-scoped | #40422 Basic Information |
| Command-heavy with delegate | #42331 Chime |
| Writable scalar + features | #42968 Switch |
| Runtime-only (no Ember defaults) | #71552 Flow Measurement |
| Identify/timer-driven | #41232 Identify |

Read the reference PR's merged diff end-to-end before writing any code.

### 0.4 Decide the backward-compat strategy

Using the §0.2 inventory, classify every external consumer into one of three
buckets. This decision shapes Phase 3 work.

**Tier A — preservable via `CodegenIntegration` forwarders.** The consumer
calls `<Name>Server::X()`, `Matter<Name>ClusterServerInitCallback`, or a
similar public-API method. Keep the legacy class/callback alive in
`CodegenIntegration.h/.cpp` as a thin forwarder to the new cluster. Existing
consumers keep compiling unchanged. **This is the preferred outcome.** Cited
reviewer preference: #42968 "should we preserve the old API interface of
`class SwitchServer`? that way we make life easier for people upgrading";
#43720 "can we preserve previous API and have mInterface expose methods?".

**Tier B — preservable via header compat stub only.** The consumer only
`#include`s the old header but doesn't touch a removed class. Leave
`<name>-server.h` as a one-line stub that forwards to the new headers.
Consumer compiles unchanged. Covered by Phase 1.2.

**Tier C — unavoidable breakage.** The consumer writes to the Ember RAM
buffer via `Attributes::X::Set()`, and the code-driven cluster no longer
reads that buffer — so wrapping `Set` in `CodegenIntegration` cannot
propagate the value into the cluster's member state. Cited: #71424 "The Set()
accessors write to the Ember attribute store, which the code-driven cluster
no longer reads — so keeping compat there isn't feasible". In this case you
**must** do **both** of the following in Phase 3:

1. **Update the example apps' consumer files** (sensor-manager,
   delegate, main.cpp) to use the new cluster API. Each updated file goes
   into PR #3, not a separate PR.
2. **Write a cluster `README.md`** describing the before/after, the legacy
   pattern, the new pattern, and the reason compat was not feasible. Use
   [`src/app/clusters/actions-server/README.md`](../../../src/app/clusters/actions-server/README.md)
   as the template. See §3.12 below for structure.

### 0.5 Read the rules

- [`docs/guides/code_driven_cluster_dos_and_donts.md`](../../../docs/guides/code_driven_cluster_dos_and_donts.md)
  — blocking review findings, with PR citations
- [`docs/guides/code_driven_cluster_ai_playbook.md`](../../../docs/guides/code_driven_cluster_ai_playbook.md)
  — the full procedural playbook (this SKILL is the condensed version)
- `.agents/skills/code-driven-cluster-development/SKILL.md`
  — code-pattern reference for the new cluster class

---

## Phase 1 — File renames only (PR #1)

Produce exactly one PR containing nothing but renames and compat stubs.

### 1.1 Rename the `.cpp`

```bash
git mv src/app/clusters/<name>-server/<name>-server.cpp \
       src/app/clusters/<name>-server/<Name>Cluster.cpp
```

PascalCase. `RelativeHumidityMeasurementCluster.cpp`, not
`relative-humidity-measurement-cluster.cpp`.

### 1.2 Split the `.h` file

Two common shapes:

**Shape A — legacy header only held the server class.**
Rename it directly:
```bash
git mv src/app/clusters/<name>-server/<name>-server.h \
       src/app/clusters/<name>-server/<Name>Cluster.h
```
Then create a **one-line compat stub** at the original path so existing
`#include` lines keep working:
```cpp
// src/app/clusters/<name>-server/<name>-server.h
// Backward-compat stub — do not add new content here.
#pragma once
#include "<Name>Cluster.h"
```

**Shape B — legacy header held multiple types (delegate, server, helpers).**
Split into separate headers:
```
<Name>Cluster.h           // cluster class
<Name>Delegate.h          // delegate interface (if one exists)
CodegenIntegration.h      // declarations for the compat layer (if exposed)
<name>-server.h           // compat stub including the above
```

Rule: `class Foo` lives in `Foo.h`.

### 1.3 Update build files in the same PR

Keep every file listed. Do **not** add new files that didn't exist before.

```gn
# src/app/clusters/<name>-server/BUILD.gn
source_set("<name>-server") {
  sources = [
    "<Name>Cluster.cpp",
    "<Name>Cluster.h",
    "<Name>Delegate.h",   # if you created one
    "<name>-server.h",    # compat stub
  ]
  # public_deps unchanged
}
```

If `CodegenIntegration.cpp` is being created in this PR (Shape B), list it in
`app_config_dependent_sources.cmake` and `app_config_dependent_sources.gni` —
**not** `BUILD.gn`, because it pulls in codegen-dependent headers.

### 1.4 Verify

```bash
# Confirm git sees these as renames
git diff --color-moved=dimmed-zebra

# The build should still work — no logic changed
source scripts/activate.sh
gn gen out/host
ninja -C out/host <name>-server   # or the example app that consumes it
```

### 1.5 Open PR #1

Title: `[<Cluster>] PR#1 — rename for code-driven conversion`
Body: explicitly state "file renames only; no logic changes". This sets
reviewer expectations and enables the fast-track review path.

---

## Phase 2 — Code moves only (PR #2, optional)

Skip this phase entirely if there is no meaningful reordering to do. Do not
invent churn.

Legitimate reasons to include this phase:
- Moving helper functions into an anonymous namespace at the top of the file.
- Grouping command handlers together.
- Alphabetizing attribute case handlers.

No logic change. Reviewers verify via `git diff --color-moved=dimmed-zebra`.

Title: `[<Cluster>] PR#2 — code reordering only`

---

## Phase 3 — The conversion (PR #3)

This is where the substance lives. Follow the sub-steps in order.

### 3.1 Strip the Ember cluster shell

Delete from `<Name>Cluster.cpp`:
- `emberAfReadAttribute` / `emberAfWriteAttribute` calls
- `EmberAfStatus` return types
- `MatterPostAttributeChangeCallback` overrides
- `Attributes::X::Set` / `Attributes::X::Get` calls (these will be replaced by
  C++ members and `CodegenIntegration` default-reading logic)
- `PluginClusterInitCallback` / `PluginClusterShutdownCallback` — these will
  be reinstated inside `CodegenIntegration.cpp` if needed

Leave a compile-broken state intentionally — you will rebuild the implementation
from scratch in the next sub-steps. Do not try to preserve behavior line by
line; the goal is to re-express the cluster in the new idiom, keeping
**observable** behavior identical.

### 3.2 Build the `Config` struct and class header

```cpp
// <Name>Cluster.h
#pragma once

#include <app/server-cluster/DefaultServerCluster.h>
#include <app/server-cluster/OptionalAttributeSet.h>
#include <clusters/<Name>/Attributes.h>
#include <clusters/<Name>/Metadata.h>

namespace chip::app::Clusters {

class <Name>Cluster : public DefaultServerCluster
{
public:
    // Hardware-fixed (spec F) attributes go here. NO runtime setters for these.
    struct Config {
        DataModel::Nullable<int16_t> minValue{};
        DataModel::Nullable<int16_t> maxValue{};
        OptionalAttributeSet<
            <Name>::Attributes::<OptionalAttr>::Id
            /* more optional ids */
        > optionalAttributeSet;

        // Fluent builders for optional attributes with associated values.
        Config & WithTolerance(uint16_t value) {
            mTolerance = value;
            optionalAttributeSet.Set<<Name>::Attributes::Tolerance::Id>();
            return *this;
        }

        uint16_t Tolerance() const { return mTolerance; }
    private:
        uint16_t mTolerance = 0;
    };

    explicit <Name>Cluster(EndpointId endpointId, const Config & config = {});

    // ServerClusterInterface overrides
    DataModel::ActionReturnStatus ReadAttribute(
        const DataModel::ReadAttributeRequest & request,
        AttributeValueEncoder & encoder) override;
    CHIP_ERROR Attributes(const ConcreteClusterPath & path,
        ReadOnlyBufferBuilder<DataModel::AttributeEntry> & builder) override;
    // Override WriteAttribute ONLY if the cluster has writable attributes.
    // Override InvokeCommand / AcceptedCommands ONLY if the cluster has commands.

    // Application-facing API. Exactly one setter per **runtime-mutable**
    // attribute. Fixed attributes have no setter.
    CHIP_ERROR SetMeasuredValue(DataModel::Nullable<int16_t> value);

protected:
    // Expose via a Testable<Name>Cluster subclass for tests — do not make public.
    Config mConfig;
    DataModel::Nullable<int16_t> mMeasuredValue{};
};

} // namespace chip::app::Clusters
```

**Header checklist — every box must be checked:**

- [ ] `class <Name>Cluster` is in `<Name>Cluster.h`
- [ ] Inherits from `DefaultServerCluster`
- [ ] `Config` is **nested** inside the class (no standalone config class +
      `friend` + `using`)
- [ ] `Config` is named `Config` — not `StartupConfiguration`
- [ ] No `using DataModel::X = …` aliases in this header
- [ ] No stacked short aliases that hide types
- [ ] No forward declaration of the cluster class inside its own header
- [ ] No `EndpointId mEndpointId;` member
- [ ] No `mIsRegistered` member
- [ ] No `mContext` member (`DefaultServerCluster::Startup` stores context)
- [ ] No `Init()` method
- [ ] Fixed attributes live in `Config`, no public setter for them
- [ ] Exactly one setter per runtime-mutable attribute

### 3.3 Implement `ReadAttribute`

```cpp
DataModel::ActionReturnStatus <Name>Cluster::ReadAttribute(
    const DataModel::ReadAttributeRequest & request, AttributeValueEncoder & encoder)
{
    using namespace <Name>::Attributes;
    switch (request.path.mAttributeId)
    {
    case ClusterRevision::Id: return encoder.Encode(<Name>::kRevision);
    case FeatureMap::Id:      return encoder.Encode(mFeatureMap.Raw());
    case MeasuredValue::Id:    return encoder.Encode(mMeasuredValue);
    case MinMeasuredValue::Id: return encoder.Encode(mConfig.minValue);
    case MaxMeasuredValue::Id: return encoder.Encode(mConfig.maxValue);
    case Tolerance::Id:        return encoder.Encode(mConfig.Tolerance());
    default:
        return Protocols::InteractionModel::Status::UnsupportedAttribute;
    }
}
```

**Read checklist:**

- [ ] No `VerifyOrReturnError(path != kInvalidAttributeId, …)` before the switch
- [ ] No per-case `VerifyOrReturnError(HasFeature(…), …)` — `Attributes()` gates
- [ ] `ClusterRevision` and `FeatureMap` **MUST** be handled here — the base
      class does NOT handle them for reads.
- [ ] `default:` returns `UnsupportedAttribute` directly (do not delegate to
      base class for reads).

### 3.4 Implement `Attributes()`

```cpp
CHIP_ERROR <Name>Cluster::Attributes(const ConcreteClusterPath & path,
    ReadOnlyBufferBuilder<DataModel::AttributeEntry> & builder)
{
    AttributeListBuilder listBuilder(builder);

    static constexpr DataModel::AttributeEntry kOptional[] = {
        <Name>::Attributes::Tolerance::kMetadataEntry,
    };

    // listBuilder.Append handles mandatory (including globals), optional, and the runtime-enabled set.
    return listBuilder.Append(
        Span(<Name>::Attributes::kMandatoryMetadata),
        Span(kOptional),
        mConfig.optionalAttributeSet);
}
```

**Attributes checklist:**

- [ ] Mandatory set comes from generated `kMandatoryMetadata` — it includes
      the required global attributes (ClusterRevision, FeatureMap, etc.).
- [ ] Optional attributes routed through `OptionalAttributeSet` — not
      if/else chains.
- [ ] Unit tests **must** cover every optional-feature combination.

### 3.5 Implement `WriteAttribute` (only if writable attributes exist)

If the cluster has zero writable attributes, **do not override
`WriteAttribute` at all** — the base class already returns `UnsupportedWrite`.

```cpp
DataModel::ActionReturnStatus <Name>Cluster::WriteAttribute(
    const DataModel::WriteAttributeRequest & request, AttributeValueDecoder & decoder)
{
    switch (request.path.mAttributeId)
    {
    case <Name>::Attributes::MeasuredValue::Id: {
        DataModel::Nullable<int16_t> value;
        ReturnErrorOnFailure(decoder.Decode(value));
        return NotifyAttributeChangedIfSuccess(request.path.mAttributeId,
                                               SetMeasuredValue(value));
    }
    default:
        return Protocols::InteractionModel::Status::UnsupportedWrite;
    }
}
```

**Write checklist:**

- [ ] `default:` branch returns `UnsupportedWrite` directly.
- [ ] If the setter already notifies (e.g. via `SetAttributeValue`), the
      outer `WriteAttribute` does NOT wrap it again — no double-notify.
- [ ] Runtime range violations return `CHIP_IM_GLOBAL_STATUS(ConstraintError)`,
      not `CHIP_ERROR_INVALID_ARGUMENT`.
- [ ] If the cluster has no writable attributes: do not override this method
      at all.

### 3.6 Implement `InvokeCommand` / `AcceptedCommands` (only if commands exist)

```cpp
std::optional<DataModel::ActionReturnStatus> <Name>Cluster::InvokeCommand(
    const DataModel::InvokeRequest & request,
    chip::TLV::TLVReader & input, CommandHandler * handler)
{
    switch (request.path.mCommandId)
    {
    case <Name>::Commands::DoThing::Id: {
        <Name>::Commands::DoThing::DecodableType req;
        ReturnErrorOnFailure(DataModel::Decode(input, req));
        return HandleDoThing(req, handler);
    }
    default:
        return Protocols::InteractionModel::Status::UnsupportedCommand;
    }
}
```

**Commands checklist:**

- [ ] `default:` returns `Status::UnsupportedCommand` (wrapped in `std::optional`).
- [ ] Returning `std::nullopt` means the command was handled (e.g. `handler->AddResponse` was called).
- [ ] Returning a status (e.g. `Status::Success`) means the framework will automatically call `handler->AddStatus`.
- [ ] No `VerifyOrReturnValue(HasFeature(…), …)` inside command handlers —
      `AcceptedCommands` gates.
- [ ] `Handle<Cmd>` helpers are `protected`, not `public`.
- [ ] Status returns are bare (`return Status::Success;`) — `ActionReturnStatus`
      auto-casts; do not wrap in `static_cast`.

### 3.7 Write `CodegenIntegration.cpp`

This is the **only** file where Ember/ZAP APIs are allowed. Mirror the
reference PR's integration file closely.

Key responsibilities:
1. `LazyRegisteredServerCluster<<Name>Cluster>` array sized
   `fixed + CHIP_DEVICE_CONFIG_DYNAMIC_ENDPOINT_COUNT`.
2. `IntegrationDelegate` that reads ZAP defaults into a `Config` — tolerate
   read failure with a safe fallback.
3. `MatterFooClusterInitCallback` / `MatterFooClusterShutdownCallback`.
4. `FindClusterOnEndpoint(EndpointId)` accessor for apps.
5. Thin legacy accessors (`Set<X>(endpointId, value)`) that forward to the
   found cluster — these preserve the old public API.

See the development SKILL for the full template
(`.agents/skills/code-driven-cluster-development/SKILL.md` §CodegenIntegration
Layer).

**CodegenIntegration checklist:**

- [ ] No Ember APIs inside `<Name>Cluster.cpp` — only here
- [ ] ZAP default reads tolerate failure (safe fallback, never `VerifyOrDie`)
- [ ] Invalid ZAP range (`min >= max` because both default to 0) → null both,
      don't crash
- [ ] Singleton pointers null-checked before dereference
      (`Server::GetInstance().GetCASESessionManager()` may return null)
- [ ] Multi-instance arrays indexed by
      `emberAfGetClusterServerEndpointIndex` — not raw `EndpointId`
- [ ] Per-endpoint state packed in one struct array, not parallel arrays
- [ ] Empty `PluginInit`/`PluginShutdown` callback stubs exist **only** if ZAP
      declared them

### 3.8 Write unit tests

Create `src/app/clusters/<name>-server/tests/BUILD.gn` and
`Test<Name>Cluster.cpp`. Mirror an existing tests directory from your
reference cluster.

Minimum required coverage:

- [ ] `Attributes()` returns the correct list for **every** optional-feature
      combination
- [ ] Every mandatory attribute reads successfully after construction
- [ ] Valid write returns `CHIP_NO_ERROR`; out-of-range returns
      `CHIP_IM_GLOBAL_STATUS(ConstraintError)`
- [ ] Boundary tests: below min, at min, at max, above max, same-value no-op
- [ ] `NotifyAttributeChanged` fires on real change; does **not** fire on no-op
      or failed write
- [ ] Reserved null-sentinel rejection (e.g. `0xFFFF` for `uint16_t`) for
      every nullable numeric attribute
- [ ] `Startup` → `Shutdown` cycle is clean
- [ ] Commands tested via `ClusterTester::Invoke` — not direct `Handle<Cmd>`

Use a `TestableFooCluster` subclass to expose protected members; do not
loosen real-class visibility for tests. Use `TestServerClusterContext` fresh
per test.

### 3.9 Update build, config, and ZAP files

All of the following must land in PR #3:

**ZAP / spec side:**
- `src/app/zap-templates/zcl/zcl.json` — add all non-list attributes under
  `attributeAccessInterfaceAttributes`
- `src/app/zap-templates/zcl/zcl-with-test-extensions.json` — same list
- `src/app/zap-templates/zcl/data-model/chip/<name>-cluster.xml` — only if the
  XML needs storage-flag changes (rare; usually not needed)

**Build side:**
- `src/app/clusters/<name>-server/BUILD.gn` — ensure all new `.h` and `.cpp`
  files are listed
- `src/app/clusters/<name>-server/app_config_dependent_sources.cmake` and
  `.gni` — list `CodegenIntegration.cpp` (and `.h` if it has an
  ember-dependent include)
- `src/app/clusters/<name>-server/tests/BUILD.gn` — new, for the test suite
- `src/app/clusters/BUILD.gn` — add the cluster dir to the `# keep-sorted`
  `public_deps` list in `source_set("clusters")`
- `src/BUILD.gn` — add the tests path to `chip_test_group("tests")`,
  alphabetically sorted

**Framework registration:**
- `src/app/zap_cluster_list.json` — add the cluster constant → directory
  mapping under `ServerDirectories` (if the directory was brand-new; most
  migrations, it already exists)
- `src/app/common/templates/config-data.yaml` —
  - **Add** the cluster under `CodeDrivenClusters`
  - **Remove** it from `CommandHandlerInterfaceOnlyClusters` if listed there

### 3.10 Regenerate and commit

**CRITICAL:** Only run this step **AFTER** you have completed Step 3.9 (updating ZAP JSONs and build files). The regeneration script relies on these files to determine which clusters are now code-driven.

```bash
source scripts/activate.sh       # first time per shell
source scripts/bootstrap.sh      # if env is stale
./scripts/tools/zap_regen_all.py
```

`zap_regen_all.py` takes 10–30 minutes. It regenerates:
- `.matter` files across every example app
- `zzz_generated/*/endpoint_config.h` / `.cpp`
- `zzz_generated/app-common/cluster-callbacks.cpp`
- `zzz_generated/app-common/CodeDrivenInitShutdown.cpp`
- C++ accessors (`zzz_generated/app-common/clusters/<Name>/…`)

**Commit every file it changes.** Cherry-picking only some of them breaks
other apps silently. If `git status` shows modifications in `examples/*/*.matter`
or `zzz_generated/`, add them all.

If the regen fails with a python environment error, re-run `scripts/bootstrap.sh`
and try again. If a specific `.matter` file conflicts, run
`scripts/tools/zap_regen_all.py --type specific` for Darwin-only or re-run
after resolving the merge.

### 3.11 Verify build and smoke-test

```bash
gn gen out/host
ninja -C out/host                       # full build
ninja -C out/host check                 # runs unit tests, including yours

# Run just your new test suite
ninja -C out/host src/app/clusters/<name>-server/tests:tests_run
```

If `ninja check` does not include your new tests, step 3.9's `src/BUILD.gn`
update is missing.

**REPL smoke-test** (strongly recommended before opening PR):
```bash
rm /tmp/chip_*
# Terminal 1
out/host/chip-all-clusters-app
# Terminal 2
./scripts/tests/run_python_test.py --script tools/matter/matter-repl.py
# In REPL:
> commission
> read <cluster-name> <one-mandatory-attribute>
```

A misconfigured cluster crashes on the `read` — catch it now.

### 3.12 Update example-app consumers (Tier C only)

If the §0.4 decision placed any consumer in Tier C (unavoidable breakage),
update every such file now, **in this same PR**. Splitting example-app
updates into a follow-up PR breaks CI because the main build depends on the
examples.

Common consumer shapes to update:

- **Sensor-manager / data-provider shims** under
  `examples/<app>/*/include/*-sensor-manager.h` / `.cpp`. Old pattern:
  ```cpp
  <Name>::Attributes::MeasuredValue::Set(endpointId, value);
  ```
  New pattern:
  ```cpp
  LogErrorOnFailure(<Name>::SetMeasuredValue(endpointId, value));
  // SetMeasuredValue is defined in CodegenIntegration.cpp and forwards to
  // the cluster instance found by FindClusterOnEndpoint.
  ```

- **Application delegates** that depend on removed plugin callbacks
  (`Matter<Name>ClusterServerAttributeChangedCallback`). Rewrite them to
  observe via the cluster's own notification path or the application's own
  event plumbing.

- **`main.cpp` / `AppTask.cpp`** files that construct a `<Name>Server`
  directly. Prefer switching to the `FindClusterOnEndpoint` API; if the app
  has genuinely code-driven-only ambitions, use
  `RegisteredServerCluster<<Name>Cluster>` per the development SKILL.

After each update, build the example app locally:
```bash
gn gen out/<app> --args='chip_project_config_include_dirs=...'
ninja -C out/<app>
```
Every example app that the grep in §0.2 identified must build cleanly.

### 3.13 Write a `README.md` when backward compat is not preserved

If you did **any** Tier-C work in §3.12, or if the migration changes the
public API in a way existing apps need to know about, add or update
`src/app/clusters/<name>-server/README.md`. Use
[`src/app/clusters/actions-server/README.md`](../../../src/app/clusters/actions-server/README.md)
as the template.

The README must cover:

1. **Scope constraint** — is this cluster node-singleton or per-endpoint?
   Any constraints the app must honor (e.g. "must appear on the aggregator
   endpoint only").
2. **Architecture (two-layer)** —
   - The new `<Name>Cluster` class as the canonical implementation (preferred
     for new code), with a minimal usage snippet.
   - The `<Name>Server` / `CodegenIntegration` wrapper as the
     backward-compatibility path (if you kept one), with a minimal usage
     snippet.
3. **ZAP-generated callbacks** — whether the plugin callbacks are left as
   empty stubs, actively used, or removed. State which, so app authors know
   what to wire.
4. **Usage examples** — one code block for each of:
   - "New code (code-driven data model)"
   - "Legacy / backwards-compatible code"
5. **Delegate interface** — if the cluster has a delegate, list each method
   and what it must return.
6. **Asynchronous event generation** — if the cluster emits events, how
   applications trigger them post-command-acceptance.
7. **Breaking change notice** — for Tier-C migrations only: a short section
   titled "Migrating from the Ember API" listing each removed or changed API
   with the replacement snippet.

Keep the README tight. The actions-server README is ~100 lines and covers
all seven points; mirror that length.

### 3.14 Open PR #3

Title: `[<Cluster>] Migrate <Cluster> cluster to code-driven implementation`

PR body must include:
- Link to PR #1 (renames) and PR #2 (moves, if applicable).
- Link to the Matter spec clause(s) you implemented against.
- Flash-size delta — measure via the master-vs-branch size report; reviewers
  will ask if you don't provide it.
- If any Tier-C consumer changes landed: an explicit list of the example
  apps whose sensor-manager / delegate files changed, and a pointer to the
  cluster `README.md` for the migration instructions.
- "How to use this cluster in Codegen vs. CodeDriven mode" — either a
  paragraph in the PR body or a link to the new/updated
  `src/app/clusters/<name>-server/README.md`.

Do **not** mix unrelated cleanups into this PR. Open them as follow-ups.

---

## Ember → code-driven mapping reference

When rewriting the cluster, translate each Ember construct to its code-driven
equivalent:

| Ember construct | Code-driven equivalent | Where it lives |
|---|---|---|
| `EmberAfStatus MatterFooClusterServerPreAttributeChangedCallback(...)` | `WriteAttribute` method + `SetFooValue` setter with validation | `<Name>Cluster.cpp` |
| `MatterFooClusterServerAttributeChangedCallback(...)` | Built into `SetAttributeValue` / `NotifyAttributeChangedIfSuccess` — no explicit callback needed | `<Name>Cluster.cpp` |
| `MatterFooClusterServerInitCallback(EndpointId)` | `MatterFooClusterInitCallback` in `CodegenIntegration.cpp` (calls `CodegenClusterIntegration::RegisterServer`) | `CodegenIntegration.cpp` |
| `MatterFooClusterServerShutdownCallback(EndpointId)` | `MatterFooClusterShutdownCallback` calling `UnregisterServer` | `CodegenIntegration.cpp` |
| `emberAfReadAttribute(...)` | C++ member access + `encoder.Encode(member)` | `<Name>Cluster.cpp` |
| `emberAfWriteAttribute(...)` | `SetAttributeValue(member, value, attrId)` | `<Name>Cluster.cpp` |
| `Attributes::X::Set(endpointId, value)` | `cluster->SetX(value)` | call sites updated; thin forwarder in `CodegenIntegration.cpp` preserves API |
| `Attributes::X::Get(endpointId, &value)` | `cluster->GetX()` | same |
| `MatterReportingAttributeChangeCallback(path)` | Automatic — `SetAttributeValue` / `NotifyAttributeChanged*` does it | n/a (remove) |
| `class FooServer : public AttributeAccessInterface` | `class <Name>Cluster : public DefaultServerCluster` | `<Name>Cluster.h` |
| `CHIP_ERROR FooServer::Init()` | `CHIP_ERROR <Name>Cluster::Startup(ServerClusterContext &)` (and usually just delegate to base) | `<Name>Cluster.cpp` |
| Feature-flag check in a read/write switch | **Delete** — `Attributes()` / `AcceptedCommands()` is the gate | n/a |

---

## Common blockers and how to unblock

### "My `ReadAttribute` compiles but returns garbage for `ClusterRevision`"
`DefaultServerCluster` handles global attributes (`ClusterRevision`,
`FeatureMap`, `AttributeList`, etc.) — do not re-enumerate them in your
`ReadAttribute` or `Attributes()` override. Remove the cases.

### "Append fails silently in `AcceptedCommands`"
`ReadOnlyBufferBuilder::Append` requires pre-allocation. Switch to
`AppendElements` with a pre-counted span, or use `ReferenceExisting` with a
`constexpr` array. Write tests that exercise every feature combination —
this is where the failure shows up.

### "My setter is notifying twice"
You probably wrapped a `SetAttributeValue`-based setter in
`NotifyAttributeChangedIfSuccess`. Pick one:
- Setter calls `SetAttributeValue` (notifies internally) → `WriteAttribute`
  just returns the setter's status, no wrapper needed.
- Setter does NOT notify → `WriteAttribute` wraps in
  `NotifyAttributeChangedIfSuccess`.

### "Unit tests pass but REPL read crashes"
Usually `CodegenIntegration` didn't wire the cluster up — `RegisterServer`
wasn't called, or the cluster instance array is zero-sized because
`Foo::StaticApplicationConfig::kFixedClusterConfig` isn't imported. Diff
against the reference PR's `CodegenIntegration.cpp`.

### "`zap_regen_all.py` fails with cluster-not-found"
Check `src/app/zap_cluster_list.json` — the cluster constant must map to the
directory name under `ServerDirectories`. Also verify
`src/app/common/templates/config-data.yaml` lists it under
`CodeDrivenClusters`.

### "Reviewer asks for flash-size delta"
Run the size report against master. If the delta is positive, look at the
suggestions in dos-and-donts §12 (inline one-liners, remove non-essential
logs, etc.).

### "`NotifyAttributeChanged` isn't firing in my test"
Wrap your setter call in a helper that also creates a `ClusterTester`, and
assert against the dirty-list notifications on `TestServerClusterContext`.
See `src/app/clusters/identify-server/tests/` for the pattern.

---

## Must-not-do (blocking review findings)

Numbered so reviewers and agents can cite them:

1. `using DataModel::X = …` in a header (dos-and-donts §2.1)
2. Forward-declare the cluster class inside its own header (§2.4)
3. `class Foo` outside `Foo.h` (§2.5)
4. `#include <data-model-providers/codegen/…>` or `<app/util/…>` in
   `<Name>Cluster.cpp` (§2.7)
5. `Init()` method on the cluster class (§3.7)
6. `EndpointId` member on the cluster class (§4.3)
7. `mIsRegistered` / registry calls in the cluster class (§4.4, §4.5)
8. Constructor with 7+ parameters, 3+ consecutive same-type (§3.1)
9. Runtime setter for a hardware-fixed attribute (§3.4)
10. Path-validity or feature check inside Read/Write/Invoke (§6.1–§6.3)
11. `WriteAttribute` default branch returning `UnsupportedAttribute` — must
    delegate to base (§7.1)
12. Double-notify (§7.4)
13. Ember `Attributes::X::Set` inside the cluster class (§16.1)
14. Hex constant for a decimal spec bound — `0x270F` for `9999` (§10.1)
15. New `TEMPORARY_RETURN_IGNORED` (§9.3)
16. Unlisted `.h` file (§17)
17. Missing test for optional-feature combinations of `Attributes()` (§14.2)
18. `static_cast<ActionReturnStatus>(Status::X)` — let it auto-cast (§8)

---

## Rejected proposals — do not re-raise

Pushes-back on these have been accepted with reasoned justification. Do not
re-propose:

- "Add a setter for every RW attribute" — YAGNI. Add when an app actually
  needs it.
- "Keep the old `Attributes::X::Set` accessor for backwards compat" —
  infeasible when the accessor wrote to the Ember RAM buffer the code-driven
  cluster no longer reads. Compat lives in `CodegenIntegration`, not in the
  accessor.
- "Extract a template base for all measurement clusters" — deferred. Type
  differences (`int16_t` Temperature, `uint16_t` Humidity, enum LightSensor)
  make this non-trivial. Revisit after more clusters convert.
- "Add a path-validity check before the switch for safety" — not needed; the
  framework guarantees path validity.

---

## Final self-check

Before telling the user the migration is complete, confirm every item:

- [ ] All three PRs are separate, with clear titles
- [ ] Cluster class lives in `<Name>Cluster.h` / `.cpp`
- [ ] `Config` is nested, named `Config`, fixed-attributes only
- [ ] No `Init()`, no `mEndpointId`, no `mIsRegistered`, no manual `mContext`
- [ ] `ReadAttribute` default returns `UnsupportedAttribute`
- [ ] `WriteAttribute` (if overridden) default delegates to base
- [ ] No feature checks inside Read/Write/Invoke
- [ ] `CodegenIntegration.cpp` is the **only** file touching Ember/ZAP APIs
- [ ] Tests exist for every optional-feature combination of `Attributes()` /
      `AcceptedCommands()`
- [ ] Tests exist for null-sentinel rejection on nullable numeric attributes
- [ ] `zap_regen_all.py` has been run and every generated file is committed
- [ ] Cluster listed in `CodeDrivenClusters`, removed from
      `CommandHandlerInterfaceOnlyClusters`
- [ ] Cluster directory in `src/app/clusters/BUILD.gn` `public_deps` (sorted)
- [ ] Tests path in `src/BUILD.gn` `chip_test_group("tests")` (sorted)
- [ ] `ninja check` runs the new tests and they pass
- [ ] REPL smoke-test succeeded for a read and (if applicable) a write + command
- [ ] `examples/` inventory from §0.2 is complete; every consumer classified
      as Tier A / B / C in §0.4
- [ ] Tier-A consumers still compile unchanged because
      `CodegenIntegration.cpp` forwards the legacy API
- [ ] Tier-B consumers still compile unchanged because the `<name>-server.h`
      compat stub is in place
- [ ] Tier-C consumers (sensor-managers, delegates, `main.cpp`) have been
      updated in this PR and every affected example app builds locally
- [ ] If any Tier-C work was done, or the public API changed in a
      user-visible way, `src/app/clusters/<name>-server/README.md` has been
      written/updated using
      [`actions-server/README.md`](../../../src/app/clusters/actions-server/README.md)
      as the template
- [ ] Flash-size delta measured and in PR body
- [ ] Copyright years on modified files include the **original** year

If every box is checked, the migration is ready for review.

---

## References

- Development skill (for new clusters or general code patterns):
  `.agents/skills/code-driven-cluster-development/SKILL.md`
- Full procedural playbook (this SKILL condensed from it):
  `docs/guides/code_driven_cluster_ai_playbook.md`
- Prescriptive rules with PR citations:
  `docs/guides/code_driven_cluster_dos_and_donts.md`
- Deduplicated review rules:
  `docs/guides/code_driven_cluster_review_rules.md`
- Raw review comments (532 across 25 PRs):
  `docs/guides/code_driven_cluster_review_comments_raw.md`
- PR index (all code-driven conversion PRs):
  `docs/guides/code_driven_cluster_conversion_prs.md`
- Human migration guide:
  `docs/guides/migrating_ember_cluster_to_code_driven.md`
- Writing new clusters:
  `docs/guides/writing_clusters.md`
- Base class:
  `src/app/server-cluster/DefaultServerCluster.h`
- Testing helpers:
  `src/app/server-cluster/testing/`
- Integration helper:
  `src/data-model-providers/codegen/ClusterIntegration.h`
