# FMKFDCAN startup ownership

`FMKFDCAN_Init` initializes module-owned queues and arms CFG. It performs no
hardware initialization. The driver initialization function is local to
`FMK_FDCAN.c`; application modules cannot call or link to it.

Configure each node in `FMKFDCAN_ConfigPrivate.h`, in the const table
`c_FmkFdcan_NodeStartupCfg_as`:

- `driverCfg_s` contains its `t_sFMKFDCAN_DrvNodeCfg` settings.
- `initialize_b = TRUE` enables framework-owned initialization.
- `initialize_b = FALSE` leaves that node unconfigured and inactive.

The checked-in policy enables node 1 with classical CAN at 250 kbit/s. Node 2,
and node 3 on G4 configurations, are disabled. The table is outside generated
sections and is also present in the configuration template. Existing hardware
buffer, instance, GPIO, clock and interrupt configuration remains independent.
An enabled node must have nonzero RX/TX storage and valid driver settings.

`FMKFDCAN_Cyclic` processes at most one node per CFG call, then activates
configured nodes during PREOPE. It enters OPE only after startup completes.
`RC_WARNING_PENDING` preserves the current state. Hardware failures enter ERROR.
The existing operational monitoring and recovery path then owns active nodes.
Shared peripheral clock-divider constraints still apply to enabled nodes.

Clients wait for `FMKFDCAN_GetState` to report OPE and register their own
`FMKFDCAN_ConfigureRxItemEvent` subscriptions. They send through
`FMKFDCAN_SendTxItem`. Neither action configures a driver. APP_SIG preserves each
completed gate subscription while another gate remains pending.

The UDS module owns its CAN transport directly in
`APP_CTRL/APP_UDS/Src/APP_UDS.c`; the separate BoardPort files and former
`APPUDS_Fdcan.c/.h` files have been removed. It follows the same lifecycle and
public subscription/transmit APIs. UDS retains its approved disabled profile.

The PC simulator uses the same private startup table. The native acceptance
executable tests node 1 only, node 2 only, all nodes disabled, and invalid driver
settings. Run it together with the UDS suite:

```powershell
.pio/uds_venv/Scripts/python.exe -m unittest discover -s tests/uds -v
pio run
```

## Deferred application requests

After `FMKFDCAN_Init`, RX subscriptions can be stored before node configuration,
without HAL access. A full table returns `RC_WARNING_BUSY` and preserves existing
entries. Disabled nodes retain their software subscriptions.

`FMKFDCAN_SendTxItem` copies the header and payload into the bounded per-node
FIFO and sets `TxQueuePending_b`, independently of node readiness. The current
board's classic-CAN hardware and software payload capacity is eight bytes.
`RC_OK` means queued; a full queue returns `RC_WARNING_BUSY` without overwriting.

The existing CAN fast task remains the single TX consumer. It waits until the
node is configured and active, peeks the oldest frame, and discards it only after
the HAL copies it into message RAM. Hardware saturation or HAL_BUSY preserves
the frame and pending flag. An error retains the frame and raises the node
diagnostic. Short ConfigSpecific critical sections protect updates without
masking interrupts across HAL calls. FIFO indices remain private to `Library/FIFO`.
