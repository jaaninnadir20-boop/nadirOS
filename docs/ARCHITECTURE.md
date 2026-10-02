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

## First milestone

The first implementation milestone is a bootable x86_64 kernel that can print a deterministic message and halt safely.

Later milestones will add hardware discovery, memory management, input, storage, userspace, and graphics.
