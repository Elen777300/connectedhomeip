# Code-Driven Cluster: Dos and Don'ts

Actionable rules for implementing and migrating clusters to the code-driven
pattern. Each rule cites the PR(s) where reviewers raised it. For the full raw
review corpus see
[`code_driven_cluster_review_comments_raw.md`](./code_driven_cluster_review_comments_raw.md);
for the PR index see
[`code_driven_cluster_conversion_prs.md`](./code_driven_cluster_conversion_prs.md).
When in doubt, also read [Writing Clusters](./writing_clusters.md) and
[Migrating Ember Clusters](./migrating_ember_cluster_to_code_driven.md).

PRs cited here are GitHub pull-request numbers on
`project-chip/connectedhomeip`; e.g. `#71424`.

---

## PR / Review Strategy

### Do split migrations into multiple PRs

Structure your migration as at least three PRs:

| PR | Contents | Why it can be fast-tracked |
|----|----------|---------------------------|
| 1 | File renames and moves **only** | `git diff --color-moved` makes it trivially reviewable |
| 2 | Code reordering / anonymous-namespace cleanup **only** | No logic changes to evaluate |
| 3 | The actual migration logic | Small and focused after PRs 1 & 2 |

Never mix renames with logic changes — reviewers cannot tell what changed.
(Cited: #41849 "Would have been nice to have the code moves in a separate PR…
when I was reviewing this, I had to try to match up the removed/added bits
across the file"; #43633 "This is a lot of code that feels like 'new' however
it is moved from 'logic'. Would it make sense to have an intermediate 'move
code PR'…".)

### Do keep the old public header as a backward-compat stub

When renaming or splitting a header, leave the original file as a one-line
forwarding include:

```cpp
// backward-compat stub — do not add new content here
#include "CodegenIntegration.h"
```

Existing consumers keep compiling without modification.
(Cited: #43720, #42968 — reviewers asked to preserve `class XxxServer` for
downstream consumers.)

### Do keep `# keep-sorted` lists sorted

`public_deps`, `sources`, include lists — faster lookup, fewer merge
conflicts. (Cited: #41064 "We should really keep this list sorted: faster
lookup and less chances of merge conflicts".)

### Do preserve original copyright years when renaming or reusing code

Keep the original range (e.g. `2021-2026`) — don't reset to just the current
year. (Cited: #40422, #41064, #40788 "I think we are modifying and re-using a
lot of existing code, so if we update the license we should keep the old year
at least".)

---

## Header Design

### Don't add `using` aliases for `DataModel::` types in headers

```cpp
// Bad — pollutes every includer's namespace
using ActionReturnStatus = DataModel::ActionReturnStatus;

// Good — use the full name at the call site
DataModel::ActionReturnStatus MyCluster::ReadAttribute(...) { ... }
```

Short `using` aliases for deeply-nested cluster-local types (e.g.
`using Foo = Clusters::Bar::Structs::Foo::Type`) are acceptable when they
appear just before the type that needs them, with a comment explaining why.
Never alias `DataModel::` names. (Cited: #43423 "we try to minimize alternate
names for the same thing".)

### Don't stack short aliases that hide the real types

Multiple one-letter or two-letter aliases next to each other make it
impossible to tell at a glance whether a value is `Nullable<uint16_t>`,
`uint16_t`, or an enum. Reviewers cannot evaluate correctness without
expanding them. (Cited: #42884 "The aliases make the code really really hard
to read" / "As a reviewer my feedback is that this makes the code very hard
to review — the data types are too hidden away in aliases of namespaces".)

### Do nest `Config` inside the cluster class

Place the `Config` struct as a public nested type directly inside the cluster
class, not as a standalone class outside it. This avoids a forward declaration,
a `friend class`, and a `using` alias — all of which are unnecessary.

```cpp
// Good — nested, matches OccupancySensing pattern
class RelativeHumidityMeasurementCluster : public DefaultServerCluster
{
public:
    struct Config { ... };
    explicit RelativeHumidityMeasurementCluster(EndpointId endpointId);
    RelativeHumidityMeasurementCluster(EndpointId endpointId, const Config & config);
    ...
};

// Bad — standalone class with friend + using alias
class RelativeHumidityMeasurementConfig { ... };
class RelativeHumidityMeasurementCluster : public DefaultServerCluster
{
    friend class RelativeHumidityMeasurementConfig;
    using Config = RelativeHumidityMeasurementConfig;
    ...
};
```

Name it `Config` — not `StartupConfiguration` — to match the convention used
across other code-driven clusters. (Cited: #71424 "I wouldn't name this
`Startup`. This is the configuration, not just a startup value"; #43423.)

### Don't add unnecessary forward declarations

A forward declaration of the cluster class inside its own header indicates odd
coupling. If a forward declaration is truly needed, place it immediately before
the type that requires it and add a comment explaining the dependency.
(Cited: #43423 "why do we need a forward declaration? This may indicate some
odd coupling — can we remove it?".)

### Place `class Foo` in `Foo.h`

The new cluster class (`ZoneManagementCluster`, `ActionsCluster`, etc.) must
live in its own header named after the class. The legacy header
(`<name>-server.h`) becomes a compat stub that includes the new header.
(Cited: #43423.)

### Don't include `DataModel`, `codegen`, or `app/util` headers in the cluster `.cpp`

```cpp
// Bad — these are codegen/Ember headers; they belong only in CodegenIntegration.cpp
#include <data-model-providers/codegen/CodegenDataModelProvider.h>
#include <app/util/util.h>

// Good — cluster implementation only needs cluster-specific and server-cluster headers
#include "ZoneManagementCluster.h"
#include <app/server-cluster/AttributeListBuilder.h>
#include <app/persistence/AttributePersistence.h>
#include <clusters/ZoneManagement/Metadata.h>
```

(Cited: #43423 "Include is not allowed in code driven clusters: you are not
allowed to include codegen or src/app/util in most cases (the latter one is
ember)".)

### Don't write `chip::Foo` inside `namespace chip`

Redundant qualification hurts readability and is a sign of copy-paste from
another translation unit. (Cited: #41849 "Why the various `chip::` prefixes
here? This is inside `namespace chip`".)

---

## Class Structure

### Do inherit from `DefaultServerCluster`

```cpp
// Good
class ZoneManagementCluster : public DefaultServerCluster { ... };

// Bad — old Ember pattern
class ZoneMgmtServer : public AttributeAccessInterface, public CommandHandlerInterface { ... };
```

### Don't add an `Init()` method to the cluster class

`Init()` is the old Ember pattern. Code-driven clusters use `Startup()` and
`Shutdown()`. If you need an `Init()`-style entry point for the compat layer,
put it in `CodegenIntegration.cpp`, not in the cluster class itself.
(Cited: #43423 "Init on new clusters is odd ... we should probably not have
this".)

### Don't store `EndpointId` as a member

`DefaultServerCluster` already owns the endpoint via its path. Use
`GetPaths()[0].mEndpointId` or the path from the request. A duplicate
`mEndpointId` member wastes memory and can drift out of sync.
(Cited: #43423, #42331 "could we instead use `mPath.mEndpointId` in the
meantime".)

### Don't add `mIsRegistered` to the cluster class

Registration state belongs in `CodegenIntegration.cpp`, where registration
actually happens. If you feel the need to track this in the cluster, it is a
sign that registry logic has leaked into the wrong layer. (Cited: #43423.)

### Don't add `ServerClusterRegistration` or registry calls to the cluster class

```cpp
// Bad — registry logic in the cluster class
CHIP_ERROR ZoneManagementCluster::Init()
{
    ReturnErrorOnFailure(CodegenDataModelProvider::Instance().Registry().Register(mRegistration));
    mIsRegistered = true;
    return CHIP_NO_ERROR;
}

// Good — registry lives in CodegenIntegration.cpp
CHIP_ERROR ActionsServer::Init()
{
    ReturnErrorOnFailure(CodegenDataModelProvider::Instance().Registry().Register(mCluster.Registration()));
    mRegistered = true;
    return CHIP_NO_ERROR;
}
```

The cluster class exposes `Startup()`/`Shutdown()`; `CodegenIntegration.cpp`
calls those and handles registration/unregistration. Consider
`LazyRegisteredServerCluster` to combine registration with construction.
(Cited: #43423, #43085 "you could use `LazyRegisteredServerCluster` — that
combines a registration and a constructor as well", #40788 "use
`ClusterIntegration.h` to save some minor flash and readability for
registration/unregistration".)

### Don't re-implement what `DefaultServerCluster` already provides

Before adding a new helper method, check `DefaultServerCluster.h`. The base
class already provides:

- `NotifyAttributeChanged(AttributeId)`
- `NotifyAttributeChangedIfSuccess(AttributeId, ActionReturnStatus)`
- `SetAttributeValue(member, newValue, AttributeId)` — handles no-op guard and
  dirty marking
- `GetPaths()` — provides the endpoint/cluster path

(Cited: #43423 "I think DefaultServerCluster already has this method"; #43085
"`NotifyAttributeChanged` already checks that context != nullptr, so no need
to repeat that".)

---

## Constructor & Initialization

### Don't use constructors with multiple consecutive parameters of the same type

A constructor with 7 parameters where 3+ consecutive ones are integers or
booleans is error-prone and unreadable.

```cpp
// Bad
ZoneManagementCluster(Delegate & delegate, EndpointId endpointId,
                      BitFlags<Feature> features, uint8_t maxUserDefinedZones,
                      uint8_t maxZones, uint8_t sensitivityMax, ...);

// Good — bundle config into a named struct
struct Config {
    BitFlags<Feature> features;
    uint8_t maxUserDefinedZones;
    uint8_t maxZones;
    uint8_t sensitivityMax;
};
ZoneManagementCluster(EndpointId endpointId, Delegate & delegate, const Config & config);
```

Keep the old flat-parameter signature in the `CodegenIntegration` compat layer
only. (Cited: #43423 "This constructor has 7 parameters, out of which 3
consecutive are integers. Lets keep that for compat in CodegenIntegration
and have the cluster use a context structure".)

### Do validate constructor constraints immediately

If the cluster receives invalid configuration (e.g. `minValue > maxValue`,
`AlarmSuppress` without either `Visual` or `Audible`, `Tolerance > 2048`),
fail in `Startup()` (or `VerifyOrDie` in the constructor for hardware-fixed
invariants) before any state is committed. Do not defer validation to first
use. (Cited: #71424 "Tolerance is capped at 2048, needs to be checked too" /
"Should we validate the values here? otherwise this would end up allowing the
app to configure incorrect/out of bounds values"; #43204; #41849 "Shouldn't
we just fail startup if configured with AlarmSuppress but not Visual or
Audible".)

### Do use `WithX()` builder methods for optional attributes that have an associated value

When an optional attribute has a value (e.g. `Tolerance`), hide the presence
flag from the public API and expose a builder method that sets both atomically.
This prevents the user from setting a value without enabling the attribute, or
enabling it with a forgotten value.

```cpp
// Good — one call sets value + presence flag together
struct Config
{
    Config & WithTolerance(uint16_t value)
    {
        mTolerance = value;
        mOptionalAttributeSet.Set<Tolerance::Id>();
        return *this;
    }
private:
    OptionalAttributeSet mOptionalAttributeSet;
    uint16_t mTolerance = 0;
};

// Bad — two separate fields, easy to forget one
struct Config
{
    uint16_t mTolerance = 0;
    OptionalAttributeSet mOptionalAttributeSet; // user must remember to set this too
};
```

Beware ordering traps in fluent builders: `WithMinLevel(10).WithLighting()`
can silently drop `MinLevel` if it requires the Lighting feature. Solve this
by having `WithMinLevel` `VerifyOrDie` that the feature is already set.
(Cited: #71424; #42748 "This is an ordering thing though, as
`WithMinLevel(10).WithLighting()` the min will not work. … could we instead
just VerifyOrDie on having the lighting feature".)

### Do put hardware-fixed attributes in `Config` — don't add runtime setters for them

Attributes like `MinMeasuredValue`, `MaxMeasuredValue`, `Tolerance`,
`NumberOfPositions`, `MultiPressMax`, `MinSetpointTemperature` — these are
hardware characteristics. Set them once via `Config` at construction. Only the
actual sensor reading (e.g. `MeasuredValue`) needs a runtime `Set` method.

```cpp
// Good — fixed attributes in Config, only reading has a setter
RelativeHumidityMeasurementCluster cluster(endpointId, config); // sets min/max/tolerance
cluster.SetMeasuredValue(newReading);                            // updates the live reading

// Bad — runtime setters for fixed attributes
cluster.SetMinMeasuredValue(0);     // these should not exist
cluster.SetMaxMeasuredValue(10000);
```

(Cited: #71424, #42884, #43204, #43394, #42968, #71552 "MinMeasuredValue and
MaxMeasuredValue are fixed, why do we need setter?"; #41954 — the reverse of
the old SetOccupied/UnoccupiedDelay globals being removed.)

### Do mark immutable members `const`

If the feature map, hold-time-limits, min/max/step, or similar never change
after construction, declare the member `const`. (Cited: #43630 "could the
features be const?"; #41954 "should some of these be const? I don't think we
allow feature map modification past creation".)

### Do decide singleton vs. multi-instance before writing code

- If the cluster spec says "Scope: Node" but multiple aggregator endpoints are
  valid, allow multiple instances and track `sInstanceCount` for diagnostics.
- If the cluster is truly endpoint-scoped, do not use a global singleton.

---

## Dependency Injection

### Do use a `Context` struct with references for required dependencies

```cpp
// Bad — nullable pointer, null-checks scattered everywhere
MyCluster(Delegate * delegate);  // What if nullptr?

// Good — reference guarantees non-null at compile time
struct Context {
    Delegate & delegate;
    DeviceLayer::DeviceInstanceInfoProvider & deviceInfoProvider;
};
MyCluster(EndpointId endpointId, const Context & context);
```

(Cited: #41954 "should this be a reference to not allow nullptr?".)

### Do inject dependencies from `main` / setup code — not inside the cluster

The cluster must not call global singletons internally:

```cpp
// Bad — cluster fetches its own dependency
CHIP_ERROR MyCluster::Startup(ServerClusterContext & ctx)
{
    mProvider = DeviceLayer::GetDeviceInstanceInfoProvider(); // Don't do this
    ...
}

// Good — caller injects it
// In application main.cpp:
MyCluster::Context ctx{ .delegate = gDelegate,
                        .deviceInfoProvider = *DeviceLayer::GetDeviceInstanceInfoProvider() };
MyCluster cluster(kEndpointId, ctx);
```

The same principle applies inside unit tests: populating a `Context` from
`Server::GetInstance()` inside the test partially defeats the decoupling.
(Cited: #42748 "Should we inject this from main instead so that we have a
single delegate overall?"; #71461 "The unit test continues to rely on global
singletons to populate the `Context` struct. This partially defeats the
purpose of the decoupling refactor".)

### Don't use `static` inside anonymous namespaces

`namespace { }` already gives internal linkage. `static` is redundant.

```cpp
namespace {
static BasicInfoDelegate gDelegate; // Bad — static is redundant
BasicInfoDelegate gDelegate;        // Good
} // namespace
```

### Do expose a Shutdown/Deinit path for globally-registered clusters

Because `CodegenDataModelProvider` is a global, there is no guaranteed
destruction order. If your cluster holds state that outlives a registration
cycle, expose an explicit deinit API. (Cited: #43085 "The server may be not
started but still registered. Given that the CodegenDataModelProvider is a
global, is there any guarantee of ordering here? Maybe we should provide a
Shutdown/Deinit API here for the caller".)

---

## `Startup()` / `Shutdown()`

### Do call `DefaultServerCluster::Startup()` first — don't re-implement it

```cpp
// Bad — manually storing context that the base class already handles
CHIP_ERROR MyCluster::Startup(ServerClusterContext & context)
{
    mContext = &context;  // DefaultServerCluster already does this
    return CHIP_NO_ERROR;
}

// Good
CHIP_ERROR MyCluster::Startup(ServerClusterContext & context)
{
    ReturnErrorOnFailure(DefaultServerCluster::Startup(context));
    // Only real initialization work goes here
    LoadPersistentAttributes();
    return CHIP_NO_ERROR;
}
```

(Cited: #43471 "DefaultServerCluster saves the context — that is all its
startup does. We should not need this".)

### Do call `Shutdown()` explicitly before destroying the cluster

The destructor must not silently clean up. If `Shutdown()` was not called,
log an error and clean up gracefully — do not `VerifyOrDie` unconditionally,
as some shutdown callbacks may be no-ops in the current codegen framework.
(Cited: #42331.)

### Do use `ScheduleWork` when calling setters from non-Matter threads

Mutating cluster state must happen on the Matter thread. If your application
can trigger a setter from a sensor-sampling thread, wrap the call with
`DeviceLayer::PlatformMgr().ScheduleWork(...)` and document this in the
cluster README. (Cited: #41954 "This must be called on the Matter context,
so a ScheduleWork must be used if done from a different application thread.
This is no different than other times but it's good to mention it".)

---

## `Attributes()`, `AcceptedCommands()`, `GeneratedCommands()`

### Don't add path-validity checks before the `ReadAttribute` switch

The framework only calls `ReadAttribute` for paths that are in the `Attributes()`
list. An upfront check before the switch is redundant and wastes flash.

```cpp
// Bad — redundant check before switch
DataModel::ActionReturnStatus FooCluster::ReadAttribute(...)
{
    VerifyOrReturnError(request.path.mAttributeId != kInvalidAttributeId,
                        Status::UnsupportedAttribute); // never needed
    switch (request.path.mAttributeId) { ... }
}

// Good — switch directly
DataModel::ActionReturnStatus FooCluster::ReadAttribute(...)
{
    switch (request.path.mAttributeId)
    {
    case MeasuredValue::Id:
        return encoder.Encode(mMeasuredValue);
    default:
        return Protocols::InteractionModel::Status::UnsupportedAttribute;
    }
}
```

(Cited: #43423 "API contract enforces that Read/Write/Invoke is called on an
existent path — if you have ::Attributes work correctly, we do not need these
extra checks".)

### Don't add redundant feature checks inside `ReadAttribute`/`WriteAttribute`

The framework guarantees these methods are only called for paths declared in
`Attributes()`. If `Attributes()` correctly gates optional attributes behind
feature-flag checks, the per-attribute feature check inside `ReadAttribute` is
redundant, wastes flash, and adds noise.

```cpp
// Bad — redundant check
case Sensitivity::Id:
    VerifyOrReturnError(HasFeature(Feature::kPerZoneSensitivity),
                        Status::UnsupportedAttribute);
    return encoder.Encode(mSensitivity);

// Good — no check needed; Attributes() already excludes this for non-supported features
case Sensitivity::Id:
    return encoder.Encode(mSensitivity);
```

(Cited: #43423 "Remove feature checks. Applies throughout — I will stop
commenting on these"; #41954.)

### Don't add feature checks inside `InvokeCommand` — `AcceptedCommands()` is the gatekeeper

```cpp
// Bad — redundant feature check in InvokeCommand
case Commands::CreateTwoDCartesianZone::Id:
    VerifyOrReturnValue(HasFeature(Feature::kUserDefined), ...);
    ...

// Good — AcceptedCommands() already excludes unsupported commands
case Commands::CreateTwoDCartesianZone::Id:
    HandleCreateTwoDCartesianZone(request, handler);
    break;
```

(Cited: #43423 "Marker for entire invoke: no need to check feature maps, we
already should check them in `AcceptedCommands`"; #43720 "If you have
implemented AcceptedCommands properly, shouldn't the IM already deny and not
forward the command to the cluster?".)

### Do append mandatory attributes unconditionally; guard optional ones

```cpp
CHIP_ERROR MyCluster::Attributes(const ConcreteClusterPath & path,
                                  ReadOnlyBufferBuilder<DataModel::AttributeEntry> & builder)
{
    // Mandatory: always included
    ReturnErrorOnFailure(builder.ReferenceExistingBuffer(kMandatoryAttributes));

    // Optional: only when feature is active
    if (HasFeature(Feature::kUserDefined)) {
        ReturnErrorOnFailure(builder.Append(Attributes::MaxUserDefinedZones::kMetadataEntry));
    }
    return CHIP_NO_ERROR;
}
```

Global attributes (`FeatureMap`, `ClusterRevision`) are handled by
`DefaultServerCluster`; don't re-enumerate them. (Cited: #43630, #71552.)

### Do pre-allocate before using `Append` in `AcceptedCommands`/`GeneratedCommands`

`ReadOnlyBufferBuilder::Append` fails if the buffer has not been pre-allocated.
Use `AppendElements` with a pre-counted span, or ensure sufficient capacity
before calling `Append`. Write unit tests that cover every feature combination
— this is how allocation bugs survive until integration testing. (Cited:
#43423 "Append requires pre-allocation or it will fail. … Could you make sure
that we test this somehow".)

---

## Writing Attributes

### Do delegate unknown attribute IDs to the base in `WriteAttribute`

`WriteAttribute` is the **opposite** of `ReadAttribute` for the default case:
return `DefaultServerCluster::WriteAttribute(request, decoder)` for unknown
IDs, not `UnsupportedAttribute` directly.

```cpp
DataModel::ActionReturnStatus FooCluster::WriteAttribute(
    const DataModel::WriteAttributeRequest & request, AttributeValueDecoder & decoder)
{
    switch (request.path.mAttributeId)
    {
    case WritableAttr::Id: {
        uint16_t value{};
        ReturnErrorOnFailure(decoder.Decode(value));
        return SetWritableAttr(value);
    }
    default:
        return DefaultServerCluster::WriteAttribute(request, decoder); // not UnsupportedAttribute
    }
}
```

(Cited: #71424, #71552.)

### Do use `NotifyAttributeChangedIfSuccess` for the write return path

Centralize the "validate → write → notify" flow in a single wrapper so
notification is never forgotten:

```cpp
DataModel::ActionReturnStatus MyCluster::WriteAttribute(
    const DataModel::WriteAttributeRequest & request, AttributeValueDecoder & decoder)
{
    return NotifyAttributeChangedIfSuccess(request.path.mAttributeId, WriteImpl(request, decoder));
}
```

(Cited: #41064 "I wonder if we should have a `WriteImpl` and then just a
`NotifyAttributeChangedIfSuccess(..., WriteImpl(...))` to have only one place
where we call NotifyAttributeChangedIfSuccess".)

### Do use `SetAttributeValue` for simple member-backed attribute writes

```cpp
// Good — handles no-op guard and dirty marking automatically
if (SetAttributeValue(mSensitivity, newSensitivity, Attributes::Sensitivity::Id))
{
    return Status::Success; // value unchanged, no-op
}
// Persist and notify
```

(Cited: #43423, #43633 "is maybe more standard if we can use it throughout";
#43720 — note: `SetAttributeValue` initially did not support `Nullable<T>`;
see the follow-up in #71543 / issue #43757 if you hit that.)

### Don't double-notify: if a `Set…` helper already notifies, don't wrap it in `WriteImpl`

Some setters (e.g. `SetBreadCrumb`, `SetAirQuality`) already perform
value-change detection and `NotifyAttributeChanged`. Wrapping them in a
`WriteImpl` that the outer `WriteAttribute` then sends through
`NotifyAttributeChangedIfSuccess` causes a redundant second notification.
Either call the setter directly from `WriteAttribute` (and return its status),
or remove the notify from the setter. (Cited: #40788 "SetBreadCrumb handles
notification, so then you do not need a WriteImpl — so you do not double-
notify"; #43630 "DefaultServerCluster base class handles global attributes".)

### Do only notify on actual value change

`NotifyAttributeChanged` should only fire when the new value differs from the
old one. For raw member writes use `SetAttributeValue` (which performs the
check); for more complex types do an explicit equality check first.
(Cited: #43085 "We should only notify of attribute change if the value
actually changed").

### Don't override `WriteAttribute` for read-only clusters

`DefaultServerCluster::WriteAttribute` already returns `UnsupportedWrite`. If
your cluster has no writable attributes, do not override `WriteAttribute` at
all — the base class handles it correctly. (Cited: #71424.)

### Don't call legacy `Reporting::` APIs

That is the old Ember reporting path. Code-driven clusters use
`NotifyAttributeChanged` or `NotifyAttributeChangedIfSuccess` from
`DefaultServerCluster`. (Cited: #43471 "Reporting is suspect — we should not
be needing this".)

---

## Commands

### Do return `std::nullopt` for unknown commands in `InvokeCommand`

```cpp
std::optional<DataModel::ActionReturnStatus> FooCluster::InvokeCommand(
    const DataModel::InvokeRequest & request, chip::TLV::TLVReader & input, CommandHandler * handler)
{
    switch (request.path.mCommandId)
    {
    case Foo::Commands::DoSomething::Id: {
        Foo::Commands::DoSomething::DecodableType req;
        ReturnErrorOnFailure(DataModel::Decode(input, req));
        return HandleDoSomething(req, handler);
    }
    default:
        return std::nullopt; // unknown command — not an error status
    }
}
```

### Don't return casts of `Status::…` — let `ActionReturnStatus` auto-cast

```cpp
// Bad — explicit cast obscures intent and produces worse code
return static_cast<DataModel::ActionReturnStatus>(Status::UnsupportedAttribute);

// Good — ActionReturnStatus constructs from both Status and CHIP_ERROR
return Status::UnsupportedAttribute;
```

(Cited: #41232 "no need for the cast. Can return and it will auto-cast";
#42748 "ActionReturnStatus auto-casts from both status and chip-error and
such".)

### Don't let `Handle…` be callable for commands not in `AcceptedCommands`

If command listing is correct, `Handle*` is only reached via the IM path — so
the API contract already guarantees the command is supported. Extra guards
inside `Handle*` are redundant. Consider whether `Handle*` needs to be
`public` at all — a `protected` method (with a `TestableFooCluster` subclass
for tests) usually suffices. (Cited: #43720 "If listing commands is
implemented correctly, this actually violates the API contract (you are not
allowed to call a handle when the command is not supported … also should
Handle be public?)".)

### Do test commands via `InvokeCommand` / `clusterTester`, not direct `Handle*`

Direct calls bypass the IM path that the production code will take. Tests
that hit `Handle*` directly can mask real routing bugs. (Cited: #43720.)

---

## Events

### Do emit events via `eventsGenerator`

```cpp
Foo::Events::StateChanged::Type event{ /* fields */ };
mContext->interactionContext.eventsGenerator.GenerateEvent(event, mPath.mEndpointId);
```

Override `EventInfo` only when non-default read privileges are needed for an
event. For standard events, the base class handles privilege correctly.

---

## CodegenIntegration

### Do keep the legacy public API in the compat layer

When a cluster exposes classes like `SwitchServer`, `ChimeServer`,
`ClosureDimensionServer`, downstream consumers may depend on them. Keep them
live in `CodegenIntegration` as thin wrappers that forward to the new
cluster class; don't ask every caller to migrate simultaneously. (Cited:
#42968 "should we preserve the old API interface of `class SwitchServer`?
that way we make life easier for people upgrading"; #43720 "can we preserve
previous API and have mInterface expose methods?".)

### Do expose pre-construction setters via a compat overload

Some apps need to configure values *before* the cluster is instantiated
(e.g. chef test setup). Provide that overload in `CodegenIntegration` rather
than forcing the cluster constructor itself to accept them. (Cited: #42968
"allow override before cluster is created. I believe this is what happens
here".)

### Do use `emberAfGetClusterServerEndpointIndex` for multi-instance indexing

Per-endpoint state arrays must be indexed by the 0-based cluster-server
index, not the raw endpoint ID — otherwise the index space is
endpoint-sparse and wastes memory. (Cited: #43720 "These should not be
endpointID — we should use the ember function to convert endpointid to
0-based index".)

### Do pack per-endpoint state in one struct, not parallel arrays

Instead of `sFooValues[]`, `sBarFlags[]`, `sBazContexts[]`, use one
`array<EndpointContext, N>`. (Cited: #43720 "Instead of separate arrays, how
about defining a struct that contains all that we need and creating a single
array?".)

### Do comment why a feature map is hard-coded

If the constructor omits `features` because `#define`s or ZAP config drives
them, add a comment saying so. Readers otherwise wonder where the feature
set came from. (Cited: #40788 "This is both define AND feature map
dependent. At the same time the constructor only takes the optional
attributes and not T&C define … a comment should explain that we hard code
the feature map".)

### Do tolerate failure when reading Ember attribute defaults

Read ZAP-configured defaults with `Get()`, but use a safe fallback (null, zero,
or a spec-neutral default) if the read fails — never `VerifyOrDie` on it.

```cpp
FooCluster::Config config;
if (Foo::Attributes::MinMeasuredValue::Get(endpointId, &config.minMeasuredValue)
    != Protocols::InteractionModel::Status::Success)
{
    config.minMeasuredValue.SetNull(); // safe fallback
}
```

### Do handle invalid ZAP range defaults gracefully — null them out, don't die

When both `minMeasuredValue` and `maxMeasuredValue` are read from ZAP and form
an invalid range (e.g. `min >= max` because both default to 0 in a new ZAP
config), treat both as null rather than crashing.

```cpp
if (!config.minMeasuredValue.IsNull() && !config.maxMeasuredValue.IsNull() &&
    config.maxMeasuredValue.Value() < config.minMeasuredValue.Value() + 1)
{
    config.minMeasuredValue.SetNull();
    config.maxMeasuredValue.SetNull();
}
```

### Don't read the "current" value of an attribute from ZAP

ZAP holds only defaults. The live value lives in the cluster instance. Old
Ember accessors returned the default, not the live value — don't carry that
confusion forward. If a ZAP-configured starting value is genuinely needed,
document it. (Cited: #43204 "Getter is still there, right? Just not setter.
And getter will not get the 'current' value, just the default one".)

### Don't add empty plugin callbacks unless ZAP declares them

Only stub out `MatterFooPluginServerInitCallback` / `ShutdownCallback` if ZAP
actually generates them. Adding empty stubs for callbacks ZAP doesn't declare
creates dead code and confuses readers.

### Do ensure every header file appears in a build file

Every `.h` file in the cluster directory must be listed in either `BUILD.gn`
(for core files) or `app_config_dependent_sources.cmake`/`.gni` (for
CodegenIntegration). An unlisted header is a review red flag — it won't be
compiled on all platforms and may silently drift out of sync.

### Do use `ClusterIntegration` helper for registration plumbing

`src/data-model-providers/codegen/ClusterIntegration.h` saves some minor
flash and cleans up the Register/Unregister boilerplate. (Cited: #40788.)

---

## Includes

### Do include narrow, cluster-specific headers

```cpp
// Good — only what is needed
#include <clusters/Actions/Ids.h>
#include <clusters/Actions/Enums.h>
#include <clusters/Actions/Metadata.h>

// Bad — pulls in everything
#include <app-common/zap-generated/cluster-objects.h>
```

(Cited: #42331 "could we include just the `<clusters/Chime/…>` headers to be
more specific? cluster-objects is a 'include the world'"; #43471 "could we
include the smaller actions cluster specific structs/enums/commands etc
headers?".)

### Do look up C++ types from generated headers — never guess

Open `zzz_generated/.../clusters/<ClusterName>/Attributes.h` (or the
equivalent generated header) to find exact types, e.g.
`DataModel::Nullable<uint16_t>`. Use `Metadata.h` for constants like
`kMandatoryMetadataEntry`. Do not assume types from attribute names alone.
(Cited: #42884 — extended exchange about `measuredValue` being
`Nullable<uint16_t>` and `LightSensorType` being `Nullable<LightSensorTypeEnum>`,
both misidentified on first pass.)

---

## Constants

### Do use decimal for spec-defined bounds — not hex

The spec expresses attribute bounds as decimal numbers. Use decimal in code so
reviewers can cross-check against the spec without converting.

```cpp
// Good
constexpr uint16_t kMinMeasuredValueMax = 9999;
constexpr uint16_t kMeasuredValueMax    = 10000;

// Bad
constexpr uint16_t kMinMeasuredValueMax = 0x270F;
constexpr uint16_t kMeasuredValueMax    = 0x2710;
```

(Cited: #71424 "do we care about 0x270F? spec says `max 9999` so decimal
number seems reasonable".)

### Do name spec-defined bounds after what they bound

Name constants after the attribute and the direction of the constraint, not
after a generic range label.

```cpp
// Good — clear what each constant applies to
constexpr uint16_t kMinMeasuredValueMax = 9999;  // upper bound of MinMeasuredValue
constexpr uint16_t kMeasuredValueMax    = 10000; // upper bound of MeasuredValue / MaxMeasuredValue

// Bad — ambiguous
constexpr uint16_t kMaxMin = 9999;
constexpr uint16_t kMinMax = 10000;
```

(Cited: #71424 "Naming `kMaxMin` and `kMinMax` seems odd really".)

### Do introduce a named constant for every spec-defined magic number

`2048`, `32766`, `0xFFFE` — each gets a `constexpr` with a comment pointing
to the spec clause. (Cited: #43204 "should we have a named constant for 2048
as well?".)

---

## Nullable Handling

### Do use `ValueOr(fallback)` instead of manually branching on `IsNull`

```cpp
// Bad — asserts on null
if (measuredValue.Value() != 0) { … }

// Good
if (measuredValue.ValueOr(0) != 0) { … }
```

(Cited: #42884.)

### Don't make an optional attribute nullable unless the spec requires it

If "value unknown" is representable by the attribute being absent, don't also
make it nullable — that's two ways to say the same thing. Review the spec's
conformance letter (`O` vs `X`) before adding `Nullable<…>` to an optional
field. (Cited: #42884 "Now why do we have a nullable optional attribute?
Why would the attribute not be missing if the value is unknown …".)

### Do test reserved null-sentinel rejection for nullable numeric attributes

The spec reserves `0xFFFF` (for `uint16_t`) and similar max-range values as
the null sentinel. Verify that writing the sentinel value via the data model
is rejected with `ConstraintError`. Note: some older measurement clusters
accept it; new conversions should fix this. (Cited: #71552.)

---

## Error Handling

### Do replace `TEMPORARY_RETURN_IGNORED` with proper handling

`TEMPORARY_RETURN_IGNORED` is a technical-debt marker. When you write new code:

- If the error is truly non-critical: use `LogErrorOnFailure(...)`.
- If the error matters: propagate it with `ReturnErrorOnFailure(...)`.

Never add `TEMPORARY_RETURN_IGNORED` to newly written cluster code.
(Cited: #43423 "Should we flip this to a LogErrorOnFailure at least to
remove temporary_return_ignored?"; #43204.)

### Do use `VerifyOrReturnError` / `VerifyOrReturnValue` for early returns

```cpp
// Bad
if (!mFeatureMap.Has(Feature::kOnOff)) return Status::UnsupportedCommand;

// Good
VerifyOrReturnError(mFeatureMap.Has(Feature::kOnOff), Status::UnsupportedCommand);
```

(Cited: #42748 "use `VerifyOrReturnError` or `VerifyOrReturnValue` for these";
#43204.)

### Do chain with `ReturnErrorOnFailure` instead of nested `if`s

```cpp
// Bad
CHIP_ERROR err = Foo();
if (err != CHIP_NO_ERROR) return err;

// Good
ReturnErrorOnFailure(Foo());
```

(Cited: #42748 "Lets just use `ReturnErrorOnFailure` everywhere here instead
of this if — this is what other clusters do and seems cleaner".)

### Do prefer `ConstraintError` over `InvalidArgument` for out-of-range values

Both are technically correct, but `ConstraintError` maps back to the IM as
"constraint violation" instead of a generic `Status::Failure`.
(Cited: #43085, #43204, #42968, #43633 "should this be a
`CHIP_IM_GLOBAL_STATUS(ConstraintError)` instead? invalid argument is also
correct, but will map to Status::Failure for IM".)

### Do pick specific error codes for state vs. argument failures

- `CHIP_ERROR_INCORRECT_STATE` — endpoint/cluster not initialized
- `CHIP_ERROR_NOT_FOUND` — cluster/entry not present
- `CHIP_ERROR_INVALID_ARGUMENT` — the caller's value is out of range

Reviewers called out that generic `INVALID_ARGUMENT` is misleading when the
real failure is "object isn't ready yet". (Cited: #71424.)

### Don't use `exit:` as an error-only label

`SuccessOrExit` routes both success and failure paths through `exit:`. Put
`ChipLogError` *before* the macro that gates the error, not after.
(Cited: #41064 "Exit should always be met. `exit:` is not `error:`. Please
do not use `exit` for a path not always reached".)

### Do wrap fire-and-forget setter calls with `LogErrorOnFailure`

If you call a setter and don't propagate the return value (e.g. in a callback),
wrap it so failures are not silently swallowed.

```cpp
// Bad — error silently ignored
cluster->SetMeasuredValue(newValue);

// Good
LogErrorOnFailure(cluster->SetMeasuredValue(newValue));
```

---

## Style & Formatting

### Don't mix bracing conventions within a file

Some files brace every `if`; others omit the braces on single statements.
Follow the surrounding code. Do **not** add single-line `if`s into a file
that brace-wraps everything, and vice versa. (Cited: #42748 "nit throughout:
please add `{}` for all ifs"; #43720 "you can remove a bunch of `{}`
wrappers here"; #40788 "remove extra `{}` throughout".)

### Don't add redundant scoping blocks

```cpp
// Bad — extra { } does nothing
{
    {
        DoThing();
    }
}
```

(Cited: #43204 "the double-scoping feels weird throughout. Please remove it".)

### Prefer `enum class` over `bool` for binary choices

`kClient` / `kCommand` reads better than `true` / `false` at the call site.
Flash is equivalent; readability wins. (Cited: #41232 "bools are somewhat
harder to read. How about an `enum class` that is either `kClient` or
`kCommand`").

### Do inline trivial private one-liners to save flash

A private helper whose body is a single expression can often be inlined at
the call sites without loss of readability — and sometimes saves flash.
(Cited: #41064 "This is a private one-liner. Lets inline these directly, see
if that saves a bit of flash".)

### Do keep log category consistent within a file

Don't use `ChipLogProgress(AppServer, …)` in one block and `ChipLogError(Zcl, …)`
in the next. Pick the category the file uses. (Cited: #42886.)

### Do drop non-essential logs in embedded builds

Logging an unrecoverable error on a constrained device just wastes flash.
Gate verbose logs behind a build flag when they're informational-only.
(Cited: #41954 "Why log this? It's likely not recoverable. Suggest avoiding
the log on embedded builds".)

---

## Code Reuse (and When *Not* to Reuse)

### Do check `DefaultServerCluster` before adding a helper

Covered above. The base class already ships notify, set-with-notify, path
accessors, and default read/write behavior.

### Don't force a helper when two callers validate different contracts

Example: constructor validates hardware config once at startup; runtime
setter validates each new reading at runtime. They check similar-looking
conditions but represent different contracts — extracting a helper obscures
this. Document the difference instead. (Cited: #71424 — reviewer suggested a
helper; author explained why it doesn't apply, and that response was
accepted.)

### Don't prematurely extract a template base class

The "15 measurement clusters have the same data model" observation is true
but misleading: `int16_t` (Temperature), `uint16_t` (Humidity), enum
(LightSensor) make a template base non-trivial. Land several conversions
first; extract a base class afterward when the common surface is actually
stable. (Cited: #71424.)

---

## ZAP / Build Configuration

### Do update both `zcl.json` and `zcl-with-test-extensions.json`

Both files must list all non-list attributes under
`attributeAccessInterfaceAttributes`. Missing the test-extensions file causes
test-only failures that are hard to diagnose.

### Do update `config-data.yaml` correctly

- Add the cluster to `CodeDrivenClusters`.
- Remove it from `CommandHandlerInterfaceOnlyClusters` if it was listed there.

(Cited: #41064 "Forgot to add Access Control to
`CommandHandlerInterfaceOnlyClusters` … I've updated `config-data.yaml`".)

### Do commit all files generated by `zap_regen_all.py`

The regen script updates `.matter` files, `endpoint_config` files,
`cluster-callbacks.cpp`, `CodeDrivenInitShutdown.cpp`, and C++ accessors
across every app that uses the cluster. All generated files must be included in
the PR — cherry-picking only some will break other apps.

### Do add new cluster directories to `zap_cluster_list.json`

Pure-Ember clusters had no `src/app/clusters/<name>/` directory. After creating
it, add the cluster constant → directory mapping under `ServerDirectories` in
`src/app/zap_cluster_list.json`.

### Do register the cluster in `src/app/clusters/BUILD.gn`

Add the cluster directory to the `# keep-sorted` `public_deps` list in
`source_set("clusters")` in `src/app/clusters/BUILD.gn`. This wires the cluster
into the application build.

```gn
"illuminance-measurement-server",
"relative-humidity-measurement-server",   # ← add (sorted alphabetically)
"level-control",
```

### Do register the cluster's tests in `src/BUILD.gn`

Add the tests path to the deps list in `chip_test_group("tests")` in
`src/BUILD.gn`, sorted alphabetically alongside other cluster test entries:

```gn
"${chip_root}/src/app/clusters/illuminance-measurement-server/tests",
"${chip_root}/src/app/clusters/relative-humidity-measurement-server/tests",
"${chip_root}/src/app/clusters/joint-fabric-administrator-server/tests",
```

Without this, tests will never be compiled or run by `ninja check`.

---

## Testing

### Do smoke-test with the REPL before writing unit tests

```bash
rm /tmp/chip_*                   # clear stale commissioning state
# terminal 1: run all-clusters-app (or the relevant example app)
# terminal 2: build and run matter-repl
#   > commission
#   > read attribute <your-cluster> <attribute-name>
```

A misconfigured cluster crashes immediately here, saving hours of debugging.

### Do use a `Testable*` subclass to expose protected methods

When tests need to call `protected` methods, create a thin subclass in the test
file rather than making those methods public in the cluster class itself.

```cpp
class TestableFooCluster : public FooCluster
{
public:
    using FooCluster::FooCluster;
    using FooCluster::SomeProtectedMethod; // re-exports as public
};
```

### Do test reserved null-sentinel rejection for nullable numeric attributes

Covered in the Nullable section. (Cited: #71552.)

### Do use `LogErrorOnFailure` on fire-and-forget setter calls

Covered in Error Handling.

### Do unit-test the cluster class directly — bypass `CodegenIntegration`

Instantiate `<Name>Cluster` in tests with injected mock values. Cover:

- `Attributes()` returns the correct list with and without each optional feature
  enabled.
- `ReadAttribute` succeeds (no error, correct value) for every mandatory
  attribute.
- Write validation: below minimum, above maximum, on the boundary, same value
  (no-op).
- `NotifyAttributeChanged` is triggered on a successful write and **not**
  triggered on a failed write or a no-op write.
- `AcceptedCommands()` and `GeneratedCommands()` return the correct sets for
  every feature combination — allocation failures here are silent without tests.
- String-length boundaries for bounded text attributes (32-byte SSID, 32-byte
  NodeLabel, etc.).

(Cited: #43423 — feature-combo Append tests; #43085 — notify-on-change tests
+ SSID length test; #41232 — unit-test the cluster directly.)

### Don't write "change detector" tests

Tests that hard-code codegen output (e.g. a specific `ClusterRevision` value)
fail every time codegen bumps the version. Certification already covers this
surface. (Cited: #41232 "This is a change detector and a buggy one at that:
when our codegen changes the version, we will start failing this and will
require a manual update. lets delete this entire test".)

### Do use a fresh `TestServerClusterContext` per test case

`TestServerClusterContext` accumulates events and state. Starting fresh each
test avoids coupling and hidden ordering dependencies. (Cited: #41232.)

### Do factor the test endpoint id into a file-level constant

`constexpr EndpointId kTestEndpointId = 1;` beats repeating the same literal
inside every test. (Cited: #41232 "this could be a global `kTestEndpointId`
instead of a const in every test".)

### Don't hardcode magic constants copied from another file

Do not copy a constant and leave a comment like `// see SomeFile.cpp#L61`.
Either import the defining header or force the caller to supply the value
explicitly (remove the default). This applies equally to subscription limits,
buffer sizes, and timeout values.

---

## Documentation

### Do use relative `.md` links — not deployed HTML URLs

```markdown
<!-- Good — works locally and survives doc moves -->
[Writing Clusters](./writing_clusters.md)

<!-- Bad — breaks local builds and rots when URLs change -->
[Writing Clusters](https://project-chip.github.io/connectedhomeip-doc/guides/writing_clusters.html)
```

### Do comment non-obvious guards and side effects

When a condition silently skips a side-effecting operation, add a short comment
explaining:

- What the condition is checking.
- What happens if it is false (the silent case).
- Any observable side effects (e.g. "only registers if PlatformManager delegate
  is not already set").

### Do remove Ember references from comments and READMEs on conversion

Conversion PRs often inherit comments that talk about "emulating Ember" or
"post-attribute-change callback". Those references are misleading in the new
world — rewrite or delete them. (Cited: #42331 "I would remove references to
Ember whenever possible" / "I know this was llm generated but I do not think
it is relevant that we are emulating ember".)

### Do document migration impact in the cluster README

When you remove global attribute getters/setters or post-attribute-change
callbacks, add a README paragraph so downstream apps know what to replace.
(Cited: #41954 "Updated Readme with some notes about global attribute
getters/setters being removed and also post attribute change callbacks".)

---

## Rejected / Superseded Suggestions

Things reviewers **asked for** but were later retracted with justification
— worth knowing so you don't re-open the same discussion:

### "Add setters for every RW attribute" — rejected as YAGNI

If an attribute is only ever modified via Matter writes (not by the app),
don't add a setter until an app actually needs it. (Cited: #42634 — `OnTime`,
`OffWaitTime`, `StartUpOnOff` setters omitted: "I would like to YAGNI and not
set a rule of 'provide setters for everything'".)

### "Keep backward-compat for the old `Attributes::X::Set` accessors" — infeasible

If the `Set` accessor wrote to the Ember RAM buffer that the code-driven
cluster no longer reads, compat at that layer is impossible. Move compat into
`CodegenIntegration` (state-read path) instead of into the legacy accessor.
(Cited: #71424 — "The Set() accessors write to the Ember attribute store,
which the code-driven cluster no longer reads — so keeping compat there isn't
feasible. Instead, CodegenIntegration reads [the live state]".)

### "Extract a template base for all measurement clusters" — deferred

The proposal is reasonable in principle but the type differences
(`int16_t`, `uint16_t`, enum) make a template base non-trivial. Land more
conversions first; extract the abstraction once the common surface settles.
(Cited: #71424.)

### "Validate path ID against `kInvalidAttributeId` before the switch" — not needed

The framework guarantees that paths are valid. Adding the check is a
no-op that just costs flash. (Cited: #43423.)

---

## Coverage Notes

- This document merges the curated Dos and Don'ts with PR-review citations
  drawn from **532 non-bot review comments across 25 conversion PRs**.
- Top reviewers referenced: **andy31415** (205 comments), **soares-sergio**
  (69), **zaid-google** (31), **arielsz71** (28), **bzbarsky-apple** (25),
  **lpbeliveau-silabs** (23), **shubhamdp** (18), **tcarmelveilleux** (15),
  **pimpalemahesh** (15), **Elen777300** (14), **shripad621git** (11),
  **ksperling-apple** (9).
- Full raw-comment corpus:
  [`code_driven_cluster_review_comments_raw.md`](./code_driven_cluster_review_comments_raw.md).
- Full PR index:
  [`code_driven_cluster_conversion_prs.md`](./code_driven_cluster_conversion_prs.md).
