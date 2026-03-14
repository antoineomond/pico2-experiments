name <- "minimums"
confs <- c(
	"PLL (min f)",
	"ROSC (min f)",
	"PLL (min v)",
	"XOSC (min v)",
	"ROSC (min v)",
	"PLL (min f v)",
	"XOSC (min f v)",
	"ROSC (min f v)"
)
names = c(
	"5"  = "PLL (min f)",
	"11" = "ROSC (min f)",
	"17" = "PLL (min v)",
	"23" = "XOSC (min v)",
	"29" = "ROSC (min v)",
	"35" = "PLL (min f v)",
	"41" = "XOSC (min f v)",
	"47" = "ROSC (min f v)"
)
clock_colors <- c(
	"PLL (min f)"   = "black",
	"ROSC (min f)"  = "purple",
	"PLL (min v)"  = "brown",
	"XOSC (min v)" = "dark green",
	"ROSC (min v)" = "red",
	"PLL (min f v)" = "grey",
	"XOSC (min f v)" = "green",
	"ROSC (min f v)" = "orange"
)
freqs <- c(
	"PLL (min f)"    = 15429000, 
	"ROSC (min f)"   = 2902000, 
	"PLL (min v)"    = 150000000, 
	"XOSC (min v)"   = 12000000, 
	"ROSC (min v)"   = 10881000, 
	"PLL (min f v)"  = 15428000, 
	"XOSC (min f v)" = 12000000, 
	"ROSC (min f v)" = 2902000 
)
df <- read.csv(paste(name, ".csv", sep=""))
max_expe_num <- 47

df <- df %>%
	filter(expe_num %in% c(0:5, 12:17, 30:35))
