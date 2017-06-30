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

###############################

# ggplot settings
ggplot_settings <- list(theme_dark(),  
                        geom_point(aes(colour = w)),  
                        scale_colour_distiller(palette = "Reds", direction = 1))

# plot the different sampling funtions, disk and square

# circular samples weighted
samples =  read.csv("sample_462_weights.txt",header = FALSE)
names(samples) <- c("x","y","w")

plot_samples_disk_462 <-  ggplot(samples, aes(x = x, y = y)) + 
  ggplot_settings
  

ymax <- max(samples$w) / 2 # visualise the gauss distrib
plot_samples_disk_462_gauss <- ggplot(samples, aes(x = x, y = w)) + 
  geom_vline(xintercept = 1.3) +
  geom_vline(xintercept = -1.3) +
  geom_hline(yintercept = ymax) +
  ggplot_settings


# same for square
samples =  read.csv("sample_square_462_weights.txt",header = FALSE)
names(samples) <- c("x","y","w")

plot_samples_sqaure_462 <- ggplot(samples, aes(x = x, y = y)) + 
  ggplot_settings

ymax <- max(samples$w) / 2 # visualise the gauss distrib
plot_samples_sqaure_462_gauss <- ggplot(samples, aes(x = x, y = w)) + 
  geom_vline(xintercept = 1.3) +
  geom_vline(xintercept = -1.3) +
  geom_hline(yintercept = ymax) +
  ggplot_settings


# circular samples small
samples =  read.csv("sample_132_weights.txt",header = FALSE)
names(samples) <- c("x","y","w")
plot_samples_disk_132 <- ggplot(samples, aes(x = x, y = y)) + 
  ggplot_settings

ymax <- max(samples$w) / 2 # visualise the gauss distrib
plot_samples_disk_132_gauss <- ggplot(samples, aes(x = x, y = w)) + 
  geom_vline(xintercept = 1.3) +
  geom_vline(xintercept = -1.3) +
  geom_hline(yintercept = ymax) +
  ggplot_settings


# saqure samples small
samples =  read.csv("sample_square_132_weights.txt",header = FALSE)
names(samples) <- c("x","y","w")

plot_samples_square_132 <- ggplot(samples, aes(x = x, y = y)) + 
  ggplot_settings

# square the gauss distrib
ymax <- max(samples$w) / 2 # visualise the gauss distrib
plot_samples_square_132_gauss <- ggplot(samples, aes(x = x, y = w)) + 
  geom_vline(xintercept = 1.3) +
  geom_vline(xintercept = -1.3) +
  geom_hline(yintercept = ymax) +
  ggplot_settings

# circular samples very small
samples =  read.csv("sample_56_weights.txt",header = FALSE)
names(samples) <- c("x","y","w")

plot_samples_disk_56 <- ggplot(samples, aes(x = x, y = y)) + 
  ggplot_settings

ymax <- max(samples$w) / 2 # visualise the gauss distrib
plot_samples_disk_56_gauss <- ggplot(samples, aes(x = x, y = w)) + 
  geom_vline(xintercept = 1.3) +
  geom_vline(xintercept = -1.3) +
  geom_hline(yintercept = ymax) +
  ggplot_settings

# square samples very small
samples =  read.csv("sample_square_56_weights.txt",header = FALSE)
names(samples) <- c("x","y","w")

plot_samples_square_56 <- ggplot(samples, aes(x = x, y = y)) + 
  ggplot_settings

ymax <- max(samples$w) / 2 # visualise the gauss distrib
plot_samples_square_56_gauss <- ggplot(samples, aes(x = x, y = w)) + 
  geom_vline(xintercept = 1.3) +
  geom_vline(xintercept = -1.3) +
  geom_hline(yintercept = ymax) +
  ggplot_settings

# combine plots to singe plot
multiplot(plot_samples_square_56,plot_samples_square_56_gauss,plot_samples_disk_56,plot_samples_disk_56_gauss,
          plot_samples_square_132, plot_samples_square_132_gauss, plot_samples_disk_132,plot_samples_disk_132_gauss,
          plot_samples_sqaure_462, plot_samples_sqaure_462_gauss, plot_samples_disk_462, plot_samples_disk_462_gauss,
          cols=3)



# test gauss
samples <-  read.csv("test_sample_462_weights.txt",header = FALSE)
names(samples) <- c("x","y","w")

ymax <- max(samples$w) / 2

# visualise the gauss distrib
ggplot(samples, aes(x = x, y = w)) + 
  geom_vline(xintercept = 1.55) +
  geom_vline(xintercept = -1.55) +
  geom_hline(yintercept = ymax) +
  ggplot_settings



# Multiple plot function
#
# ggplot objects can be passed in ..., or to plotlist (as a list of ggplot objects)
# - cols:   Number of columns in layout
# - layout: A matrix specifying the layout. If present, 'cols' is ignored.
#
# If the layout is something like matrix(c(1,2,3,3), nrow=2, byrow=TRUE),
# then plot 1 will go in the upper left, 2 will go in the upper right, and
# 3 will go all the way across the bottom.
#
multiplot <- function(..., plotlist=NULL, file, cols=1, layout=NULL) {
  library(grid)
  
  # Make a list from the ... arguments and plotlist
  plots <- c(list(...), plotlist)
  
  numPlots = length(plots)
  
  # If layout is NULL, then use 'cols' to determine layout
  if (is.null(layout)) {
    # Make the panel
    # ncol: Number of columns of plots
    # nrow: Number of rows needed, calculated from # of cols
    layout <- matrix(seq(1, cols * ceiling(numPlots/cols)),
                     ncol = cols, nrow = ceiling(numPlots/cols))
  }
  
  if (numPlots==1) {
    print(plots[[1]])
    
  } else {
    # Set up the page
    grid.newpage()
    pushViewport(viewport(layout = grid.layout(nrow(layout), ncol(layout))))
    
    # Make each plot, in the correct location
    for (i in 1:numPlots) {
      # Get the i,j matrix positions of the regions that contain this subplot
      matchidx <- as.data.frame(which(layout == i, arr.ind = TRUE))
      
      print(plots[[i]], vp = viewport(layout.pos.row = matchidx$row,
                                      layout.pos.col = matchidx$col))
    }
  }
}