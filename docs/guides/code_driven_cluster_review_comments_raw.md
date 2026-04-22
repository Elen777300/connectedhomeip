# Code-Driven Cluster Review Comments (Raw)

Review comments scraped from code-driven cluster conversion PRs. Bot comments
(gemini-code-assist, github-actions, restyled-io) are filtered out. A handful of
Copilot comments are retained for visibility of automated review observations.

Data gathered 2026-04-21 across 25 PRs.

See also:
- [`code_driven_cluster_conversion_prs.md`](./code_driven_cluster_conversion_prs.md) — PR index
- [`code_driven_cluster_review_rules.md`](./code_driven_cluster_review_rules.md) — deduplicated rules
- [`code_driven_cluster_dos_and_donts.md`](./code_driven_cluster_dos_and_donts.md) — curated dos and don'ts

**Total:** 532 non-bot review comments across 25 PRs.

---

## PR [#40422](https://github.com/project-chip/connectedhomeip/pull/40422)

- **@andy31415** on `src/app/persistence/String.h`

  > This is part of #40311 that is just merged part of this PR.
  > Realistically only having a string of maximum length 1 seems somewhat questionable (it is a char in that case...) so I am not sure we want it as much. Erring on the side of "this seems good for now". The strings we care about are around 32 characters (labels and such)

- **@ksperling-apple** on `src/app/clusters/basic-information/CodegenIntegration.cpp`

  > ```suggestion
  > // BasicInformationCluster implementation is specifically implemented
  > ```

- **@zaid-google** on `src/app/persistence/AttributePersistence.cpp`

  > Was the attribute persistence work originally meant to be in its own PR? That's what it sounded like based on the commit message, although I suppose having it bundled with this PR is fine as well since this is the first cluster to use it

- **@zaid-google** on `src/app/clusters/basic-information/BasicInformationCluster.h`

  > Is there another way to do this? I understand one of these is mutable and the other isn't, but seems a bit odd to have both functions

- **@ksperling-apple** on `src/app/persistence/PascalString.h`

  > Nothing his being allocated here... should this say something like "Interprets the given span as a pascal buffer" ?

- **@andy31415** on `src/app/persistence/AttributePersistence.cpp`

  > No, I am waiting for #40311 to merge after which it will look cleaner.

- **@andy31415** on `src/app/persistence/PascalString.h`

  > Will make this change as a separate commit. Technically should be part of #40311 however that one already has had 6hr+ of CI (plus flakyness even more) so I would wait for that to merge and then update the comment.

- **@andy31415** on `src/app/persistence/PascalString.h`

  > changes in #40459

- **@andy31415** on `src/app/persistence/AttributePersistence.cpp`

  > Merge done.

- **@andy31415** on `src/app/clusters/basic-information/BasicInformationCluster.h`

  > I can remove the const version, it is not needed anymore. I wanted to return a reference because I need to modify it at runtime as a singleton (and save flash in this implementation ... even though some set/constructor would be cleaner).

- **@soares-sergio** on `src/app/clusters/basic-information/tests/TestBasicInformationCluster.cpp`

  > Should we add more coverage on these unit tests? Probably out of scope of this PR, as there were zero tests before, but now that we have a unit testable cluster it feels like we could add some more tests.
  > 
  > Maybe these are already covered by the integration tests but here are some ideas:
  > 
  > * Read-only checks: Verifying that writes to read-only attributes like VendorName, VendorID, ProductName, and ProductID return an UNSUPPORTED_WRITE error
  > * Write/Read validation: Confirming that writable attributes like NodeLabel and LocalConfigDisabled can be successfully written to and read back with the new value
  > * Data validation: maybe some checks on attribute values, valid ranges, string lengths, etc. For example: write out-of-range or invalid values to attributes (e.g., a string longer than 32 bytes to NodeLabel) and assert that the write fails with the appropriate error code.
  > * verify that the StartUp and ShutDown events are correctly emitted when the cluster is initialized and shut down
  > * Check optional attributes (ProductAppearance or ConfigurationVersion)

- **@soares-sergio** on `src/app/clusters/basic-information/BasicInformationCluster.h`

  > nit: I would group those 2 comments together at the top, as they're related.
  > 
  > ```suggestion
  > /// This class provides a code-driven implementation for the Basic Information cluster,
  > /// centralizing its logic and state. It is designed as a singleton because the cluster
  > /// is defined to exist only once per node, specifically on the root endpoint (Endpoint 0).
  > ///
  > /// As a PlatformManagerDelegate, it automatically hooks into the node's lifecycle to
  > /// emit the mandatory StartUp and optional ShutDown events, ensuring spec compliance.
  > ///
  > /// Note on the implementation of the singleton pattern:
  > /// The constructor is public to allow for a global variable instantiation. This approach
  > /// can save flash memory compared to a function-static instance, which often requires
  > /// additional thread-safety mechanisms. The intended usage is via the static
  > /// `Instance()` method, which returns a reference to the global instance.
  > ```

- **@soares-sergio** on `src/app/clusters/basic-information/BasicInformationCluster.h`

  > ```suggestion
  > ```

- **@andy31415** on `src/app/clusters/basic-information/tests/TestBasicInformationCluster.cpp`

  > I believe we cannot commit to add full unit tests for converted clusters. This is better than before - shows that unit tests  are possible, however creating comprehensive tests for all converted clusters would be a massive time sink. Right now I rely on integration test as before, have unit tests as a proof of concept that testing is possible.

- **@andy31415** on `src/app/clusters/basic-information/tests/TestBasicInformationCluster.cpp`

  > I added a few new tests to show how things could be tested. I believe we need better Read/Write attribute support for testing as the boilerplate is still quite large.

- **@bzbarsky-apple** on `src/app/clusters/basic-information/CodegenIntegration.cpp`

  > This is the wrong callback.  See https://github.com/project-chip/connectedhomeip/pull/39698#discussion_r2264527455

- **@bzbarsky-apple** on `src/app/clusters/basic-information/CodegenIntegration.cpp`

  > Followup: I really wonder whether all these function calls are actually smaller codesize than just code-generating a small table of which optional attribute IDs are enabled that can then be referenced directly from this code. Yes, that would partially duplicate existing things in endpoint_config.h, but that duplication might still be smaller than what we have here.

- **@bzbarsky-apple** on `src/app/clusters/basic-information/CodegenIntegration.cpp`

  > Why do they require this?  This piece feels like it needs a much more extensive explanation.

- **@bzbarsky-apple** on `src/app/clusters/basic-information/BasicInformationCluster.cpp`

  > Note that this says nothing about bool being 8 bits or one byte; just means it's the same size as char (which is not guaranteed to by one byte by the standard).
  > 
  > So it's not really clear to me what this is really asserting and why....

- **@bzbarsky-apple** on `src/app/clusters/basic-information/BasicInformationCluster.cpp`

  > Why do we have both this and kExpectedFixedLocationLength?  At the very least that should be documented... but I think we should only have one constant for this constant, honestly.  Keep the one that was already here: kExpectedFixedLocationLength.

- **@bzbarsky-apple** on `src/app/clusters/basic-information/BasicInformationCluster.cpp`

  > ```suggestion
  >  *    Copyright (c) 2020-2025 Project CHIP Authors
  > ```
  > Given that the file was just renamed and a bunch of code kept.

- **@bzbarsky-apple** on `src/app/clusters/basic-information/BasicInformationCluster.cpp`

  > Why does this need the explicit specialization?

- **@bzbarsky-apple** on `src/app/clusters/basic-information/BasicInformationCluster.cpp`

  > I would think `Encode(0u)` should work just fine?

- **@bzbarsky-apple** on `src/app/clusters/basic-information/BasicInformationCluster.cpp`

  > Why is the `<bool>` needed?  Shouldn't be needed...  `true` is most certainly a bool, so that should end up with the right template instantiation anyway, no?

- **@bzbarsky-apple** on `src/app/clusters/basic-information/BasicInformationCluster.cpp`

  > Followup: we should really figure out what the actual needs are around this "disable the mandatory attribute" business and why.  Duplicating all of this is very fragile; it would be nice if we could just use the code-generated mandatory attribute list....
  > 
  > If we really do need this, I would prefer that we added some sort of special-case to codegen to be able to flag mandatory attributes to be left out of its generated list (either hardcoded for this one, or based on some config file or something), so we don't have to duplicate things here.

- **@bzbarsky-apple** on `src/app/clusters/basic-information/BasicInformationCluster.cpp`

  > The detail logging here and in OnShutDown seems to have gotten lost.  Please put it back. 
  > 
  > The error logging on failure to generate the event also got lost.... I am not sure how useful that is in practice, but it's odd that it was just summarily removed.

- **@bzbarsky-apple** on `src/app/clusters/basic-information/BasicInformationCluster.cpp`

  > This ScheduleUrgentEventDeliverySync() call is critical.  I don't see anything replacing it, unless I am seriously missing something.  Changes requested for this part; not having this breaks shutdown events completely, so we should either do this or stop generating them, because no one will ever see them without this call.

- **@bzbarsky-apple** on `src/app/clusters/basic-information/BasicInformationCluster.cpp`

  > Why the explicit specialization?  mLocalConfigDisabled is a bool; that will get the right instantiation, no?

- **@andy31415** on `src/app/clusters/basic-information/CodegenIntegration.cpp`

  > we do this because of dynamic cluster support: we do not know in advance what is enabled until dynamic clusters are set up.


## PR [#40788](https://github.com/project-chip/connectedhomeip/pull/40788)

- **@shripad621git** on `src/app/clusters/general-commissioning-server/general-commissioning-logic.cpp`

  > Done.

- **@shripad621git** on `src/app/clusters/general-commissioning-server/general-commissioning-cluster.cpp`

  > Removed the debug print.

- **@shripad621git** on `src/app/clusters/general-commissioning-server/CodegenIntegration.cpp`

  > Removed the debug print.

- **@shripad621git** on `src/app/clusters/network-commissioning/NetworkCommissioningLogic.cpp`

  > Removed the debug print.

- **@andy31415** on `src/app/clusters/general-commissioning-server/CodegenIntegration.cpp`

  > use https://github.com/project-chip/connectedhomeip/blob/master/src/data-model-providers/codegen/ClusterIntegration.h to save some minor flash and readability for registration/unregistration.

- **@andy31415** on `src/app/clusters/general-commissioning-server/general-commissioning-cluster.cpp`

  > remove extra `{}` throughout.

- **@andy31415** on `src/app/clusters/general-commissioning-server/general-commissioning-logic.h`

  > This is NOT allowed in code driven: accessors are ember RAM buffer

- **@andy31415** on `src/app/clusters/general-commissioning-server/general-commissioning-cluster.cpp`

  > This is not allowed: this getter is ember RAM buffer. What you should have is a breadcrumb member that you can get/set.
  > 
  > Then other integrations (specifically network commissioning) should update this member.

- **@andy31415** on `src/app/clusters/general-commissioning-server/general-commissioning-cluster.cpp`

  > How about network recovery?

- **@andy31415** on `src/app/clusters/general-commissioning-server/general-commissioning-cluster.cpp`

  > should we set a TODO on network recovery? I see that would have RecoveryIdentifier, NetworkRecoveryReason and IsCommissioningWithoutPower. May want to add a TODO and add those in to still return UnsupportedAttribute

- **@shripad621git** on `src/app/clusters/general-commissioning-server/CodegenIntegration.cpp`

  > Done.

- **@shripad621git** on `src/app/clusters/general-commissioning-server/general-commissioning-cluster.cpp`

  > Done.

- **@shripad621git** on `src/app/clusters/general-commissioning-server/general-commissioning-logic.h`

  > Done.

- **@shripad621git** on `src/app/clusters/general-commissioning-server/general-commissioning-cluster.cpp`

  > Done.

- **@shripad621git** on `src/app/clusters/general-commissioning-server/general-commissioning-cluster.cpp`

  > Done.

- **@andy31415** on `src/app/clusters/general-commissioning-server/general-commissioning-cluster.h`

  > cluster does not support any features. We don't need this member and can just return 0 in attribut reads.

- **@andy31415** on `src/app/clusters/general-commissioning-server/general-commissioning-cluster.cpp`

  > Is there anything to do here? No attributes are writable, so I would say lets not implement it and leave it to the DefaultServerCluster implementation which returns the spec-correct `UnsupportedWrite` rather than not implemented:
  > 
  > https://github.com/project-chip/connectedhomeip/blob/master/src/app/server-cluster/DefaultServerCluster.cpp#L116

- **@shripad621git** on `src/app/clusters/general-commissioning-server/general-commissioning-cluster.h`

  > I guess we require it as it has the termsandcondition feature, also network recovery feature.

- **@shripad621git** on `src/app/clusters/general-commissioning-server/general-commissioning-cluster.cpp`

  > The  breadcrumb attribute is writable. The CI tests as well as my local test to write the breadcrumb failed as WriteAttribute was not implemented. Implemented and tested the breadcrumb write successfully post the change.

- **@andy31415** on `src/app/clusters/general-commissioning-server/tests/TestGeneralCommissioningCluster.cpp`

  > This test has change detector vibes: it validates against fixed values in the same way the code does. However the code seems to take `optionalAttributeBits` so how about we test those?
  > 
  > That would also make this method play nicely in case some other method changes optionalAttributeBits, since as it stands right now, this method may fail depending on how optional attributes are set.

- **@andy31415** on `src/app/clusters/general-commissioning-server/general-commissioning-cluster.cpp`

  > ```suggestion
  >  *    Copyright (c) 2021-2025 Project CHIP Authors
  > ```
  > 
  > I think we are modifying and re-using a lot of existing code, so if we update the license we should keep the old year at least.

- **@andy31415** on `src/app/clusters/general-commissioning-server/tests/TestGeneralCommissioningCluster.cpp`

  > This is both define AND feature map dependent. At the same time the constructor only takes the optional attributes and not T&C define ... a comment should explain that we hard code the feature map (and we should actually do it)

- **@andy31415** on `src/app/clusters/general-commissioning-server/general-commissioning-cluster.cpp`

  > do we need a member? I see we generally do not pass this in the constructor, so I assumed this is fixed by `#define` settings. Maybe a `kFeatures` as a constant would be better.

- **@andy31415** on `src/app/clusters/general-commissioning-server/general-commissioning-cluster.cpp`

  > SetBreadCrumb handles notification, so then you do not need a WriteImpl (so you do not double-notify)

- **@andy31415** on `src/app/clusters/general-commissioning-server/general-commissioning-cluster.cpp`

  > Even more: SetBreadCrumb does the right thing on not notifying if noop ... so we really do not want a WriteImpl.


## PR [#40935](https://github.com/project-chip/connectedhomeip/pull/40935)

- **@zaid-google** on `src/app/clusters/descriptor/tests/TestDescriptorCluster.cpp`

  > Additional tests will be handled in a follow-up

- **@zaid-google** on `src/data-model-providers/codedriven/endpoint/SpanEndpoint.cpp`

  > Fixed

- **@zaid-google** on `src/app/clusters/descriptor/descriptor-cluster.cpp`

  > Done

- **@zaid-google** on `src/app/clusters/descriptor/CodegenIntegration.cpp`

  > Fixed this

- **@zaid-google** on `src/app/clusters/descriptor/CodegenIntegration.cpp`

  > Leaving this as-is to stay consistent with other CodegenIntegration files

- **@zaid-google** on `src/app/clusters/descriptor/descriptor-cluster.h`

  > Fixed typo, the comment however was mainly about pointing out how there is currently no optional attribute set like used in other clusters, and am looking at a macro instead to preserve functionality

- **@zaid-google** on `src/app/clusters/descriptor/descriptor-cluster.cpp`

  > I don't think my original code here was not correct. Removed this and now it simply encodes the feature map member variable

- **@zaid-google** on `src/data-model-providers/codedriven/CodeDrivenDataModelProvider.cpp`

  > Done

- **@zaid-google** on `src/data-model-providers/codedriven/endpoint/EndpointInterface.h`

  > Done

- **@zaid-google** on `src/app/clusters/descriptor/descriptor-cluster.cpp`

  > Fixed

- **@andy31415** on `src/app/common/templates/config-data.yaml`

  > Should we add this to ClustersWithShutdownFunctions as well since we have a shutdown function too?

- **@andy31415** on `src/app/clusters/descriptor/CodegenIntegration.cpp`

  > Should be false since this is callback (so ember has no such thing)

- **@andy31415** on `src/app/clusters/descriptor/CodegenIntegration.cpp`

  > use maxclustercount here

- **@zaid-google** on `src/app/common/templates/config-data.yaml`

  > Fixed this for now, but will need to be updated once #41006 is in

- **@zaid-google** on `src/app/clusters/descriptor/CodegenIntegration.cpp`

  > Fixed

- **@zaid-google** on `src/app/clusters/descriptor/CodegenIntegration.cpp`

  > Fixed

- **@bzbarsky-apple** on `src/darwin/Framework/CHIP/ServerEndpoint/MTRIMDispatch.mm`

  > This won't work: there is no "per app" codegen involved here.
  > 
  > You probably need to just directly declare MatterDescriptorClusterInitCallback/MatterClusterServerShutdownCallback in this file.

- **@Copilot** on `src/app/clusters/descriptor/descriptor-cluster.cpp`

  > Using 'mPath.mEndpointId' instead of 'request.path.mEndpointId'. This should use the endpoint from the request parameter for consistency with other attribute reads.


## PR [#41064](https://github.com/project-chip/connectedhomeip/pull/41064)

- **@zaid-google** on `src/app/clusters/access-control-server/access-control-cluster.cpp`

  > No this isn't correct. The global attributes are not added this way in any of the clusters and the Append() function of the AttributeListBuilder does not accept these arguments. Global attributes are part of the existing append function implementation and do not need to be added explicitly like this

- **@zaid-google** on `src/app/clusters/access-control-server/tests/TestAccessControlCluster.cpp`

  > The suggested change here does not relate to the fix of checking for "mandatory global attributes". In any case, the check for global attributes already exists in this code since they are added to the `expectedBuilder`.

- **@zaid-google** on `src/app/clusters/access-control-server/access-control-cluster.cpp`

  > I've added `ChipErrorToImErrorMap()` from the old cluster code to fix the mapping issues

- **@zaid-google** on `src/app/clusters/access-control-server/tests/TestAccessControlCluster.cpp`

  > Fixed the typo

- **@andy31415** on `src/app/clusters/access-control-server/CodegenIntegration.cpp`

  > This needs to be converted to a InvokeCommand as well

- **@zaid-google** on `src/app/clusters/access-control-server/CodegenIntegration.cpp`

  > Forgot to add Access Control to `CommandHandlerInterfaceOnlyClusters`, so this was required for some of the generated `IMClusterCommandHandler.cpp` files. I've updated `config-data.yaml` so this is no longer needed and I've already handled the command in `InvokeCommand()`.

- **@tcarmelveilleux** on `src/app/clusters/access-control-server/access-control-cluster.cpp`

  > Exit should always be met. `exit:` is not `error:`.
  > 
  > Please do not use `exit` for a path not always reached. Move the ChipLogError i not the SuccessOrExit.

