# nadirOS Architecture

## Direction

nadirOS will start as a small x86_64 operating system instead of trying to replace an existing Linux distribution immediately.

## Layers

```
Firmware (UEFI/BIOS)
        |
        v
Boot stage
        |
        v
Kernel
  |-- CPU / GDT / IDT
  |-- Memory manager
  |-- Interrupts
  |-- Scheduler
  |-- Device layer
        |
        v
System services
  |-- Resource governance (planned)
  |-- Device bridge and trust policy (planned)
        |
        v
Userspace
  |-- Shell
  |-- System utilities
  |-- Apps
        |
        v
Graphics / Desktop
```

## Adaptive resource governance (planned)

NadirOS is intended to adapt to the capabilities and available resources of each connected device instead of forcing a fixed workload onto it. The proposed default mode is **Adaptive**, with a **1024 MiB maximum cache budget** that is a ceiling, not a pre-allocation.

Resource-heavy tasks must use bounded memory, storage, queues, and worker pools; respond to resource pressure; and stop safely when limits are reached. Any user-approved budget increase must be explicit, bounded, scoped to the task, and remain subject to operating-system and hardware restrictions. A denial must not trigger repeated prompts.

For the detailed requirements, limits, capability preflight, permission handling, and acceptance tests, see [Adaptive Resource Governance](ADAPTIVE_RESOURCE_GOVERNANCE.md).

This is a design specification, not a claim that resource governance or Android integration is implemented in the current kernel prototype.

## First milestone

The first implementation milestone is a bootable x86_64 kernel that can print a deterministic message and halt safely.

Later milestones will add hardware discovery, memory management, input, storage, userspace, and graphics.
