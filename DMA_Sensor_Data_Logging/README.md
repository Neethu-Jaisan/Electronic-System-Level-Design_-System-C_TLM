# TLM-2.0 Based DMA Sensor Data Logging System  


## 1. Introduction

This project implements a simplified embedded data logging subsystem using SystemC and TLM-2.0.  
The objective is to demonstrate transaction-level modeling (TLM) for early architectural performance estimation in an ESL design flow.

A CPU configures a DMA controller to transfer battery sensor samples into a memory logging buffer. The system models communication and timing behavior at the transaction level.

---

## 2. Objective

- Model a DMA-based data transfer subsystem using TLM-2.0
- Implement blocking transport (`b_transport`)
- Perform timing annotation using `sc_time`
- Evaluate transfer latency and effective bandwidth
- Demonstrate ESL abstraction before RTL implementation

---

## 3. System Architecture

The system consists of three modules:

### CPU (Initiator)
- Programs DMA registers (source, destination, size)
- Initiates DMA transfer

### DMA Controller (Target + Initiator)
- Acts as a target for CPU configuration
- Acts as an initiator toward memory
- Performs byte-wise data transfer

### Memory (Target)
- Responds to transactions
- Models fixed access latency (5 ns per access)

---

## 4. Functional Flow

1. Sensor samples are preloaded into memory.
2. CPU writes configuration registers in DMA:
   - Source address
   - Destination address
   - Transfer size
3. DMA initiates read and write transactions.
4. Memory responds with annotated delay.
5. Total transfer time and bandwidth are computed.

---

## 5. Performance Analysis

For 32 bytes of sensor data:

- Each byte requires:
  - 1 read transaction
  - 1 write transaction
- Memory latency: 5 ns per access

Total Transfer Time:

32 × 2 × 5 ns = **320 ns**

Effective Bandwidth:

32 bytes / 320 ns = **1e8 Bytes/sec**

This demonstrates how memory latency directly impacts overall subsystem performance at transaction level.

---

## 6. ESL Concepts Demonstrated

- Transaction-Level Modeling (TLM-2.0)
- Initiator and Target sockets
- Blocking transport interface
- Timing annotation using `sc_time`
- Memory-mapped register modeling
- Early architectural performance estimation

---

## 7. Conclusion

The project illustrates how ESL modeling enables early evaluation of system performance without detailed RTL implementation. It highlights the impact of memory access latency on DMA-based data transfers and demonstrates abstraction-driven system-level design methodology.
