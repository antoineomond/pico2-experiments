pll_freq <- 150000000
xosc_freq <- 12000000
rosc_freq <- 10887000
lposc_freq <- 20000

name <- "baseline"
confs <- c("PLL", "XOSC", "ROSC", "LPOSC")
names = c(
	"5"  = "PLL",
	"11" = "XOSC",
	"17" = "ROSC"
)
clock_colors <- c(
	"PLL"   = "black",
	"XOSC"  = "purple",
	"ROSC"  = "brown",
	"LPOSC" = "dark green"
)
freqs <- c(
	"PLL"   = 150000000,
	"XOSC"  = 12000000,
	"ROSC"  = 10887000,
	"LPOSC" = 20000
)
df <- read.csv(paste(name, ".csv", sep=""))
max_expe_num <- 23
