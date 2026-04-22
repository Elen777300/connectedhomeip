# Code-Driven Cluster Review Rules (Deduplicated)

Distilled from **532 review comments across 25 code-driven cluster conversion PRs**.
Each rule cites the PR(s) where reviewers raised it. Use this alongside
[`code_driven_cluster_dos_and_donts.md`](./code_driven_cluster_dos_and_donts.md) —
the dos-and-donts document captures the **prescriptive** form; this file groups
the **raw-data** evidence so you can trace a rule back to its source discussions.

See also:
- [`code_driven_cluster_review_comments_raw.md`](./code_driven_cluster_review_comments_raw.md) — full raw review comments
- [`code_driven_cluster_conversion_prs.md`](./code_driven_cluster_conversion_prs.md) — PR index

---

## 1. PR strategy & review hygiene

### 1.1 Split renames, code-moves, and conversion logic into separate PRs
Reviewers repeatedly asked for smaller, reviewable diffs and called out that
"moved" code mixed with "new" logic makes review nearly impossible.
Cited: #41849 ("Would have been nice to have the code moves in a separate PR…
when I was reviewing this, I had to try to match up the removed/added bits
across the file"), #43633 ("This is a lot of code that feels like 'new'
however it is moved from 'logic'. Would it make sense to have an intermediate
'move code PR'…").

### 1.2 Keep `# keep-sorted` lists sorted
`BUILD.gn` include lists, deps lists, etc. must stay sorted for diff-friendly
merges and faster lookup. Cited: #41064.

### 1.3 Preserve original copyright years when renaming / reusing
When renaming a file or reusing a significant chunk of code, keep the original
year range (e.g. `2021-2026`) — don't reset to the current year. Cited: #40422,
#41064, #40788.

---

## 2. Class structure & header design

### 2.1 Don't use `using` for `DataModel::` types in headers
Headers must not pollute includers with `using DataModel::X = …`. Short aliases
for deeply nested cluster-local types are acceptable only adjacent to the type
that needs them, with a comment. Cited: #43423.

### 2.2 Avoid short aliases that hide the real types
Multiple short aliases (`using Foo = Nullable<uint16_t>; using Bar = …`) next
to each other hide the type information reviewers need to evaluate nullability
and width. Cited: #42884 ("The aliases make the code really really hard to read");
also: "As a reviewer my feedback is that this makes the code very hard to
review — the data types are too hidden away in aliases of namespaces."

### 2.3 Nest `Config` inside the cluster class — avoid friend + alias
Put `Config` as a public nested struct in the cluster class. A standalone
`XxxConfig` class with a `friend class` and `using Config = XxxConfig` is
rejected. Cited: #71424, #43423.

### 2.4 Forward declarations inside a cluster header are a smell
A forward declaration usually indicates odd coupling. If truly needed, move it
adjacent to the type that requires it and add a comment. Cited: #43423 ("why do
we need a forward declaration? This may indicate some odd coupling - can we
remove it?").

### 2.5 Place `class Foo` in `Foo.h`
New cluster classes live in their own header named after the class
(`ZoneManagementCluster.h`). The legacy `<name>-server.h` becomes a compat
stub. Cited: #43423.

### 2.6 Include narrow cluster-specific headers — don't `#include cluster-objects.h`
`<app-common/zap-generated/cluster-objects.h>` is "include the world". Prefer
`<clusters/<ClusterName>/Ids.h>`, `<clusters/<ClusterName>/Enums.h>`,
`<clusters/<ClusterName>/Metadata.h>`. Cited: #42331, #43471.

### 2.7 Don't include codegen or `app/util` headers in the cluster `.cpp`
Those are Ember / codegen integration headers. They belong **only** in
`CodegenIntegration.cpp`. Cited: #43423 ("Include is not allowed in code
driven clusters: you are not allowed to include codegen or src/app/util in
most cases").

### 2.8 Don't include `chip::` redundantly inside `namespace chip`
Avoid `chip::Foo` when the surrounding scope is already `namespace chip`.
Cited: #41849 ("Why the various `chip::` prefixes here? This is inside
`namespace chip`").

