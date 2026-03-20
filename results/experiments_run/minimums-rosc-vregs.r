name <- "minimums-rosc-vregs"
confs <- c(
	"ROSC (baseline)",
	"ROSC (1.05V)",
	"ROSC (1.00V)",
	"ROSC (0.95V)",
	"ROSC (0.90V)",
	"ROSC (0.85V)",
	"ROSC (0.80V) (min)"
)
names = c(
	"5" = "ROSC (baseline)",
	"11" = "ROSC (1.05V)",
	"17" = "ROSC (1.00V)",
	"23" = "ROSC (0.95V)",
	"29" = "ROSC (0.90V)",
	"35" = "ROSC (0.85V)",
	"41" = "ROSC (0.80V) (min)"
)
clock_colors <- c(
	"ROSC (baseline)" = "black",
	"ROSC (1.05V)" = "purple",
	"ROSC (1.00V)" = "red",
	"ROSC (0.95V)" = "orange",
	"ROSC (0.90V)" = "dark green",
	"ROSC (0.85V)" = "dark grey",
	"ROSC (0.80V) (min)" = "brown"
)
freqs <- c(
	"ROSC (baseline)"    = 11020000,
	"ROSC (1.05V)"       = 9745000,
	"ROSC (1.00V)"       = 8577000,
	"ROSC (0.95V)"       = 7399000,
	"ROSC (0.90V)"       = 6196000,
	"ROSC (0.85V)"       = 5182000,
	"ROSC (0.80V) (min)" = 4298000
)
max_expe_num <- 41
df <- read.csv(paste(name, ".csv", sep=""))
x_power_median <- 300
y_power_median_offset <- 0.05
x_clock_freq <- 250 
y_max <- 25
