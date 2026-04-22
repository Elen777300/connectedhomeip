# GitHub Copilot Instructions — connectedhomeip

This file configures GitHub Copilot (chat, coding agent, and inline
suggestions) for the `project-chip/connectedhomeip` repository.

## Applies to the whole repo

- **Language:** C++17 (embedded-friendly subset). Avoid exceptions, avoid
  dynamic allocation in hot paths, prefer `chip::Span` / `chip::Optional` /
  `chip::Nullable` over raw pointers + sizes.
- **Style:** Match the existing file's conventions. When a file braces every
  `if`, you must too; when it omits braces on single statements, you must too.
- **Error handling:** Use `ReturnErrorOnFailure`, `VerifyOrReturnError`,
  `VerifyOrReturnValue`, `VerifyOrDie` from `<lib/support/CodeUtils.h>`.
  Never silently swallow `CHIP_ERROR`; if you truly need to, wrap with
  `LogErrorOnFailure`.
- **Copyright:** When renaming or modifying an existing file, preserve the
  **original** copyright year range. Do not reset to the current year.

## Code-driven cluster migrations

If the task involves **any of the following**, you **must** follow the
playbook at
[`docs/guides/code_driven_cluster_ai_playbook.md`](../docs/guides/code_driven_cluster_ai_playbook.md):

- Converting an Ember cluster under `src/app/clusters/<name>-server/` to use
  `DefaultServerCluster`.
- Creating or editing a `CodegenIntegration.cpp` / `CodegenIntegration.h` file.
- Introducing a new `<Name>Cluster.h` / `<Name>Cluster.cpp` pair.
- Removing Ember `Attributes::X::Set/Get` accessor usage from cluster code.
- Adding unit tests under `src/app/clusters/*/tests/` for a code-driven
  cluster.

Before writing code for these tasks, also read:

1. [`docs/guides/writing_clusters.md`](../docs/guides/writing_clusters.md)
2. [`docs/guides/migrating_ember_cluster_to_code_driven.md`](../docs/guides/migrating_ember_cluster_to_code_driven.md)
3. [`docs/guides/code_driven_cluster_dos_and_donts.md`](../docs/guides/code_driven_cluster_dos_and_donts.md)

### Blocking rules (Copilot must not suggest these)

When generating code for a code-driven cluster, never produce:

1. `using DataModel::<X> = …` aliases in a header.
2. A forward declaration of the cluster class inside its own header.
3. An `Init()` method on the cluster class — use `Startup()` / `Shutdown()`.
4. An `EndpointId mEndpointId;` member — use `GetPaths()[0].mEndpointId`.
5. An `mIsRegistered` member or registry calls on the cluster class — that
   goes in `CodegenIntegration.cpp`.
6. A constructor with multiple consecutive same-type parameters — bundle into
   a nested `Config` struct.
7. Runtime setters for hardware-fixed attributes (`MinMeasuredValue`,
   `Tolerance`, etc.) — those belong in `Config`.
8. Path-validity checks (`path != kInvalidAttributeId`) before the switch in
   `ReadAttribute`.
9. Feature checks inside `ReadAttribute` / `WriteAttribute` / `InvokeCommand`
   — `Attributes()` and `AcceptedCommands()` are the gatekeepers.
10. A `WriteAttribute` default branch that returns `UnsupportedAttribute` —
    delegate to `DefaultServerCluster::WriteAttribute(...)` instead.
11. `#include <data-model-providers/codegen/…>` or `#include <app/util/…>` in
    `<Name>Cluster.cpp` — those belong **only** in `CodegenIntegration.cpp`.
12. Ember accessors (`Attributes::X::Set`, `emberAfReadAttribute`) **inside
    the cluster class**.
13. Hex constants for decimal spec bounds (write `9999`, not `0x270F`).
14. Double-notification (wrapping a setter that already calls
    `NotifyAttributeChanged` in another `NotifyAttributeChangedIfSuccess`).
15. New `TEMPORARY_RETURN_IGNORED` markers — use `LogErrorOnFailure` or
    propagate the error.

### Preferred patterns

- Nest `Config` inside the cluster class; do **not** use a standalone config
  class with `friend` + `using` alias.
- Use `WithX()` builder methods when an optional attribute has an associated
  value (set value + presence flag atomically).
- Use `DefaultServerCluster::SetAttributeValue(member, newValue, AttrId)` for
  simple member-backed writes — it handles no-op detection and notification.
- Use `NotifyAttributeChangedIfSuccess(attrId, WriteImpl(...))` as the single
  exit path in `WriteAttribute`.
- Return `std::nullopt` from `InvokeCommand` for unknown commands.
- Return `Status::…` values directly — `ActionReturnStatus` auto-casts; do not
  wrap in `static_cast`.
- Prefer `ConstraintError` over `InvalidArgument` for out-of-range values.
- Use `enum class { kX, kY }` instead of `bool` for binary parameters.

### Testing expectations

When writing a `Test<Name>Cluster.cpp`:

- Subclass as `Testable<Name>Cluster` to expose protected members (don't
  loosen real-class visibility for tests).
- Use `TestServerClusterContext` fresh per test.
- Cover every feature combination for `Attributes()` and `AcceptedCommands()`.
- Verify `NotifyAttributeChanged` fires on real change and does **not** fire
  on no-op / failed writes.
- Test reserved null-sentinel rejection (e.g. `0xFFFF` for `uint16_t`) for
  nullable numeric attributes.
- Test via `ClusterTester::Invoke` — not by calling `Handle<Cmd>` directly.

### PR structure

Migrations must be split into at least 3 PRs:

1. **PR 1** — file renames only, no logic changes.
2. **PR 2** — code reordering only (optional if no meaningful reorder needed).
3. **PR 3** — the actual conversion.

See the playbook §2–§4 for details. Do not mix renames with logic changes in
one PR.

## Build and commit discipline

- Every `.h` must be listed in `BUILD.gn` or
  `app_config_dependent_sources.{cmake,gni}`. Unlisted headers silently break
  on some platforms.
- After a code-driven cluster change, run `scripts/tools/zap_regen_all.py`
  and commit **all** generated files (`.matter`, `endpoint_config.*`,
  `cluster-callbacks.cpp`, `CodeDrivenInitShutdown.cpp`, C++ accessors).
- Update `config-data.yaml` (`CodeDrivenClusters` list) and
  `zap_cluster_list.json` as described in the playbook §4.8.
- Add the cluster directory to the `# keep-sorted` `public_deps` list in
  `src/app/clusters/BUILD.gn`.
- Add the tests path to `chip_test_group("tests")` in `src/BUILD.gn` or the
  tests never run.

## When the user asks for something this file forbids

Politely push back, link to the relevant rule, and offer the correct pattern.
Do not silently comply with requests that violate the playbook.