---

## 3. Constructors & initialization

### 3.1 No constructors with multiple consecutive same-type parameters
A 7-parameter constructor with 3 consecutive integers/booleans is error-prone.
Bundle into a named `Config` struct. Keep the old flat signature only in the
`CodegenIntegration` compat layer. Cited: #43423 ("This constructor has 7
parameters, out of which 3 consecutive are integers. Lets keep that for compat
in CodegenIntegration and have the cluster use a context structure").

### 3.2 Put fixed (spec `F`) attributes in `Config`, not behind setters
`MinMeasuredValue`, `MaxMeasuredValue`, `Tolerance`, `NumberOfPositions`,
`MultiPressMax`, min/max/step temperatures — these are hardware-fixed and
should be set once via `Config` at construction, with no runtime setter. Only
the live sensor reading (e.g. `MeasuredValue`) gets a setter.
Cited: #71424, #42884, #43204, #43394, #42968, #71552, #41954.

### 3.3 Use `WithX()` builder methods for optional attributes with a value
When an optional attribute has an associated value (e.g. `Tolerance`), expose a
builder that sets both the value *and* the presence flag atomically; hide the
raw flag from the public API. Cited: #71424, #42748 ("That `mPreviousIdentifyTime`
was ugly. Fixed like you suggested, much better now thanks"), #42748 feature-
builder ordering: "`WithMinLevel(10).WithLighting()` — the min will not work".
Solution: `VerifyOrDie` on the required feature inside `WithMinLevel`.

### 3.4 Validate configuration at construction — fail fast
Out-of-bounds `minMeasuredValue`, invalid feature combinations
(`AlarmSuppress` without `Visual` or `Audible`), and spec-defined limits
(Tolerance ≤ 2048) must be rejected at construction / Startup, not silently
accepted. Cited: #71424, #43204, #41849.

### 3.5 Name the config struct `Config` — not `StartupConfiguration`
`Config` is the convention across code-driven clusters. Cited: #71424 ("I
wouldn't name this `Startup`. This is the configuration, not just a startup
value").

### 3.6 `const` the feature map when it's immutable after construction
If `features` / `mFeatureMap` never changes after construction, mark it
`const`. Same for other "set once" members. Cited: #43630, #41954.

### 3.7 No `Init()` on new clusters
`Init()` is the old Ember pattern. Code-driven clusters use `Startup()` and
`Shutdown()`. Any `Init()`-style entry point belongs in `CodegenIntegration.cpp`,
not in the cluster. Cited: #43423 ("Init on new clusters is odd ... we should
probably not have this").

---

## 4. Dependency injection & global state

### 4.1 Inject dependencies — don't fetch globals inside the cluster
The cluster must not call `DeviceLayer::Get*` or `Server::GetInstance()` itself.
The caller (main / CodegenIntegration) constructs a `Context` and passes it in.
Cited: #42748 ("Should we inject this from main instead so that we have a
single delegate overall? … This essentially ties the device to globals"),
#71461 ("this partially defeats the purpose of the decoupling refactor").

### 4.2 Prefer references over pointers for required dependencies
Nullable pointers force null-checks scattered throughout. Use `T &` for
required collaborators — `nullptr` is not a valid state. Cited: #41954
("should this be a reference to not allow nullptr?").

### 4.3 Don't store `EndpointId` — use `GetPaths()[0].mEndpointId`
`DefaultServerCluster` already owns the endpoint path. A duplicate
`mEndpointId` member wastes memory and can drift. Cited: #43423, #42331
("could we instead use `mPath.mEndpointId` in the meantime").

### 4.4 Don't track `mIsRegistered` in the cluster
Registry state belongs in `CodegenIntegration.cpp`. If you feel the need to
track it in the cluster, the layering is wrong. Cited: #43423.

### 4.5 Registration code goes in `CodegenIntegration`, not the cluster
`ServerClusterRegistration`, registry calls, `Init()` wrappers for
registration — all belong in `CodegenIntegration.cpp`. Consider
`LazyRegisteredServerCluster` to combine registration with construction.
Cited: #43423, #43085 ("You could use LazyRegisteredServerCluster … that
combines a registration and a constructor").

