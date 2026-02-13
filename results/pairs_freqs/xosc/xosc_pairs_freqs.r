library("ggplot2")
library("dplyr")
library(rlang)

df <- read.csv("xosc_pairs_freqs_auto.csv")

# Associate expe_num with parameter (vreg)
df <- df %>%
  mutate(
    legend_group = case_when(
      expe_num %in% c(0:5)   ~ "1.1V",
      expe_num %in% c(6:11)   ~ "1.05V",
      expe_num %in% c(12:17)   ~ "1.00V",
      expe_num %in% c(18:23)   ~ "0.95V",
      expe_num %in% c(24:29)   ~ "0.90V",
      expe_num %in% c(30:35)   ~ "0.85V",
      expe_num %in% c(36:41)   ~ "0.80V",
      expe_num %in% c(42:47)   ~ "0.75V",
    )
  ) 

# If experiments are chained, the timestamp is not valid anymore. Values have to be grouped and plotted according to row number 
df <- df %>%
	group_by(legend_group) %>%
	arrange(legend_group) %>%
	mutate(index = row_number()) %>%
	ungroup()

p <- ggplot(df, aes(x = index, y = current_sample, color=legend_group, group=factor(expe_num))) +
	geom_line(na.rm = TRUE) +
	scale_y_continuous(limits=c(2, 16), n.breaks=15) +
	labs(title = "Running all 5 benchmarks one after the other. \nFor each target frequency, the closest configurations with the lowest \nand highest vreg are selected.", x = "Timestamp in seconds", y = "Current sample in mA") +
  scale_color_manual(
		name = "Configuration",
    values = c(
      "1.1V"      = "black",
      "1.05V"      = "purple",
      "1.00V"      = "brown",
      "0.95V"      = "dark green",
      "0.90V"      = "dark blue",
      "0.85V"      = "orange",
      "0.80V"      = "dark grey",
      "0.75V"      = "blue"
    )
  )
ggsave("xosc_pairs_freqs_auto.pdf", plot=p)
