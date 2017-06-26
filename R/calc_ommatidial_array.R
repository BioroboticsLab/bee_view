
# TODO code directly and not store in csv to prevent fragmentation
azimuth <- read.csv("azimuth_max.csv", stringsAsFactors=FALSE, encoding = "UTF-8")

# create bilinear interpolationfunctions for the eye boundaries (azimuth max), 
# get elevation as parameter, return azimuth 
# (for elevation > 70: problem(?) very large return values)
# azimuthMaxFun_1_2 for zones 1 and 2, azimuthMaxFun_3_4 for zones 3 and 4
azimuthMaxFun_1_2 <- splinefun(azimuth$elevation_1_2, azimuth$azimuth_max_1_2, method = "monoH.FC") #or method = ffm
azimuthMaxFun_3_4 <- splinefun(azimuth$elevation_3_4, azimuth$azimuth_max_3_4, method = "monoH.FC")

# horizontal and vertical interomatidial angle constants
ioa_h_min <- 2.4
ioa_h_mid <- 3.7
ioa_h_max <- 4.6

ioa_v_min <- 1.5
ioa_v_max <- 4.5

# transforms the horizontal interommatidial angle that is
# given as an angle on a great circle, to increments of the azimuth
# angle
# R assumes degrees: (degToRad = x * pi/180 = x * 0.01745329251)
ioa_h_to_azimuth <- function(ioa_h,elevation) {
  
  rad <- 0.01745329251
  
  return(2*asin((sin((ioa_h/2)*rad))/(cos(elevation*rad)))*(180/pi))
}

# horizontal interommatidial angle for zone 1 and 2
ioa_h_1_2 <- function(a, e) {
  
  if(a >= 0 && a <= 45) {
    return(ioa_h_mid + (a/45)*(ioa_h_min - ioa_h_mid))
  }
  if(a > 45 && a <= 90) {
    return (ioa_h_min + ((a - 45)/45)*(ioa_h_mid - ioa_h_min))
  }
  if(a > 90 && a <= 150 && abs(e) <= 50) {
    return (ioa_h_mid + ((a - 90)/60)*(ioa_h_max - ioa_h_mid))
  }
  if(a > 90 && a <= 150 && abs(e) > 50) {   
    return (ioa_h_mid + ((a - 90)/60)*(ioa_h_max - ioa_h_mid) * ((90 - abs(e)) / 40))
  }
  if(a > 150 && a <= 180 && abs(e) <= 50) {
    return(ioa_h_max)
  }
  if(a > 150 && a <= 180 && abs(e) > 50) {
    return (ioa_h_mid + (ioa_h_max - ioa_h_mid)*((90 - abs(e))/40))
  }
  if(a > 180 && a <= 270) {
    return (ioa_h_max)
  }
}

# horizontal interommatidial angle for zone 3
ioa_h_3 <- function(a, e) {
  
  if(a >= -90 && a <= -45 && e <= -50) {
    return ( ioa_h_mid + ( (abs(a)-45)/45 ) * (ioa_h_max - ioa_h_mid) * ( (90-abs(e))/40 ) )
  }
  
  if(a >= -45 && a <= 0 && e <= -50) {
    return ( ioa_h_mid )
  }
  
  if(a >= -45 && a <= 0 && e > -50 && e <= 0) {
    return ( ioa_h_mid + ( abs(a)/45 ) * (ioa_h_max - ioa_h_mid) * ( (90 - e)/40) )
  }
}

# horizontal interommatidial angle for zone 4
ioa_h_4 <- function(a, e) {
  
  if(a >= -90 && a <= -45 && e >= 50) {
    return(ioa_h_max + (ioa_h_max -ioa_h_mid) * ((90 - abs(e)) / 40 ))
  }
  
  if(a > -45 && a <= 0 && e >= 0 && e <= 50) { # problem: e > 0: for e = 0 not defined
    return( ioa_h_mid + (abs(a)/45) * (ioa_h_max - ioa_h_mid))
  }
  
  if(a >= -45 && a <= 0 && e >= 50) {
    return( ioa_h_mid + (abs(a)/45) * (ioa_h_max - ioa_h_mid) * ((90 - e)/40)) # TODO probably shoulkd be  abs(e), no e alway positive
  }
}

get_ioa_h <- function(a, e, zone) {
  
  if(zone == 1 || zone == 2) {
    return (ioa_h_1_2(a, e))
  }
  
  if(zone == 3) {
    return (ioa_h_3(a, e))
  }
  
  if(zone == 4) {
    return (ioa_h_4(a, e))
  }
  
}