### 4.6 "Started" ≠ "Registered"; provide a Shutdown/Deinit if order matters
Given the global `CodegenDataModelProvider`, there's no guaranteed shutdown
order — clusters must expose a deterministic deinit API. Cited: #43085.

### 4.7 Use `ClusterIntegration` helper
`src/data-model-providers/codegen/ClusterIntegration.h` saves flash and
improves readability for registration/unregistration. Cited: #40788.

---

## 5. `Startup()` / `Shutdown()`

### 5.1 `DefaultServerCluster::Startup()` already saves context
Don't re-assign `mContext = &context` yourself — call the base first and only
add real init work. Cited: #43471 ("DefaultServerCluster saves the context —
that is all its startup does. We should not need this"), #42331.

### 5.2 Shutdown symmetry — but don't `VerifyOrDie`
If `Shutdown()` wasn't called, log and clean up gracefully. Don't
unconditionally `VerifyOrDie`, because some shutdown callbacks are no-ops in
the current codegen framework. Cited: #42331 ("should we also assume
[Shutdown called] for the sake of consistency"), general best practice from
#42331 reviewers.

### 5.3 Invoke must be on the Matter thread — use `ScheduleWork` from others
Document this in the README when the cluster exposes a setter that may be
called from non-Matter threads. Cited: #41954.

---

## 6. `Attributes()`, `AcceptedCommands()`, `GeneratedCommands()`

### 6.1 `Attributes()` is the gatekeeper — no path-validity checks in ReadAttribute
The framework only calls `ReadAttribute` for paths in `Attributes()`. An
`if (path != kInvalid) …` check before the switch is redundant and wastes
flash. Cited: #43423 ("API contract enforces that Read/Write/Invoke is called
on an existent path — if you have ::Attributes work correctly, we do not need
these extra checks").

### 6.2 No feature checks inside ReadAttribute / WriteAttribute
`Attributes()` already excludes optional attributes behind disabled features.
Don't duplicate the check per-attribute. Cited: #43423 ("Remove feature
checks. Applies throughout — I will stop commenting on these"), #41954.

### 6.3 No feature checks inside `InvokeCommand` — `AcceptedCommands()` is the gatekeeper
Same principle. Cited: #43423, #43720 ("If you have implemented
AcceptedCommands properly, shouldn't the IM already deny and not forward the
command to the cluster?").

### 6.4 Append requires pre-allocation; use `AppendElements` or test the fallback
`ReadOnlyBufferBuilder::Append` fails silently without pre-allocation. Use
`AppendElements` with a pre-counted span, or assert capacity. Cover every
feature combination with unit tests. Cited: #43423 ("Append requires
pre-allocation or it will fail. … Could you make sure that we test this
somehow").

### 6.5 Global attributes are handled by DefaultServerCluster
Don't re-enumerate global attributes (FeatureMap, ClusterRevision, etc.). The
base class emits them. Cited: #43630, #71552.

---

## 7. Writing attributes

### 7.1 Delegate unknown IDs to the base, not `UnsupportedAttribute`
`WriteAttribute` defaults to `return DefaultServerCluster::WriteAttribute(request, decoder)` —
not `return Status::UnsupportedAttribute`. The base handles cascading behavior
(including the read-only case). Cited: #71424, #71552.

### 7.2 Use the `WriteImpl + NotifyAttributeChangedIfSuccess` pattern
```cpp
return NotifyAttributeChangedIfSuccess(request.path.mAttributeId, WriteImpl(request, decoder));
```
One place to call `NotifyAttributeChangedIfSuccess`, not scattered throughout
the switch. Cited: #41064 ("I wonder if we should have a `WriteImpl` and then
just a `NotifyAttributeChangedIfSuccess(..., WriteImpl(...))` to have only
one place").

