# Project 1 — Remote GitHub Development and Performance Monitoring
### Sonoma State University — CS 351
**Student:** Kevin Rodriguez  
**Repository:** Pwingles/SSU-CS-351  
**Project:** Project 1

---

## Q1 – Which program is fastest? Is it always the fastest?

| Program | Avg real time (s) @ 100k blocks | Min (s) | Max (s) |
|:--------|:-------------------------------:|:-------:|:-------:|
| alloca  | 0.169 | 0.160 | 0.200 |
| list    | 0.178 | 0.170 | 0.210 |
| malloc  | **0.167** | 0.160 | 0.210 |
| new     | 0.174 | 0.160 | 0.200 |

> **Answer:**  
> The `malloc.cpp` implementation is generally the fastest overall, averaging around **0.167 s**, with `alloca.cpp` close behind at **0.169 s**.  
> When data sizes or optimizations change, `alloca` occasionally matches or slightly beats `malloc`,  
> so `malloc` is usually but not always the fastest.

---

## Q2 – Which program is slowest? Is it always the slowest?

| Program | Avg Real (s) Debug | Avg Real (s) Optimized |
|:---------|:------------------:|:-----------------------:|
| alloca | 0.92 | 0.33 |
| list | 3.19 | 0.34 |
| malloc | 0.84 | 0.35 |
| **new** | **3.24** | **0.36** |

> **Answer:**  
> The `new.cpp` version is the slowest overall, averaging about **3.24 s** in debug and **0.36 s** in optimized mode.  
> It performs almost identically to `list.cpp` but is consistently a little slower.  
> Therefore, `new.cpp` is **consistently the slowest** implementation in both debug and optimized builds.

---

## Q3 – Was there a trend in program execution time based on the size of data in each Node?

| Data Size (MIN–MAX bytes) | alloca | list | malloc | new |
|:--------------------------:|:------:|:----:|:------:|:---:|
| 10–10   | 0.010 | 0.020 | 0.019 | 0.022 |
| 100–1000| 0.164 | 0.177 | 0.165 | 0.169 |
| 512–4096| 0.684 | 0.695 | 0.640 | 0.646 |

> **Answer:**  
> Execution time increases as the amount of data per node increases.  
> With small nodes (10 bytes), runtime is qucik.
> As node size grows all programs seem to slow down proportionally.

---

## Q4 – Was there a trend in program execution time based on the length of the block chain?

| Chain Length (Blocks) | alloca | list | malloc | new |
|:----------------------:|:------:|:----:|:------:|:---:|
| 10,000  | 0.020 | 0.019 | 0.019 | 0.020 |
| 100,000 | 0.177 | 0.178 | 0.166 | 0.176 |
| 1,000,000 | 1.741 | 1.864 | **1.627** | 1.736 |

> **Answer:**  
> Runtime grows roughly linearly with the number of blocks.  
> `malloc` seems to remain the fastest even at larger sizes.

---

## Q5 – Consider heap breaks. What’s noticeable?
### Does increasing the stack size affect the heap? Similarities/differences?

| Program | brk() Calls | Notes |
|:--------|:------------:|:------|
| alloca  | **69** | Allocates on stack — minimal heap use |
| list    | 242 | Allocates via STL allocator |
| malloc  | 205 | Allocates explicitly from heap |
| new     | 242 | Uses `new`, which also draws from heap |

> **Answer:**  
> Heap-based programs (`list`, `malloc`, `new`) all caused multiple **brk()** calls as the heap expanded.  
> `alloca` relies on the stack and caused almost no heap growth. 
>
>
> Increasing stack size (`ulimit -s unlimited`) doesn’t change heap allocation.
> The main similarity among heap-based versions is that they all trigger periodic page expansions.

---


### Q6 Considering either malloc.cpp or alloca.cpp, generate a diagram showing two Nodes.**

```

head ──► [ Node A ] ──► [ Node B ] ──► nullptr
             ^                ^
             |                |
           first            tail


struct Node {
    size_t size;    // number of data bytes
    uint8_t* bytes; // pointer to allocated memory
    Node* next;     // pointer to next node
};

Example: Node allocating 6 bytes of data

[ Node X ]
  size  = 6                ←── "size in bytes"
  bytes → +0  +1  +2  +3  +4  +5
            ^
            └── `bytes` points to the first byte (+0)
  next  = pointer to the next node (or nullptr for the last node)
```

---

### Q7 There's an overhead to allocating memory, initializing it, and eventually processing (in our case, hashing it). For each program, were any of these tasks the same? Which one(s) were different?
> **Answer:**  
All four programs perform the same main tasks — allocating memory, filling it with data, and hashing it — so initialization and hashing are identical.  
The difference lies in **how memory is allocated**:
- `alloca.cpp` → stack allocation (fastest, no heap use)  
- `malloc.cpp` → heap allocation using `malloc()` + placement `new`  
- `new.cpp` → heap allocation using `operator new`  
- `list.cpp` → uses `std::list` and its allocator (most overhead)


---
### Q8 As the size of data in a Node increases, does the significance of allocating the node increase or decrease?

**Answer:**  
As the data stored in each Node gets larger, the time spent on allocation becomes **less significant** compared to processing and hashing the data.  
For small Nodes, allocation overhead is noticeable, but as Node size grows, hashing dominates runtime, and the differences between allocation methods (stack vs heap) become smaller.