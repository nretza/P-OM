
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt

# read the CSV file into a Pandas DataFrame
df = pd.read_csv("P-OM/data/out00.csv")

# filter DataFrame to only include photon absorption events in a photocathode
photocathode_df = df[(df["out_VolumeName"].str.contains("photocathode")) & (df["out_ProcessName"] == "OpAbsorption")]

# Extract the copy number of the photocathode where the particle was absorbed
photocathode_copy_number = photocathode_df['out_Volume_CopyNo']

# create a numpy histogram for evaluation
hist, edges = np.histogram(photocathode_df['out_Volume_CopyNo'], bins=max(photocathode_copy_number)+1)
median = np.median(hist)

# Create a histogram of the photocathode copy numbers
plt.hist(photocathode_copy_number, bins=max(photocathode_copy_number)+1)
plt.axhline(median, color='gray', linestyle='--')
plt.xlabel('Photocathode Number')
plt.ylabel('Frequency')
plt.title('Hits per Photocathode')
plt.show()
