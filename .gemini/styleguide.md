# Gemini Code Assist — Repository Style Guide

This file is consumed by Gemini Code Assist during pull-request reviews and
inline suggestions. It complements [`.gemini/config.yaml`](./config.yaml) and
applies repo-wide. For code-driven cluster work specifically, Gemini must also
load the playbook referenced in §3.

---

## 1. General conventions

- **Language:** C++17. Avoid exceptions, avoid `new`/`delete` in hot paths,
  prefer `chip::Span`, `chip::Optional`, `chip::Nullable`.
- **File header:** every source file has an Apache 2.0 license header. When
  editing an existing file, keep the **original** copyright year range
  (e.g. `Copyright (c) 2021-2026`) — never reset to the current year alone.
- **Braces:** match the surrounding file. Do not add braces to single-line
  `if`s in a file that omits them, and vice versa.
- **No redundant `chip::` inside `namespace chip`.** Inside an already-chip
  namespace, write `Foo` not `chip::Foo`.
- **Error handling:** use `ReturnErrorOnFailure`, `VerifyOrReturnError`,
  `VerifyOrReturnValue`, `VerifyOrDie`, `LogErrorOnFailure` from
  `<lib/support/CodeUtils.h>`. Never silently swallow a `CHIP_ERROR`.
- **`exit:` is not `error:`.** `SuccessOrExit` routes both success and failure
  paths through `exit:` — any `ChipLogError` must come *before* the guarding
  macro, not after.
- **Named constants for spec bounds.** Every spec-defined bound gets a
  `constexpr` with a comment pointing to the spec clause; use **decimal**
  values (`9999`), not hex (`0x270F`).
- **No `TEMPORARY_RETURN_IGNORED` in new code.** Either propagate the error
  (`ReturnErrorOnFailure`) or log and continue (`LogErrorOnFailure`).

## 2. Header-file rules

- Place `class Foo` in `Foo.h`. A "<name>-server.h" header that still contains
  a cluster class is a review red flag.
- Do not write `using DataModel::<X> = …` in a header. Short aliases for deep
  cluster-local types are allowed only immediately before the type that needs
  them, with a comment.
- Do not stack short aliases (`using F = Nullable<uint16_t>; using G = …`) —
  they hide the types reviewers need to see.
- Forward declarations of the cluster class inside its own header indicate
  odd coupling; justify them with a comment or remove them.
- Prefer narrow includes (`<clusters/Foo/Ids.h>`, `<clusters/Foo/Metadata.h>`)
  over `<app-common/zap-generated/cluster-objects.h>`.

## 3. Code-driven cluster work

**When reviewing a PR that touches `src/app/clusters/*/`, apply the
cluster-specific rules below in addition to §1–§2.**

The authoritative playbook is
[`docs/guides/code_driven_cluster_ai_playbook.md`](../docs/guides/code_driven_cluster_ai_playbook.md).
The prescriptive rules doc with PR citations is
[`docs/guides/code_driven_cluster_dos_and_donts.md`](../docs/guides/code_driven_cluster_dos_and_donts.md).

### 3.1 Blocking review findings

Flag any of these at **high** priority:

1. `#include <data-model-providers/codegen/…>` or `#include <app/util/…>` in
   `<Name>Cluster.cpp`. These belong only in `CodegenIntegration.cpp`.
2. Ember accessor calls (`Attributes::X::Set`, `emberAfReadAttribute`) inside
   the cluster class.
3. A cluster class with an `Init()` method, an `mEndpointId` member, or an
   `mIsRegistered` member.
4. A constructor with 7+ parameters, especially with 3+ consecutive same-type
   parameters. Ask the author to bundle into a nested `Config` struct.
5. Runtime setters for hardware-fixed attributes (`MinMeasuredValue`,
   `MaxMeasuredValue`, `Tolerance`, `NumberOfPositions`, etc.). These belong
   in `Config` with no public setter.
6. Path-validity checks in `ReadAttribute` / `WriteAttribute` before the
   switch — the framework guarantees validity.
7. Feature-flag checks inside `ReadAttribute` / `WriteAttribute` /
   `InvokeCommand`. `Attributes()` and `AcceptedCommands()` are the
   gatekeepers.
8. `WriteAttribute` default branch returning `UnsupportedAttribute` instead of
   delegating to `DefaultServerCluster::WriteAttribute`.
9. Double-notification: a `SetX` method that already calls
   `NotifyAttributeChanged`, wrapped in `NotifyAttributeChangedIfSuccess` by
   the caller.
10. A PR that mixes file renames with logic changes. Migrations must be split
    into PRs per the playbook §2.
11. Unlisted `.h` file — not present in any `BUILD.gn` or
    `app_config_dependent_sources.{cmake,gni}`.
