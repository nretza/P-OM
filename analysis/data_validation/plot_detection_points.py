
import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

# read the CSV file into a Pandas DataFrame
df = pd.read_csv("P-OM/data/out_photons.csv")

# colors
pmt_colors = plt.cm.tab20(np.linspace(0, 1, 16))

# filter DataFrame to only include photon absorption events in a photocathode
photocathode_df = df[(df["out_VolumeName"] == "photocathode") & (df["out_ProcessName"] == "OpAbsorption")]

# create a 3D scatter plot of the photon absorption events
fig = plt.figure()
ax = fig.add_subplot(projection="3d")
ax.scatter(photocathode_df["out_x"], photocathode_df["out_y"], photocathode_df["out_z"], c=pmt_colors[photocathode_df['out_Volume_CopyNo']], marker="o")
ax.set_title("points of absorbtions in the photocathode")
ax.set_xlabel("X")
ax.set_ylabel("Y")
ax.set_zlabel("Z")
plt.show()

