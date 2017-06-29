#install.packages("ggplot2")

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


# ggplot settings
ggplot_settings <- list(theme_dark(),  
                        geom_point(aes(colour = w)),  
                        scale_colour_distiller(palette = "YlOrRd", direction = 1))

# circular samples weighted
samples =  read.csv("sample_462_weights.txt",header = FALSE)
names(samples) <- c("x","y","w")
ggplot(samples, aes(x = x, y = y)) + 
  ggplot_settings
  

# visualise the gauss distrib
ggplot(samples, aes(x = x, y = w)) + 
  ggplot_settings


# same for square
samples =  read.csv("sample_square_462_weights.txt",header = FALSE)
names(samples) <- c("x","y","w")
ggplot(samples, aes(x = x, y = y)) + 
  ggplot_settings

# visualise the gauss distrib
ggplot(samples, aes(x = x, y = w)) + 
  ggplot_settings


# circular samples small
samples =  read.csv("sample_132_weights.txt",header = FALSE)
names(samples) <- c("x","y","w")
ggplot(samples, aes(x = x, y = y)) + 
  ggplot_settings

# visualise the gauss distrib
ggplot(samples, aes(x = x, y = w)) + 
  ggplot_settings


# saqure samples small
samples =  read.csv("sample_square_132_weights.txt",header = FALSE)
names(samples) <- c("x","y","w")
ggplot(samples, aes(x = x, y = y)) + 
  ggplot_settings

# square the gauss distrib
ggplot(samples, aes(x = x, y = w)) + 
  ggplot_settings

# circular samples very small
samples =  read.csv("sample_56_weights.txt",header = FALSE)
names(samples) <- c("x","y","w")
ggplot(samples, aes(x = x, y = y)) + 
  ggplot_settings

# visualise the gauss distrib
ggplot(samples, aes(x = x, y = w)) + 
  ggplot_settings

# square samples very small
samples =  read.csv("sample_square_56_weights.txt",header = FALSE)
names(samples) <- c("x","y","w")
ggplot(samples, aes(x = x, y = y)) + 
  ggplot_settings

# visualise the gauss distrib
ggplot(samples, aes(x = x, y = w)) + 
  ggplot_settings
