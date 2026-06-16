#!/usr/bin/python3

import numpy as np
import os
import shutil

N = 1e7

energies = [186.21,  295.224,  351.932, 609.321,   768.36,
            806.17,   839.04, 1120.294, 1238.11,  1377.67,
            1407.98, 1509.21, 1764.491, 1847.42, 2118.513,
            2204.21, 2447.69]

print(f'{len(energies)} peaks')

macro="""
# Geometry =====================================================================
/Bench/Construct
/CeBr3/GeometryFile geology.geo

/Source/Simple {en} keV

# Initialize run manager =======================================================
/run/initialize

/Output/Filename e{en}.out
/run/beamOn {n}
"""

for i in range(len(energies)):
    e = f"{energies[i]:.1f}"
    n = f"{N:.0f}"
    macFileName = f"e{e}.mac"
    if not os.path.exists(macFileName):
        macFile = open(macFileName, 'w')
        macFile.write( macro.format(en = e, n = n) )
        macFile.close()
