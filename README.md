# Electronic System Level Design – SystemC & TLM

This repository contains ESLD term projects implemented using SystemC and TLM-2.0.

The focus is on modeling system-level behavior, transaction-based communication, and early architectural exploration without RTL complexity.

---

## Projects Included

### 1. DMA Sensor Data Logging (TLM)
- CPU configured DMA-based data transfer
- Timing-annotated memory transactions
- Effective bandwidth analysis
- Loosely-Timed blocking transport model

### 2. Smart Room Controller (TLM)
- Temperature, Humidity, and PIR Motion sensors
- Threshold-based fan control logic
- Blocking (sensor read) + Non-blocking (fan write)
- Randomized testbench stimulus

---

## Concepts Covered

- Transaction-Level Modeling (TLM-2.0)
- Initiator and Target sockets
- Blocking vs Non-blocking transport
- Timing abstraction using `sc_time`
- Embedded control system modeling
- Architectural performance reasoning

---

## Objective

To understand ESL abstraction levels and model system behavior using SystemC before moving to RTL implementation.
