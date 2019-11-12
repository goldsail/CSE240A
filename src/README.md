# Implementation Details

## Custom Branch Predictor: YAGS

The YAGS branch predictor by A. N. Eden and T. Mudge. 
[Paper](https://people.eecs.berkeley.edu/~kubitron/courses/cs252-F99/handouts/papers/mudge_yags.pdf)

A. N. Eden and T. Mudge, "The YAGS branch prediction scheme," Proceedings. 31st Annual ACM/IEEE International Symposium on Microarchitecture, Dallas, TX, USA, 1998, pp. 69-77. doi: 10.1109/MICRO.1998.742770

## Specs and Size Budget

- Specs:
    - 6-bit tags
    - One-way associated cache
- Total Size: 62 Kb
    - 26 Kb: Choice PHT (pattern history table): `2^11` entries, each entry 13 bits wide.
    - 16 Kb: BHT (branch history table): `2^13` entries, each entry 2 bit saturating counter.
    - 20 Kb: Two direction cache tables: `2^10` entries each, each entry 8 bits wide (6-bit tag plus 2-bit saturating counter)