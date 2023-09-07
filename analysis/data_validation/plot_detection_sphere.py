import pandas as pd
import numpy as np
import matplotlib.pyplot as plt

# PMT position dict containing the angles for each PMT
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

data_files = ["/home/nretza/ECP_libraries/Geant4-Simulations/P-OM/data/out_photon_ref_0.csv",
              "/home/nretza/ECP_libraries/Geant4-Simulations/P-OM/data/out_photon_ref_1.csv",
              "/home/nretza/ECP_libraries/Geant4-Simulations/P-OM/data/out_photon_ref_2.csv",
              "/home/nretza/ECP_libraries/Geant4-Simulations/P-OM/data/out_photon_ref_3.csv",
              ]

df = pd.DataFrame()

for file in data_files:
    df_temp = pd.read_csv(file)
    df_temp = df_temp.query("PID == -22 and g_x != 0 and g_y != 0 and g_z != 0 and out_ProcessName == 'OpAbsorption' and out_VolumeName.str.contains('photocathode')")
    df = pd.concat([df, df_temp], ignore_index=True)
flange_thickness = 90 # mm
df['g_z'] = df['g_z'] - flange_thickness/2 * np.sign(df["g_z"])


# transform glass contact points to polar coordinates
photons_g_r     = np.sqrt(df['g_x']**2 + df['g_y']**2 + (df['g_z'])**2)
photons_g_theta = np.pi/2 - np.arccos((df['g_z'])/photons_g_r)
photons_g_phi   = np.arctan2(df['g_y'], df['g_x'])

# Calculate the angular distance between glass contact point and detection pmt position
photon_g_angles            = np.array([photons_g_phi, photons_g_theta]).T
pmt_positions_for_photons  = np.take(np.deg2rad(list(pmt_positions.values())), df['out_Volume_CopyNo'], axis=0)
angular_distances_g_to_pmt = np.arccos(  np.sin(photon_g_angles[:,1]) * np.sin(pmt_positions_for_photons[:,1])
                                    + np.cos(photon_g_angles[:,1]) * np.cos(pmt_positions_for_photons[:,1])
                                    * np.cos(photon_g_angles[:,0] -pmt_positions_for_photons[:,0]))

# Calculate the angle between photon momentum and position (normal vector to glass).
photon_g_position = - np.array([df['g_x'], df['g_y'], df['g_z']]).T
photon_g_momentum = np.array([df['g_px'], df['g_py'], df['g_pz']]).T
angle_g_pos_mom   = np.arccos(np.clip(np.sum(photon_g_position * photon_g_momentum, axis=1)/(np.linalg.norm(photon_g_position, axis=1) * np.linalg.norm(photon_g_momentum, axis=1)), -1.0, 1.0))


photon_colors = []
photon_sizes  = []
for x, y in zip(angular_distances_g_to_pmt, angle_g_pos_mom):

    x = np.rad2deg(x)
    y = np.rad2deg(y)

    # cluster a
    if x < (15 + y * 5/45) or x < (20 + (y-45)*5/30):
        photon_colors.append('tab:orange')
        photon_sizes.append(0.2)

    #cluster b
    elif y > 20 and y < 40 and x < 55:
        photon_colors.append('tab:purple')
        photon_sizes.append(0.2)

    # cluster c
    elif x > 35 and x < 50 and y > 50 and y < 62:
        photon_colors.append('tab:red')
        photon_sizes.append(0.5)

    # else    
    else:
        photon_colors.append('tab:gray')
        photon_sizes.append(0.5)

# histogram to keep track of PMT hits
pmt_hits, pmt_hit_bins = np.histogram(df['out_Volume_CopyNo'], bins=range(len(pmt_positions) + 1))

# plot settings
dpi        = 300
font_size  = 6
s_min      = 50
s_max      = 500
alpha      = 0.5

plt.rcParams['figure.dpi']  = dpi
plt.rcParams['savefig.dpi'] = dpi

plt.figure()
plt.subplot(projection="mollweide")
plt.grid(True)
plt.xlabel("phi", fontsize = font_size)
plt.ylabel("theta", fontsize = font_size)
plt.xticks(fontsize=font_size) 
plt.yticks(fontsize=font_size)

# scatter plot for glass contact points
plt.scatter(photons_g_phi, photons_g_theta, s=photon_sizes, c=photon_colors, cmap='viridis')

# Plot circles for each PMT
for i, pos in pmt_positions.items():
    x = np.deg2rad(pos[0])
    y = np.deg2rad(pos[1])
    size = (s_max - s_min) * pmt_hits[i] / max(pmt_hits) + s_min
    plt.scatter(x, y, s=size, color="lightgray", alpha=alpha)
    plt.annotate(f"{pmt_hits[i]}", xy=(x, y), ha='center', va='center', size=font_size)
    if x == np.pi:
        plt.scatter(-np.pi, y, s=size, color="lightgray", alpha=alpha)
        plt.annotate(f"{pmt_hits[i]}", xy=(-np.pi, y), ha='center', va='center', size=font_size)

plt.show()