12. Missing `zap_regen_all.py` output (`.matter`, `endpoint_config.*`,
    `cluster-callbacks.cpp`, `CodeDrivenInitShutdown.cpp`) in a conversion PR.

### 3.2 Medium-priority findings

Flag at **medium** priority (request changes but not necessarily blocking):

1. `using FooConfig = …` + `friend class` pattern — prefer nesting `Config`
   directly in the cluster class.
2. Named `StartupConfiguration` instead of `Config`.
3. `Status::UnsupportedAttribute` returned with `static_cast<ActionReturnStatus>(…)`.
   `ActionReturnStatus` auto-casts; drop the cast.
4. `CHIP_ERROR_INVALID_ARGUMENT` where `CHIP_IM_GLOBAL_STATUS(ConstraintError)`
   is more accurate (out-of-range value from IM).
5. `CHIP_ERROR_INVALID_ARGUMENT` where `CHIP_ERROR_INCORRECT_STATE` or
   `CHIP_ERROR_NOT_FOUND` is more accurate (cluster not initialized / not
   found for endpoint).
6. `bool isClient` parameter — prefer `enum class { kClient, kCommand }`.
7. Redundant nested scopes (`{ { … } }`) in `.cpp` files.
8. Missing unit test for a feature combination of `Attributes()` /
   `AcceptedCommands()`.
9. Missing unit test for reserved-null-sentinel rejection on nullable numeric
   attributes.
10. Callers that invoke `Handle<Cmd>` directly in tests rather than via
    `ClusterTester::Invoke` / `InvokeCommand`.
11. "Change detector" tests that hard-code codegen output (e.g. specific
    `ClusterRevision` value) — suggest deletion.
12. Hard-coded feature map without a comment explaining why.

### 3.3 Low-priority / style findings

- Log-category inconsistency within a file (`ChipLogProgress(AppServer, …)`
  mixed with `ChipLogError(Zcl, …)`).
- Verbose logging of non-recoverable errors (suggest gating behind a build
  flag or removing).
- `using namespace` in a header file.

## 4. Typo / spelling review

The Matter project uses American English spelling in code, comments, and
documentation (`initialize`, not `initialise`; `authorize`, not `authorise`).
Flag typos at **medium** priority when they appear in:

- Public API documentation comments.
- Spec-referenced attribute / command / event names.
- Copy-pasted boilerplate that references the wrong cluster (e.g. Doxygen
  comments on `RequestCommissioningApproval` that describe a cooking
  parameter).

## 5. Test code

- Tests use GTest (`pw_unit_test/framework.h`).
- Use a file-level `constexpr EndpointId kTestEndpointId = 1;` instead of
  repeating the literal.
- Subclass the real cluster as `TestableFooCluster` to expose protected
  members — do not relax real-class visibility.
- Use `TestServerClusterContext` fresh per test; do not reach into
  `Server::GetInstance()` from tests when injecting is an option.

## 6. Rejected / superseded suggestions (do not re-request)

These suggestions have been raised previously and explicitly rejected with
reasoned justification. Do not re-raise them in reviews:

- "Add a setter for every RW attribute" — YAGNI; add only when an application
  needs it.
- "Preserve the old `Attributes::X::Set` accessor for backward compatibility"
  — infeasible where the accessor wrote to the Ember RAM buffer the
  code-driven cluster no longer reads; compat lives in `CodegenIntegration`.
- "Extract a template base class for all measurement clusters" — deferred;
  type differences (`int16_t` Temperature, `uint16_t` Humidity, enum
  LightSensor) make this non-trivial.
- "Add a path-validity check before the switch for safety" — not needed; the
  framework guarantees the path exists.

## 7. References

- Cluster migration playbook (primary AI reference):
  [`docs/guides/code_driven_cluster_ai_playbook.md`](../docs/guides/code_driven_cluster_ai_playbook.md)
- Prescriptive rules with PR citations:
  [`docs/guides/code_driven_cluster_dos_and_donts.md`](../docs/guides/code_driven_cluster_dos_and_donts.md)
- Human migration guide:
  [`docs/guides/migrating_ember_cluster_to_code_driven.md`](../docs/guides/migrating_ember_cluster_to_code_driven.md)
- Writing new clusters:
  [`docs/guides/writing_clusters.md`](../docs/guides/writing_clusters.md)
- PR index:
  [`docs/guides/code_driven_cluster_conversion_prs.md`](../docs/guides/code_driven_cluster_conversion_prs.md)
- Raw review comments:
  [`docs/guides/code_driven_cluster_review_comments_raw.md`](../docs/guides/code_driven_cluster_review_comments_raw.md)