- **@tcarmelveilleux** on `src/app/clusters/access-control-server/access-control-cluster.cpp`

  > For scope-keeping:
  > 
  > ```suggestion
  >     if (entry != nullptr)
  >     {
  >         AclStorage::EncodableEntry encodableEntry(*entry);
  >         CHIP_ERROR err = encodableEntry.Stage();
  >         if (!err.Success()) {
  >            ChipLogError(DataManagement, "AccessControlCluster: event failed %" CHIP_ERROR_FORMAT, err.Format());
  >            return;
  >          }
  >          
  >         event.latestValue.SetNonNull(encodableEntry.GetStagingEntry());
  >         // NOTE: EncodableEntry can only be constructed from ref so we need to use it within the right scope
  >         // after we determined the entry is not null. This is why we repeat the generate event call.
  >         mContext->interactionContext.eventsGenerator.GenerateEvent(event, 0);
  >         return;
  >     }
  > 
  >     mContext->interactionContext.eventsGenerator.GenerateEvent(event, 0);
  > ```

- **@tcarmelveilleux** on `src/app/clusters/access-control-server/access-control-cluster.cpp`

  > ```suggestion
  > ```

- **@tcarmelveilleux** on `src/app/clusters/access-control-server/access-control-cluster.cpp`

  > ```suggestion
  > ```

- **@tcarmelveilleux** on `src/app/clusters/access-control-server/access-control-cluster.cpp`

  > ```suggestion
  >         VerifyOrReturn((mContext != nullptr) && (subjectDescriptor != nullptr));
  > ```

- **@tcarmelveilleux** on `src/app/clusters/access-control-server/access-control-cluster.cpp`

  > ```suggestion
  > ```

- **@zaid-google** on `src/app/clusters/access-control-server/access-control-cluster.cpp`

  > Fixed following your other suggestions

- **@andy31415** on `src/app/common/templates/config-data.yaml`

  > We should really keep this list sorted: faster lookup and less chances of merge conflicts

- **@andy31415** on `src/app/clusters/access-control-server/access-control-cluster.cpp`

  > ```suggestion
  >  *    Copyright (c) 2021-2025 Project CHIP Authors
  > ```
  > 
  > We have to keep the original year since we do reuse a lot of code

- **@andy31415** on `src/app/clusters/access-control-server/access-control-cluster.cpp`

  > ```suggestion
  >     return validationIterator.GetStatus();
  > ```

- **@andy31415** on `src/app/clusters/access-control-server/access-control-cluster.cpp`

  > remove extra brackets.
  > 
  > I wonder if we should have a `WriteImpl` and then just a `NotifyAttributeChangedIfSuccess(..., WriteImpl(...))` to have only one place where we call NotifyAttributeChangedIfSuccess.

- **@andy31415** on `src/app/clusters/access-control-server/access-control-cluster.cpp`

  > These are config defined, so I wonder if we could somehow do:
  > 
  > 
  > ```cpp
  >     constexpr Span<AttributeListBuilder::OptionalAttributeEntry> kOptionalEntries({
  >     #if CHIP_CONFIG_ENABLE_ACL_EXTENSIONS
  >         { true, Attributes::Extension::kMetadataEntry },
  >     #endif
  >     #if CHIP_CONFIG_USE_ACCESS_RESTRICTIONS
  >         { true, Attributes::CommissioningARL::kMetadataEntry },
  >         { true, Attributes::Arl::kMetadataEntry },
  >     #endif
  >     });
  > ```
  > 
  > I don't know if this actually compiles and empty array will not ... however maybe we can make it work somehow? Did you try something similar?
  > 
  > if not existing code is also ok, it just adds extra flash (minor) even if the features are disabled.

- **@andy31415** on `src/app/clusters/access-control-server/access-control-cluster.cpp`

  > remove extra return

- **@andy31415** on `src/app/clusters/access-control-server/access-control-cluster.cpp`

  > This is a private one-liner. Lets inline these directly, see if that saves a bit of flash.

- **@zaid-google** on `src/app/clusters/access-control-server/access-control-cluster.cpp`

  > I considered the `WriteImpl`, but I think it may be worse for flash? We either make WriteImpl a private function or add it to the anonymous namespace. If you do add it to anonymous namespace, I also need to pass ```mContext``` into it as well because it may be needed if calling ```WriteExtension()```, which is also protected behind an ifdef, so place that argument in an ifdef as well? This just felt a bit simpler to me, but I can try it out if you think it's the better approach

- **@andy31415** on `src/app/clusters/access-control-server/access-control-cluster.cpp`

  > Ah ... this is an override ... never mind.

- **@zaid-google** on `src/app/clusters/access-control-server/access-control-cluster.cpp`

  > I tried this, but it doesn't seem to compile either. Originally made the code this way to avoid the issue of declaring an empty array (which also wouldn't compile).

- **@zaid-google** on `src/app/clusters/access-control-server/access-control-cluster.cpp`

  > Done

- **@zaid-google** on `src/app/clusters/access-control-server/access-control-cluster.cpp`

  > Done

- **@zaid-google** on `src/app/clusters/access-control-server/access-control-cluster.cpp`

  > Fixed

- **@zaid-google** on `src/app/common/templates/config-data.yaml`

  > Done


## PR [#41232](https://github.com/project-chip/connectedhomeip/pull/41232)

- **@zaid-google** on `src/app/clusters/identify-server/app_config_dependent_sources.gni`

  > I think `CodegenIntegration.h` should also be here?

- **@andy31415** on `src/app/clusters/identify-server/IdentifyCluster.h`

  > bools are somewhat harder to read. How about an `enum class` that is either `kClient` or `kCommand`. The code size will likely be equivalent, however the code will be much more readable.

- **@andy31415** on `src/app/clusters/identify-server/IdentifyCluster.cpp`

  > You need to return a "no write" result when identify time is unchanged.
  > 
  > https://github.com/project-chip/connectedhomeip/blob/master/src/app/data-model-provider/ActionReturnStatus.h#L51

