import numpy as np
import pandas as pd

pmt_positions = {
    0:  (     0,     25),
    1:  (    90,     25),
    2:  (   180,     25),
    3:  ( -  90,     25),
    4:  (    45,   57.5),
    5:  (   135,   57.5),
    6:  ( - 135,   57.5),
    7:  ( -  45,   57.5),
    8:  (    90, -   25),
    9:  (     0, -   25),
    10: ( -  90, -   25),
    11: (   180, -   25),
    12: (    45, - 57.5),
    13: ( -  45, - 57.5),
    14: ( - 135, - 57.5),
    15: (   135, - 57.5),
}

x = 200 # mm
y = 41  # mm
# threshold_rad = np.arctan(y/x)
threshold_rad = np.arcsin(75e-3 / 2 / 0.3)
print(f"angular threshold: {round(np.rad2deg(threshold_rad),2)}°")

df = pd.read_csv("/home/nretza/ECP_libraries/Geant4-Simulations/P-OM/data/out_photons_open_frame.csv")
df = df.query("PID == -22 and g_x != 0 and g_y != 0 and g_z != 0")

flange_thickness = 90 # mm
df['g_z'] = df['g_z'] - flange_thickness/2 * np.sign(df["g_z"])

photons_pos_r      = np.sqrt(df['g_x']**2 + df['g_y']**2 + (df['g_z'])**2)
photons_pos_theta  = np.pi/2 - np.arccos((df['g_z'])/photons_pos_r)
photons_pos_phi    = np.arctan2(df['g_y'], df['g_x'])
photons_pos_angles = np.array([photons_pos_phi, photons_pos_theta]).T

for i, pmt in pmt_positions.items():
    
    # https://en.wikipedia.org/wiki/Angular_distance
    photons_angular_distances_to_pmt = np.arccos( np.sin(photons_pos_angles[:,1]) * np.sin(np.deg2rad(pmt[1]))
                                                + np.cos(photons_pos_angles[:,1]) * np.cos(np.deg2rad(pmt[1]))
                                                * np.cos(photons_pos_angles[:,0]  - np.deg2rad(pmt[0])))
    
    photons_above_pmt = np.sum(photons_angular_distances_to_pmt < threshold_rad)
    photons_above_pmt_detected = df[(df['out_ProcessName'] == 'OpAbsorption')
                                  & (df['out_VolumeName'].str.contains('photocathode'))
                                  & (df['out_Volume_CopyNo'] == i)
                                  & (photons_angular_distances_to_pmt < threshold_rad)].shape[0]
    
    print(f"PMT {i:}\tPhotons above: {photons_above_pmt}\tPhotons detected: {photons_above_pmt_detected}\t{round(photons_above_pmt_detected/photons_above_pmt*100,2)}%")