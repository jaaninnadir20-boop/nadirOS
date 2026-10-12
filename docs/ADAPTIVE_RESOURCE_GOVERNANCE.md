# Adaptive Resource Governance

**Status:** Proposed specification — not implemented yet  
**Default mode:** Adaptive  
**Default cache budget:** 1024 MiB (1 GiB) maximum, not a pre-allocation

## Purpose

Adaptive Resource Governance (ARG) lets NadirOS use only the resources a connected device can safely offer. NadirOS must adapt to the device instead of assuming that every phone has abundant RAM, storage, or CPU capacity. Resource limits are enforced by the component performing the work, not only by the user interface.

This specification applies to future device-bridge and companion-device sessions, including Android devices. It does not claim that the current bare-metal x86_64 prototype can inspect or control Android resources yet.

## Core principles

1. **Adaptive by default:** begin with a conservative footprint and scale only when measurements and capability checks justify it.
2. **No unbounded consumption:** every cache, queue, transfer, worker pool, and temporary workspace has an explicit limit.
3. **No forced escalation:** exceeding a configured budget requires clear, task-specific user approval. Approval cannot override limits enforced by the operating system, hardware, or security policy.
4. **Capability before permission:** inspect supported APIs, service availability, OS version, and current grants before asking the user for anything.
5. **Least privilege:** request only the permissions required for the selected task. A permission for one task must not grant unrelated access.
6. **Respect the host OS:** NadirOS must not bypass Android sandboxing, permission prompts, thermal management, memory pressure handling, or storage restrictions.
7. **Fail safely:** if capability or resource state cannot be determined reliably, use the conservative path or decline the task.
8. **Transparent controls:** show the task, estimated resource needs, current budget, reason for any increase, and an immediate stop option.

## Resource budgets

### Cache and storage

- Default maximum cache budget: **1024 MiB (1 GiB)** per NadirOS device-bridge installation/session policy.
- This is a ceiling, not a request to allocate or reserve 1 GiB immediately.
- Start with a small working set and grow on demand, only while within the budget and available-storage policy.
- Keep temporary files in an app-owned or explicitly authorized location. Do not write to arbitrary user directories.
- Use bounded queues and streaming transfers rather than buffering entire files or streams in memory.
- On reaching the cache limit, evict eligible regenerable cache entries first. Never silently delete user-created files or data that cannot be regenerated.
- If no safe eviction is possible, pause or reject the task and explain why.
- Any increase above the configured cache budget requires explicit, task-specific approval and a new upper bound. Never interpret approval as permission for unlimited storage.
- Clean up session-owned temporary data at session end, subject to clear disclosure and safe handling of files still in use.

### RAM

- Do not reserve a fixed large block of RAM on connection.
- Estimate a task's working set and keep it within a conservative, configurable budget.
- Prefer streaming, bounded queues, lazy loading, and a limited number of concurrent workers.
- Observe memory-pressure signals exposed by the host OS. Reduce concurrency, release caches, pause work, or stop the task when pressure rises.
- Never rely on a single memory snapshot as a guarantee that memory will remain available.
- On Android, NadirOS can use only the memory information and controls available to its app/service context; it cannot promise a guaranteed RAM reservation or override Android's process management.
- If a task cannot fit within safe limits, offer a lower-resource mode or decline it.

### CPU and thermal behavior

- Use bounded worker pools and cooperative cancellation.
- Adaptive mode should begin with low concurrency and scale gradually when the device is responsive and within thermal/power constraints.
- Respect Android and hardware thermal throttling; do not try to bypass it or manipulate hidden system controls.
- When heat, battery pressure, foreground responsiveness, or CPU saturation crosses configured thresholds, reduce work or pause background tasks.
- CPU percentages are targets or estimates, not hard guarantees unless the platform provides an enforceable mechanism.
- Any user-approved higher-performance mode remains bounded by platform, thermal, battery, and security limits.

## Operating modes

- **Minimal:** smallest practical footprint, few concurrent tasks, conservative cache use.
- **Adaptive (default):** dynamically adjusts concurrency and cache use to current capabilities, available resources, and task requirements.
- **Performance:** permits a higher user-configured budget but remains bounded; it never bypasses OS or hardware safeguards.

Mode changes must be visible and reversible. Switching to Performance must not silently change permissions or trust relationships.

## Capability and permission preflight

Before a task starts, the Device Capability Inspector and Adaptive Permission Firewall must:

1. Identify the peer and verify its authenticated identity.
2. Determine whether the requested feature is supported by the peer OS, available APIs, and installed companion service.
3. Determine which permissions are already granted, which can be requested through supported system flows, and which are unavailable or prohibited.
4. Estimate required storage, memory, CPU concurrency, and network/USB resources.
5. Request only missing permissions that are necessary and requestable.
6. Stop without prompting if the feature is unsupported, prohibited by policy, or impossible to perform.
7. Respect a prior denial for the same device, operation, and permission scope. Do not repeatedly prompt; retry only after a user-initiated action or a meaningful change in conditions.
8. Re-check grants and capabilities before sensitive operations and after reconnects or relevant state changes.

Biometric/PIN authentication stays within the operating system's trusted authentication flow. NadirOS must not collect or store the user's biometric data or device unlock PIN.

## Approval to exceed a budget

When a task needs more resources than its current budget, NadirOS must present a concise prompt showing:

- the task and reason for the increase;
- current and requested upper bounds;
- expected impact on storage, memory, CPU, heat, or battery where measurable;
- whether the request is temporary or persistent;
- the choices to approve the bounded increase, continue in reduced mode, or cancel.

Approval is scoped to the named task and budget, expires according to policy, and never grants unrelated permissions. Repeated prompts after a denial are prohibited unless the user explicitly retries or the relevant conditions materially change. No prompt may offer to override an OS-enforced or security-policy restriction.

## Stop and recovery behavior

- Provide a visible stop/cancel control for active sessions and resource-intensive tasks.
- On cancellation, stop scheduling new work, cancel cancellable operations, close session resources, and release owned memory and handles.
- Do not terminate unrelated user or system processes to free resources.
- Preserve user data and report any cleanup that could not safely complete.
- Record minimal diagnostic events without collecting unnecessary personal content.

## Required telemetry

Collect only what is needed to govern resources: estimated/current cache use, task working-set estimates where available, queue depth, worker count, task state, resource-pressure signals, and stop/limit events. Avoid logging file contents, credentials, biometric data, or unrelated personal data. Users should be able to inspect and clear local diagnostic history.

## Acceptance criteria

1. A new connection does not immediately allocate the full 1024 MiB cache budget.
2. Cache, queues, temporary files, and worker counts cannot grow without explicit bounds.
3. Exceeding a configured budget pauses or rejects the task until a bounded increase is approved.
4. A denial does not trigger repeated permission or resource prompts.
5. Unsupported or prohibited capabilities are reported without futile permission requests.
6. Under memory pressure, NadirOS reduces work or stops safely instead of killing unrelated processes.
7. Android integration uses supported APIs and never claims guaranteed RAM reservation or unrestricted CPU control.
8. Cancelling a session stops new work and releases NadirOS-owned resources without deleting user data.
9. Adaptive is the default mode, and the cache budget defaults to 1024 MiB.
10. Automated tests cover budget exhaustion, approval/denial, capability absence, reconnects, memory pressure, cancellation, and cleanup.

## Implementation status

This document records intended behavior only. Each feature must be labelled separately as planned, implemented, tested, and verified on target hardware. Android-specific behavior requires a companion component running under Android's normal security model; the bare-metal NadirOS kernel cannot directly enforce Android app permissions.