### 7.3 Use `SetAttributeValue` for simple member-backed writes
Handles the no-op guard and dirty-mark automatically. Reviewers wanted
universal adoption. Cited: #43423, #43633 ("is maybe more standard if we can
use it throughout"), #43720 (note: `SetAttributeValue` didn't initially
support `Nullable<T>` — see #43757 for the follow-up).

### 7.4 If a setter already handles notify+noop, don't double-notify in WriteImpl
`SetBreadCrumb`, `SetAirQuality`, etc. perform both value-change detection and
notification. Don't wrap them in a `WriteImpl` that also calls
`NotifyAttributeChangedIfSuccess`. Cited: #40788 ("SetBreadCrumb handles
notification, so then you do not need a WriteImpl — so you do not double-notify");
#43630.

### 7.5 Don't override `WriteAttribute` at all for read-only clusters
`DefaultServerCluster::WriteAttribute` returns `UnsupportedWrite` already.
Cited: #71424.

### 7.6 Only notify on actual value change
`NotifyAttributeChanged` should only fire when the value differs from the
current one. The base has helpers for this; apply them. Cited: #43085
("We should only notify of attribute change if the value actually changed").

### 7.7 Don't manually call legacy reporting APIs
`Reporting::` calls are the old Ember path. Code-driven clusters use
`NotifyAttributeChanged` / `NotifyAttributeChangedIfSuccess`. Cited: #43471
("Reporting is suspect — we should not be needing this").

---

## 8. Commands

### 8.1 Return `std::nullopt` from `InvokeCommand` for unknown commands
Unknown command is not an error status. Cited: inferred from multiple PRs
plus the default pattern documented in reviewer feedback.

