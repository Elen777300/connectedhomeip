# Code-Driven Cluster Migration — AI Playbook

**Audience:** Large-language-model agents (GitHub Copilot, Gemini Code Assist,
Claude Code, Cursor, any IDE assistant) working on the `connectedhomeip` repo.

**Scope:** Step-by-step instructions for converting an Ember-based cluster
under `src/app/clusters/` into a code-driven cluster (one that has a
`CodegenIntegration.cpp` file and inherits from `DefaultServerCluster`).

**Authoritative references** (read these before making substantive changes):

1. [`writing_clusters.md`](./writing_clusters.md) — architecture overview
2. [`migrating_ember_cluster_to_code_driven.md`](./migrating_ember_cluster_to_code_driven.md) — human-oriented migration guide
3. [`code_driven_cluster_dos_and_donts.md`](./code_driven_cluster_dos_and_donts.md) — prescriptive rules with PR citations
4. [`code_driven_cluster_review_rules.md`](./code_driven_cluster_review_rules.md) — deduplicated review rules
5. [`code_driven_cluster_conversion_prs.md`](./code_driven_cluster_conversion_prs.md) — prior-art PR index
6. `.agents/skills/code-driven-cluster-development/SKILL.md` — Claude-format skill file

If any instruction below conflicts with those documents, **those documents win**.

---

## 0. Pre-flight checklist

Before you write any code, confirm each of the following. If you cannot
confirm one, stop and ask the user.

- [ ] **Cluster directory is identified.** It lives under
      `src/app/clusters/<name>-server/`.
- [ ] **The cluster is Ember-backed today** (i.e. no `CodegenIntegration.cpp`
      exists in that directory). If `CodegenIntegration.cpp` *already* exists,
      you are not migrating — you are editing a code-driven cluster.
- [ ] **The cluster has a spec reference** you can cite. Locate it in the
      Matter spec before making attribute-semantics decisions.
- [ ] **There is a reference conversion** in
      [`code_driven_cluster_conversion_prs.md`](./code_driven_cluster_conversion_prs.md)
      you can mirror. Prefer a recent merged PR from a similar cluster shape
      (measurement, command-heavy, singleton, multi-instance).
- [ ] **A Testable subclass, `TestServerClusterContext`, and `ClusterTester`
      pattern** exists in other cluster tests you can mirror. See
      `src/app/clusters/relative-humidity-measurement-server/tests/` or
      `src/app/clusters/identify-server/tests/` for modern references.
- [ ] **You have a way to run `zap_regen_all.py` locally.** It regenerates
      `.matter`, endpoint configs, cluster callbacks, and C++ accessors — all
      of which must be committed in the conversion PR.

If the user asked for a broader task (e.g. "add a new feature and migrate"),
**split it**. Migration PRs must not mix with feature work.

---

## 1. Structure the work as three PRs

