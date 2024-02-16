library(tidyverse)
library(ggplot2)
#-------------------------------------------------------------------------------
# WITH 1 OMP THREAD
mpi_timings_1<- read.csv("hybrid_scaling_240M_1thread.csv")

mpi_timings_1<- mpi_timings_1 %>% 
                select(!Threads) %>%
                group_by(Processes) %>%
                summarise(Time = mean(Time, na.rm = TRUE))

mpi_timings_1$SpeedUp<- mpi_timings_1$Time[1] / mpi_timings_1$Time
mpi_timings_1$omp<- "1 Thread"

mpi_timings_1 %>%
  ggplot(aes(x = Processes, y = Time)) +
  geom_vline(xintercept = c(2,4,8,16,32,64), linetype = "dashed", color = "grey")+
  annotate("text", x = c(2,4,8,16,32,64), y = rep(0,6), label = c(2,4,8,16,32,64), 
           vjust = 0, hjust = -0.1,color="grey50")+
  geom_line(col="indianred1") +
  geom_point(size=2)+
    labs(title = "Time vs MPI Processes (Size=240M ; OMP_NUM_THREADS=1)",
         x = "Processes",
         y = "Time") +
    theme_minimal()

# EMBARASSING :(
mpi_timings %>%
  ggplot(aes(x = Processes, y = SpeedUp)) +
  geom_point(size=2)+
  geom_line() +
  geom_vline(xintercept = c(2,4,8,16,32,64), linetype = "dashed", color = "grey")+
  annotate("text", x = c(2,4,8,16,32,64), y = rep(0,6), label = c(2,4,8,16,32,64),
           vjust = 0, hjust = -0.1,color="grey")+
  geom_abline(intercept = 0, slope = 1, linetype = "dashed", color = "black") +
  labs(title = "MPI SpeedUp",
       x = "Processes",
       y = "SpeedUp") +
  ylim(0,10)+
  theme_minimal()

#-------------------------------------------------------------------------------
# WITH 2 OMP THREAD
mpi_timings_2<- read.csv("hybrid_scaling_240M_2threads.csv")

mpi_timings_2<- mpi_timings_2 %>% 
  select(!Threads) %>%
  group_by(Processes) %>%
  summarise(Time = mean(Time, na.rm = TRUE))

mpi_timings_2$SpeedUp<- mpi_timings_2$Time[1] / mpi_timings_2$Time
mpi_timings_2$omp<- "2 Threads"

mpi_timings_2 %>%
  ggplot(aes(x = Processes, y = Time)) +
  geom_vline(xintercept = c(2,4,8,16,32,64), linetype = "dashed", color = "grey")+
  annotate("text", x = c(2,4,8,16,32,64), y = rep(0,6), label = c(2,4,8,16,32,64), 
           vjust = 0, hjust = -0.1,color="grey50")+
  geom_line(col="indianred1") +
  geom_point(size=2)+
  labs(title = "Time vs MPI Processes (Size=240M ; OMP_NUM_THREADS=2)",
       x = "Processes",
       y = "Time") +
  theme_minimal()
#-------------------------------------------------------------------------------
# WITH 4 OMP THREAD
mpi_timings_4<- read.csv("hybrid_scaling_240M_4threads.csv")

mpi_timings_4<- mpi_timings_4 %>% 
  select(!Threads) %>%
  group_by(Processes) %>%
  summarise(Time = mean(Time, na.rm = TRUE))

mpi_timings_4$SpeedUp<- mpi_timings_4$Time[1] / mpi_timings_4$Time
mpi_timings_4$omp<- "4 Threads"

mpi_timings_4 %>%
  ggplot(aes(x = Processes, y = Time)) +
  geom_vline(xintercept = c(2,4,8,16,32,64), linetype = "dashed", color = "grey")+
  annotate("text", x = c(2,4,8,16,32,64), y = rep(0,6), label = c(2,4,8,16,32,64), 
           vjust = 0, hjust = -0.1,color="grey50")+
  geom_line(col="indianred1") +
  geom_point(size=2)+
  labs(title = "Time vs MPI Processes (Size=240M ; OMP_NUM_THREADS=4)",
       x = "Processes",
       y = "Time") +
  theme_minimal()
#-------------------------------------------------------------------------------

# Plots overlapped

df= rbind(mpi_timings_1,mpi_timings_2,mpi_timings_4)
df %>%
  ggplot(aes(x = Processes, y = Time, color = omp)) +
  geom_vline(xintercept = c(2,4,8,16,32,64), linetype = "dashed", color = "grey60")+
  annotate("text", x = c(2,4,8,16,32,64), y = rep(0,6), label = c(2,4,8,16,32,64), 
           vjust = 0, hjust = -0.1,color="grey60")+
  geom_point(size=1)+
  geom_line(size=0.6) +
  scale_color_manual(values = c("#3498DB","#E67E22","#82CD47")) +
  labs(title = "Time vs MPI processes (SIZE= 240M size_t)",
       x = "MPI processes",
       y = "Time") +
  #ylim(0,10)+
  theme_minimal()





mpi_timings_4 %>%
  ggplot(aes(x = Processes, y = SpeedUp)) +
  geom_point(size=1.5)+
  geom_line(size=0.6) +
  geom_vline(xintercept = c(2,4,8,16,32,64), linetype = "dashed", color = "grey")+
  annotate("text", x = c(2,4,8,16,32,64), y = rep(0,6), label = c(2,4,8,16,32,64),
           vjust = 0, hjust = -0.1,color="grey")+
  geom_function(fun = function(x) log2(x), linetype = "dashed", col = "blue",size=0.6)+
  labs(title = "MPI SpeedUp",
       x = "Processes",
       y = "SpeedUp") +
  ylim(0,10)+
  theme_minimal()
