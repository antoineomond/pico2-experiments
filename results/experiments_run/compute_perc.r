library("dplyr")
library(gridExtra)
options(dplyr.print_max = 1e9, pillar.width = Inf, width = 200)
first <- "pll_range/"
second <- "rosc_range_low_vreg/"
df1_energy <- read.csv(paste(first, "energy_consumption.csv", sep=""))
df2_energy <- read.csv(paste(second, "energy_consumption.csv", sep=""))
df1_power <- read.csv(paste(first, "power_summary.csv", sep=""))
df2_power <- read.csv(paste(second, "power_summary.csv", sep=""))
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
fill_matrix <- matrix("white", nrow = nrow(df), ncol = ncol(df))

fill_matrix[, which(names(df) == "energy_gain")] <-
  ifelse(df$energy_gain > 0, "#ffcccc",
         ifelse(df$energy_gain < 0, "#ccffcc", "white"))

fill_matrix[, which(names(df) == "power_gain")] <-
  ifelse(df$power_gain > 0, "#ffcccc",
         ifelse(df$power_gain < 0, "#ccffcc", "white"))

tt <- ttheme_default(
  core = list(
    bg_params = list(fill = fill_matrix)
  )
)

print(df)
filename <- "11V_PLL_11V_ROSC_colors"
write.csv(df, paste(filename, ".csv", sep=""))
pdf(paste(filename, ".pdf", sep=""), height = 10, width = 20)
grid.table(df, theme = tt)