get_sign_a <- function(zone) {
  if(zone == 1 || zone == 2) {
    return( 1)
  }
  if(zone == 3 || zone == 4) {
    return(-1)
  }
  
}

get_sign_e <- function(zone) {
  if(zone == 1 || zone == 4) {
    return(1)
  }
  if(zone == 2 || zone == 3) {
    return(-1)
  }
}

get_azimuthMaxFun <- function(zone) {
  if(zone == 1 || zone == 2){
    return(azimuthMaxFun_1_2)
  }
  if(zone == 3 || zone == 4){
    return(azimuthMaxFun_3_4)
  }
}

# condition for loop
get_cond <- function(a, e, zone) {
  if(zone == 1 || zone == 2){
    return(abs(a)<=abs(get_azimuthMaxFun(zone)(e)) && a < 270 && abs(e)<=88.9)
  }
  if(zone == 3 || zone == 4){
    return(abs(a)<=abs(get_azimuthMaxFun(zone)(e)) && a > -90 && abs(e)<=88.9)
  }
}

get_omatidia_count <- function(zone) {
  if(zone == 1){
    return(2483)
  }
  if(zone == 2){
    return(2382)
  }
  if(zone == 3){
    return(358)
  }
  if(zone == 4){
    return(311)
  }
}

# main function
calculate_angles <- function(zone){

  omatidia <- data.frame(matrix(data = 0, nrow = get_omatidia_count(zone), ncol = 2))
  names(omatidia) <- c("x","y")
  
  #!TODO do I need i for arrangement of ommatidia?
  
  j <- 0
  counter <- 0
  
  e <- 0
  
  while(abs(e)<=90) {
    
    a <- ifelse(j%%2==0,0,ioa_h_to_azimuth(get_sign_a(zone) * ioa_h_mid/2,e)) # for even/odd index
    
    while(get_cond(a,e,zone)) { # for zone 1 and 2: a <270, for zone 3: a > -90
      
      #print(paste0("a: ",a,"; e: ",e,"; counter: ", counter, "; j: ", j))
      
      omatidia[counter,1] <- a
      omatidia[counter,2] <- e
      
      ioa_h <- get_ioa_h(a, e,zone)
      
      #a <- a - ifelse(is.nan(ioa_h_to_azimuth(ioa_h , e)),285,ioa_h_to_azimuth(ioa_h , e)) # e hier max 88 sonst NaN
      
      a <- a + get_sign_a(zone)*ioa_h_to_azimuth(ioa_h , e) # hier fehler: muss - anstatt + fuer zone 3 und 4
     
       counter <- counter + 1
    }
    
    ioa_v <- ioa_v_min + (ioa_v_max - ioa_v_min) * (abs(e) / 90)
    
    e <- e + get_sign_e(zone) * ioa_v / 2
    
    j <- j + 1
  }

  return(omatidia)
}

r1 <- calculate_angles(1)
r2 <- calculate_angles(2)
r3 <- calculate_angles(3)
r4 <- calculate_angles(4)

r <- rbind(r1,r2,r3,r4)

#!TODO remove duplicates

plot(r$x,r$y,type="p")

write.table(file='ommatidia.csv', x=r, quote = FALSE, sep = ",",row.names = FALSE,col.names = FALSE)


# TEST: generate sampling directions TODO: delete
n <- 441
sqrtn <- 21
x <- rep(0,n)
y <- rep(0,n)
counter <- 0;
for(i in 0:sqrtn){
  for(j in 0:sqrtn)
  {
    R1 <- i/sqrtn;
    R2 <- j/sqrtn;
    
    if (R1 == 0 && R2 == 0) {
      x <- 0; 
      y <- 0;
      break;
    }
    
    phi <- 0; 
    radius <- r;
    a <- (2 * R1) - 1;
    b <- (2 * R2) - 1;
    
    if ((a*a) > (b*b)) { 
      radius  <- radius * a;
      phi <- (pi/4) * (b/a);
    }
    else {
      radius <- radius * b;
      phi <- (pi/2) - ((pi/4) * (a/b)); 
    }
    
    x[counter] <- cos(phi) * radius;
    y[counter] <- sin(phi) * radius;
    
    counter <- counter + 1;
  }
}

Csamp <- function(n,rad=1,centre=c(0,0)){
  x0 <- centre[1] ; y0 <- centre[2]
  u <- 2*pi*runif(n)
  r <- sqrt(runif(n))
  rad*cbind(x=r*cos(u)+x0, y=r*sin(u)+y0)
} 

plot(x,y,asp=1) 
#plot(Csamp(1000),asp=1) 

