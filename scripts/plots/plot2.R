library(tidyr)
library(ggplot2)

general_width <- 8
general_height <- 5

data_gbn <- read.csv(file="data_gbn_big.csv", header=TRUE, sep=",")
data_gbn$type <- "gbn"

data <- rbind(data_gbn)

#data_final <- data[which(data$n_places < 15),]
data_final <- data

ggplot(data_final,aes(x = n_places, y = milliseconds, color=type)) +
  geom_smooth() +
  theme_classic() +
  xlab("#Places in CNU") +
  ylab("Milliseconds") + 
  labs(color="CNU Type")+
  ggtitle("Linear growth when increasing number of places for GBN \n(random CNUs, differing number of places, at least 1/3 successful transitions)")

ggsave("big_gbn_comparison.png",width=general_width,height=general_height)