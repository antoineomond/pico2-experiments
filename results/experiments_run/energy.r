library("ggplot2")
library("dplyr")
library(rlang)

df <- read.csv("baseline.csv")
energy_consumption <- df %>%
  filter(expe_num %in% c(5, 11, 17), !is.na(energy_sample)) %>%
  group_by(expe_num) %>%
  slice_tail(n = 1) %>%
  mutate(expe_name = recode(expe_num,
	 `5`  = "PLL",
	 `11` = "XOSC",
	 `17` = "ROSC")) %>%
  ungroup()

p <- ggplot(energy_consumption, aes(x = expe_name, y = energy_sample, fill=expe_name)) +
  geom_bar(stat = "identity") +
  labs(x = "Clock source",
	  y = "Total energy consumption in mJ",
	  title = "Energy consumption per clock source"
  ) +
  scale_color_manual(name = "Configuration",
    values = c(
      "PLL" = "black",
      "XOSC" = "purple",
      "ROSC" = "brown"
    )
  )

ggsave("energy.pdf", plot=p)
