import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
from pandas.plotting import table
import os 
# Add in stats for compressed csv format

c_csv_list = os.listdir("compressed_csv_data/")
c_csv_list = sorted(c_csv_list)
c_csv_list = [os.path.join("compressed_csv_data/", x) for x in c_csv_list]
c_csv_byte_list = [os.path.getsize(x) for x in c_csv_list]

print(c_csv_list)
print(c_csv_byte_list)

# exit(0)



df = pd.read_csv("results.txt")

df_sorted = df.sort_values(by="file_name")

# ax = plt.subplot(frame_on=False) # no visible frame
# ax.xaxis.set_visible(False)  # hide the x axis
# ax.yaxis.set_visible(False)  # hide the y axis

# statistics = {
#     "average": df_sorted[["speedup", "space_saving_percent", "compression_ratio"]].mean(),
#     "min": df_sorted[["speedup", "space_saving_percent", "compression_ratio"]].min(),
#     "max": df_sorted[["speedup", "space_saving_percent", "compression_ratio"]].max(),
# }



# Combine statistics into a single DataFrame
# df_statistics = pd.DataFrame(statistics).T.rename_axis("Statistic").reset_index()

df_sorted["csv_size_bits"] = df_sorted["csv_size_bits"] / 8
df_sorted["bin_size_bits"] = df_sorted["bin_size_bits"] / 8

days = list(range(1, 32))

df_sorted.insert(0, "compressed_csv_size", c_csv_byte_list)
df_sorted.insert(0, "compression_ratio_csv", (df_sorted["csv_size_bits"] / df_sorted["compressed_csv_size"]))
# df["compression_ratio_csv"] = (df_sorted["csv_size_bits"] / df_sorted["compressed_csv_size"])
print("Average compression csv format: ", df_sorted["compression_ratio_csv"].mean())
plt.figure()
plt.bar(days, df_sorted["csv_read_time"], label="CSV")
plt.bar(days, df_sorted["binary_read_time"], label="Compressed")

plt.xticks(np.arange(1, 32, 2))
plt.xlabel("File")
plt.ylabel("Time in ms")
plt.title("Binary read time vs. raw csv time")
plt.yscale("log")
plt.legend()

plt.tight_layout()
plt.savefig("compressed_graphs/read_times.png")
# plt.show()

# Graph file sizes

plt.figure()
plt.bar(days, df_sorted["csv_size_bits"], label="Raw")
plt.bar(days, df_sorted["compressed_csv_size"], label = "Compressed CSV")
plt.bar(days, df_sorted["bin_size_bits"], label="Compressed BIN")

plt.xticks(np.arange(1, 32, 2))
plt.xlabel("File")
plt.ylabel("Byte count")
plt.title("Byte count of raw vs compressed data")
plt.legend()
plt.tight_layout()

plt.savefig("compressed_graphs/byte_counts.png")
plt.show()
exit(0)
# Plot compression ratio
# plt.figure()
# plt.bar(days, df_sorted["compression_ratio"])
# plt.xlabel("File")
# plt.ylabel("Compression Ratio")
# plt.show()
# exit(0)

# # Plot space saving percentage
# plt.figure(figsize=(10, 6))
# plt.bar(days, df_sorted["space_saving_percent"])
# plt.xlabel("File")
# plt.ylabel("Space Saving Percentage")
# plt.title("Space savings of compression")

# # Plot speedup
# plt.figure()
# plt.bar(days, df_sorted["speedup"])
# plt.title("Speed up of reading information")
# plt.xlabel("File")
# plt.ylabel("Speedup")

# # Plot number of ships
# plt.figure(figsize=(10, 6))
# plt.bar(days, df_sorted["num_ships"])
# plt.xlabel("File")
# plt.ylabel("Number of Ships")
# plt.title("Number of ships per day")

# plt.xlabel("File")
# plt.ylabel("Time in ms")

plt.show()



# print(df_sorted)