library("ggplot2")
library("dplyr")
library(rlang)

df <- read.csv("results.csv")
#df <- df %>%
#  mutate(
#    x_range = cut(
#      expe_num,
#      breaks = c(0, 15, 30),   # upper bound is exclusive by default
#      right = FALSE,              # [0,15), [15,30), ...
#      labels = c("non trimmed", "trimmed")
#    )
#  )
df <- df %>%
  mutate(
    legend_group = case_when(
      expe_num %in% c(0:5)     ~ "30MHz low vco",
      expe_num %in% c(12:17)   ~ "50MHz low vco",
      expe_num %in% c(24:29)   ~ "70MHz low vco",
      expe_num %in% c(36:41)   ~ "110MHz low vco",
      expe_num %in% c(48:53)   ~ "130MHz low vco",
      expe_num %in% c(60:65)   ~ "150MHz low vco",
      expe_num %in% c(6:11)    ~ "30MHz high vco",
      expe_num %in% c(18:23)   ~ "50MHz high vco",
      expe_num %in% c(30:35)   ~ "70MHz high vco",
      expe_num %in% c(42:47)   ~ "110MHz high vco",
      expe_num %in% c(54:59)   ~ "130MHz high vco",
      expe_num %in% c(66:71)   ~ "150MHz high vco",
      expe_num %in% c(78:83)   ~ "11MHz"
    )
  )

# If experiments are chained, the timestamp is not valid anymore. Values have to be grouped and plotted according to row number 
df <- df %>%
	group_by(legend_group) %>%
	arrange(legend_group) %>%
	mutate(index = row_number()) %>%
	ungroup()

#df <- df %>% filter(expe_num > 3)
p <- ggplot(df, aes(x = index, y = current_sample, color=legend_group, group=factor(expe_num))) +
	geom_line(na.rm = TRUE) +
	scale_y_continuous(limits=c(2, 16), n.breaks=15) +
	#geom_hline(yintercept = median(df[df$expe_num == 0,]$current_sample, na.rm = TRUE), color = "red") +
	#facet_wrap(~ x_range) +
	labs(title = "Running all 5 benchmarks one after the other. \nFor each target frequency, the closest configurations with the lowest \nand highest vreg are selected.", x = "Timestamp in seconds", y = "Current sample in mA") +
  scale_color_manual(
		name = "Configuration",
    values = c(
      "30MHz low vco"      = "black",
      "50MHz low vco"      = "purple",
      "70MHz low vco"      = "brown",
      "110MHz low vco"      = "dark green",
      "130MHz low vco"      = "dark blue",
      "150MHz low vco"      = "orange",
      "30MHz high vco"      = "black",
      "50MHz high vco"      = "purple",
      "70MHz high vco"      = "brown",
      "110MHz high vco"      = "dark green",
      "130MHz high vco"      = "dark blue",
      "150MHz high vco"      = "orange",
      "11MHz"      = "dark grey"
    )
  )
ggsave("results.pdf", plot=p)
