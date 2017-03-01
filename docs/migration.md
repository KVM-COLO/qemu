## VM Live Migration: Memory
- **Pre-copy**:
  - Memory state is copied first to the destination (through a repetitive process), after which the processor is transferred
  - Implemented by most Hypervisors (e.g., Xen, KVM, VMWare)

- **Post-copy**:
  - The VM's processor state is sent first, then its memory contents are transferred

## Pre-Copy Memory Migration
1. **Warm-up phase**:

  - Source VM is running while the hypervisor copies all memory pages from source to destination.

  - If some memory pages changed (become 'dirty') during this process, they will be copied until the rate of re-copied pages is not less than page dirtying rate.

2. **Stop-and-copy phase**: Source VM will be stopped; the CPU state and the remaining dirty pages will be copied. The destination VM is started.

"down-time": The time between stopping the VM on the original host and resuming it on destination.

## Post-COpy Memory Migration
1. **Suspend the VM at the source host**. A minimal subnet of the execution state of the VM (CPU state, registers, and optionally non-pageable memory) is transferred to the target.

2. **Resume the VM at the destination without any memory content**. Concurrently, the source actively pushes the remaining memory pages of the VM to the target - an activity known as pre-paging.

3. At the target, if the VM tries to access pages that have not been transferred, the VM is temporarily stopped and the fault pages are demand paged over the network from source. These faults are known as 'network faults'.