- **@andy31415** on `src/app/clusters/identify-server/IdentifyCluster.cpp`

  > This method overall feels odd:
  > 
  > - seems like a post-processing function that uses mPreviousIdentifyTime, which is required to be stored by the caller. I would prefer this method being a `SetIdetifyTime` and it can store previous time as a local variable. Saves some minor ram and makes the code clearer without imposing prerequisites on the caller.
  > 
  > - Logic for changed or not could be encapsulating by returning kWriteSuccessNoOp (https://github.com/project-chip/connectedhomeip/blob/master/src/app/data-model-provider/ActionReturnStatus.h#L51) if we want to indicate a "success but do not modify". Unsure if this makes the code more readable, however a single place for calling NotifyAttributeChanged seems desirable.

- **@soares-sergio** on `src/app/clusters/identify-server/app_config_dependent_sources.gni`

  > Yup and `identify-server.h` as well. thanks

- **@andy31415** on `src/app/clusters/identify-server/IdentifyCluster.cpp`

  > I think we should just `ReturnErrorOnFailure` on these. If decoding must return invalid command, it should be up to the command decoder to return a correct CHIP_GLOBAL_IM_ERROR

- **@andy31415** on `src/app/clusters/identify-server/IdentifyCluster.cpp`

  > no need for the cast. Can return and it will auto-cast.

- **@andy31415** on `src/app/clusters/identify-server/IdentifyCluster.cpp`

  > you can return the invalid command right away - defaultservercluster cannot know any more about this processing that the implementation. You are fully handling this and do not require the base class here or in other parts that you have the authoritative answer.

- **@andy31415** on `src/app/clusters/identify-server/IdentifyCluster.cpp`

  > can we have this be part of CodegenIntegration and force callers of the real cluster to always provide this?

- **@andy31415** on `src/app/clusters/identify-server/CodegenIntegration.cpp`

  > you must use the constants here. This is a common footgun, however what ember requires is to know its static counts to find the correct cluster. this MUST be the ember fixed cluster `size()` for ember calls to work.
  > 
  > Then again I am unclear if we want to have the CodegenClusterIntegration at all here ... you do not use any of `clusterInstanceIndex` , `optionalAttributeBits` or `featureMap` so I would say we should NOT try to use this.
  > 
  > What you probably do need is detection of enabled commands since you have an optional command.

- **@soares-sergio** on `src/app/clusters/identify-server/IdentifyCluster.h`

  > Yupp, also didn't like that bool. Fixed, thanks!

- **@soares-sergio** on `src/app/clusters/identify-server/IdentifyCluster.cpp`

  > I think I fixed this, also added a unit test. Could you double check if this is correct now? thanks.

- **@soares-sergio** on `src/app/clusters/identify-server/IdentifyCluster.cpp`

  > Yupp, that `mPreviousIdentifyTime` was ugly. Fixed like you suggested, much better now thanks.

- **@soares-sergio** on `src/app/clusters/identify-server/IdentifyCluster.cpp`

  > Removed casts, thanks!

- **@soares-sergio** on `src/app/clusters/identify-server/IdentifyCluster.cpp`

  > Yup, Moved `sDefaultTimerDelegate` to CodegenIntegration and made timerDelegate a reference

- **@soares-sergio** on `src/app/clusters/identify-server/IdentifyCluster.cpp`

  > Fixed, used `ReturnErrorOnFailure` as suggested.

- **@soares-sergio** on `src/app/clusters/identify-server/CodegenIntegration.cpp`

  > Got rid of the IntegrationDelegate entirely. thanks

- **@soares-sergio** on `src/app/clusters/identify-server/IdentifyCluster.cpp`

  > Done

- **@andy31415** on `src/app/clusters/identify-server/tests/TestIdentifyCluster.cpp`

  > using globals feels like a bit like it could cause ordering issues especially if some test fails. I don't think this is the case here as you are resetting them in startup, however wondering what would prevent us from building some RAII on this. And the reasoning seems to be the `on*` methods that just take in the cluster. If we would allow a `intptr_t` argument like generic C callbacks, we would allow a context or, maybe even better, if we would allow a Delegate pattern we could subclass it and make it work.
  > 
  > So could we:
  >   - Make the callbacks be actually a pure virtual class of IdentifyDelegate that has methods and RAAI this?
  >   - or (a bit uglier) make the callbacks take a intptr_t and still RAII this with propper casts?

- **@andy31415** on `src/app/clusters/identify-server/tests/TestIdentifyCluster.cpp`

  > We may want a context for every test that needs one: the test context contains several accumulators (e.g. for events) and starting fresh each time seems desirable.

- **@andy31415** on `src/app/clusters/identify-server/tests/TestIdentifyCluster.cpp`

  > this could be a globak kTestEndpointId instead of a const in every test.

- **@andy31415** on `src/app/clusters/identify-server/tests/TestIdentifyCluster.cpp`

  > This is a change detector and a buggy one at that: when our codegen changes the version, we will start failing this and will require a manual update.
  > 
  > lets delete this entire test.

- **@andy31415** on `src/app/clusters/identify-server/tests/TestIdentifyCluster.cpp`

  > Please remove change detector tests. I believe this is one as well, let's delete it. This cluster has no features.

- **@andy31415** on `src/app/clusters/identify-server/tests/TestIdentifyCluster.cpp`

  > Check that "change notifications" are properly enqueued. That is a sideffect that should work.

- **@soares-sergio** on `src/app/clusters/identify-server/tests/TestIdentifyCluster.cpp`

  > There's already a more complete test that validates the change notifications `IdentifyTimeAttributeReportingTest`. I'll remove this one

- **@soares-sergio** on `src/app/clusters/identify-server/tests/TestIdentifyCluster.cpp`

  > Good suggestion, added a delegate, looks nicer. I hope it doesn't increase code size too much. thank you 👍

- **@soares-sergio** on `src/app/clusters/identify-server/tests/TestIdentifyCluster.cpp`

  > Moved this to the individual test cases. Thanks!

- **@soares-sergio** on `src/app/clusters/identify-server/tests/TestIdentifyCluster.cpp`

  > Yup, good idea, moved to a globak kTestEndpointId

- **@soares-sergio** on `src/app/clusters/identify-server/tests/TestIdentifyCluster.cpp`

  > Removed the check for v5 but kept the read as it checks it doesn't error out.


## PR [#41849](https://github.com/project-chip/connectedhomeip/pull/41849)

- **@Copilot** on `src/app/clusters/boolean-state-configuration-server/boolean-state-configuration-cluster.cpp`

  > The logic for `mDefaultSensitivityLevel` is incorrect. It should use `std::min` instead of `std::max` to clamp the value to be less than `mSupportedSensitivityLevels`. Currently, if `defaultSensitivityLevel` is 1 and `mSupportedSensitivityLevels` is 3, this would set it to 2 (max of 1 and 2), but the intent appears to be ensuring it doesn't exceed the maximum, not that it meets a minimum.
  > ```suggestion
  >     mDefaultSensitivityLevel(std::min(config.defaultSensitivityLevel, static_cast<uint8_t>(mSupportedSensitivityLevels - 1))),
  > ```

- **@Copilot** on `src/app/clusters/boolean-state-configuration-server/boolean-state-configuration-cluster.cpp`

  > Line 319 should clear `mAlarmsSuppressed` but is instead clearing `mAlarmsActive` again. This is a copy-paste error that prevents the AlarmsSuppressed attribute from being properly cleared.
  > ```suggestion
  >         mAlarmsSuppressed.ClearAll();
  > ```

- **@Copilot** on `src/app/clusters/boolean-state-configuration-server/CodegenIntegration.cpp`

  > The namespace `chip::app::Clusters::BooleanStateConfiguration::Attributes` is imported twice (lines 30 and 32). Remove the duplicate import on line 32 as it's redundant with line 30.
  > ```suggestion
  > 
  > ```

- **@Copilot** on `src/app/clusters/boolean-state-configuration-server/boolean-state-configuration-cluster.cpp`

  > Missing namespace qualification for `kMandatoryMetadata`. Should be `BooleanStateConfiguration::Attributes::kMandatoryMetadata` to match the pattern used in other code-driven clusters. The unqualified reference may fail to compile or resolve to the wrong symbol.
  > ```suggestion
  >     return listBuilder.Append(Span(BooleanStateConfiguration::Attributes::kMandatoryMetadata), Span<const AttributeEntry>{ optionalAttributesMeta },
  > ```

- **@andy31415** on `src/app/clusters/boolean-state-configuration-server/boolean-state-configuration-cluster.cpp`

  > It compiles fine.

- **@Copilot** on `src/app/clusters/boolean-state-configuration-server/boolean-state-configuration-server.h`

  > The deprecated API `SetCurrentSensitivityLevel` does not propagate the CHIP_ERROR return value from `cluster->SetCurrentSensitivityLevel(level)`. This method can return errors (e.g., CHIP_IM_GLOBAL_STATUS(ConstraintError)), but the wrapper always returns CHIP_NO_ERROR. This should be:
  > ```cpp
  > return cluster->SetCurrentSensitivityLevel(level);
  > ```
  > ```suggestion
  >     return cluster->SetCurrentSensitivityLevel(level);
  > }
  > ```

- **@Copilot** on `src/app/clusters/boolean-state-configuration-server/boolean-state-configuration-server.h`

  > Remove the extra blank line at the beginning of the function body. This is inconsistent with the other inline functions in this file.
  > ```suggestion
  > 
  > ```

- **@zaid-google** on `src/app/clusters/boolean-state-configuration-server/boolean-state-configuration-cluster.cpp`

  > ```suggestion
  > #include <app/clusters/boolean-state-configuration-server/boolean-state-configuration-cluster.h>
  > ```

- **@zaid-google** on `src/app/clusters/boolean-state-configuration-server/boolean-state-configuration-cluster.cpp`

  > I'm unsure why we need both `enabledOptionalAttributes` and `mOptionalAttributes`? Can we not modify `mOptionalAttributes` directly based on the feature map?

- **@andy31415** on `src/app/clusters/boolean-state-configuration-server/boolean-state-configuration-cluster.cpp`

  > The reason I had this was `mOptionalAttributes` is restricted as a type to `OptionalAttributeSet` which is only truly optional attributes (i.e. not the ones guided by the feature map).
  > 
  > I can move this type to the class itself and use the feature map to set the mOptionalAttributes as well, I believe the same code would run either way. Will make the change and see what looks more readable.

- **@andy31415** on `src/app/clusters/boolean-state-configuration-server/boolean-state-configuration-cluster.cpp`

  > Updated and used https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#Res-lambda-init to initialize a const variable.... unsure if this is great readability though...

- **@bzbarsky-apple** on `src/app/clusters/boolean-state-configuration-server/boolean-state-configuration-cluster.cpp`

  > ```suggestion
  >         // constructs the attribute set, that once constructed stays const
  > ```

- **@bzbarsky-apple** on `src/app/clusters/boolean-state-configuration-server/boolean-state-configuration-cluster.cpp`

  > Shouldn't we just fail startup if configured with AlarmSuppress but not Visual or Audible.

- **@bzbarsky-apple** on `src/app/clusters/boolean-state-configuration-server/boolean-state-configuration-cluster.cpp`

  > Would have been nice to have the code moves in a separate PR...

- **@Copilot** on `src/app/clusters/boolean-state-configuration-server/boolean-state-configuration-cluster.cpp`

  > The check for no-op should occur before validation to avoid unnecessary error checking when the value hasn't changed. Move this line before line 276.
  > ```suggestion
  >     VerifyOrReturnError(mCurrentSensitivityLevel != level, CHIP_NO_ERROR);
  >     VerifyOrReturnError(level < mSupportedSensitivityLevels, CHIP_IM_GLOBAL_STATUS(ConstraintError));
  > ```

- **@bzbarsky-apple** on `src/app/clusters/boolean-state-configuration-server/boolean-state-configuration-cluster.cpp`

  > If we are renaming it anyway, it should be called GenerateAlarmsStateChangedEvent, and the boolean guard should be called "generateEvent".

- **@bzbarsky-apple** on `src/app/clusters/boolean-state-configuration-server/boolean-state-configuration-cluster.cpp`

  > Again, if we are renaming, GenerateSensorFault.

- **@bzbarsky-apple** on `src/app/clusters/boolean-state-configuration-server/boolean-state-configuration-server.h`

  > Why the various `chip::` prefixes here?  This is inside `namespace chip`.

- **@bzbarsky-apple** on `src/app/clusters/boolean-state-configuration-server/tests/TestBooleanStateConfigurationCluster.cpp`

  > Did not review this part.

- **@andy31415** on `src/app/clusters/boolean-state-configuration-server/boolean-state-configuration-cluster.cpp`

  > We have no precedent for this - currently all Startup() calls always succeed.
  > 
  > I can do the check if that seems reasonable. The trouble I have is that our constructor takes all these arguments and the arguments allow invalid state repreesntation (i.e. suppress without visual/audible alarm).
  > 
  > I believe this is better than before: code enforces some spec correctness even if receiving invalid values in constructors.

- **@andy31415** on `src/app/clusters/boolean-state-configuration-server/boolean-state-configuration-cluster.cpp`

  > I did have one pass where I moved all methods in the same order that they were before. Unfortunately the changes are too large.

- **@andy31415** on `src/app/clusters/boolean-state-configuration-server/boolean-state-configuration-cluster.cpp`

  > Code size will be the same and I would like to preserve previous logic (I guess this would check if the current level is invalid ... even though that seems unlikely).

- **@bzbarsky-apple** on `src/app/clusters/boolean-state-configuration-server/boolean-state-configuration-cluster.cpp`

  > > We have no precedent for this - currently all Startup() calls always succeed.
  > 
  > We certainly have precedent for clusters with Init() calls that can fail.  How do we plan to model that in the new setup?
  > 
  > I agree this is better than before, and I am fine with other improvements in followups, but I feel like we should be able to handle this better than this PR does.

- **@bzbarsky-apple** on `src/app/clusters/boolean-state-configuration-server/boolean-state-configuration-cluster.cpp`

  > Too large for what?  When I was reviewing this, I had to try to match up the removed/added bits across the file... and I have very low confidence I got it right.

- **@zaid-google** on `src/app/clusters/boolean-state-configuration-server/boolean-state-configuration-cluster.cpp`

  > I think this looks okay. Either way is fine in my opinion, in the end I just wanted to know why this was needed.

## PR [#41954](https://github.com/project-chip/connectedhomeip/pull/41954)

- **@andy31415** on `src/app/clusters/occupancy-sensor-server/OccupancySensingCluster.cpp`

  > SCI accessors must only call read/write/invoke when the path is valid. So as long as you correctly return these in `::Attributes` there is no need for you to check (and explicitly you should not check, otherwise people will copy & paste larger code and add extra maintainability overhead to understand the code).

- **@soares-sergio** on `src/app/clusters/occupancy-sensor-server/OccupancySensingCluster.cpp`

  > Yup! Removed the redundant check, thanks!

- **@tcarmelveilleux** on `src/app/clusters/occupancy-sensor-server/README.md`

  > This must be called on the Matter context, so a ScheduleWork must be used if done from a different application thread. This is no different than other times but it's good to mention it.

- **@tcarmelveilleux** on `src/app/clusters/occupancy-sensor-server/README.md`

  > Also need an example of when it's based on legacy Ember callbacks and global ::Set calls on the occupancy attribute. Some guide of how to take a previous sample and making it work would be great to have, that also properly gets the instance from the endpoint using calls like your new get cluster on endpoint.
  > 
  > You have to allude that when transitioning to the new method, you should not maintain the holdtime timer on your application directly,

- **@tcarmelveilleux** on `src/app/clusters/occupancy-sensor-server/README.md`

  > I see some of this is done below, but no mention of removing the global attribute setter/getters or usage of post attribute change callback for the old Occupied->UnoccupiedDelay attributes

- **@tcarmelveilleux** on `src/app/clusters/occupancy-sensor-server/OccupancySensingCluster.cpp`

  > I think here it's better not to blow-up on fail...

- **@tcarmelveilleux** on `src/app/clusters/occupancy-sensor-server/OccupancySensingCluster.cpp`

  > Why reinterpret cast explicitly? This is dangerous to fail if someone ever changes the width. Suggest making the width mandatory as N bytes and doing explicit conversion of original type to a byte array with a given width to catch such things.

- **@tcarmelveilleux** on `src/app/clusters/occupancy-sensor-server/OccupancySensingCluster.cpp`

  > There should be an option to decide whether to show the previous delay attributes or not... They are deprecated, but can/should be enabled by default for legacy compat.

- **@tcarmelveilleux** on `src/app/clusters/occupancy-sensor-server/OccupancySensingCluster.cpp`

  > Suggest `newHoldTimeSeconds`. The 16 bits is not relevant, but the units are and I was wondering on first reading.

- **@tcarmelveilleux** on `src/app/clusters/occupancy-sensor-server/OccupancySensingCluster.cpp`

  > Why log this? It's likely not recoverable. Suggest avoiding the log on embedded builds (perhaps this is another config?)

- **@tcarmelveilleux** on `src/app/clusters/occupancy-sensor-server/OccupancySensingCluster.cpp`

  > Hold time limits are not supposed to change... They are fixed. So there should be no need to notify, and IIRC my original impl let the driver/delegate report the limits. Since you handle the hold time timer yourself, perhaps handle the hold time getting from the delegate, but on error or not implemented, use sane defaults

- **@soares-sergio** on `src/app/clusters/occupancy-sensor-server/README.md`

  > Updated readme to mention ScheduleWork, thanks!

- **@soares-sergio** on `src/app/clusters/occupancy-sensor-server/README.md`

  > Updated Readme with some notes about global attribute getters/setters being removed and also post attribute change callbacks. Thanks!

- **@soares-sergio** on `src/app/clusters/occupancy-sensor-server/OccupancySensingCluster.cpp`

  > Renamed to newHoldTimeSeconds, thanks.

- **@soares-sergio** on `src/app/clusters/occupancy-sensor-server/OccupancySensingCluster.cpp`

  > I added this just a convenience method.
  > The application should really just pass it in the constructor if possible. But I added SetHoldLimits in case the application is not ready to set this at the time of instantiation (maybe needs to talk to a device to get the limits?) and then can set this later on... I can make this private and let the application only set this via constructor if you prefer.

- **@soares-sergio** on `src/app/clusters/occupancy-sensor-server/OccupancySensingCluster.cpp`

  > Removed the log, thanks.

- **@soares-sergio** on `src/app/clusters/occupancy-sensor-server/OccupancySensingCluster.cpp`

  > Currently, I'm always enabling the legacy attributes if holdTime is enabled and the featureMap bit is set for the specific sensor type.
  > 
  > Do you want an option to DISABLE showing the legacy attributes? I can add that if it's useful but I thought it would be OK to just show both holdTime + the legacy equivalent?

- **@soares-sergio** on `src/app/clusters/occupancy-sensor-server/OccupancySensingCluster.cpp`

  > Removed erroring out on startup. Thanks

- **@soares-sergio** on `src/app/clusters/occupancy-sensor-server/OccupancySensingCluster.cpp`

  > I see. I'll fix this thanks.

- **@soares-sergio** on `src/app/clusters/occupancy-sensor-server/OccupancySensingCluster.cpp`

  > Fixed thanks!

- **@soares-sergio** on `src/app/clusters/occupancy-sensor-server/OccupancySensingCluster.cpp`

  > Added a flag to the Config to optionally hide the deprecated attributes (enabled by default).

- **@soares-sergio** on `src/app/clusters/occupancy-sensor-server/OccupancySensingCluster.cpp`

  > I'll remove this setter as discussed, since the app can do this via constructor and now the codedriven clusters can be instantiated at any time. We can revisit in the future if necessary

- **@soares-sergio** on `src/app/clusters/occupancy-sensor-server/OccupancySensingCluster.cpp`

  > Done!

- **@andy31415** on `src/app/clusters/occupancy-sensor-server/CodegenIntegration.cpp`

  > shifts feel awkward. How about using:
  > 
  > ```suggestion
  >         if (AttributeSet(optionalAttributeBits).IsSet(Attributes::HoldTime::Id)|| hasSensorFeature)
  > ```

- **@andy31415** on `src/app/clusters/occupancy-sensor-server/CodegenIntegration.cpp`

  > Could we describe where these defaults came from? I don't see any spec defaults really.

- **@andy31415** on `src/app/clusters/occupancy-sensor-server/OccupancySensingCluster.cpp`

  > consider switching the order to be PHY,US,PIR so I can read the numbers from the mask as binary (since lowest bit is PIR). could maybe also add a "mask value" column for clear read.

- **@andy31415** on `src/app/clusters/occupancy-sensor-server/CodegenIntegration.cpp`

  > where is `SetHoldTimeLimits` ? I don't see it in a grep on CodegenIntegration.h

- **@andy31415** on `src/app/clusters/occupancy-sensor-server/OccupancySensingCluster.h`

  > should some of these be const? I don't think we allow feature map modification past creation, hold time limits has no write option

- **@andy31415** on `src/app/clusters/occupancy-sensor-server/OccupancySensingCluster.h`

  > should this be a reference to not allow nullptr?


## PR [#42331](https://github.com/project-chip/connectedhomeip/pull/42331)

- **@soares-sergio** on `src/app/clusters/chime-server/ChimeCluster.cpp`

  > Pre-existing bug. Fixed anyways.

- **@soares-sergio** on `src/app/clusters/chime-server/ChimeCluster.cpp`

  > Pre-existing bug. Fixed anyways.

- **@soares-sergio** on `src/app/common/templates/config-data.yaml`

  > This was already there.

- **@lpbeliveau-silabs** on `src/app/clusters/chime-server/README.md`

  > I would remove references to Ember whenever possible.

- **@lpbeliveau-silabs** on `src/app/clusters/chime-server/tests/TestChimeClusterBackwardsCompatibility.cpp`

  > I know this was llm generated but I do not think it is relevant that we are emulating ember.

- **@lpbeliveau-silabs** on `src/app/clusters/chime-server/ChimeCluster.cpp`

  > Was this forgotten or do we intentionally want to do the conversion in iterative steps?

- **@lpbeliveau-silabs** on `src/app/clusters/chime-server/ChimeCluster.cpp`

  > This is only called here and in the tests, could we instead use `mPath.mEndpointId` in the meantime so we don't leave the method in when we remove the AttributePersistenceProvider dependency?

- **@lpbeliveau-silabs** on `src/app/clusters/chime-server/CodegenIntegration.cpp`

  > I notice this is the same comment as in the identify server's code integration, are we expected to have this comment whenever we call `CodegenDataModelProvider::Instance()`?

- **@lpbeliveau-silabs** on `src/app/clusters/chime-server/ChimeCluster.h`

  > Should we use DataModel::ActionReturnStatus instead for the status codes?

- **@lpbeliveau-silabs** on `src/app/clusters/chime-server/ChimeCluster.h`

  > Why is this not in its own ChimeDelegate.h file?

- **@lpbeliveau-silabs** on `src/app/clusters/chime-server/ChimeCluster.h`

  > My assumption was that the delegate was meant for the chip task to interact with the app, not the other way around.
  > 
  > Now if the app task, through its delegate calls set or get on the clusters's attribute while at the same time a command is performing a read or write, we should need some synchronisation mechanism no?

- **@lpbeliveau-silabs** on `src/app/clusters/chime-server/ChimeCluster.h`

  > Since the getters and setters are public and the delegate exposes the cluster to the app, shouldn't we have some synchronization mechanism for the various Set & Get?

- **@lpbeliveau-silabs** on `src/app/clusters/chime-server/ChimeCluster.cpp`

  > This is probably out of the scope of this PR,  but as it is right now, the `ChimeDelegate` from camera linux app calls:
  > ```
  >     // check if we are enabled
  >     if (!mChimeServer->GetEnabled())
  >     {
  >         return Protocols::InteractionModel::Status::Failure;
  >     }
  > ```
  > Without a nullptr check. So having the cluster setting a nullptr in its destructor might be a source of problems in the future.
  > 
  > Or do we assume the lifetime of the object makes this impossible to happen?

- **@lpbeliveau-silabs** on `src/app/clusters/chime-server/ChimeCluster.cpp`

  > Isn't this redundant? We would call the parent method without this right?

- **@lpbeliveau-silabs** on `src/app/clusters/chime-server/ChimeCluster.cpp`

  > The constructor doesn't call Startup, so we assume it was called in the usage of the cluster. Should we also assume this for the Shutdown? I know it doesn't do much but just for the sake of consistency.

- **@lpbeliveau-silabs** on `src/app/clusters/chime-server/ChimeCluster.cpp`

  > There is also a condition in the Spec that says we should do the same behaviour if a chime is currently playing, could we instrument the delegate with a "IsPlaying" method to verify this?

- **@andy31415** on `src/app/clusters/chime-server/ChimeCluster.cpp`

  > Assuming we do not set nullptr, this would result in a wrong memory access instead, so bad either way.
  > In my mind if we set things to nullptr, at least the delegate has a chance to check, so this is probably a good improvement.

- **@andy31415** on `src/app/clusters/chime-server/ChimeCluster.cpp`

  > Agree with this - I think we should be symmetric: e.g. if an `Init/Startup` does X, then `Shutdown/Deinit` can undo X. However if constructor does not do X, we should not try to undo in destructor. From that perspective, I thinkg setting chime cluster to nullptr is ok (constructor sets it to non-null), but shutdown is unclear. If we do shutdown, we need a comment explaining why.

- **@soares-sergio** on `src/app/clusters/chime-server/ChimeCluster.cpp`

  > Intentionally. We'll do that as a follow up once the SafeAttribute migration path is well defined.

- **@andy31415** on `src/app/clusters/chime-server/ChimeCluster.cpp`

  > ```suggestion
  >         return status;
  > ```

- **@andy31415** on `src/app/clusters/chime-server/ChimeCluster.cpp`

  > ```suggestion
  >         return Status::UnsupportedCommand;
  > ```

- **@andy31415** on `src/app/clusters/chime-server/ChimeCluster.h`

  > could we include just the `<clusters/Chime/...>` headers to be more specific? cluster-objects is a "include the world"


## PR [#42634](https://github.com/project-chip/connectedhomeip/pull/42634)

- **@Copilot** on `src/app/clusters/on-off-server/BUILD.gn`

  > The file OnOffEffectDelegate.h is missing from the sources list in BUILD.gn. Header-only files used by the implementation should be included in the sources list for proper dependency tracking and IDE support.
  > ```suggestion
  >     "OnOffDelegate.h",
  >     "OnOffEffectDelegate.h",
  > ```

- **@Copilot** on `src/app/clusters/on-off-server/OnOffDelegate.h`

  > The word "togggled" is misspelled in the comment. It should be "toggled".
  > ```suggestion
  >     /// enum (i.e. it would be forced on or off or toggled)
  > ```

- **@andy31415** on `src/app/clusters/on-off-server/OnOffLightingCluster.cpp`

  > This bit I am unclear of ... I generally would expect a reboot to be a noop, so to start the same every time if the cluster is not interacted with.
  > 
  > This proposes that every reboot toggles the state ... this could be done as well however I am unsure if we should pay for that complexity or not :/

- **@Copilot** on `src/app/clusters/on-off-server/OnOffLightingCluster.h`

  > The documentation for SetOnOffWithTimeReset is unclear. The phrase "however it is specific for needs other integrations" is grammatically awkward. Consider revising to something like "This method is specifically designed for integration with other clusters" or "This method is intended for use by other clusters when setting the OnOff value".
  > ```suggestion
  >     /// Sets the OnOff value. This method is intended for use by other clusters when updating
  >     /// the OnOff attribute.
  >     ///
  >     /// In particular, the Lighting feature (1.5.4.1) requires that if the Level Control cluster
  >     /// sets the OnOff value then:
  >     ///   - if the value is set to false, OnTime is set to 0
  >     ///   - if the value is set to true, OffWaitTime is set to 0
  > ```

- **@Copilot** on `src/app/clusters/on-off-server/OnOffLightingCluster.h`

  > The comment contains a grammatical error. "if level control cluster sets then onoff value then" should be "if the level control cluster sets the onoff value then".
  > ```suggestion
  >     /// In particular, the 1.5.4.1 Lighting feature requires that if the Level Control cluster sets
  >     /// the OnOff value, then:
  > ```

- **@Copilot** on `src/app/clusters/on-off-server/OnOffLightingCluster.cpp`

  > The comment contains a grammatical error. "the correct on-off clusters setup" should be "the correct on-off cluster setup" (cluster should be singular).
  > ```suggestion
  >     //       the correct on-off cluster setup.
  > ```

- **@Copilot** on `src/app/clusters/scenes-server/ScenesIntegrationDelegate.h`

  > The documentation has a grammatical error. The text "Stores the current state of into the Global Scene" is missing words. It should read something like "Stores the current state into the Global Scene" or "Stores the current state of the cluster into the Global Scene".
  > ```suggestion
  >     /// Stores the current state into the Global Scene for the given fabric.
  > ```

- **@Copilot** on `src/app/clusters/scenes-server/ScenesIntegrationDelegate.h`

  > The comment style is inconsistent. This line uses `//` while the line above uses `// ==`. For consistency with the rest of the file, this should use the same style as line 34 and 43, with `// ==` delimiters for section headers.
  > ```suggestion
  >     // == Scene Invalidation Methods ==
  > 
  >     /// Marks all scenes on the associated endpoint as invalid for all fabrics.
  > ```

- **@Copilot** on `src/app/clusters/on-off-server/OnOffLightingCluster.cpp`

  > There's an extraneous blank line at the start of this function. This should be removed to maintain consistency with coding style in this file and similar functions like HandleOnWithRecallGlobalScene.
  > ```suggestion
  > 
  > ```

- **@Copilot** on `src/app/clusters/on-off-server/OnOffLightingCluster.cpp`

  > Potential null pointer dereference: `request.subjectDescriptor` is accessed without a null check. While the test infrastructure sets this pointer, production code should validate that `request.subjectDescriptor` is not null before dereferencing it, especially since the `InvokeRequest` structure allows this field to be optional.
  > ```suggestion
  >         {
  >             if (request.subjectDescriptor == nullptr)
  >             {
  >                 return Status::Failure;
  >             }
  > ```

- **@Copilot** on `src/app/clusters/on-off-server/OnOffLightingCluster.cpp`

  > Potential null pointer dereference: `request.subjectDescriptor` is accessed without a null check. While the test infrastructure sets this pointer, production code should validate that `request.subjectDescriptor` is not null before dereferencing it, especially since the `InvokeRequest` structure allows this field to be optional.

- **@Copilot** on `src/app/clusters/on-off-server/OnOffLightingCluster.h`

  > The documentation comment is unclear and contains a grammatical error. "Sets the on off value, however it is specific for needs other integrations" should be rewritten more clearly, for example: "Sets the OnOff value with special behavior for integration with other clusters" or "Sets the OnOff value, with specific behavior required for integration with other clusters."
  > ```suggestion
  >     /// Sets the OnOff value with special behavior for integration with other clusters.
  > ```

- **@Copilot** on `src/app/clusters/scenes-server/ScenesIntegrationDelegate.h`

  > The comment has a grammatical error - "Stores the current state of into the Global Scene" should be "Stores the current state into the Global Scene" (remove "of").
  > ```suggestion
  >     /// Stores the current state into the Global Scene for the fabric.
  > ```

- **@andy31415** on `src/app/clusters/on-off-server/OnOffLightingCluster.cpp`

  > subjectDescriptor must be non-null (since every request must originate from somewhere). 
  > 
  > The null was for "internal" request intents, however we ended up not really using that one. For now I think we are ok to assume non-null and eventually we should make it a reference.

- **@Copilot** on `src/app/clusters/scenes-server/ScenesIntegrationDelegate.h`

  > The comment for MakeSceneInvalidForAllFabrics is missing the documentation comment marker (///) to match the style of the other methods in this interface.
  > ```suggestion
  >     /// Marks all scenes on the associated endpoint as invalid for all fabrics.
  > ```

- **@Copilot** on `src/app/clusters/on-off-server/OnOffLightingCluster.cpp`

  > Missing UpdateTimer call after resetting time attributes. When OnTime or OffWaitTime is set to 0, UpdateTimer should be called to cancel the timer if no longer needed, similar to how HandleOff and HandleOn call UpdateTimer after modifying these attributes.
  > ```suggestion
  > 
  >     bool timersReset = false;
  > 
  >     if (!on && (mOnTime != 0))
  >     {
  >         mOnTime = 0;
  >         NotifyAttributeChanged(Attributes::OnTime::Id);
  >         timersReset = true;
  >     }
  > 
  >     if (on && (mOffWaitTime != 0))
  >     {
  >         mOffWaitTime = 0;
  >         NotifyAttributeChanged(Attributes::OffWaitTime::Id);
  >         timersReset = true;
  >     }
  > 
  >     if (timersReset)
  >     {
  >         UpdateTimer();
  >     }
  > ```

- **@Copilot** on `src/app/clusters/on-off-server/OnOffLightingCluster.h`

  > Typo in the description: "litghting" should be "lighting".

- **@Copilot** on `src/app/clusters/scenes-server/ScenesIntegrationDelegate.h`

  > An asterisk was removed from the copyright comment block, which changes its formatting. This appears to be an unintentional change that should be reverted to maintain consistency with the project's standard copyright format.

- **@Copilot** on `src/app/clusters/on-off-server/tests/TestOnOffCluster.cpp`

  > The OnOffClusterTestAccess class is defined in both the header file (OnOffCluster.h as a friend declaration at line 105) and in the test file (lines 39-48). The test file defines it within the chip::app::Clusters namespace to access private members. This creates a duplicate definition that could be confusing. Consider either: (1) defining the test access class only in a test-specific header, or (2) adding a comment explaining the intentional redefinition for test purposes.

- **@andy31415** on `src/app/clusters/on-off-server/tests/TestOnOffCluster.cpp`

  > This is a strange comment ... the whole purpose of the friend bit is to allow tests to access some private things. This is NOT a redefinition ... a friend declaration is not a class definition. Closing.

- **@soares-sergio** on `src/app/clusters/on-off-server/tests/BUILD.gn`

  > ```suggestion
  > # Copyright (c) 2026 Project CHIP Authors
  > ```

- **@soares-sergio** on `src/app/clusters/on-off-server/tests/BUILD.gn`

  > check other files too

- **@soares-sergio** on `src/app/clusters/on-off-server/tests/TestOnOffCluster.cpp`

  > ```suggestion
  >  *    Copyright (c) 2026 Project CHIP Authors
  > ```

- **@soares-sergio** on `src/app/clusters/on-off-server/OnOffLightingCluster.h`

  > Are we missing Setters for:
  > * OnTime
  > * OffWaitTime
  > * StartUpOnOff
  > 
  > as they're RW attributes?

- **@soares-sergio** on `src/app/clusters/on-off-server/OnOffCluster.h`

  > I found it a bit odd that here you chose to implement the timer stuff as a private class but in OnOffLightingCluster you inherit from `TimerContext` (which is the usual way we do in the SDK).
  > 
  > Could you comment on the rationale here?

- **@andy31415** on `src/app/clusters/on-off-server/OnOffLightingCluster.h`

  > They are not used outside of cluster functionality. Since no application seems to need calling it, I would like to YAGNI and not set a rule of "provide setters for everything".

- **@soares-sergio** on `src/app/clusters/on-off-server/OnOffLightingCluster.h`

  > What do you mean with `not used outside of cluster functionality` ?
  > 
  > If the application needs to configure this attribute, it can't? Only controllers can?

- **@andy31415** on `src/app/clusters/on-off-server/OnOffCluster.h`

  > Added a comment. 
  > 
  > Essentially OnOff has a timer that sets the onoff value for scenes (set it when a scene is loaded with a delay). However at the same time the Lighting implementation has a timer for "timer off/time on" that is completely separate, so I need 2 timers.
  > 
  > Trying to get both to be different TimerContext does not work well (ambigous base class) so I figured this one is very stand alone and clean, where as the lighting one is a bit more involved in timer firing.

- **@andy31415** on `src/app/clusters/on-off-server/OnOffLightingCluster.h`

  > I believe applications are generally supposed to react to matter requests (where those attributes are writable) and are not expected to change the values themselves ... unless proven otherwise and then we can add the functionality.


## PR [#42748](https://github.com/project-chip/connectedhomeip/pull/42748)

- **@soares-sergio** on `src/app/clusters/level-control/LevelControlCluster.cpp`

  > That's BS, this attribute cant be nullable according to the spec. Bad bot.
  > 
  > https://github.com/CHIP-Specifications/connectedhomeip-spec/blob/master/src/app_clusters/LevelControl.adoc#6-attributes

- **@andy31415** on `examples/all-devices-app/all-devices-common/devices/speaker/SpeakerDevice.cpp`

  > This comment does not seem to help explain the code. Should we make a pass and clean things up?

- **@andy31415** on `examples/all-devices-app/all-devices-common/devices/speaker/impl/LoggingSpeakerDevice.cpp`

  > Should we inject this from main instead so that we have a single delegate overall? I would expect main to create this and pass to devices. We do not want every device to have its own copy. This essentialy ties the device to globals (hidden inside this delegate as DeviceLayer::SystemLayer calls)

- **@andy31415** on `src/app/clusters/level-control/LevelControlCluster.h`

  > ```suggestion
  >         app::OptionalAttributeSet< //
  >             LevelControl::Attributes::MinLevel::Id, //
  >             LevelControl::Attributes::MaxLevel::Id, //
  >             LevelControl::Attributes::OnOffTransitionTime::Id, //
  >             LevelControl::Attributes::OnTransitionTime::Id, //
  >             LevelControl::Attributes::OffTransitionTime::Id, //
  >             LevelControl::Attributes::DefaultMoveRate::Id //
  >         >;
  > ```
  > 
  > supernit: I wonder if we could order these as in the spec (easier to review) and fight the restyler to place these one per line

- **@andy31415** on `src/app/clusters/level-control/LevelControlCluster.h`

  > This is an ordering thing though, as `WithMinLevel(10).WithLighting() the min will not work. Also it seems unintuitive that `WithMinLevel(10)` if no lighting would end up with min-level.
  > 
  > Could we instead just VerifyOrDie on having the lighting feature? Same comment above.

- **@andy31415** on `src/app/clusters/level-control/LevelControlCluster.h`

  > ActionReturnStatus is a very broad result ... it includes things like "Cluster specific status" and "kWriteNoop".
  > 
  > Should we maybe just CHIP_ERROR these instead?

- **@andy31415** on `src/app/clusters/level-control/LevelControlCluster.cpp`

  > This is awkward ... notify if success and then passing in success. We should NotifyAttributeChanged..

- **@andy31415** on `src/app/clusters/level-control/LevelControlCluster.cpp`

  > This method is responsivle for NotifyAttributeChanged. In that case we do not need to tell the caller this info.
  > 
  > We should CHIP_ERROR if this can fail or more likely in this case we should have it `void`.

- **@andy31415** on `src/app/clusters/level-control/LevelControlCluster.cpp`

  > could we bring the load from above down to here and have a single Has check for the feature? I think that would make things more readable.

- **@andy31415** on `src/app/clusters/level-control/LevelControlCluster.cpp`

  > nit throughout: please add {} for all ifs.

- **@andy31415** on `src/app/clusters/level-control/LevelControlCluster.cpp`

  > do we know if level changed (compared to what was there by default)?
  > Why are we calling this in case startup is set, but we do not call it when we load from persistent storage when we have no startup?
  > 
  > I think delegate should know about the level loaded from NVS. Should we consider a separate delegate call for startup vs for "general update at runtime" ?

- **@andy31415** on `src/app/clusters/level-control/LevelControlCluster.cpp`

  > ```suggestion
  >         return UnsupportedAttribute;
  > ```
  > 
  > ActionReturnStatus auto-casts from both status and chip-error and such.

- **@andy31415** on `src/app/clusters/level-control/LevelControlCluster.cpp`

  > UpdateRemainingTime seems to also be `Q` ... where is the logic for that? Should we have some specific quiet settings for it?

- **@andy31415** on `src/app/clusters/level-control/LevelControlCluster.cpp`

  > ```suggestion
  > 
  >     if (mFeatureMap.Has(Feature::kOnOff))
  >     {
  >         ReturnErrorOnFailure(builder.AppendElements({
  >             Commands::MoveToLevelWithOnOff::kMetadataEntry, //
  >             Commands::MoveWithOnOff::kMetadataEntry, //
  >             Commands::StepWithOnOff::kMetadataEntry, //
  >             Commands::StopWithOnOff::kMetadataEntry, //
  >             });
  >     }
  > 
  >     return builder.AppendElements({
  >         Commands::MoveToLevel::kMetadataEntry, //
  >         Commands::Move::kMetadataEntry, //
  >         Commands::Step::kMetadataEntry, //
  >         Commands::Stop::kMetadataEntry, //
  >     });
  > ```
  > 
  > I think this is more redable and probably will result in smaller code. We are using heap anyway by using appendelements, this seems less error prone too.

- **@andy31415** on `src/app/clusters/level-control/LevelControlCluster.cpp`

  > Lets just use `ReturnErrorOnFailure` everywhere here instead of this if ... this is what other clusters do and seems cleaner.

- **@andy31415** on `src/app/clusters/level-control/LevelControlCluster.cpp`

  > use `VerifyOrReturnError` or `VerifyOrReturnValue` for these.

- **@andy31415** on `src/app/clusters/level-control/LevelControlCluster.cpp`

  > ```suggestion
  >     VerifyOrReturnValue(mFeatureMap.Has(Feature::kOnOff), true);
  > ```

- **@andy31415** on `src/app/clusters/level-control/LevelControlCluster.cpp`

  > Using this method makes it hader to match against spec .... I am looking at appclusters.html#_effect_on_receipt_23 to see the processing that the spec requests and it is hard to correlate that to the implementation,
  > 
  > Could you add some implementation explaination/blurb on this general handler and why we funnell several commands into it?

- **@andy31415** on `src/app/clusters/level-control/LevelControlCluster.cpp`

  > We should use VerifyOrReturnValue/Error for these `if (foo) return constant`.

- **@andy31415** on `src/app/clusters/level-control/LevelControlCluster.cpp`

  > ```suggestion
  >     const uint32_t transitionTimeMs = transitionTimeDS.ValueOr(mOnOffTransitionTime) * 100;
  > ```
  > 
  > does this work?

- **@andy31415** on `src/app/clusters/level-control/LevelControlCluster.cpp`

  > ```suggestion
  >     currentLevel = mCurrentLevel.ValueOr(currentLevel);
  > ```
  > 
  > dependency here seems a bit unclear to me ... how do the commands flow regarding On/Off and level? Do we expect On/Off to affect level during callbacks?


## PR [#42884](https://github.com/project-chip/connectedhomeip/pull/42884)

- **@arielsz71** on `src/app/clusters/illuminance-measurement-server/CodegenIntegration.cpp`

  > I have just checked the documentation and it seems LightSensorType is optional (Conformance: O).

- **@arielsz71** on `src/app/clusters/illuminance-measurement-server/IlluminanceMeasurementCluster.cpp`

  > I have just checked the documentation and it seems LightSensorType is optional (Conformance: O).

- **@arielsz71** on `src/app/clusters/illuminance-measurement-server/IlluminanceMeasurementCluster.h`

  > I have just checked the documentation and it seems LightSensorType is optional (Conformance: O).

- **@arielsz71** on `src/app/clusters/illuminance-measurement-server/IlluminanceMeasurementCluster.h`

  > Addressed.

- **@arielsz71** on `examples/all-clusters-app/esp32/main/DeviceWithDisplay.cpp`

  > Addressed.

- **@arielsz71** on `examples/all-clusters-minimal-app/esp32/main/DeviceWithDisplay.cpp`

  > Addressed.

- **@arielsz71** on `examples/all-clusters-app/esp32/main/DeviceWithDisplay.cpp`

  > Addressed.

- **@arielsz71** on `examples/all-clusters-minimal-app/esp32/main/DeviceWithDisplay.cpp`

  > Addressed.

- **@andy31415** on `src/app/clusters/illuminance-measurement-server/IlluminanceMeasurementCluster.h`

  > what are these types? I find it hard to read this.
  > 
  > Could we use uint16_t and such? Everything is uint16_t or LightSensorTypeEnum

- **@andy31415** on `src/app/clusters/illuminance-measurement-server/IlluminanceMeasurementCluster.cpp`

  > measuredValue is Nullable<uint16_t> so it seems we could set it to null if we accept this type, so this will assert on null.
  > 
  > We seem to want:
  > 
  > ```suggestion
  >     if (measuredValue.ValueOr(0) != 0)
  > ```

- **@andy31415** on `src/app/clusters/illuminance-measurement-server/IlluminanceMeasurementCluster.cpp`

  > VerifyOrReturnError(measuredValue.Value() >= mConfig.minMeasuredValue.ValueOr(1), CHIP_ERROR_INVALID_ARGUMENT);

- **@andy31415** on `src/app/clusters/illuminance-measurement-server/IlluminanceMeasurementCluster.cpp`

  > ```suggestion
  >         VerifyOrReturnError(measuredValue.Value() >= mConfig.minMeasuredValue.ValueOr(1), CHIP_ERROR_INVALID_ARGUMENT);
  > ```

- **@andy31415** on `src/app/clusters/illuminance-measurement-server/IlluminanceMeasurementCluster.cpp`

  > ```suggestion
  >         // According to the spec, absolute maximum value is 0xFFFE / 65534
  >         VerifyOrReturnError(measuredValue.Value() <= mConfig.maxMeasuredValue.ValueOr(0xFFFE), CHIP_ERROR_INVALID_ARGUMENT);
  > ```

- **@andy31415** on `src/app/clusters/illuminance-measurement-server/IlluminanceMeasurementCluster.h`

  > ```suggestion
  >         Nullable<uint16_t> minMeasuredValue{};
  >         Nullable<uint16_t> maxMeasuredValue{};
  >         uint16_t tolerance{};
  >         Nullable<LightSensorTypeEnum> lightSensorType{};
  > ```
  > 
  > The aliases make the code really really hard to follow. They could be anything.

- **@andy31415** on `src/app/clusters/illuminance-measurement-server/IlluminanceMeasurementCluster.h`

  > please do this throughout.

- **@arielsz71** on `src/app/clusters/illuminance-measurement-server/IlluminanceMeasurementCluster.h`

  > These are just short aliases for the longer type declarations, in this case all of them except tolerance are Nullable<> types, so it is correct to use the type in the metadata, I think the aliases are clear. Even LightSensorType is Nullable < LightSensorTypeEnum >

- **@arielsz71** on `src/app/clusters/illuminance-measurement-server/IlluminanceMeasurementCluster.h`

  > Ok, but LightSensorType should be Nullable < LightSensorTypeEnum >

- **@andy31415** on `src/app/clusters/illuminance-measurement-server/IlluminanceMeasurementCluster.h`

  > As a reviewer my feedback is that this makes the code very hard to review - the data  types are too hidden away in aliases of namespaces.

- **@andy31415** on `src/app/clusters/illuminance-measurement-server/IlluminanceMeasurementCluster.h`

  > Ah, messed up reading the spec ... yes, the enum should be nullable. 
  > 
  > Now why do we have a nullable optional attribute? Why would the attribute not be missing if the value is unknown .... :shrug:

- **@andy31415** on `src/app/clusters/illuminance-measurement-server/IlluminanceMeasurementCluster.h`

  > Tolerance is also not nullable.

- **@andy31415** on `src/app/clusters/illuminance-measurement-server/IlluminanceMeasurementCluster.h`

  > Updated my suggestion ... it also had tons of typos :/


## PR [#42886](https://github.com/project-chip/connectedhomeip/pull/42886)

- **@Copilot** on `src/app/data-model-provider/StringBuilderAdapters.cpp`

  > There is an extra closing parenthesis in this line. The expression has `static_cast<int>(status))` with two closing parentheses at the end, but only one opening parenthesis for the cast. This will cause a compilation error.
  > ```suggestion
  >     *os << "Status<" << chip::Protocols::InteractionModel::StatusName(status) << "/" << static_cast<int>(status) << ">";
  > ```

- **@Copilot** on `src/app/data-model-provider/StringBuilderAdapters.cpp`

  > There is an extra closing parenthesis in this line. The line contains `static_cast<int>(status))` with two closing parentheses, which is a syntax error. It should be `static_cast<int>(status)` with only one closing parenthesis.

- **@Copilot** on `src/app/clusters/groups-server/GroupsCluster.cpp`

  > The iterator returned by IterateGroupKeys can be nullptr if no iterator instances are available. The code should check if 'it' is null before using it to prevent potential null pointer dereference.
  > ```suggestion
  >     auto it = provider.IterateGroupKeys(fabricIndex);
  >     if (it == nullptr)
  >     {
  >         return false;
  >     }
  > ```

- **@Copilot** on `src/app/clusters/groups-server/tests/TestGroupsCluster.cpp`

  > After filling the group table up to max_groups, the code attempts to map an additional group to index 0, which would overwrite the first entry. This may not correctly test the RESOURCE_EXHAUSTED scenario. Consider either not mapping the key at all (to test the key setup failure path) or mapping to a new index that would fail (though the group table is full).
  > ```suggestion
  > 
  > ```

- **@andy31415** on `src/app/clusters/groups-server/GroupsCluster.cpp`

  > Pre-existing ... but correct. Will update.

- **@Copilot** on `src/app/clusters/groups-server/tests/BUILD.gn`

  > The test file uses DefaultSessionKeystore from crypto/DefaultSessionKeystore.h, but the BUILD.gn file for tests does not explicitly include a dependency on the crypto library. While this might work through transitive dependencies, it would be more explicit and maintainable to add the crypto dependency directly to the test BUILD.gn file.
  > ```suggestion
  >     "${chip_root}/src/lib/support",
  >     "${chip_root}/src/crypto",
  > ```

- **@Copilot** on `examples/all-devices-app/all-devices-common/devices/on-off-light/LoggingOnOffLightDevice.h`

  > The variable name contains a typo: "Gropups" should be "Groups".

- **@Copilot** on `examples/all-devices-app/all-devices-common/devices/on-off-light/LoggingOnOffLightDevice.cpp`

  > The variable name contains a typo: "Gropups" should be "Groups". This appears in multiple places in the Register method.

- **@Copilot** on `examples/all-devices-app/all-devices-common/devices/on-off-light/LoggingOnOffLightDevice.cpp`

  > The variable name contains a typo: "Gropups" should be "Groups". This appears in the UnRegister method.

- **@Copilot** on `src/app/clusters/groups-server/GroupsCluster.cpp`

  > Typo in comment: "marco" should be "macro" and "sine" should be "since".
  > ```suggestion
  > // macro (i.e. no line & file info) since the underlying error would have the original cause and logs would contain
  > ```

- **@Copilot** on `src/app/clusters/groups-server/GroupsCluster.cpp`

  > The KeyExists function implementation has changed from the original. The old implementation checked if an actual keyset exists by calling provider.GetKeySet() to verify the keyset is configured. The new implementation only checks if a GroupKey mapping exists, but doesn't verify that the keyset itself is configured. This could allow groups to be added even when the keyset doesn't exist, which may violate the security requirements. Consider restoring the keyset validation logic to ensure that the keyset is actually configured before allowing group operations.

- **@andy31415** on `src/app/clusters/groups-server/GroupsCluster.cpp`

  > This is actually a great catch ... fixed.

- **@lpbeliveau-silabs** on `src/app/clusters/groups-server/GroupsCluster.cpp`

  > Do we want to remove the GroupInfo previously added here in case of failure?

- **@lpbeliveau-silabs** on `src/app/clusters/groups-server/GroupsCluster.cpp`

  > So there are multiple failure points here and we always return Success. I see this is the proper Spec prescribed behavior but should we open a spec issue or since we are transitioning away form groups to groupcast this is acceptable?

- **@lpbeliveau-silabs** on `src/app/clusters/groups-server/GroupsCluster.cpp`

  > Nit but most logs in here are Zcl logs, is there a reason why the errors are AppServer and the normal logs Zcl?

- **@lpbeliveau-silabs** on `src/app/clusters/groups-server/GroupsCluster.cpp`

  > ```suggestion
  >         ReturnErrorOnFailure(request_data.Decode(input_arguments, fabricIndex));
  > ```

- **@lpbeliveau-silabs** on `src/app/clusters/groups-server/GroupsCluster.cpp`

  > ```suggestion
  >         response.status  = to_underlying(AddGroup(request_data.groupID, request_data.groupName, fabricIndex));
  > ```

- **@lpbeliveau-silabs** on `src/app/clusters/groups-server/GroupsCluster.cpp`

  > The UI is a bit messed up so I might miss a few but, in general, I believe either we should not store `request.GetAccessingFabricIndex()` in `const FabricIndex fabricIndex` for the sake of ram or we reuse `fabricIndex` eveywhere for the sake of readability.

- **@lpbeliveau-silabs** on `src/app/clusters/groups-server/GroupsCluster.cpp`

  > This block and the next one are identical unless we take action on the next one to remove the GroupInfo on mapping failure, could we combine this in a `if(conditino1 || condition 2){}` if we don't apply the suggestion to remove the GroupInfo on failure? or maybe a helper? If we do decide the next branch is different this this is more of a nit but otherwise I think we could reduce code duplication this way.

- **@lpbeliveau-silabs** on `src/app/clusters/groups-server/GroupsCluster.h`

  > ```suggestion
  >                                                                TLV::TLVReader & input_arguments,
  > ```

- **@lpbeliveau-silabs** on `src/app/clusters/groups-server/GroupsCluster.h`

  > ```suggestion
  >     Protocols::InteractionModel::Status AddGroup(GroupId groupID, CharSpan groupName, FabricIndex fabricIndex);
  > ```
  > and add a chip {...} namespace so reduce the chip:: usage

- **@andy31415** on `src/app/clusters/groups-server/GroupsCluster.cpp`

  > Sure. I think previous code did not do this, so this will cost a bit of flash, however generally we probably want to have things work better.

- **@andy31415** on `src/app/clusters/groups-server/GroupsCluster.cpp`

  > crated https://github.com/CHIP-Specifications/connectedhomeip-spec/issues/12691
  > 
  > It does make sense to me that we want to go through all removal steps and not early-exit on failure, however we should probably also be able to report failure to callers if not everything worked.

- **@andy31415** on `src/app/clusters/groups-server/GroupsCluster.cpp`

  > Will update, I think it was just what I typed. Consistency is indeed better.

- **@andy31415** on `src/app/clusters/groups-server/GroupsCluster.cpp`

  > Changed a bunch.

- **@andy31415** on `src/app/clusters/groups-server/GroupsCluster.cpp`

  > I applied the "remove group info" instead.
  > 
  > I did check logging overhead and it seems minimal, even though the ChipLogDetail is identical. I think code is readable and now that we have some cleanup in the 2nd case the code is probably ok to be split like this. The message repeating feels redundant but it only happens once.

- **@Copilot** on `src/app/clusters/groups-server/GroupsCluster.cpp`

  > The accept_group lambda iterates through mCommandData.groupList using an iterator but doesn't check for errors after the while loop completes. If the iterator encounters an error during iteration, the loop will exit but the error status is not checked. Consider adding `ReturnErrorOnFailure(iter.GetStatus());` after the while loop at line 143 to ensure iteration errors are properly handled.
  > ```suggestion
  >                     }
  >                     VerifyOrReturnValue(iter.GetStatus() == CHIP_NO_ERROR, false);
  > ```

- **@andy31415** on `src/app/clusters/groups-server/GroupsCluster.cpp`

  > Previous code did not do that either. It is unclear to me if we have an error if we should accept the group or not. Saying "i accept the group if iteration fails" seems odd ... so generally I would claim this is probably fine.

- **@soares-sergio** on `src/app/clusters/groups-server/GroupsCluster.cpp`

  > Formatting seems odd here


## PR [#42968](https://github.com/project-chip/connectedhomeip/pull/42968)

- **@arielsz71** on `examples/light-switch-app/silabs/src/LightSwitchMgr.cpp`

  > This was done this way so that the cleanup code that follows the switch statement gets executed.

- **@arielsz71** on `examples/light-switch-app/silabs/src/LightSwitchMgr.cpp`

  > This was done this way so that the cleanup code that follows the switch statement gets executed.

- **@andy31415** on `src/app/clusters/switch-server/CodegenIntegration.h`

  > shoud we preserve the old API interface of `class SwitchServer` ? that way we make life easier for people upgrading.

- **@andy31415** on `src/app/clusters/switch-server/SwitchCluster.h`

  > Is this called or `should be called` ? 
  > 
  > As I understand this is an API interface that asks/allow the app to trigger an event when its hardware detects it.
  > 
  > Could we make these comments more compact? All this `@brief` and extra newlines makes it quite verbose.  Some of the comments re-read the code, like `OnLongPress` is obviously to be called on a long press.

- **@andy31415** on `src/app/clusters/switch-server/SwitchCluster.h`

  > number of positions and multipress max are fixed ... we should likely not provide setters. They can be set in constructor.

- **@andy31415** on `examples/chef/common/clusters/switch/SwitchManager.cpp`

  > This is chef being silly ... this is not allowed.

- **@andy31415** on `examples/light-switch-app/qpg/src/SwitchManager.cpp`

  > maybe provide an overload in codegenintegration specifically? I.e. allow override before cluster is created. I believe this is what happens here.

- **@andy31415** on `src/app/clusters/switch-server/SwitchCluster.cpp`

  > Maybe constraint_error instead of invalid argument is clearer?

- **@andy31415** on `src/app/clusters/switch-server/SwitchCluster.cpp`

  > Do we need these logs?

- **@arielsz71** on `src/app/clusters/switch-server/SwitchCluster.h`

  > Addressed.

- **@arielsz71** on `src/app/clusters/switch-server/SwitchCluster.h`

  > Addressed.

- **@arielsz71** on `src/app/clusters/switch-server/SwitchCluster.cpp`

  > Addressed.

- **@andy31415** on `examples/all-clusters-app/linux/AllClustersCommandDelegate.cpp`

  > ```suggestion
  >     // This is why the cluster does not provide a getter for this attribute.
  >     // we read this via ember (i.e. defaults that were set during codegen startup)
  > ```


## PR [#43085](https://github.com/project-chip/connectedhomeip/pull/43085)

- **@Copilot** on `src/data-model-providers/codegen/ClusterIntegration.h`

  > `DefaultClusterIntegration` uses `std::array`, `VerifyOrReturnValue`, and `CHIP_DEVICE_CONFIG_DYNAMIC_ENDPOINT_COUNT`, but this header doesn’t include the standard/library headers that define them. This makes compilation depend on transitive includes and can break in TUs that include `ClusterIntegration.h` directly. Add explicit includes (e.g. `<array>`, `<lib/support/CodeUtils.h>`, and the header that defines `CHIP_DEVICE_CONFIG_DYNAMIC_ENDPOINT_COUNT`).

- **@Copilot** on `src/app/clusters/wifi-network-management-server/WiFiNetworkManagementCluster.cpp`

  > The new `InvokeCommand` implementation isn’t covered by the added unit tests (e.g. successful NetworkPassphraseRequest response, and rejection when authMode is not CASE). Since this is security-relevant behavior (passphrase disclosure gating), please add tests that invoke `NetworkPassphraseRequest` via `ClusterTester::Invoke(...)` and validate both the response payload and the access check.

- **@ksperling-apple** on `src/app/clusters/wifi-network-management-server/WiFiNetworkManagementCluster.h`

  > Changed back to private

- **@soares-sergio** on `examples/network-manager-app/linux/main.cpp`

  > Are you planning to keep backwards compatibility?
  > 
  > If so, you should probably have kept `wifi-network-management-server.h` and made it just include `CodegenIntegration.h` so no example apps would need to change.
  > 
  > Then, in `CodegenIntegration.h` you could keep the OLD API and reimplement them redirecting to the new api behind the scenes.
  > 
  > For example, see Chime (https://github.com/project-chip/connectedhomeip/blob/master/src/app/clusters/chime-server/chime-server.h) and Identify (https://github.com/project-chip/connectedhomeip/blob/master/src/app/clusters/identify-server/identify-server.h) where we kept the old api for legacy (zap-based apps).

- **@soares-sergio** on `src/app/clusters/wifi-network-management-server/WiFiNetworkManagementCluster.h`

  > Should we have getters here too?
  > * `GetSSID()`
  > * `GetPassphraseSurrogate()`

- **@soares-sergio** on `src/app/clusters/wifi-network-management-server/WiFiNetworkManagementCluster.cpp`

  > I believe `NotifyAttributeChanged` already checks that context != nullptr, so no need to repeat that
  > ```suggestion
  >     NotifyAttributeChanged(Ssid::Id);
  >     NotifyAttributeChanged(PassphraseSurrogate::Id);
  > ```

- **@soares-sergio** on `src/app/clusters/wifi-network-management-server/WiFiNetworkManagementCluster.cpp`

  > ```suggestion
  >     if (ssidChanged)
  > ```

- **@soares-sergio** on `src/app/clusters/wifi-network-management-server/WiFiNetworkManagementCluster.cpp`

  > ```suggestion
  >     NotifyAttributeChanged(PassphraseSurrogate::Id);
  > ```

- **@soares-sergio** on `src/app/server-cluster/DefaultServerCluster.h`

  > Likely not needed if it was just used before calling NotifyAttributeChanged
  > 
  > ```suggestion
  > ```

- **@soares-sergio** on `src/app/clusters/wifi-network-management-server/tests/TestWiFiNetworkManagementCluster.cpp`

  > Add a test for SSID too long? 32-byte?

- **@soares-sergio** on `src/app/clusters/wifi-network-management-server/tests/TestWiFiNetworkManagementCluster.cpp`

  > Check that NotifyAttributeChanged is triggered for both Ssid and PassphraseSurrogate when ClearNetworkCredentials is called

- **@soares-sergio** on `src/app/clusters/wifi-network-management-server/WiFiNetworkManagementCluster.h`

  > ```suggestion
  >     WiFiNetworkManagementCluster(EndpointId endpoint) : DefaultServerCluster({ endpoint, WiFiNetworkManagement::Id }) {}
  > ```

- **@ksperling-apple** on `src/app/clusters/wifi-network-management-server/WiFiNetworkManagementCluster.h`

  > DefaultClusterIntegration accesses the id via `constexpr static ClusterId kClusterId        = ClusterImpl::Id;`, unless we have another way of easily accessing it?

- **@ksperling-apple** on `src/app/server-cluster/DefaultServerCluster.h`

  > I had some other uses for it in a different cluster I'm working on, e.g. enforcing `!IsStarted()` in a `SetDelegate` method, and having a method for it seems a bit more readable than checking for `mContext != nullptr` directly. (I thought I remember mContext was private but I see it's actually protected)

- **@Copilot** on `src/app/clusters/wifi-network-management-server/WiFiNetworkManagementCluster.cpp`

  > InvokeCommand dereferences request.subjectDescriptor without checking for nullptr. OperationTypes.h explicitly allows subjectDescriptor to be null for internal requests, which would lead to a crash here. Please guard against null (e.g., treat missing subjectDescriptor as UnsupportedAccess/InvalidAction) before reading authMode.
  > ```suggestion
  >         VerifyOrReturnValue(request.subjectDescriptor != nullptr &&
  >                                 request.subjectDescriptor->authMode == Access::AuthMode::kCase,
  > ```

- **@Copilot** on `src/app/clusters/wifi-network-management-server/WiFiNetworkManagementCluster.cpp`

  > InvokeCommand unconditionally calls handler->AddResponse(...) but handler can be nullptr in some call paths/tests (e.g. default cluster tests exercise InvokeCommand with a null handler). Add a null check and return an appropriate status (e.g. InvalidAction) if no handler is provided.

- **@ksperling-apple** on `examples/network-manager-app/linux/main.cpp`

  > I've put back wifi-network-management-server.{h,cpp} now and preserved the old API. This means there is no "CodeGenIntegration" now providing zap-driven storage for the cluster instance, since the existing API already required the app to manually instantiate and register the cluster. Please have a look.

- **@ksperling-apple** on `src/app/clusters/wifi-network-management-server/WiFiNetworkManagementCluster.h`

  > This data is "owned" by the application and it has to push changes into the cluster as needed, so I'm not sure getters are needed?

- **@ksperling-apple** on `src/app/clusters/wifi-network-management-server/WiFiNetworkManagementCluster.cpp`

  > > subjectDescriptor null check: The comment is right that OperationTypes.h allows it to be null for internal requests (flagged with kInternal). However, NetworkPassphraseRequest is a spec-defined command that can only arrive via the interaction model — the CASE auth check itself is the spec-required access control. Other code-driven clusters (GeneralCommissioningCluster, OnOffLightingCluster) also dereference subjectDescriptor->authMode without null-checking in InvokeCommand, relying on the same assumption. Adding a guard isn't unreasonable, but it's not a correctness issue in practice, and it's not the established pattern.

- **@ksperling-apple** on `src/app/clusters/wifi-network-management-server/WiFiNetworkManagementCluster.cpp`

  > > handler null check: The codegen provider at CodegenDataModelProvider.cpp:172 passes handler straight through from its own parameter — and its caller (InteractionModelEngine) always provides a real handler for IM-originated commands. The test mocks in TestCodegenModelViaMocks.cpp do pass nullptr for handler, but only to clusters whose InvokeCommand implementations are stubs that return immediately without using handler. Groupcast checks for null because it genuinely needs to handle the case; our cluster only calls handler->AddResponse on the success path after two VerifyOrReturnValue guards. There's no real call path where a null handler reaches a live cluster's success path.

- **@Copilot** on `src/app/clusters/wifi-network-management-server/WiFiNetworkManagementCluster.cpp`

  > The NotifyAttributeChanged call for PassphraseSurrogate::Id is incorrectly placed outside the if statement that checks passphraseChanged. This means the attribute will be marked dirty even when only the SSID changes (not the passphrase), which contradicts the logic that only increments mPassphraseSurrogate when the passphrase changes. This line should be indented to be inside the if (passphraseChanged) block.

- **@Copilot** on `src/app/clusters/wifi-network-management-server/app_config_dependent_sources.gni`

  > The build configuration references CodegenIntegration.cpp and CodegenIntegration.h files (lines 16-17) which do not exist in the repository. These files are necessary to provide backwards compatibility wrappers (like WiFiNetworkManagementServer) that are used by the example code. Without these files, the build will fail. These files should either be added to this PR or the build configuration should be updated to not reference them.
  > ```suggestion
  > 
  > ```

- **@Copilot** on `src/app/clusters/wifi-network-management-server/app_config_dependent_sources.cmake`

  > The build configuration references CodegenIntegration.cpp and CodegenIntegration.h files (lines 19-20) which do not exist in the repository. These files are necessary to provide backwards compatibility wrappers (like WiFiNetworkManagementServer) that are used by the example code in examples/network-manager-app/linux/main.cpp. Without these files, the build will fail. These files should either be added to this PR or the build configuration should be updated to not reference them.
  > ```suggestion
  > 
  > ```

- **@andy31415** on `src/app/clusters/wifi-network-management-server/CodegenIntegration.h`

  > You could use https://github.com/project-chip/connectedhomeip/blob/master/src/app/server-cluster/ServerClusterInterfaceRegistry.h#L76 `LazyRegisteredServerCluster`
  > 
  > That combines a registration and a constructor as well. That way you do not need to figure out started or not.

- **@andy31415** on `src/app/clusters/wifi-network-management-server/CodegenIntegration.cpp`

  > Started is not the same as registered. The server may be not started but still registered.
  > 
  > Given that the CodegenDataModelProvider is a global, is there any guarantee of ordering here? Maybe we should provide a Shutdown/Deinit API here for the callers to have better control over the cluster.

- **@andy31415** on `src/app/clusters/wifi-network-management-server/WiFiNetworkManagementCluster.cpp`

  > We should only notify of attribute change if the value actually changed. Could we add a check of "if not empty yet, empty and notify" ?

- **@andy31415** on `src/app/clusters/wifi-network-management-server/WiFiNetworkManagementCluster.cpp`

  > I am wondering if we should return invalid argument or IM ConstraintError here.
  > 
  > They are technically equivalent, however IM error seems to translate better if these get sent as a result of an action.

- **@andy31415** on `src/app/clusters/wifi-network-management-server/WiFiNetworkManagementCluster.h`

  > I think this is about readability - `Id` is not super obvious even though code is probably correct.
  > 
  > In this case I also believe the prefix makes it clearer even if redundant.

- **@andy31415** on `src/app/clusters/wifi-network-management-server/WiFiNetworkManagementCluster.h`

  > Getters probably do not cost anything and it seems a good idea if a class has a setter to also have the corresponding getter. That way something that acts on the cluster has full access without extra coupling.
  > 
  > I agree there may be some element of YAGNI here, however we also assume this is a cluster for apps to use and we do not know how they use it, so symmetrical API seems reasonable. Slight lean towards "should have an API" however 🤷


## PR [#43142](https://github.com/project-chip/connectedhomeip/pull/43142)

- **@Copilot** on `src/app/clusters/bridged-device-basic-information-server/BridgedDeviceBasicInformationCluster.cpp`

  > `ToSpan` only accepts `std::optional<std::string>`, but it's also being called with `mRequiredData.uniqueId` (a `std::string`). This relies on an implicit temporary `std::optional<std::string>` conversion (and a string copy) that is easy to miss. Consider adding an overload for `const std::string &` (or encoding the `uniqueId` directly) to avoid the hidden allocation/copy and make intent clearer.

- **@Copilot** on `src/app/clusters/bridged-device-basic-information-server/BridgedDeviceIcdDelegate.h`

  > Typo in the comment: "Notification taht" should be "Notification that".

- **@Copilot** on `src/app/clusters/bridged-device-basic-information-server/BridgedDeviceBasicInformationCluster.h`

  > The public method parameter name `promoisedActiveMs` is misspelled. Since this is part of the public API surface, it should be renamed consistently (e.g. `promisedActiveMs` or `promisedActiveDurationMs`) to avoid propagating the typo to callers.

- **@Copilot** on `src/app/clusters/bridged-device-basic-information-server/BridgedDeviceBasicInformationCluster.cpp`

  > The parameter name `promoisedActiveMs` is misspelled; please keep it consistent with the header rename (e.g. `promisedActiveMs`) so the API and implementation match and the typo doesn't persist.

- **@Copilot** on `src/app/clusters/bridged-device-basic-information-server/tests/TestBridgedDeviceBasicInformationCluster.cpp`

  > Test coverage is missing for the case where SetReachable is called with the same value (no change). Similar to the BooleanStateCluster tests (see TestBooleanStateCluster::NoEventWhenValueUnchanged), there should be a test verifying that no event is generated when SetReachable is called with the current value. This ensures the early return on line 186 of BridgedDeviceBasicInformationCluster.cpp works correctly.
  > ```suggestion
  > 
  > TEST_F(TestBridgedDeviceBasicInformationCluster, TestNoReachableChangedEventWhenValueUnchanged)
  > {
  >     BridgedDeviceBasicInformationCluster cluster(kTestEndpointId, { .uniqueId = "event-dev", .reachable = false }, {}, {});
  >     EXPECT_EQ(cluster.Startup(mContext.Get()), CHIP_NO_ERROR);
  > 
  >     // Calling SetReachable with the same value should not generate an event.
  >     cluster.SetReachable(false);
  > 
  >     std::optional<LogOnlyEvents::EventInformation> eventInfo = mContext.EventsGenerator().GetNextEvent();
  >     EXPECT_EQ(eventInfo, std::nullopt);
  > }
  > ```

- **@Copilot** on `src/app/clusters/bridged-device-basic-information-server/tests/TestBridgedDeviceBasicInformationCluster.cpp`

  > Test coverage is missing for the negative case where KeepActive command is invoked on a cluster without an ICD delegate. According to the implementation (BridgedDeviceBasicInformationCluster.cpp:159), this should return Status::UnsupportedCommand. Consider adding a test that creates a cluster without an ICD delegate and verifies that invoking KeepActive returns UnsupportedCommand.
  > ```suggestion
  > 
  > TEST_F(TestBridgedDeviceBasicInformationCluster, TestKeepActiveCommandWithoutIcdDelegate)
  > {
  >     BridgedDeviceBasicInformationCluster cluster(kTestEndpointId, { .uniqueId = "no-icd-dev" }, {}, {});
  >     EXPECT_EQ(cluster.Startup(mContext.Get()), CHIP_NO_ERROR);
  >     ClusterTester tester(cluster);
  > 
  >     Commands::KeepActive::Type request;
  >     request.stayActiveDuration = 1000;
  >     request.timeoutMs          = 30000;
  > 
  >     auto response = tester.Invoke<Commands::KeepActive::Type>(request);
  >     EXPECT_FALSE(response.IsSuccess());
  >     EXPECT_EQ(response.GetStatus(), Status::UnsupportedCommand);
  > }
  > ```

- **@lpbeliveau-silabs** on `src/app/clusters/bridged-device-basic-information-server/BridgedDeviceBasicInformationCluster.cpp`

  > More a general Q than a comment: Moving forward, when attributes become mandatory past a certain revision, what is the difference between updating the kMetadata and hardcoding it the optional array?

- **@Copilot** on `src/app/clusters/bridged-device-basic-information-server/tests/TestBridgedDeviceBasicInformationCluster.cpp`

  > Test coverage gap: There's no test verifying that the ClusterRevision attribute is readable and returns the correct value (kRevision = 6). Global attributes like ClusterRevision are mandatory and should be tested.
  > 
  > Add a test that reads the ClusterRevision attribute and verifies it returns the expected value.
  > ```suggestion
  > 
  > TEST_F(TestBridgedDeviceBasicInformationCluster, TestClusterRevisionAttribute)
  > {
  >     BridgedDeviceBasicInformationCluster cluster(kTestEndpointId, { .uniqueId = "cluster-rev-dev" }, {}, {});
  >     EXPECT_EQ(cluster.Startup(mContext.Get()), CHIP_NO_ERROR);
  >     ClusterTester tester(cluster);
  > 
  >     uint16_t clusterRevision = 0;
  >     EXPECT_EQ(tester.ReadAttribute(Attributes::ClusterRevision::Id, clusterRevision), CHIP_NO_ERROR);
  >     EXPECT_EQ(clusterRevision, Clusters::BridgedDeviceBasicInformation::kRevision);
  > }
  > ```

- **@Copilot** on `src/app/clusters/bridged-device-basic-information-server/tests/TestBridgedDeviceBasicInformationCluster.cpp`

  > For consistency with other cluster tests in the codebase (e.g., BooleanStateCluster tests), tests that call Startup() should also explicitly call Shutdown() before the cluster object is destroyed. This ensures proper cleanup and follows the established testing pattern.
  > 
  > Consider adding a TearDown() method to the test fixture that calls Shutdown() on the cluster instances that were started, or call Shutdown() explicitly at the end of tests that call Startup().

- **@Copilot** on `src/app/clusters/bridged-device-basic-information-server/BridgedDeviceBasicInformationCluster.cpp`

  > The ToSpan function only handles std::optional<std::string>, but is also called with a plain std::string on line 89 for uniqueId. While this will compile due to implicit conversion creating a temporary std::optional, it's inconsistent and potentially confusing.
  > 
  > Consider adding an overload for plain std::string or directly using CharSpan::fromCharString(mRequiredData.uniqueId.c_str()) on line 89 for clarity and consistency.
  > ```suggestion
  > 
  > CharSpan ToSpan(const std::string & s)
  > {
  >     return CharSpan::fromCharString(s.c_str());
  > }
  > ```

- **@andy31415** on `src/app/clusters/bridged-device-basic-information-server/BridgedDeviceBasicInformationCluster.cpp`

  > meh ... it compiles and works as expected ....

- **@andy31415** on `src/app/clusters/bridged-device-basic-information-server/BridgedDeviceBasicInformationCluster.cpp`

  > I think it depends on if we track master (and then we essentlly say we are the latest revision, so we report mandatory) or we try to support both versions.
  > 
  > https://github.com/project-chip/connectedhomeip/blob/master/src/app/clusters/basic-information/BasicInformationCluster.cpp#L311 for example is the only cluster I know we tried to support both revisions.
  > 
  > I think for the general case for now we always do "support the latest". At the same time I wish (but it is very hard) we could build any version of the SDK or at least the latest certifiable SDK from master. I am unsure how to achieve that in a clean way (without code duplication or some flash increase)

- **@andy31415** on `src/app/clusters/bridged-device-basic-information-server/tests/TestBridgedDeviceBasicInformationCluster.cpp`

  > This seems like it would be a change-detector really. This is already checked by certification.

- **@soares-sergio** on `scripts/tools/check_includes_config.py`

  > Is this change intentional?

- **@soares-sergio** on `src/app/clusters/bridged-device-basic-information-server/BridgedDeviceBasicInformationCluster.cpp`

  > This seems incorrect.
  > 
  > I believe configurationVersion is supposed to be mutable/increase when something changes.
  > 
  > Can you double check?

- **@soares-sergio** on `src/app/clusters/bridged-device-basic-information-server/BridgedDeviceBasicInformationCluster.cpp`

  > Besides notifying the app, the spec defines some other stuff regarding `TimeoutMs` and maintaining a `pending active` state...
  > 
  > Could you double check that and see if the cluster needs to implement more logic upon receiving this command?

- **@Copilot** on `src/app/clusters/bridged-device-basic-information-server/BridgedDeviceBasicInformationCluster.cpp`

  > SetNodeLabel assigns `mRequiredData.nodeLabel = { nodeLabel.data(), nodeLabel.size() };`. When callers clear the label via an empty `CharSpan` (e.g. `{}`), `nodeLabel.data()` may be `nullptr`; constructing `std::string` from a null pointer is not guaranteed to be valid even when the size is 0. Handle the empty-span case explicitly (e.g. `clear()` when `nodeLabel.empty()`), otherwise copy from the span buffer.
  > ```suggestion
  >     if (nodeLabel.empty())
  >     {
  >         mRequiredData.nodeLabel.clear();
  >     }
  >     else
  >     {
  >         mRequiredData.nodeLabel.assign(nodeLabel.data(), nodeLabel.size());
  >     }
  > ```

- **@Copilot** on `scripts/tools/check_includes_config.py`

  > `DENY` contains `locae.h`, which looks like a typo for `locale.h`. As written, `<locale.h>` will no longer be denied and the checker will deny a header that likely never appears, changing the include policy unintentionally.
  > ```suggestion
  > 
  > ```

- **@Copilot** on `scripts/tools/check_includes_config.py`

  > The IGNORE list change exempts a broad set of directories (cameras, commissioners, media, etc.) from include checking, not just the new bridged-device-basic-information server. This is a significant scope expansion beyond what the PR description calls out; consider narrowing the exemption to what this PR needs, or explicitly documenting/justifying the wider policy change in the PR description.

- **@Copilot** on `src/app/clusters/bridged-device-basic-information-server/BridgedDeviceBasicInformationDelegate.h`

  > This new public header is missing the standard Apache 2.0 license block seen on other cluster delegate headers (e.g. `src/app/clusters/audio-output-server/audio-output-delegate.h:1-17`) and also differs from the other new files added in this PR. Add the project-standard license header for consistency and compliance.

- **@andy31415** on `scripts/tools/check_includes_config.py`

  > Ooops ... reverted. Thank you for catching!

- **@andy31415** on `src/app/clusters/bridged-device-basic-information-server/BridgedDeviceBasicInformationCluster.cpp`

  > You are right ... the spec is somewhat frustrating by referecing all attributes from one cluster to another.
  > 
  > I found this when trying to migrate the existing bridge app: I updated config version to be updatable, made nodelabel updatable and changed delegates for the nodelabel since that has to be NVMed by the bridge (I did not auto-do this as I was thinking bridge should manage device data).

- **@andy31415** on `src/app/clusters/bridged-device-basic-information-server/BridgedDeviceBasicInformationCluster.cpp`

  > This seems to be a question of "does the app maintain the state or does the cluster do it and let the app query it". I think "app maintains it" is simpler for a first pass.

- **@Copilot** on `src/lib/core/TLVReader.cpp`

  > TLVReader::Get(CharSpan&) returns success for zero-length strings by setting `v = {}` (data == nullptr). Several call sites in the codebase construct `std::string(span.data(), span.size())` from decoded CharSpans; with `data == nullptr` this can trigger UB/asserts even when `size() == 0`. Consider returning an empty CharSpan with a non-null pointer (e.g. pointing at a static "" buffer, or have GetDataPtr return a valid pointer even when length is 0) to keep empty-string reads safe for these conversions.

- **@Copilot** on `src/app/clusters/bridged-device-basic-information-server/BridgedDeviceBasicInformationCluster.cpp`

  > SetNodeLabel assigns `mRequiredData.nodeLabel = { nodeLabel.data(), nodeLabel.size() }`. If `nodeLabel` is empty (which can happen for clears, and also from TLV decoding of a zero-length string where data may be nullptr), constructing a std::string from a nullptr pointer is undefined behavior on some standard libraries. Handle the empty case explicitly (e.g. clear the string / assign "") before constructing from the span buffer.

- **@Copilot** on `src/app/clusters/bridged-device-basic-information-server/BridgedDeviceBasicInformationCluster.cpp`

  > ReadAttribute encodes default/empty values for FixedData fields even when the attribute is omitted from the attribute list (Attributes() only advertises these when the corresponding optional has a value). This makes the cluster report unsupported attributes as readable (e.g. VendorName/ProductName/etc. returning empty, VendorID returning NotSpecified, ProductAppearance returning a default struct). To match the advertised attribute list, return UnsupportedAttribute for any optional attribute whose FixedData field is not present.
  > ```suggestion
  >         if (!mFixedData.vendorName.has_value())
  >         {
  >             return Status::UnsupportedAttribute;
  >         }
  >         return encoder.Encode(ToSpan(mFixedData.vendorName));
  >     case VendorID::Id:
  >         if (!mFixedData.vendorId.has_value())
  >         {
  >             return Status::UnsupportedAttribute;
  >         }
  >         return encoder.Encode(mFixedData.vendorId.value());
  >     case ProductName::Id:
  >         if (!mFixedData.productName.has_value())
  >         {
  >             return Status::UnsupportedAttribute;
  >         }
  >         return encoder.Encode(ToSpan(mFixedData.productName));
  >     case ProductID::Id:
  >         if (!mFixedData.productId.has_value())
  >         {
  >             return Status::UnsupportedAttribute;
  >         }
  >         return encoder.Encode(mFixedData.productId.value());
  >     case NodeLabel::Id:
  >         return encoder.Encode(ToSpan(mRequiredData.nodeLabel));
  >     case HardwareVersion::Id:
  >         if (!mFixedData.hardwareVersion.has_value())
  >         {
  >             return Status::UnsupportedAttribute;
  >         }
  >         return encoder.Encode(mFixedData.hardwareVersion.value());
  >     case HardwareVersionString::Id:
  >         if (!mFixedData.hardwareVersionString.has_value())
  >         {
  >             return Status::UnsupportedAttribute;
  >         }
  >         return encoder.Encode(ToSpan(mFixedData.hardwareVersionString));
  >     case SoftwareVersion::Id:
  >         if (!mFixedData.softwareVersion.has_value())
  >         {
  >             return Status::UnsupportedAttribute;
  >         }
  >         return encoder.Encode(mFixedData.softwareVersion.value());
  >     case SoftwareVersionString::Id:
  >         if (!mFixedData.softwareVersionString.has_value())
  >         {
  >             return Status::UnsupportedAttribute;
  >         }
  >         return encoder.Encode(ToSpan(mFixedData.softwareVersionString));
  >     case ManufacturingDate::Id:
  >         if (!mFixedData.manufacturingDate.has_value())
  >         {
  >             return Status::UnsupportedAttribute;
  >         }
  >         return encoder.Encode(ToSpan(mFixedData.manufacturingDate));
  >     case PartNumber::Id:
  >         if (!mFixedData.partNumber.has_value())
  >         {
  >             return Status::UnsupportedAttribute;
  >         }
  >         return encoder.Encode(ToSpan(mFixedData.partNumber));
  >     case ProductURL::Id:
  >         if (!mFixedData.productUrl.has_value())
  >         {
  >             return Status::UnsupportedAttribute;
  >         }
  >         return encoder.Encode(ToSpan(mFixedData.productUrl));
  >     case ProductLabel::Id:
  >         if (!mFixedData.productLabel.has_value())
  >         {
  >             return Status::UnsupportedAttribute;
  >         }
  >         return encoder.Encode(ToSpan(mFixedData.productLabel));
  >     case SerialNumber::Id:
  >         if (!mFixedData.serialNumber.has_value())
  >         {
  >             return Status::UnsupportedAttribute;
  >         }
  >         return encoder.Encode(ToSpan(mFixedData.serialNumber));
  >     case Reachable::Id:
  >         return encoder.Encode(mRequiredData.reachable);
  >     case UniqueID::Id:
  >         return encoder.Encode(ToSpan(mRequiredData.uniqueId));
  >     case ProductAppearance::Id:
  >         if (!mFixedData.productAppearance.has_value())
  >         {
  >             return Status::UnsupportedAttribute;
  >         }
  >         return encoder.Encode(mFixedData.productAppearance.value());
  > ```


## PR [#43204](https://github.com/project-chip/connectedhomeip/pull/43204)

- **@andy31415** on `examples/all-clusters-app/esp32/main/DeviceWithDisplay.cpp`

  > Maybe `LogErrorOnFailure` instead of temporary ignored? I would avoid adding new TEMPORARY_RETURN_IGNORED for new code.

- **@bzbarsky-apple** on `src/app/clusters/temperature-measurement-server/README.md`

  > Getter is still there, right? Just not setter.  And getter will not get the "current" value, just the default one.
  > 
  > But if we stop getting it in CodegenIntegration, we could make the getter go away too, right?

- **@bzbarsky-apple** on `src/app/clusters/temperature-measurement-server/CodegenIntegration.cpp`

  > I'm not sure it makes sense to have a ZAP-configured MeasuredValue...  I think we should just start the cluster off with null until the app actually provides a measurement.

- **@arielsz71** on `src/app/clusters/temperature-measurement-server/README.md`

  > I think that the decision to still have the getters is because for some attributes there is a default value in ember that we need to read in CodegenIntegration and pass this value to the cluster in the constructor (like a startup configuration for the cluster), and also in other cases for some attributes with Quality F (Fixed) the cluster itself does not provide any getter/setter, there is no setter because the attribute can not be changed once the cluster is created and there is no getter at the cluster layer because the value was received as a configuration or startup value in the constructor so in these cases all we have is the getter in ember that we can call from CodegenIntegration layer, so at some point we need these getters from ember and I do not think that they can be disabled on a per attribute basis, what was done is to disable the setters in ember for attributes that belong to code driven clusters and this way the cluster itself implements the setter notifying that the attribute has changed.

- **@arielsz71** on `src/app/clusters/temperature-measurement-server/CodegenIntegration.cpp`

  > I also think it is a good idea to start with attribute MeasuredValue in null as we did in the Illuminance Measurement cluster, I just added this code because I saw some .matter files that were setting a default value for this attribute in ember, I will check with the team if we can allow this behavior or not. Thanks.

- **@andy31415** on `src/app/clusters/temperature-measurement-server/CodegenIntegration.cpp`

  > I think "measured value" is whatever was measured. Starting out with "nothing was measured" seems reasonable. We can assume matter files with defaults were just wrong.

- **@andy31415** on `src/app/clusters/temperature-measurement-server/README.md`

  > We may need to find some solution here - we currently keep getters because we want the default ember values. However beyond that, `::Get` has no meaning (and we removed `::Set`).
  > 
  > What we probably want is:
  >   - ability to ::GetDefault/::SetDefault
  >   - remove ::Get/::Set as an ember API
  >   - can write our own ::Get/::Set replacement that works (affects defaults or uses cluster findcluster code) in codegenintegration.
  >   
  >   That way we could have fully backwards compatible API, however it costs flash.

- **@andy31415** on `examples/air-purifier-app/air-purifier-common/include/temperature-sensor-manager.h`

  > We could LogErrorOnFailure on these

- **@andy31415** on `examples/air-purifier-app/air-purifier-common/include/temperature-sensor-manager.h`

  > should these log? I wonder if we actually are calling this before or after server init (before we have no cluster yet so this will fail).
  > 
  > Does it make sense to support setters for min/max measured values? Should those not be set in the constructor and that is it? It is indeed odd that these attributes are not fixed :/

- **@andy31415** on `examples/air-quality-sensor-app/air-quality-sensor-common/air-quality-sensor-app.matter`

  > masuredValue should also be callback, there is no reason for us to load an initial value.

- **@andy31415** on `examples/all-clusters-minimal-app/esp32/main/DeviceWithDisplay.cpp`

  > Lets LogErrorOnFailure.
  > 
  > Or provide a replacement API for `chip::app::Clusters::TemperatureMeasurement::Attributes::MeasuredValue::Set` in CodegenIntegration.h/cpp.
  > 
  > I would lean towards that: that probably makes it easier for apps to upgrade without code changes, which seems desirable.

- **@arielsz71** on `src/app/clusters/temperature-measurement-server/CodegenIntegration.cpp`

  > Addressed.

- **@arielsz71** on `examples/air-quality-sensor-app/air-quality-sensor-common/air-quality-sensor-app.matter`

  > Addressed.

- **@arielsz71** on `examples/air-purifier-app/air-purifier-common/include/temperature-sensor-manager.h`

  > I understand, but I added the setters for min/max measured values because some apps are relying on them instead of specifying a default value for them in ember.

- **@arielsz71** on `examples/air-purifier-app/air-purifier-common/include/temperature-sensor-manager.h`

  > Also the spec does not say that these attributes are fixed (Quality F), they are marked Quality X.

- **@arielsz71** on `examples/all-clusters-minimal-app/esp32/main/DeviceWithDisplay.cpp`

  > If we provide a replacement API in CodegenIntegration I am worried about the flash increase and the fact that this cluster will be used differently than others, for example boolean state, illuminance measurement, etc are not providing replacement APIs in CodegenIntegration, the apps first need to find the cluster and then invoke the operation on the cluster itself, so it will be confusing that some clusters use a different approach from the apps perspective.

- **@arielsz71** on `examples/all-clusters-app/esp32/main/DeviceWithDisplay.cpp`

  > Addressed.

- **@arielsz71** on `examples/air-purifier-app/air-purifier-common/include/temperature-sensor-manager.h`

  > I prefer to use VerifyOrReturn() because the way it is being used here it also logs an error and returns, LogErrorOnFailure() does not return, it just logs an error and the code continues normal execution as I understand.

- **@arielsz71** on `examples/air-purifier-app/air-purifier-common/include/temperature-sensor-manager.h`

  > Added the log of an error message if the condition (temperatureMeasurement != nullptr) is not true.

- **@arielsz71** on `examples/all-clusters-minimal-app/esp32/main/DeviceWithDisplay.cpp`

  > Added the LogErrorOnFailure(), let's discuss a little more about adding replacement API in CodegenIntegration and maybe address that on a different PR?

- **@andy31415** on `examples/air-purifier-app/air-purifier-common/include/temperature-sensor-manager.h`

  > Returning here if there is a failure feels questionable to me. it seems the intent of this method is to "initialize all values to some startup values". There seems to be no clear requirement that "if min setting fails, we should not be setting max".

- **@andy31415** on `src/app/clusters/temperature-measurement-server/tests/TestTemperatureMeasurementCluster.cpp`

  > the double-scoping feels weird throughout. Please remove it.

- **@andy31415** on `src/app/clusters/temperature-measurement-server/CodegenIntegration.cpp`

  > if it fails to get are we guaraneteed to receive Null back? if not we should have a "if failure, set to null" logic here.

- **@andy31415** on `src/app/clusters/temperature-measurement-server/TemperatureMeasurementCluster.cpp`

  > should we have a named constant for 2048 as well?

- **@andy31415** on `src/app/clusters/temperature-measurement-server/TemperatureMeasurementCluster.cpp`

  > should these be a IM constraint error instead? technically it is invalid, but wondering if constraint is clearer as "invalid because constraints"

- **@andy31415** on `src/app/clusters/temperature-measurement-server/TemperatureMeasurementCluster.cpp`

  > This feels off: if both min and max are missing, then there should be no constraints. If both are null here though, this ends up being `kMinimumMinMeasuredValue` ... it should be max instead.
  > 
  > ```suggestion
  >                                 mMaxMeasuredValue.ValueOr(kMaximumMinMeasuredValue),
  > ```
  > 
  > the name `MaximumMin` feels off .... maybe `kMinMeasuredValueRange` and `kMaxMeasuredValueRange` are better constant names?

- **@andy31415** on `src/app/clusters/temperature-measurement-server/TemperatureMeasurementCluster.cpp`

  > Please also add a unit test that would have caught this. We need tests for:
  >  - both min/max set
  >  - only min set
  >  - only max set
  >  - neither min nor max is set

- **@andy31415** on `src/app/clusters/temperature-measurement-server/TemperatureMeasurementCluster.cpp`

  > If I have min/max as [0, 10] and I want to update to [20, 30] then the API will fail when I try to update the min ... so I have to sequence it in a correct order (and when going back I have to set min first then max).
  > 
  > How about instead having the API being a `SetMeasuredValueRange(min, max)` and that can validate the new values and their relationship?
  > 
  > That makes it clearer as doing a full update.

- **@arielsz71** on `src/app/clusters/temperature-measurement-server/TemperatureMeasurementCluster.cpp`

  > Addressed.


## PR [#43394](https://github.com/project-chip/connectedhomeip/pull/43394)

- **@andy31415** on `src/app/clusters/temperature-control-server/TemperatureControlCluster.h`

  > Spec seems to say `F` (i.e. Fixed) for min/max/step. Should we make these const or is there a reason to have them modifiable?

- **@andy31415** on `src/app/clusters/temperature-control-server/TemperatureControlCluster.cpp`

  > nit: I wonder if we invert the logic if we can flatten the structure for easier reading. Like having:
  > 
  > ```cpp
  > VerifyOrReturnError(targetTemperature.HasValue(), Status::InvalidCommand);
  > ...
  > ```
  > 
  > Same for below.

- **@andy31415** on `src/app/clusters/temperature-control-server/TemperatureControlCluster.cpp`

  > nit, maybe this is shorter:
  > 
  > 
  > ```suggestion
  > // Specification defined ranges and limits
  > constexpr int16_t kMinTemperatureRange = -27315;
  > constexpr int16_t kMaxTemperatureRange = 32766;
  > constexpr int16_t kMinStep = 1;
  > constexpr uint8_t kMaxSelectedTemperatureLevel = 31;
  > ```

- **@andy31415** on `src/app/clusters/temperature-control-server/TemperatureControlCluster.cpp`

  > should this go to the constant list above?

- **@arielsz71** on `src/app/clusters/temperature-control-server/TemperatureControlCluster.h`

  > Addressed.


## PR [#43423](https://github.com/project-chip/connectedhomeip/pull/43423)

- **@andy31415** on `src/app/clusters/zone-management-server/zone-management-server.h`

  > We generally should not be `using` in headers. The other ones seem to declare shorter names for deep hierarchy, however for `DataModel:` please don't do this - we try to minimize alternate names for the same thing.

- **@andy31415** on `src/app/clusters/zone-management-server/zone-management-server.h`

  > why do we need a forward declaration?
  > This may indicate some odd coupling - can we remove it?

- **@andy31415** on `src/app/clusters/zone-management-server/zone-management-server.h`

  > I see ZoneInformationStorage uses it. maybe this should be moved just in front of that and with a comment.

- **@andy31415** on `src/app/clusters/zone-management-server/zone-management-server.h`

  > Cluster class should go into ZoneManagementCluster.h (we place `class Foo` in `Foo.h`).
  > 
  > This is essentially a re-write and then CodegenIntegration.h (and cpp) can maintain a compatibility layer. In this case it may be that ZoneMgmtServer will be an alias to the cluster or will be a class that contains a cluster and implements the old API.
  > 
  > In this case it seems that an `Init` method exists, so I would expect the old compatibility API to do stuff in init such as registering the cluster to codegen.

- **@andy31415** on `src/app/clusters/zone-management-server/zone-management-server.h`

  > This constructor has 7 parameters, out of which 3 consecurive are integers.
  > 
  > Lets keep that for compat in CodegenIntegration/ZoneMgmtServer and have the cluster use a context structure for most things. I would expect we want `endpoint, features, context` as arguments or similar.

- **@andy31415** on `src/app/clusters/zone-management-server/zone-management-server.h`

  > Init on new clusters is odd ... we should probably not have this

- **@andy31415** on `src/app/clusters/zone-management-server/zone-management-server.h`

  > should this be const?

- **@andy31415** on `src/app/clusters/zone-management-server/zone-management-server.h`

  > This should not be part of the cluster. It can be part of the compat layer.

- **@andy31415** on `src/app/clusters/zone-management-server/zone-management-server.h`

  > endpoint id is already part of the DefaultServerCluster path. We should delete this member.

- **@andy31415** on `src/app/clusters/zone-management-server/zone-management-server.h`

  > This should not need to be set. Unsure about the `configured by app` member ... why do we need it? if needed, it should probably be near/together with mSensitivity since they relate to the same processing.

- **@andy31415** on `src/app/clusters/zone-management-server/zone-management-server.h`

  > I think DefaultServerCluster already has this method.

- **@andy31415** on `src/app/clusters/zone-management-server/ZoneManagementCluster.cpp`

  > This is not allowed in code driven clusters: this is codegen. This logic has to go in CodegenIntegration.cpp. I expect init registers and destructor there unregisters. Please also provide a `Deinit` method since relying on destructors is error prone (if people use globals, order of destructor calls in globals is not typically defined)

- **@andy31415** on `src/app/clusters/zone-management-server/ZoneManagementCluster.cpp`

  > This part should be in the compat layer in CodegenIntegration.cpp/h

- **@andy31415** on `src/app/clusters/zone-management-server/ZoneManagementCluster.cpp`

  > Include is not allowed in code driven clusters: you are not allowed to include codegen or src/app/util in most cases (the latter one is ember).

- **@andy31415** on `src/app/clusters/zone-management-server/ZoneManagementCluster.cpp`

  > Should we flip this to a LogErrorOnFailure at least to remove temporary_return_ignored?

- **@andy31415** on `src/app/clusters/zone-management-server/ZoneManagementCluster.cpp`

  > This probably needs some comment: why do we have this logic? Could we have this logic as part of the compatibility layer somehow and not have the cluster have extra logic?
  > 
  > or have sensitivity level a std::optional and if it has_value we assume configured by app?

- **@andy31415** on `src/app/clusters/zone-management-server/ZoneManagementCluster.cpp`

  > API contract enforces that Read/Write/Invoke is called on an existent path - if you have ::Attributes work correctly, we do not need these extra checks (makes code easier to read and saves flash, although for cameras we are probably on large devices).
  > 
  > Just do the encode without checks.

- **@andy31415** on `src/app/clusters/zone-management-server/ZoneManagementCluster.cpp`

  > same comment here: we can remove these checks

- **@andy31415** on `src/app/clusters/zone-management-server/ZoneManagementCluster.cpp`

  > Remove feature checks. Applies throughout - I will stop commenting on these.

- **@andy31415** on `src/app/clusters/zone-management-server/ZoneManagementCluster.cpp`

  > We have a helper for things that could be used:
  > 
  > https://github.com/project-chip/connectedhomeip/blob/master/src/app/server-cluster/DefaultServerCluster.h#L125
  > 
  > ```cpp
  > if (SetAttributeValue(mSensitivity, sensitivity, Sensitivity::Id))
  > {
  >     return Status::Success; // no change in sensitivity
  > }
  > // TODO: persist value logic
  > mDelegate.OnAttributeChanged(request.path.mAttributeId);
  > return Status::Success;
  > ```

- **@andy31415** on `src/app/clusters/zone-management-server/ZoneManagementCluster.cpp`

  > Append requires pre-allocation or it will fail. So either allow with verify on error above (probably allocate more) or AppendElements.
  > 
  > Could you make sure that we test this somehow? I expect this to fail in the camera app and I am surprised we did not catch the failure. unit tests should also cover this.

- **@andy31415** on `src/app/clusters/zone-management-server/ZoneManagementCluster.cpp`

  > same comment here: ensure unit tests cover these

- **@andy31415** on `src/app/clusters/zone-management-server/ZoneManagementCluster.cpp`

  > Marker for entire invoke: no need to check feature maps, we already should check them in `AcceptedCommands`

- **@andy31415** on `src/app/clusters/zone-management-server/ZoneManagementCluster.cpp`

  > Marker for entire invoke: no need to check feature maps, we already should check them in `AcceptedCommands`


## PR [#43471](https://github.com/project-chip/connectedhomeip/pull/43471)

- **@andy31415** on `src/app/clusters/actions-server/ActionsCluster.cpp`

  > could we include the smaller actions cluster specific structs/enums/commands etc headers?

- **@andy31415** on `src/app/clusters/actions-server/ActionsCluster.cpp`

  > Reporting is suspect - we should not be needing this.

- **@andy31415** on `src/app/clusters/actions-server/ActionsCluster.cpp`

  > DefaultServerCluster saves the context - that is all its startup does. We should not need this.


## PR [#43630](https://github.com/project-chip/connectedhomeip/pull/43630)

- **@shubhamdp** on `examples/air-purifier-app/air-purifier-common/src/air-quality-sensor-manager.cpp`

  > changed to using `SetAirQuality()`

- **@shubhamdp** on `examples/air-quality-sensor-app/air-quality-sensor-common/src/air-quality-sensor-manager.cpp`

  > changed to using `SetAirQuality()`

- **@shubhamdp** on `examples/air-quality-sensor-app/air-quality-sensor-common/src/air-quality-sensor-manager.cpp`

  > changed to using `SetAirQuality()`

- **@shubhamdp** on `examples/chef/common/chef-air-quality.cpp`

  > fixed!

- **@Copilot** on `src/app/clusters/air-quality-server/CodegenIntegration.h`

  > `GetAirQuality()` does not modify state and can be `const`, which would better match the underlying `AirQualityCluster::GetAirQuality() const` and allow use through `const Instance` references.

- **@Copilot** on `src/app/clusters/air-quality-server/AirQualityCluster.cpp`

  > `AirQualityCluster.cpp` uses `VerifyOrReturnError(...)` in `SetAirQuality`, but the file does not include the header that defines this macro. This will fail to compile in translation units where it is not brought in indirectly. Add an explicit include for the CodeUtils/verification macros header (as done in other clusters, e.g. OnOffCluster.cpp).

- **@Copilot** on `examples/air-quality-sensor-app/linux/AirQualitySensorAppAttrUpdateDelegate.cpp`

  > If `ScheduleWork(...)` fails, `handler` will never be processed or freed, which leaks the `AirQualitySensorAttrUpdateHandler` allocated by `FromJSON`. In the error path, delete/cleanup the handler (or otherwise transfer ownership) before returning/logging.

- **@Copilot** on `src/app/clusters/air-quality-server/CodegenIntegration.cpp`

  > `Instance::~Instance()` always calls `Registry().Unregister(&mCluster.Cluster())` and logs an error on any failure. If `Init()` was never called or registration failed, `Unregister` returns `CHIP_ERROR_NOT_FOUND`, which will generate noisy error logs even though this can be a normal lifecycle. Consider tracking a “registered” flag and only unregister when registration succeeded, or explicitly ignore `CHIP_ERROR_NOT_FOUND` here.

- **@Copilot** on `src/app/clusters/air-quality-server/CodegenIntegration.h`

  > The `UpdateAirQuality` docstring says the method returns `ConstraintError` when the value is not valid, but `SetAirQuality` can also return `InvalidValue` for out-of-range enum values. Update the comment to reflect the full set of possible statuses so callers know what to handle.

- **@shubhamdp** on `src/app/clusters/air-quality-server/AirQualityCluster.cpp`

  > DefaultServerCluster base class handles global attributes

- **@shubhamdp** on `src/app/clusters/air-quality-server/tests/TestAirQualityCluster.cpp`

  > DefaultServerCluster base class handles global attributes.

- **@shubhamdp** on `src/app/clusters/air-quality-server/CodegenIntegration.cpp`

  > IMO, its even okay to print that error

- **@shubhamdp** on `src/app/clusters/air-quality-server/README.md`

  > Added some text about the lifetime

- **@andy31415** on `src/app/clusters/air-quality-server/AirQualityCluster.h`

  > could the features be const?

- **@shubhamdp** on `src/app/clusters/air-quality-server/AirQualityCluster.h`

  > Yep, added const


## PR [#43633](https://github.com/project-chip/connectedhomeip/pull/43633)

- **@pimpalemahesh** on `src/app/clusters/closure-dimension-server/ClosureDimensionCluster.h`

  > Will update in follow up MR

- **@shubhamdp** on `examples/closure-app/silabs/src/ClosureManager.cpp`

  > We could have a local reference variable for the instance which could reduce a lot of `closurePanelEndpoint.GetClusterInstance()` to may be `instance` or some other name. Could be a follow-up

- **@shubhamdp** on `src/app/tests/TestClosureDimensionCluster.cpp`

  > should this be renamed as well?

- **@pimpalemahesh** on `src/app/tests/TestClosureDimensionCluster.cpp`

  > These file is going to change in follow up PR once all setters and getters will be updated as per the code driven. So thought let's keep it as it is just to reduce the diff.

- **@pimpalemahesh** on `examples/closure-app/silabs/src/ClosureManager.cpp`

  > Ok, will update in follow up MR!

- **@andy31415** on `src/app/clusters/closure-dimension-server/ClosureDimensionCluster.cpp`

  > should this be a `CHIP_IM_GLOBAL_STATUS(ConstraintError)` instead? invalid argument is also correct, but will map to Status::Failure for IM.

- **@andy31415** on `src/app/clusters/closure-dimension-server/ClosureDimensionCluster.cpp`

  > same comment below.

- **@andy31415** on `src/app/clusters/closure-dimension-server/ClosureDimensionCluster.cpp`

  > why not `mState.unitRange.SetNonNull(unitRange.Value());` as above?
  > 
  > Overall wondering if we should have something like:
  > 
  > ```cpp
  > const bool unitRangeChanged = mState.unitRange.IsNull()  // no range currently
  >   || (unitRange.Value().min != mState.unitRange.Value().min)
  >   || (unitRange.Value().max != mState.unitRange.Value().max);
  > 
  > VerifyOrReturnValue(unitRangeChanged, CHIP_NO_ERROR);
  > 
  > mState.unitRange.SetNonNull(unitRange.Value());
  > mMatterContext.MarkDirty(Attributes::UnitRange::Id);
  > return CHIP_NO_ERROR;
  > ```

- **@andy31415** on `src/app/clusters/closure-dimension-server/ClosureDimensionCluster.cpp`

  > Does `SetAttributeValue` work for nullable? or do we have no `operator==` for structures?

- **@andy31415** on `src/app/clusters/closure-dimension-server/ClosureDimensionCluster.cpp`

  > ```suggestion
  >     SetAttributeValue(mState.translationDirection, translationDirection, Attributes::TranslationDirection::Id);
  > ```
  > 
  > is maybe more standard if we can use it throughout.

- **@andy31415** on `src/app/clusters/closure-dimension-server/ClosureDimensionCluster.cpp`

  > This is a lot of code that feels like "new" however it is moved from "logic". Would it make sense to have an intermediate "move code PR" that moves code from Logic.cpp into Cluster.cpp so that the diff is smaller and easier to review?

- **@andy31415** on `src/app/clusters/closure-dimension-server/ClosureDimensionCluster.cpp`

  > Ah ... this is this PR ... sorry, I got only so far and thought this is the conversion part. Never mind.

- **@andy31415** on `src/app/clusters/closure-dimension-server/ClosureDimensionCluster.cpp`

  > This is not yet a conversion. Never mind.

- **@andy31415** on `src/app/clusters/closure-dimension-server/ClosureDimensionCluster.cpp`

  > If this is just a copy over of code, never mind.

- **@andy31415** on `src/app/clusters/closure-dimension-server/ClosureDimensionCluster.cpp`

  > If this is just copy over of code, never mind.


## PR [#43720](https://github.com/project-chip/connectedhomeip/pull/43720)

- **@andy31415** on `src/app/clusters/closure-dimension-server/ClosureDimensionCluster.cpp`

  > Does SetAttributeValue not work with nullables here and below on line 431? If it does not, could you create a new issue for this? It think we should try to make it work ...

- **@andy31415** on `src/app/clusters/closure-dimension-server/ClosureDimensionCluster.cpp`

  > nit: I wonder if we should move these in the header so that they can be inlined (I think if we place them in the cpp file they may not get inlined).

- **@andy31415** on `src/app/clusters/closure-dimension-server/CodegenIntegration.cpp`

  > Ember has a mapping of endpointid to indexes, so we should not use the endpointID itself as an index.
  > 
  > I.e. if I define ember endpoints 0,1, 10, 20, ember can return 0,1,2,3 (and further more it has logic to skip endpoints that don't have the cluster defined).
  > 
  > Please use https://github.com/project-chip/connectedhomeip/blob/master/src/data-model-providers/codegen/ClusterIntegration.h#L32
  > 
  > or see its endpoint to index logic: https://github.com/project-chip/connectedhomeip/blob/master/src/data-model-providers/codegen/ClusterIntegration.cpp#L34 (essentially call `emberAfGetClusterServerEndpointIndex` with the right parameters).

- **@andy31415** on `src/app/clusters/closure-dimension-server/CodegenIntegration.cpp`

  > Instead of separate arrays, how about defining a struct that contains all that we need and creating a single array?

- **@andy31415** on `src/app/clusters/closure-dimension-server/CodegenIntegration.h`

  > Is this existing API? If this is new API, I wonder if we should:
  >   - document `GetInstance` only being available after server startup
  >   - have a single `SetStartupParams` where we pass in all of delegates/conformance/initparams and document in must be called before GetInstance
  > 
  > It seems we generally completely removed the old interface (used to be `Interface` with `Logic`) so in that case we should have a README.md that describes changes and usage.

- **@pimpalemahesh** on `src/app/clusters/closure-dimension-server/ClosureDimensionCluster.cpp`

  > Yes, SetAttributeValue does not currently support nullable values. I’ve raised an issue for this—please have a look: [#43757](https://github.com/project-chip/connectedhomeip/issues/43757)

- **@pimpalemahesh** on `src/app/clusters/closure-dimension-server/ClosureDimensionCluster.cpp`

  > Moved all getters to the header file so they can be inlined.

- **@pimpalemahesh** on `src/app/clusters/closure-dimension-server/CodegenIntegration.cpp`

  > Correct. I’m now using the emberAfGetClusterServerEndpointIndex API to get the cluster index and using that index instead of the endpoint ID directly.

- **@pimpalemahesh** on `src/app/clusters/closure-dimension-server/CodegenIntegration.cpp`

  > Yes, switched to using a single array of ClosureDimensionCluster context structs instead of separate arrays.

- **@pimpalemahesh** on `src/app/clusters/closure-dimension-server/CodegenIntegration.h`

  > These are new APIs. I’ve switched to a single SetStartupParams API, updated the documentation for both, and added a README describing the changes and usage.

- **@andy31415** on `src/app/clusters/closure-dimension-server/CodegenIntegration.cpp`

  > These should not be endpointID - we should use the ember function to convert endpointid to 0-based index. Applies below as well.

- **@andy31415** on `src/app/clusters/closure-dimension-server/CodegenIntegration.cpp`

  > this should be `gServer[clusterInstanceIndex]`. You should not use endpointid for array indexing ever.
  > 
  > For example https://github.com/project-chip/connectedhomeip/blob/master/examples/all-clusters-app/all-clusters-common/all-clusters-app.matter#L8986 shows `endpoint 65534`
  > 
  > Endpoints are not in order and may have gaps.

- **@pimpalemahesh** on `src/app/clusters/closure-dimension-server/CodegenIntegration.cpp`

  > You're right. I've retained the existing Interface class, so we no longer need to maintain separate arrays.

- **@pimpalemahesh** on `src/app/clusters/closure-dimension-server/CodegenIntegration.cpp`

  > Removed array dependency.

- **@andy31415** on `examples/closure-app/closure-common/src/ClosureDimensionEndpoint.cpp`

  > can we preserve previous API and have mInterface expose methods?

- **@pimpalemahesh** on `examples/closure-app/closure-common/src/ClosureDimensionEndpoint.cpp`

  > I've now kept all API's in CodegenIntegration for backward compatibility

- **@andy31415** on `src/app/clusters/closure-dimension-server/ClosureDimensionCluster.cpp`

  > you can remove a bunch of `{}` wrappers here.

- **@pimpalemahesh** on `src/app/clusters/closure-dimension-server/ClosureDimensionCluster.cpp`

  > removed.

- **@soares-sergio** on `src/app/clusters/closure-dimension-server/ClosureDimensionCluster.cpp`

  > ```suggestion
  > //  i.e Speed and latch changes are not considered when calculating the at most 5 seconds quiet reportable changes for Position.
  > ```

- **@soares-sergio** on `src/app/clusters/closure-dimension-server/ClosureDimensionCluster.cpp`

  > I don't think you need to validate this.
  > If you have implemented AcceptedCommands properly, shouldn't the IM already deny and not forward the command to the cluster?

- **@soares-sergio** on `src/app/clusters/closure-dimension-server/ClosureDimensionCluster.h`

  > We discussed this pattern on the last code driven cluster call.
  > 
  > I suggest refactoring the cluster initialization to use a builder-style Config struct, similar to the pattern used in the LevelControlCluster (see [LevelControlCluster.h:L76](https://github.com/project-chip/connectedhomeip/blob/master/src/app/clusters/level-control/LevelControlCluster.h#L76)). This pattern is more ergonomic and prevents common errors where a feature is enabled but its mandatory parameters are missing.
  > 
  > For instance, instead of having separate ClusterConformance and ClusterInitParameters structs, you could implement a Config struct with methods that manage the FeatureMap and attribute values together:
  > 
  > ```
  > struct Config {
  >     Config(EndpointId endpoint, ClosureDimensionClusterDelegate & delegate) : 
  >         mEndpointId(endpoint), mDelegate(delegate) {}
  > 
  >     // Automatically sets PS and TR features and stores the direction
  >     Config & WithTranslation(TranslationDirectionEnum direction) {
  >         mFeatureMap.Set(Feature::kPositioning);
  >         mFeatureMap.Set(Feature::kTranslation);
  >         mTranslationDirection = direction;
  >         return *this;
  >     }
  > 
  >     Config & WithMotionLatching(BitFlags latchControlModes) {
  >         mFeatureMap.Set(Feature::kMotionLatching);
  >         mLatchControlModes = latchControlModes;
  >         return *this;
  >     }
  > 
  >     // Internal state for initialization
  >     EndpointId mEndpointId;
  >     ClosureDimensionClusterDelegate & mDelegate;
  >     BitFlags mFeatureMap;
  >     // ... other params
  > };
  > 
  > ```
  > 
  > This approach encapsulates the cluster's complex conformance logic (like PS being required for TR) directly into the configuration API, making it much harder for application developers to create an invalid configuration.
  > 
  > What do you think?

- **@soares-sergio** on `src/app/clusters/closure-dimension-server/ClosureDimensionCluster.h`

  > Documented this pattern here: https://github.com/project-chip/connectedhomeip/pull/71512

- **@shubhamdp** on `src/app/clusters/closure-dimension-server/ClosureDimensionCluster.cpp`

  > should we check if the unitRange is already null before updating this and notifying?

- **@shubhamdp** on `src/app/clusters/closure-dimension-server/ClosureDimensionCluster.cpp`

  > This function does not handle the non-null values, I think the above check was missed removed and we should add that back?

- **@andy31415** on `src/app/clusters/closure-dimension-server/tests/TestClosureDimensionCluster.cpp`

  > If listing commands is implemented correctly, this actually violates the API contract (you are not allowed to call a handle when the command is not supported ... also should Handle be public? Should we not test this via InvokeCommand?)

- **@andy31415** on `src/app/clusters/closure-dimension-server/tests/TestClosureDimensionCluster.cpp`

  > I think it would be better to use Invoke via clusterTester, to test real IM paths ... unless we want apps to have access to this API as a public API (I imagine maybe we do not?)

- **@pimpalemahesh** on `src/app/clusters/closure-dimension-server/ClosureDimensionCluster.cpp`

  > Done

- **@pimpalemahesh** on `src/app/clusters/closure-dimension-server/ClosureDimensionCluster.cpp`

  > right. removed this check.

- **@pimpalemahesh** on `src/app/clusters/closure-dimension-server/ClosureDimensionCluster.cpp`

  > Good catch. Added that check back.


## PR [#71424](https://github.com/project-chip/connectedhomeip/pull/71424)

- **@Elen777300** on `src/app/clusters/relative-humidity-measurement-server/RelativeHumidityMeasurementCluster.cpp`

  > kMandatoryMetadata is declared inside the RelativeHumidityMeasurement::Attributes namespace in the generated Metadata.h (not in 
  >   the RelativeHumidityMeasurement namespace as suggested).

- **@jadhavrohit924** on `src/app/clusters/relative-humidity-measurement-server/RelativeHumidityMeasurementCluster.h`

  > We should consider creating measurement base class implementation, approximately 15 clusters share the same data model with some extension. I would suggest create a base `MeasurementCluster` with and without `Tolerance` and let other clusters extend it.
  > 
  > cc: @andy31415

- **@Elen777300** on `src/app/clusters/relative-humidity-measurement-server/RelativeHumidityMeasurementCluster.h`

  > Good point! 
  > I'd suggest to do this in a follow-up 
  > - The type differences make a template base class non-trivial. Temperature uses `int16_t`, humidity uses `uint16_t`, and other clusters may use different types. A well-designed base would likely need to be templated, which has implications for code size and complexity.
  > - `TemperatureMeasurementCluster` is already merged as-is. Introducing a base class now would require refactoring that cluster too — effectively a separate PR on its own.

- **@andy31415** on `examples/air-purifier-app/air-purifier-common/include/relative-humidity-sensor-manager.h`

  > ```suggestion
  >         ReturnOnFailure(err,
  > ```
  > 
  > maybe is clearer. Or use `ReturnAndLogOnFailure` on the above call.

- **@andy31415** on `src/app/clusters/relative-humidity-measurement-server/CodegenIntegration.cpp`

  > I wonder if `CHIP_ERROR_INCORRECT_STATE` (meaning endpoint id not initialized) or `CHIP_ERROR_NOT_FOUND` (since cluster cannot be found) would be clearer, so that people do not think that invalid argument means that the value is invalud.
  > 
  > Same comment below.

- **@andy31415** on `src/app/clusters/relative-humidity-measurement-server/RelativeHumidityMeasurementCluster.cpp`

  > do we care about 0x270F? spec says `max 9999` so decimal number seems reasonabele. Same comment below.
  > 
  > Naming `kMaxMin` and `kMinMax` seems odd really. I wonder if we can find better names for these ranges. The spec seems to say:
  > 
  >  - MinMeasuredValue is `max 9999` so I do not see a minimum. It is unsigned, so minimum should be 0 right? so then the `1` below seems wrong ...
  > - MaxMeasuredValue is from `MinMeasuredValue+1 to 10000` ... so we have a max of 10000 globally. That should be our constant.

- **@Elen777300** on `src/app/clusters/relative-humidity-measurement-server/CodegenIntegration.cpp`

  > Done.

- **@Elen777300** on `examples/air-purifier-app/air-purifier-common/include/relative-humidity-sensor-manager.h`

  > Done

- **@Elen777300** on `src/app/clusters/relative-humidity-measurement-server/RelativeHumidityMeasurementCluster.cpp`

  > Yes that's odd. Refactored the names and comments

- **@soares-sergio** on `examples/air-purifier-app/air-purifier-common/include/relative-humidity-sensor-manager.h`

  > Looks like we weren't able to keep backwards compatibility. Please add a README.md with instructions on how to use this new cluster in both CodeDriven and Codegen (legacy) way.

- **@soares-sergio** on `examples/air-purifier-app/air-purifier-common/include/relative-humidity-sensor-manager.h`

  > If Set() and Get() are the only methods the legacy implementation used, I wonder if we could keep backwards compat in CodegenIntegration? Have you tried that?
  > 
  > Then you wouldn't need to change the examples

- **@soares-sergio** on `src/app/clusters/relative-humidity-measurement-server/RelativeHumidityMeasurementCluster.h`

  > Here's what I mentioned yesterday in the call.
  > 
  > Tolerance is an optional attribute. If the app doesn't need it, it shouldn't need to pass a value here but the struct wants it.
  > 
  > The other way is also a problem, the user puts a value here (believing it's gonna enable the attribute but forgets to set the `optionalAttributeSet`).
  > 
  > My preference is to make it easier for the user. Maybe hide `optionalAttributeSet` and have a `WithTolerance(uint16_t tolerance)` method that sets both the value and the attributeSet?

- **@soares-sergio** on `src/app/clusters/relative-humidity-measurement-server/RelativeHumidityMeasurementCluster.h`

  > min,max are fixed attributes. Do we need a  Set for it? Or could we just have the constructor values?
  > 
  > Similarly, tolerance is also fixed and you don't provide a set method for it.

- **@soares-sergio** on `src/app/clusters/relative-humidity-measurement-server/RelativeHumidityMeasurementCluster.h`

  > nit: I woudn't name this `Startup`. This is the configuration, not just a startup value. These are the values for the fixed attributes.
  > 
  > I would just name it `Configuration` or ` Config`.

- **@soares-sergio** on `src/app/clusters/relative-humidity-measurement-server/RelativeHumidityMeasurementCluster.cpp`

  > Should we validate the values here? otherwise this would end up allowing the app to configure incorrect/out of bounds values.
  > 
  > Please add the validation and unit tests

- **@soares-sergio** on `src/app/clusters/relative-humidity-measurement-server/RelativeHumidityMeasurementCluster.cpp`

  > Tolerance is capped at 2048, needs to be checked too

- **@soares-sergio** on `examples/air-purifier-app/air-purifier-common/include/relative-humidity-sensor-manager.h`

  > There's an example in other clusters. I believe Identify has one

- **@Elen777300** on `examples/air-purifier-app/air-purifier-common/include/relative-humidity-sensor-manager.h`

  > Done. Added a README following the same pattern as other clusters

- **@Elen777300** on `src/app/clusters/relative-humidity-measurement-server/RelativeHumidityMeasurementCluster.h`

  > Done. `optionalAttributeSet` is now private in `Config`. `WithTolerance(uint16_t)` is the only public way to enable the tolerance  attribute — it atomically sets both the value and the presence flag. If you don't call `WithTolerance`, the tolerance attribute is not included in the attribute list at all.

- **@Elen777300** on `src/app/clusters/relative-humidity-measurement-server/RelativeHumidityMeasurementCluster.h`

  > Right, removed SetMeasuredValueRange. Min/max are now set exclusively via Config at construction and cannot be changed at  runtime — consistent with how tolerance is handled

- **@Elen777300** on `src/app/clusters/relative-humidity-measurement-server/RelativeHumidityMeasurementCluster.h`

  > Done. Renamed to `Config`, following the convention used in other code-driven clusters

- **@Elen777300** on `src/app/clusters/relative-humidity-measurement-server/RelativeHumidityMeasurementCluster.cpp`

  > Added `constexpr uint16_t kMaxTolerance = 2048` and validate it in the constructor when tolerance is enabled. Covered by a test.

- **@Elen777300** on `src/app/clusters/relative-humidity-measurement-server/RelativeHumidityMeasurementCluster.cpp`

  > Done. The constructor validates: minMeasuredValue ≤ 9999, maxMeasuredValue ≤ 10000, max > min + 1 when both are non-null (via VerifyOrDie). Tests added in `ConfigValues` and `MeasuredValue`.

- **@Elen777300** on `examples/air-purifier-app/air-purifier-common/include/relative-humidity-sensor-manager.h`

  > The Set() accessors write to the Ember attribute store, which the code-driven cluster no longer reads — so keeping compat there isn't feasible. Instead, CodegenIntegration reads the ZAP-configured defaults via Get() at startup and passes them into Config. Apps that were calling Set() in Init() should configure those values in the ZAP file instead — the cluster will pick them up automatically.

- **@soares-sergio** on `src/app/clusters/relative-humidity-measurement-server/RelativeHumidityMeasurementCluster.cpp`

  > Could you double check if we need to implement WriteAttribute() or if DefaultServerCluster already implements the right status error return?

- **@soares-sergio** on `src/app/clusters/relative-humidity-measurement-server/RelativeHumidityMeasurementCluster.cpp`

  > These bounds check are duplicated here and in the Set methods.
  > 
  > Maybe see if a helper method method would make this easier in the anon namespace?

- **@soares-sergio** on `examples/air-quality-sensor-app/air-quality-sensor-common/include/relative-humidity-sensor-manager.h`

  > Code size still seems high. Could you post the size diff here as comment so we can double check?

- **@Elen777300** on `src/app/clusters/relative-humidity-measurement-server/RelativeHumidityMeasurementCluster.cpp`

  > It returns `Protocols::InteractionModel::Status::UnsupportedWrite;` which I think is the correct response for this case

- **@Elen777300** on `src/app/clusters/relative-humidity-measurement-server/RelativeHumidityMeasurementCluster.cpp`

  > The checks are actually different
  > The constructor validates the hardware config once at startup. 
  > `SetMeasuredValue` validates each new sensor reading against that config at runtime. 
  > They check different things so the helper won't really remove duplication


## PR [#71461](https://github.com/project-chip/connectedhomeip/pull/71461)

- **@andy31415** on `src/app/server-cluster/testing/TestServerClusterContext.h`

  > This is for testing.

- **@andy31415** on `src/app/clusters/time-synchronization-server/CodegenIntegration.cpp`

  > Could we add a TODO issue maybe to separate this out somehow? InteractionModelEngine is a massive object and injecting it seems not great. I am not sure if it will ever get fixed as this seems to actually want IM capabilities (it is a read client that reads time info from somewhere else) however complexity wise this seems rough.


## PR [#71552](https://github.com/project-chip/connectedhomeip/pull/71552)

- **@shubhamdp** on `src/app/clusters/flow-measurement-server/FlowMeasurementCluster.cpp`

  > Not applicable, this matches the existing pattern used by RelativeHumidityMeasurement, TemperatureMeasurement, and all other code-driven clusters. Global attributes are handled by the framework before ReadAttribute is called.

- **@Copilot** on `examples/chef/common/chef-pump.cpp`

  > updateSetPointsOnOff() ignores the CHIP_ERROR from FlowMeasurement::SetMeasuredValue, and the "off" path sets a non-null 0 value. With the new code-driven server enforcing Min/MaxMeasuredValue constraints, this call can now fail (e.g., minMeasuredValue defaults to 1 in this file), leaving the measured value stale/null with no logging. Consider using LogErrorOnFailure and (if the intent is “no flow when off”) setting a null value instead of 0 to avoid triggering ConstraintError when minMeasuredValue > 0.
  > ```suggestion
  >         auto updatedFlow = onOff ? FlowRangeMax[epIndex] : chip::app::DataModel::Nullable<uint16_t>();
  > ```

- **@Copilot** on `src/app/clusters/flow-measurement-server/tests/TestFlowMeasurementCluster.cpp`

  > The MeasuredValue tests cover 65534 but don’t cover the reserved nullable value 0xFFFF (65535). Since SetMeasuredValue should reject 0xFFFF as a non-null value, please add a unit test case that attempts to set measuredValue to 65535 and expects ConstraintError (and keep 65534 as the upper allowed value).

- **@Copilot** on `src/app/clusters/flow-measurement-server/FlowMeasurementCluster.cpp`

  > SetMeasuredValue() currently accepts any non-null uint16_t when Min/MaxMeasuredValue are null. For a nullable uint16 attribute, 0xFFFF is reserved for null and should be rejected as a non-null value (and generally the absolute max should be enforced even when MaxMeasuredValue is null). Please add an explicit check (e.g., reject 0xFFFF / enforce <= kMaxMeasuredValueMax) before calling SetAttributeValue so invalid values return ConstraintError.

- **@jadhavrohit924** on `src/app/clusters/flow-measurement-server/FlowMeasurementCluster.h`

  > `MinMeasuredValue` and `MaxMeasuredValue` are fixed, why do we need setter?

- **@shubhamdp** on `src/app/clusters/flow-measurement-server/tests/TestFlowMeasurementCluster.cpp`

  > None of the measurement clusters reject 0xFFFF

- **@shubhamdp** on `src/app/clusters/flow-measurement-server/FlowMeasurementCluster.cpp`

  > Same as above, this is consistent with all existing measurement cluster implementations

- **@shubhamdp** on `examples/chef/common/chef-pump.cpp`

  > 👍🏻

- **@shubhamdp** on `src/app/clusters/flow-measurement-server/FlowMeasurementCluster.h`

  > since this is needed for the tests, made it protected
