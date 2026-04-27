name <- "same_freqs-min_vco-min_vreg"
confs <- c(
	"PLL 30MHz",
	"ROSC 30MHz",
	"PLL 50MHz",
	"ROSC 50MHz",
	"PLL 110MHz",
	"ROSC 110MHz",
	"PLL 130MHz",
	"ROSC 130MHz",
	"PLL 150MHz (baseline)",
	"ROSC 150MHz"
)
names = c(
	"5"  = "PLL 30MHz",
	"11" = "ROSC 30MHz",
	"17" = "PLL 50MHz",
	"23" = "ROSC 50MHz",
	"29" = "PLL 110MHz",
	"35" = "ROSC 110MHz",
	"41" = "PLL 130MHz",
	"47" = "ROSC 130MHz",
	"53" = "PLL 150MHz (baseline)",
	"59" = "ROSC 150MHz"
)
clock_colors <- c(
	"PLL 30MHz"   = "blue",
	"ROSC 30MHz"  = "dark blue",
	"PLL 50MHz"   = "green",
	"ROSC 50MHz"  = "dark green",
	"PLL 110MHz"  = "orange",
	"ROSC 110MHz" = "brown",
	"PLL 130MHz"  = "red",
	"ROSC 130MHz" = "dark red",
	"PLL 150MHz (baseline)" = "black",
	"ROSC 150MHz" = "dark grey"
)
freqs <- c(
	"PLL 30MHz"   = 30240000,
	"ROSC 30MHz"  = 28419000,
	"PLL 50MHz"   = 50401000,
	"ROSC 50MHz"  = 47780000,
	"PLL 110MHz"  = 109714000,
	"ROSC 110MHz" = 109358000,
	"PLL 130MHz"  = 130000000,
	"ROSC 130MHz" = 128177000,
	"PLL 150MHz (baseline)" = 150000000,
	"ROSC 150MHz" = 147645000
)
max_expe_num <- 59
df <- read.csv(paste(name, ".csv", sep=""))
x_power_median <- 50
y_power_median_offset <- 0.05
x_clock_freq <- 25 
y_max <- 85