### 8.2 `Handle…` helpers should not be callable for unsupported commands
If `AcceptedCommands` is correct, the IM never forwards an unsupported command.
Consider whether `Handle*` needs to be a public API at all. Cited: #43720
("If listing commands is implemented correctly, this actually violates the
API contract (you are not allowed to call a handle when the command is not
supported ... also should Handle be public?)").

### 8.3 Test commands via `InvokeCommand` / cluster tester — not direct `Handle*`
Direct `Handle*` calls bypass the IM paths. Cited: #43720.

### 8.4 Don't cast explicit Status values — return them directly
`ActionReturnStatus` auto-casts from both `Status::…` and `CHIP_ERROR`.
Cited: #42748, #41232.

---

## 9. Error handling & status codes

### 9.1 `ConstraintError` ≥ `InvalidArgument` for out-of-range values
When the IM maps the error back, `ConstraintError` is clearer than a generic
`Failure` that `InvalidArgument` produces. Cited: #43085, #43204, #42968, #43633.

### 9.2 Prefer `CHIP_ERROR_INCORRECT_STATE` / `CHIP_ERROR_NOT_FOUND` over `INVALID_ARGUMENT`
When signaling "endpoint not initialized" or "cluster not found", be specific
so readers don't interpret `INVALID_ARGUMENT` as "the input value was
invalid". Cited: #71424.

### 9.3 Replace `TEMPORARY_RETURN_IGNORED` with `LogErrorOnFailure` or propagate
Never introduce new `TEMPORARY_RETURN_IGNORED` markers. Cited: #43423
("Should we flip this to a LogErrorOnFailure at least to remove
temporary_return_ignored?"), #43204.

### 9.4 Use `VerifyOrReturnError` / `VerifyOrReturnValue` — not `if (!x) return …`
Cited: #42748 ("use `VerifyOrReturnError` or `VerifyOrReturnValue` for
these"), #43204 ("VerifyOrReturnError(measuredValue.Value() >= minMeasuredValue…)").

### 9.5 Chain with `ReturnErrorOnFailure` instead of nested `if`s
Cited: #42748 ("Lets just use `ReturnErrorOnFailure` everywhere here instead
of this if").

### 9.6 `exit:` is not `error:` — don't move success-path logging there
`SuccessOrExit` takes you through `exit:` on both paths. Put `ChipLogError`
*before* the `SuccessOrExit` that guards the error. Cited: #41064.

### 9.7 Tolerate ZAP default-read failures — don't `VerifyOrDie`
Reading Ember defaults can fail (e.g. endpoint not ready). Use a safe fallback
(null / zero / spec-neutral default), not a die. Cited: #71424 indirectly via
the generalization of reviewer feedback about ZAP range mismatches; general
pattern from multiple PRs.

### 9.8 Fire-and-forget setter calls should be wrapped in `LogErrorOnFailure`
If you're not propagating the return, at least surface failures in logs.
Cited: #43423 ("Should we flip this to a LogErrorOnFailure…"), #43204.

---

## 10. Constants & spec bounds

### 10.1 Use decimal for spec-defined bounds — not hex
The spec is decimal. Code should be too. `9999`, `10000`, `32766` — not
`0x270F`, `0x2710`, `0x7FFE`. Cited: #71424.

### 10.2 Name constants after the attribute they bound, not the range label
`kMinMeasuredValueMax` (upper bound of MinMeasuredValue) beats `kMaxMin`.
Cited: #71424 ("Naming `kMaxMin` and `kMinMax` seems odd really").

### 10.3 Introduce a named constant for any magic number
`2048`, `32766`, `0xFFFE` — every spec bound gets a `constexpr` with a
comment pointing to the spec. Cited: #43204 ("should we have a named constant
for 2048 as well?").

### 10.4 Don't guess types — look up the generated header
Open `zzz_generated/.../clusters/<Name>/Attributes.h` to confirm
`Nullable<uint16_t>` vs raw `uint16_t`. Cited: #42884 (long exchange about
`measuredValue` being `Nullable<uint16_t>` and `LightSensorType` being
`Nullable<LightSensorTypeEnum>`, both misidentified on first pass).

---

## 11. Nullable handling

### 11.1 `ValueOr(fallback)` is cleaner than a manual null check
```cpp
if (measuredValue.ValueOr(0) != 0) { … }
```
Cited: #42884, #42748.

### 11.2 Don't assert on nullable without `.ValueOr()` first
A nullable that could be null will assert in `.Value()`. Cited: #42884.

### 11.3 Don't make an *optional* attribute also nullable unless spec demands it
If the value's absence is meaningful, the attribute itself shouldn't be
present. Cited: #42884 ("Now why do we have a nullable optional attribute?
Why would the attribute not be missing if the value is unknown …").

### 11.4 Test reserved null-sentinel rejection for nullable numeric attributes
`0xFFFF` (for `uint16_t`) etc. should be rejected with `ConstraintError`.
Note: some cluster implementations historically accept it. Cited: #71552
("None of the measurement clusters reject 0xFFFF" — meaning this is a known
inconsistency that new clusters should fix).

---

## 12. Style & formatting

### 12.1 Don't add / don't remove `{}` inconsistently — match the file
`{}` on every `if` is the style in some files; removing unnecessary `{}` was
also requested elsewhere. Read the surrounding code. Cited: #42748 ("nit
throughout: please add `{}` for all ifs"), #43720 ("you can remove a bunch
of `{}` wrappers here"), #40788 ("remove extra `{}` throughout").

### 12.2 No double scoping (extra `{ … }` blocks)
Cited: #43204 ("the double-scoping feels weird throughout. Please remove it").

### 12.3 Prefer `enum class` over `bool` for 2-value choices
`kClient` / `kCommand` reads better than `true` / `false` at the call site,
with equivalent flash. Cited: #41232.

### 12.4 Inline trivial private one-liners to save flash
Cited: #41064 ("This is a private one-liner. Lets inline these directly").

### 12.5 Log consistency — Zcl vs. AppServer categories
Match the surrounding file's log category. Cited: #42886.

### 12.6 Drop non-essential logs on embedded builds
Especially unrecoverable-error logs. Cited: #41954 ("Why log this? It's
likely not recoverable. Suggest avoiding the log on embedded builds").

---

## 13. Dependency reuse / code duplication

### 13.1 Check `DefaultServerCluster` before adding a helper
`NotifyAttributeChanged`, `NotifyAttributeChangedIfSuccess`,
`SetAttributeValue`, `GetPaths()` are already provided. Don't re-implement.
Cited: #43423 ("I think DefaultServerCluster already has this method").

### 13.2 Validation in constructor vs. runtime setter — they're different
Some reviewers asked to factor duplicated bounds checks into a helper; the
author responded that the constructor validates hardware config once while
`SetMeasuredValue` validates each reading at runtime — different contracts.
The rule: don't force a helper if the contract actually differs; explain in a
comment. Cited: #71424.

### 13.3 Don't create base classes / templates prematurely
Proposal to extract a `MeasurementCluster` base for 15 clusters was
reasonably deferred because the type differences (`int16_t` vs `uint16_t`)
make a template base non-trivial. Keep migrations focused; revisit abstraction
after several clusters are converted. Cited: #71424 (jadhavrohit924 proposal,
Elen777300 response).

