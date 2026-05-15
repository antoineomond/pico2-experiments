library("dplyr")
library(gridExtra)
options(dplyr.print_max = 1e9, pillar.width = Inf, width = 200)
df1_energy <- read.csv("pll_range_low_vreg/energy_consumption.csv")
df2_energy <- read.csv("rosc_range_low_vreg/energy_consumption.csv")
df1_power <- read.csv("pll_range_low_vreg/power_summary.csv")
df2_power <- read.csv("rosc_range_low_vreg/power_summary.csv")
df <- data.frame(
	gp1 = df1_energy$gp,
	gp2 = df2_energy$gp,
	energy1 = df1_energy$avg_energy,
	energy2 = df2_energy$avg_energy,
	energy_gain = ((df2_energy$avg_energy - df1_energy$avg_energy) / df1_energy$avg_energy) * 100,
	power1 = df1_power$power_median,
	power2 = df2_power$power_median,
	power_gain = ((df2_power$power_median - df1_power$power_median) / df1_power$power_median) * 100,
	time1 = df1_energy$avg_time,
	time2 = df2_energy$avg_time,
	time_diff = ((df2_energy$avg_time - df1_energy$avg_time) / df1_energy$avg_time) * 100
	
)

print(df)
write.csv(df, "lowvreg_PLL_lowvreg_ROSC.csv")
pdf("lowvreg_PLL_lowvreg_ROSC.pdf", height = 10, width = 20)
grid.table(df)
