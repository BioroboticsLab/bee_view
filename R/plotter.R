install.packages("ggplot2")

library(ggplot2)

df <- read.csv("plot_grid2.txt",header = FALSE)
names(df) <- c("x","y")

ggplot(df, aes(x = x, y = y)) + geom_point(shape=1)

df2 <- read.csv("plot_circle2.txt",header = FALSE)
names(df2) <- c("x","y")

ggplot(df2, aes(x = x, y = y)) + geom_point(shape=1)


ommatidia <- read.csv("ommatidia.csv",header = FALSE)
names(ommatidia) <- c("azimuth","elevation")

ggplot(ommatidia, aes(x = azimuth, y = elevation)) + geom_point(shape=1,colour = c("darkblue"),size=0.1)


sample_weights =  read.csv("sample_weights.txt",header = FALSE)
names(sample_weights) <- c("x","y","w")
ggplot(sample_weights, aes(x = x, y = y)) + geom_point(shape=1,aes(size=w))