---

## 14. Testing

### 14.1 Unit-test the cluster directly — use `TestableFooCluster` for protected access
A thin subclass that re-exports protected members beats making them public on
the real class. Cited: general pattern, implicit in multiple PRs.

### 14.2 Cover every feature combination for `Attributes()`/`AcceptedCommands()`
Allocation failures from missing `AppendElements` / `Append` pre-allocation
are silent without tests. Cited: #43423.

### 14.3 Test that `NotifyAttributeChanged` fires on real change, not on no-op
Cited: #43085 ("Check that NotifyAttributeChanged is triggered for both Ssid
and PassphraseSurrogate when ClearNetworkCredentials is called").

### 14.4 Delete "change detector" tests
Tests that hard-code codegen output (e.g. ClusterRevision value) will fail
every time codegen bumps the version. Delete them. Cited: #41232 ("This is a
change detector … lets delete this entire test"). Cross-certification
already covers this space.

### 14.5 Fresh test context per test
`TestServerClusterContext` accumulates events and state; starting fresh each
test avoids coupling. Cited: #41232.

### 14.6 Use a global `kTestEndpointId` constant
Don't declare a per-test constant for the same endpoint id. Cited: #41232.

### 14.7 Test SSID/string-length boundaries (32-byte cap, etc.)
Include on-the-boundary, below, and above for every constrained attribute.
Cited: #43085.

### 14.8 Smoke-test with REPL before writing unit tests
A misconfigured cluster crashes immediately in REPL — saves debugging time.
Cited: general preamble, implicit in review discussion.

---

## 15. `CodegenIntegration` layer

### 15.1 Keep the legacy public API in the compat layer
Preserve `class SwitchServer`, `class XxxServer` for downstream consumers
while the new cluster class goes in `<Name>Cluster.h`. Cited: #42968, #43720.

### 15.2 Expose setters for pre-cluster-creation state via overload
If the app needs to configure values before the cluster is constructed (e.g.
chef test setup), provide the overload in `CodegenIntegration` — not in the
cluster itself. Cited: #42968 ("allow override before cluster is created.
I believe this is what happens here").

### 15.3 Use `emberAfGetClusterServerEndpointIndex` for multi-instance indexing
Array-per-endpoint must be indexed by 0-based cluster-server index, not raw
endpoint ID. Cited: #43720.