Do not combine these phases in one PR. Reviewers repeatedly ask for the split
(see #41849, #43633 in the dos-and-donts doc).

| PR | Contents | Must NOT contain |
|----|----------|------------------|
| **PR 1** | File renames; stub compat headers | Any logic change, any reordering |
| **PR 2** | Function reordering; anonymous-namespace moves | Any logic change |
| **PR 3** | The actual migration: new class, removed Ember paths, tests | Renames/moves not already in PR 1/2 |

You may propose further follow-up PRs (e.g. "decouple dependencies",
"add README", "remove legacy callback stubs"), but those are after PR 3 lands.

---

## 2. PR 1 — Renames only

### 2.1 Rename the `.cpp` file
```
git mv src/app/clusters/<name>-server/<name>-server.cpp \
       src/app/clusters/<name>-server/<Name>Cluster.cpp
```
Use PascalCase for the class file name (`RelativeHumidityMeasurementCluster.cpp`,
not `relative-humidity-measurement-cluster.cpp`).

### 2.2 Handle the `.h` file — one of two paths
**Path A** — the legacy header only held the server class declaration:
1. Copy `<name>-server.h` → `<Name>Cluster.h`.
2. Replace the body of `<name>-server.h` with a single line:
   ```cpp
   #include "CodegenIntegration.h"
   ```
   (Or include `<Name>Cluster.h` directly if no `CodegenIntegration.h` exists
   yet — but typically you'll have one.)

**Path B** — the legacy header held several classes (delegate, server, etc.):
1. Create `CodegenIntegration.h` and move codegen-integration pieces there.
2. Create `<Name>Delegate.h` and move the delegate there (if separable).
3. Create `<Name>Cluster.h` and move the cluster class there.
4. Leave `<name>-server.h` as a compat stub including the new headers.

### 2.3 Update build files in the same PR
Every renamed file must stay listed:

- `src/app/clusters/<name>-server/BUILD.gn` — core `.h`/`.cpp`
- `src/app/clusters/<name>-server/app_config_dependent_sources.cmake` —
  `CodegenIntegration.cpp` (if extracted)
- `src/app/clusters/<name>-server/app_config_dependent_sources.gni` — same

Do **not** add new files that didn't exist before this PR (e.g. don't create
the test directory yet).

### 2.4 Commit and open PR 1
- Title: `[<Cluster>] PR#1 — rename for code-driven conversion`
- Body: state explicitly "file renames only; no logic changes".
- Verify: `git diff --color-moved` shows mostly moves.

---

## 3. PR 2 — Code moves only (optional)

Only do this PR if the cluster has meaningful function reordering needs
(e.g. moving helpers into an anonymous namespace at the top of the file,
alphabetizing definitions, grouping command handlers).

- Title: `[<Cluster>] PR#2 — code moves only`
- Body: "function reordering only; no logic changes; diff will minimize under
  `git diff --color-moved=dimmed-zebra`".

If there is nothing meaningful to move, **skip this phase** — do not invent
churn.

---

## 4. PR 3 — The conversion

This is the substantive work. Follow these steps in order.

### 4.1 Write the new cluster class skeleton

In `<Name>Cluster.h`:

```cpp
#pragma once

#include <app/server-cluster/DefaultServerCluster.h>
#include <app/server-cluster/OptionalAttributeSet.h>
#include <clusters/<Name>/Attributes.h>
#include <clusters/<Name>/Metadata.h>

namespace chip::app::Clusters {

class <Name>Cluster : public DefaultServerCluster
{
public:
    struct Config {
        // Hardware-fixed attributes go here — NOT behind setters.
        // Use WithX() builders for optional attributes with values.
    };

    explicit <Name>Cluster(EndpointId endpointId, const Config & config = {});

    // ServerClusterInterface overrides
    DataModel::ActionReturnStatus ReadAttribute(
        const DataModel::ReadAttributeRequest & request,
        AttributeValueEncoder & encoder) override;
    CHIP_ERROR Attributes(const ConcreteClusterPath & path,
        ReadOnlyBufferBuilder<DataModel::AttributeEntry> & builder) override;

    // Application-facing API (runtime-mutable attributes only)
    CHIP_ERROR Set<LiveValue>(<Type> value);

protected:
    // Protected members, testable via a TestableFooCluster subclass in tests.
};

} // namespace chip::app::Clusters
```

**Checklist — stop and fix if any fails:**
- [ ] `class <Name>Cluster` lives in `<Name>Cluster.h`.
- [ ] `Config` is a **nested** struct (not a standalone class with
      `friend` / `using`).
- [ ] `Config` is named **`Config`** (not `StartupConfiguration`).
- [ ] No `using DataModel::X = ...` aliases in the header.
- [ ] No short-alias soup that hides types (`using F = Nullable<uint16_t>;`).
- [ ] No forward declarations unless unavoidable + commented.
- [ ] No `EndpointId` member — use `GetPaths()[0].mEndpointId`.
- [ ] No `mIsRegistered` / `mContext` member you set yourself
      (`DefaultServerCluster::Startup` already stores context).
- [ ] No `Init()` method.

### 4.2 Implement `ReadAttribute`

```cpp
DataModel::ActionReturnStatus <Name>Cluster::ReadAttribute(
    const DataModel::ReadAttributeRequest & request, AttributeValueEncoder & encoder)
{
    using namespace <Name>::Attributes;
    switch (request.path.mAttributeId)
    {
    case MeasuredValue::Id:      return encoder.Encode(mMeasuredValue);
    case MinMeasuredValue::Id:   return encoder.Encode(mConfig.minMeasuredValue);
    // ...
    default:
        return Protocols::InteractionModel::Status::UnsupportedAttribute;
    }
}
```

**Checklist:**
- [ ] No upfront path-validity check (no `VerifyOrReturnError(path != kInvalid, …)`).
- [ ] No per-case feature guards — `Attributes()` already excludes
      feature-gated attributes.
- [ ] `ClusterRevision` and `FeatureMap` are handled by `DefaultServerCluster`;
      don't re-enumerate them.

### 4.3 Implement `Attributes()`

```cpp
CHIP_ERROR <Name>Cluster::Attributes(const ConcreteClusterPath & path,
    ReadOnlyBufferBuilder<DataModel::AttributeEntry> & builder)
{
    AttributeListBuilder listBuilder(builder);

    static constexpr DataModel::AttributeEntry kOptional[] = {
        <Name>::Attributes::<Optional1>::kMetadataEntry,
        <Name>::Attributes::<Optional2>::kMetadataEntry,
    };

    return listBuilder.Append(Span(<Name>::Attributes::kMandatoryMetadata),
                              Span(kOptional), mConfig.optionalAttributeSet);
}
```

**Checklist:**
- [ ] Mandatory attributes come from the generated `kMandatoryMetadata`.
- [ ] Optional attributes go through `OptionalAttributeSet`, not manual
      if/else branches.
- [ ] `Append` is guarded by `AppendElements` or pre-allocation — tests must
      cover every feature combination.

### 4.4 Implement `WriteAttribute` (only if there are writable attributes)

```cpp
DataModel::ActionReturnStatus <Name>Cluster::WriteAttribute(
    const DataModel::WriteAttributeRequest & request, AttributeValueDecoder & decoder)
{
    switch (request.path.mAttributeId)
    {
    case <Writable>::Id: {
        <Type> value{};
        ReturnErrorOnFailure(decoder.Decode(value));
        return NotifyAttributeChangedIfSuccess(request.path.mAttributeId,
                                               Set<Writable>(value));
    }
    default:
        return DefaultServerCluster::WriteAttribute(request, decoder); // not UnsupportedAttribute
    }
}
```

**Checklist:**
- [ ] Default branch delegates to the **base class**, not
      `Status::UnsupportedAttribute`.
- [ ] If the cluster has no writable attributes, **do not override
      `WriteAttribute` at all**.
- [ ] No double-notify: if your `Set<X>` already calls
      `NotifyAttributeChanged`, do not wrap the result in
      `NotifyAttributeChangedIfSuccess`.

### 4.5 Implement `InvokeCommand` (only if there are commands)

```cpp
std::optional<DataModel::ActionReturnStatus> <Name>Cluster::InvokeCommand(
    const DataModel::InvokeRequest & request,
    chip::TLV::TLVReader & input, CommandHandler * handler)
{
    switch (request.path.mCommandId)
    {
    case <Name>::Commands::<Cmd>::Id: {
        <Name>::Commands::<Cmd>::DecodableType req;
        ReturnErrorOnFailure(DataModel::Decode(input, req));
        return Handle<Cmd>(req, handler);
    }
    default:
        return std::nullopt;
    }
}
```

**Checklist:**
- [ ] Unknown commands return `std::nullopt` (not an error).
- [ ] `Handle<Cmd>` helpers are `protected` — not `public`.
- [ ] No per-case feature check — `AcceptedCommands()` is the gatekeeper.
- [ ] Status returns are bare (`return Status::Success;`) — do not cast to
      `ActionReturnStatus`.

### 4.6 Write the `CodegenIntegration.cpp` layer

This is the **only** file where Ember/ZAP APIs are allowed. Mirror a recent
conversion — `relative-humidity-measurement-server/CodegenIntegration.cpp`,
`flow-measurement-server/CodegenIntegration.cpp`, or
`air-quality-server/CodegenIntegration.cpp` are good starting points.

**Responsibilities:**
1. Allocate via `LazyRegisteredServerCluster<<Name>Cluster>` — not manual
   constructor + registry calls.
2. Read ZAP defaults in an `IntegrationDelegate` — tolerate read failure with
   a safe fallback (null / zero / spec-neutral).
3. Register via `CodegenClusterIntegration::RegisterServer` +
   `UnregisterServer`.
4. Provide `FindClusterOnEndpoint(EndpointId)` for app access.
5. Preserve the legacy public API (`<Name>Server`, accessor-style setters) as
   thin forwarders — downstream apps depend on them.

**Checklist:**
- [ ] No `EmberAfStatus`, `emberAfReadAttribute`, `emberAfWriteAttribute`,
      or ZAP `Attributes::X::Set()` calls **inside the cluster class** —
      they are allowed only here.
- [ ] Multi-instance indexing uses `emberAfGetClusterServerEndpointIndex` —
      not raw `EndpointId` as an array index.
- [ ] Per-endpoint state is packed in **one struct array**, not parallel
      arrays.
- [ ] Every singleton pointer you dereference (e.g.
      `Server::GetInstance().GetCASESessionManager()`) is null-checked.
- [ ] Empty `MatterFooPluginServerInitCallback` / `ShutdownCallback` stubs
      exist **only** if ZAP generated them. Do not invent them.

### 4.7 Write the unit tests

Create `src/app/clusters/<name>-server/tests/Test<Name>Cluster.cpp` and
matching `BUILD.gn`. Required coverage:

- [ ] `Attributes()` returns the correct list for **every feature
      combination** — this is how allocation bugs survive without tests.
- [ ] Mandatory attributes read successfully after construction.
- [ ] Valid writes return `CHIP_NO_ERROR`; invalid ones return
      `CHIP_IM_GLOBAL_STATUS(ConstraintError)`.
- [ ] Boundary tests: below min, at min, at max, above max, same-value (no-op).
- [ ] `NotifyAttributeChanged` fires on real change, **not** on failed writes
      or no-op writes.
- [ ] Reserved null sentinel (`0xFFFF` for `uint16_t`, etc.) is rejected for
      nullable numeric attributes.
- [ ] `Startup` → `Shutdown` cycle works cleanly.
- [ ] Commands are tested via `ClusterTester::Invoke` / `InvokeCommand`, **not**
      by calling `Handle<Cmd>` directly.

Use `TestableFooCluster` subclass to expose protected methods. Use
`TestServerClusterContext` fresh per test.

### 4.8 Update configuration and generated files

Run, in order:
```
scripts/tools/zap_regen_all.py
```
Then verify and **commit** all changes to:
- `.matter` files
- `endpoint_config` / `endpoint_config.h` files
- `cluster-callbacks.cpp`
- `CodeDrivenInitShutdown.cpp`
- Generated C++ accessor files

Edit manually:
- `src/app/zap_cluster_list.json` — add the cluster under `ServerDirectories`
  if the directory is new.
- `src/app/common/templates/config-data.yaml` — add the cluster to
  `CodeDrivenClusters`; remove it from `CommandHandlerInterfaceOnlyClusters`
  if it was listed there.
- `src/app/zap-templates/zcl/zcl.json` and `zcl-with-test-extensions.json` —
  add all non-list attributes under `attributeAccessInterfaceAttributes`.
- `src/app/clusters/BUILD.gn` — add the cluster directory to the
  `# keep-sorted` `public_deps` list in `source_set("clusters")`.
- `src/BUILD.gn` — add the tests path to `chip_test_group("tests")`, sorted
  alphabetically.

### 4.9 Verify the build before committing

```
# From repo root
ninja -C out/debug
ninja -C out/debug check        # runs unit tests including your new ones
```

If `ninja check` does not include your new tests, step 4.8's `src/BUILD.gn`
update is missing.

### 4.10 Smoke-test with REPL (strongly recommended)

```
rm /tmp/chip_*
# Terminal 1
out/debug/chip-all-clusters-app
# Terminal 2
python -m matter_server.repl
> commission
> read <your-cluster> <one-attribute>
```

A misconfigured cluster crashes at `read` here — catching it now saves review
rounds.

### 4.11 Open PR 3

- Title: `[<Cluster>] Migrate <Cluster> cluster to code-driven implementation`
- Body must include:
  - Reference to PR 1 and PR 2.
  - Link to the relevant spec clause.
  - Flash-size delta (reviewers will ask — pre-empt this by posting it).
  - "How to use in Codegen vs. CodeDriven" section (or a README.md update in
    the cluster directory).
- Do **not** mix unrelated cleanups into this PR.

---

## 5. Must-not-do list (compact)

The following are **blocking review findings** — reviewers reject the PR.
Cross-referenced sections in the dos-and-donts doc given in parentheses.

1. `using DataModel::X = …` in a header. (Header Design §2.1)
2. `class FooCluster;` forward declaration inside the cluster's own header.
   (§2.4)
3. `class Foo` declared in `<name>-server.h` instead of `Foo.h`. (§2.5)
4. `#include <data-model-providers/codegen/...>` in
   `<Name>Cluster.cpp`. (§2.7)
5. `class <Name>Cluster` defines an `Init()` method. (§3.7)
6. `EndpointId mEndpointId;` as a cluster member. (§4.3)
7. `mIsRegistered` / registry calls inside the cluster class. (§4.4, §4.5)
8. Constructor with 7+ params where 3+ consecutive are the same type. (§3.1)
9. Runtime setters for hardware-fixed attributes (`MinMeasuredValue`,
   `Tolerance`, etc.). (§3.4)
10. Path-validity or feature checks inside `ReadAttribute` / `WriteAttribute` /
    `InvokeCommand`. (§6.1, §6.2, §6.3)
11. `WriteAttribute` default returns `UnsupportedAttribute` instead of
    `DefaultServerCluster::WriteAttribute(...)`. (§7.1)
12. Double-notify (wrapping a setter that already notifies). (§7.4)
13. Ember `Attributes::X::Set` calls inside the cluster class. (§16.1)
14. Hex constant for a decimal spec bound (`0x270F` for `9999`). (§10.1)
15. `TEMPORARY_RETURN_IGNORED` introduced in new code. (§9.3)
16. Unlisted `.h` file (not in any `BUILD.gn` or
    `app_config_dependent_sources.*`). (§17 / SKILL §6)
17. Missing test for optional-feature combinations of `Attributes()`. (§14.2)

---

## 6. Decision aids

### 6.1 "Do I need a `Delegate`?"
- **Yes** — if the cluster exposes a command handler that may need
  application-specific logic (e.g. `Chime`, `Actions`, `OperationalState`,
  `MicrowaveOvenControl`).
- **No** — if the cluster is a pure data store with spec-defined attribute
  semantics only (e.g. `BasicInformation`, `TemperatureMeasurement`).

### 6.2 "Singleton or multi-instance?"
- **Multi-instance** — if the spec Scope is "Endpoint", or "Node" but the
  device may host it on aggregator endpoints.
- **Singleton** — only if the cluster is truly node-scoped and the codebase
  treats it that way (e.g. `BasicInformation`, `GeneralCommissioning`).

### 6.3 "Is this attribute hardware-fixed?"
- **Yes** → goes in `Config`; no setter. Examples: `MinMeasuredValue`,
  `MaxMeasuredValue`, `Tolerance`, `NumberOfPositions`, `MultiPressMax`,
  `FeatureMap`.
- **No** → exposed via a public `Set<Attr>()` method. Examples:
  `MeasuredValue`, `NodeLabel`, `Breadcrumb`, `IdentifyTime`.

### 6.4 "Constraint failure — which error code?"
- Runtime out-of-range value from the IM → `CHIP_IM_GLOBAL_STATUS(ConstraintError)`.
- Cluster/endpoint not yet initialized → `CHIP_ERROR_INCORRECT_STATE`.
- Cluster not found for endpoint → `CHIP_ERROR_NOT_FOUND`.
- Constructor-time invariant violation → `VerifyOrDie`.

### 6.5 "Where does this code go?"
| Code kind | File |
|---|---|
| Attribute/command logic | `<Name>Cluster.cpp` |
| Config struct + class decl | `<Name>Cluster.h` |
| Delegate interface | `<Name>Delegate.h` (separate file) |
| ZAP integration, registration, Ember defaults | `CodegenIntegration.cpp` |
| Legacy public API shims (`<Name>Server`) | `CodegenIntegration.h/.cpp` |
| Application-facing convenience setters | `CodegenIntegration.cpp` |
| Unit tests | `tests/Test<Name>Cluster.cpp` |

---

## 7. Known patterns to reject even if the user asks

When a user request conflicts with repo policy, push back and link the rule.

- **"Add a setter for every RW attribute"** — no, YAGNI (dos-and-donts §18).
- **"Keep the old `Attributes::X::Set` accessor for backwards compat"** —
  no, compat at that layer is infeasible; put compat in `CodegenIntegration`.
- **"Extract a template base class for all measurement clusters"** — no,
  deferred; type differences make this non-trivial.
- **"Fetch the delegate from a global in `Startup`"** — no, inject via
  `Context`.
- **"Add a path-validity check before the switch for safety"** — no, framework
  guarantees the path exists.

---

## 8. When blocked

If the user's request doesn't match any pattern above, or if the spec clause
is ambiguous:

1. Read the nearest similar merged PR from
   [`code_driven_cluster_conversion_prs.md`](./code_driven_cluster_conversion_prs.md).
2. Read the review thread (cited in
   [`code_driven_cluster_review_comments_raw.md`](./code_driven_cluster_review_comments_raw.md)).
3. If still unclear, **stop and ask the user**. Do not guess on spec semantics,
   privilege levels, or persistence requirements.

---

## 9. Canonical reference cluster implementations

Prefer these as templates (in order of recency and faithfulness to the
current idiom):

| Pattern | Reference cluster | PR |
|---|---|---|
| Simple measurement (nullable value + min/max/tolerance) | `relative-humidity-measurement-server` | #71424 |
| Fixed config + delegate + commands | `actions-server` | #43471 |
| Multi-instance (per-endpoint state) | `closure-dimension-server` | #43720 |
| Singleton (node-scoped) | `basic-information` | #40422 |
| Command-heavy with delegate | `operational-state-server` / `chime-server` | — / #42331 |
| Writable scalar + feature map | `switch-server` | #42968 |
| Runtime-only (no Ember defaults) | `flow-measurement-server` | #71552 |
| Identify/timer-driven | `identify-server` | #41232 |

---

## 10. Self-check before declaring "done"

Go through this list before telling the user the migration is complete.

- [ ] The cluster class lives in `<Name>Cluster.h` / `.cpp`.
- [ ] `CodegenIntegration.cpp` is the **only** file that imports Ember/ZAP
      headers.
- [ ] `Config` is nested, named `Config`, and contains only fixed attributes.
- [ ] No `Init()`, no `mEndpointId`, no `mIsRegistered`.
- [ ] `ReadAttribute`'s default returns `UnsupportedAttribute`;
      `WriteAttribute`'s default delegates to the base.
- [ ] No feature checks inside `ReadAttribute` / `WriteAttribute` /
      `InvokeCommand`.
- [ ] Tests cover every feature combination for `Attributes()` /
      `AcceptedCommands()`.
- [ ] Tests cover null-sentinel rejection for nullable numeric attributes.
- [ ] `zap_regen_all.py` has been run and **every** generated file is
      committed.
- [ ] Cluster is in `config-data.yaml` under `CodeDrivenClusters`, not
      `CommandHandlerInterfaceOnlyClusters`.
- [ ] Cluster directory is in `src/app/clusters/BUILD.gn` `public_deps`.
- [ ] Test path is in `src/BUILD.gn` `chip_test_group("tests")`.
- [ ] Build passes; `ninja check` runs the new tests and they pass.
- [ ] REPL smoke-test succeeds.
- [ ] Flash-size delta is measured and documented in the PR body.
- [ ] Copyright year on modified files includes the **original** year (don't
      reset).

If every box is checked, you're done. If any box is unchecked and you cannot
justify it by pointing at this doc or the dos-and-donts doc, fix it before
finishing.
