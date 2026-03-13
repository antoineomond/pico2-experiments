pll_freq <- 150000000
xosc_freq <- 12000000
rosc_freq <- 10887000
lposc_freq <- 20000

name <- "baseline_w_ref_7-7ohms"
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
df <- read.csv(paste(name, ".csv", sep=""))
max_expe_num <- 23
