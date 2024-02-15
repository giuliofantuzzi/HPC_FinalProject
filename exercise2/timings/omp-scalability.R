library(tidyverse)
library(ggplot2)
omp_timings<- read.csv("omp_scaling.csv")
omp_timings<- omp_timings%>%
  group_by(Threads) %>%
  summarise(Time = min(Time, na.rm = TRUE))
omp_timings$SpeedUp<- omp_timings$Time[1] / omp_timings$Time
omp_timings$Type= "Not optimized"

omp_timings_L1<- read.csv("omp_scaling_L1.csv")
omp_timings_L1<- omp_timings_L1%>%
  group_by(Threads) %>%
  summarise(Time = min(Time, na.rm = TRUE))
omp_timings_L1$SpeedUp<- omp_timings_L1$Time[1] / omp_timings_L1$Time
omp_timings_L1$Type= "Optimized"


df= rbind(omp_timings,omp_timings_L1)
df$Type=as.factor(df$Type)


df %>%
  ggplot(aes(x = Threads, y = Time, color = Type)) +
  geom_point(size=2)+
  geom_line() +
  geom_vline(xintercept = c(2,4,8,16,32,64), linetype = "dashed", color = "grey")+
  annotate("text", x = c(2,4,8,16,32,64), y = rep(0,6), label = c(2,4,8,16,32,64), 
           vjust = 0, hjust = -0.1,color="grey50")+
    labs(title = "Time vs Threads",
         x = "Threads",
         y = "Time") +
    theme_minimal()

df %>%
  ggplot(aes(x = Threads, y = SpeedUp, color = Type)) +
  geom_point(size=2)+
  geom_line() +
  geom_vline(xintercept = c(2,4,8,16,32,64), linetype = "dashed", color = "grey")+
  annotate("text", x = c(2,4,8,16,32,64), y = rep(0,6), label = c(2,4,8,16,32,64), 
           vjust = 0, hjust = -0.1,color="grey")+
  geom_abline(intercept = 0, slope = 1, linetype = "dashed", color = "black") +
  labs(title = "OMP SpeedUp",
       x = "Threads",
       y = "SpeedUp") +
  ylim(0,10)+
  theme_minimal()
