library("ggplot2")
library("dplyr")
library(rlang)

name <- "baseline_w_ref_7-7ohms"
df <- read.csv(paste(name, ".csv", sep=""))
energy_consumption <- df %>%
  filter(expe_num %in% c(5, 11, 17), !is.na(energy_sample)) %>%
  group_by(expe_num) %>%
  slice_tail(n = 1) %>%
  mutate(expe_name = recode(expe_num,
	 `5`  = "PLL",
	 `11` = "XOSC",
	 `17` = "ROSC")) %>%
	mutate(expe_name = factor(expe_name, levels = c("PLL", "XOSC", "ROSC"))) %>%
  ungroup()

p <- ggplot(energy_consumption, aes(x = expe_name, y = energy_sample, fill=expe_name)) +
  geom_bar(stat = "identity", width = 0.2) +
  labs(x = "Processor clock",
	  y = "Total energy consumption in mJ",
	  title = ""
  ) +
  scale_fill_manual(name = "Processor clock",
    values = c(
      "PLL" = "black",
      "XOSC" = "purple",
      "ROSC" = "brown"
    )
  ) +
	theme(aspect.ratio = 4/1)


ggsave(paste(name, "-energy", ".pdf", sep=""), plot=p)
energy_consumption$energy_sample
