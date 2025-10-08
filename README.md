# OB-Aware: OrBital-Aware Method for LEO Satellite Networks in ns-3

This repository contains the official ns-3 implementation for the **OrBital-Aware (OB-Aware) routing method**, an extension of the AODV protocol. This work is part of a paper submitted to IEEE Access.

---

## Overview

This simulation code implements the OB-Aware routing method for Low Earth Orbit (LEO) satellite constellations. It is built as an extension of the AODV routing protocol in ns-3 and is designed to operate within a Gen. 1, and Gen. 2 satellite constellation environments.

---
## Requirements

  * **OS**: Ubuntu 22.04 LTS (Recommended)
  * **ns-3**: Version 3.35
  * **ns-3-leo**: The LEO satellite networks module, available from [https://github.com/dadada/ns-3-leo](https://github.com/dadada/ns-3-leo)

-----

## Installation Guide

1.  **Install ns-3.35.**
    Ensure you have a working installation of ns-3.

2.  **Install the ns-3-leo module.**
    Follow the instructions to install the LEO satellite module onto your ns-3.35 installation. You may find the guide at the following link helpful:

      * [Simulating LEO Satellite Network using ns-3 LEO module](https://www.projectguideline.com/simulating-leo-satellite-network-using-ns-3-leo-module/)

3.  **Replace the default AODV protocol files.**
  Rename OB-Aware-gen.1.cc (or gen.2.cc) and OB-Aware-gen.1.h (or gen.2.h) to aodv-routing-protocol.cc and aodv-routing-protocol.h respectively, and place them under the /ns-allinone-3.35/ns-3.35/src/aodv/model/ directory.

4.  **Re-build the ns-3 project.**
    Re-build the project to compile the changes.
-----

## License

The source code in this repository is licensed under the **GNU General Public License v2.0**. Please see the `LICENSE` file for more details.

```
```