### 15.4 Pack per-endpoint state in a struct — not parallel arrays
One `array<ContextStruct, N>` beats three parallel arrays. Cited: #43720
("Instead of separate arrays, how about defining a struct that contains all
that we need and creating a single array?").

### 15.5 Hard-coded feature maps need comments explaining why
When the constructor omits `features` (and `#define` configuration drives
them instead), annotate that the feature map is hard-coded and why. Cited:
#40788.

### 15.6 Only stub plugin callbacks ZAP actually generates
Don't add empty `MatterFooPluginServerInitCallback` unless ZAP declares it —
dead code confuses readers. Cited: general pattern.

---

## 16. Ember cleanup

### 16.1 No `Accessors::Set` — accessors are Ember RAM buffer
Code-driven clusters must not read or write via `Attributes::X::Set`. Store
the value as a member, expose a setter, and notify through
`NotifyAttributeChanged`. Cited: #40788 ("This is NOT allowed in code driven:
accessors are ember RAM buffer").

### 16.2 Don't read "current" values from ZAP in CodegenIntegration
ZAP only holds the defaults; the "current" value lives in the cluster
instance. The getter in Ember was reading the default, not the live value.
Cited: #43204 ("Getter is still there, right? Just not setter. And getter
will not get the 'current' value, just the default one").

### 16.3 Let measured values start null — don't load a ZAP default
"Nothing has been measured yet" is the honest state at boot. `MeasuredValue`
defaults in `.matter` files are usually wrong. Cited: #43204.

### 16.4 Remove Ember references in docs / comments
Reviewer feedback flagged LLM-generated comments that still talked about
"emulating Ember" as irrelevant in new cluster code. Cited: #42331.

### 16.5 Handle invalid ZAP range defaults gracefully
When min ≥ max because both default to 0 in ZAP, null both rather than crash.
Cited: #71424-adjacent pattern; general.

### 16.6 Post-attribute-change callbacks are removed in code-driven
Document this migration in the cluster README — the cluster class now owns
the reaction to writes. Cited: #41954 ("Updated Readme with some notes about
global attribute getters/setters being removed and also post attribute change
callbacks").

---

## 17. Build / ZAP configuration

### 17.1 Every `.h` must appear in a build file
`.h` files in the cluster directory must be listed in `BUILD.gn` (core) or
`app_config_dependent_sources.cmake` / `.gni` (CodegenIntegration). Otherwise
platforms silently skip them. Cited: general review pattern.

### 17.2 `config-data.yaml` — add to `CodeDrivenClusters`, remove from legacy lists
Remove the cluster from `CommandHandlerInterfaceOnlyClusters` if it was
listed there. Cited: #41064 ("Forgot to add Access Control to
`CommandHandlerInterfaceOnlyClusters` … I've updated `config-data.yaml`").

### 17.3 Update both `zcl.json` and `zcl-with-test-extensions.json`
Missing `attributeAccessInterfaceAttributes` in the test-extensions file
causes test-only failures that are hard to diagnose. Cited: general pattern.

### 17.4 Commit everything `zap_regen_all.py` produces
`.matter`, `endpoint_config`, `cluster-callbacks.cpp`,
`CodeDrivenInitShutdown.cpp`, C++ accessors — partial regen breaks other
apps. Cited: general pattern.

### 17.5 Register the cluster dir in `src/app/clusters/BUILD.gn`
Add to the `# keep-sorted` `public_deps` list. Cited: general pattern.

### 17.6 Register tests in `src/BUILD.gn`
Without adding the tests path under `chip_test_group("tests")`, CI never
runs the tests. Cited: general pattern.

---

## 18. Common rejected / superseded suggestions

Things reviewers **asked for** but the author pushed back on with justification
— worth knowing so you don't repeat the discussion:

- **"Add setters for every RW attribute"**: Rejected when the attribute is
  only ever modified via Matter writes. YAGNI until an app actually needs it.
  Cited: #42634 (`OnTime`, `OffWaitTime`, `StartUpOnOff` setters omitted).
- **"Keep backward-compat for the old attribute Set accessors"**: Rejected
  when the Set accessor wrote to the Ember attribute store that the code-driven
  cluster no longer reads — compat there is infeasible. Compat belongs in
  CodegenIntegration (state-read path) instead. Cited: #71424.
- **"Extract a template base class for all measurement clusters"**: Deferred —
  type differences (`int16_t` temperature, `uint16_t` humidity, enum light
  sensor) make a template base non-trivial. Revisit after more clusters convert.
  Cited: #71424.

---

## Notes on coverage

- 14 human reviewers contributed substantive comments across the sample set,
  led by andy31415 (205), soares-sergio (69), zaid-google (31), arielsz71 (28),
  bzbarsky-apple (25), lpbeliveau-silabs (23).
- Bot noise (gemini-code-assist, github-actions, restyled-io) was filtered out
  before deduplication.
- PRs sampled are listed in
  [`code_driven_cluster_review_comments_raw.md`](./code_driven_cluster_review_comments_raw.md).
