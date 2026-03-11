library("ggplot2")
library("dplyr")
library(rlang)

df <- read.csv("lposc_freqs.csv")
p <- ggplot(df, aes(x = vreg, y = clock_freq)) +
  geom_point() +
	facet_wrap(~trim) +
  labs(x = "VREG output in V",
	  y = "Clock frequency",
	  title = "Clock frequency according to VREG output and trimming value"
  )

ggsave("lposc_freq_scatter.pdf", plot=p)
