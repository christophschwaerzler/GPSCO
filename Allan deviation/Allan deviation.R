#################################################################
#################################################################
# Allan deviation                                               #
#                                                               #
#                                                               #
# Christoph Schwaerzler, OE1CGS                                 #
# 09.12.2023                                                    #
#################################################################
#################################################################

library(readxl)
library(writexl)

tau0 = 21                                     # Inherent Tau of the measured data
minsample = 20                                # Minimum required data points for calculating sigma

# Read Excel-File 'Allan.xlsx'
setwd("/mnt/mc1cifs/RCO/Tests/SHT/Allan Deviation/")
data<-read_xlsx("Allan.xlsx", col_names = FALSE, col_types = "numeric")
data = unlist(data)

nmax = floor(log2(length(data) / minsample)) # Max power of 2 to ensure minsample
erg  = matrix(0, nrow = nmax+1, ncol = 2)    # Results are stored here
nomfreq = median(data)                       # Nominal frequency is defined as median value of data

for (n in 0:nmax){                           # Loop of Tau's
  sigma = 0
  for(offset in 0:(2^n-1)){                  # Loop of Offsets for data within one Tau
    M =    0
    ysum = 0
    for(i in seq((1+offset),(length(data)-(2^n)),(2^n))){        # Loop of individual measurements
      yi1 = (data[i+2^n] / nomfreq) - 1
      yi0 = (data[i]     / nomfreq) - 1
      M = M + 1
      ysum = ysum + (yi1-yi0)^2
#      cat("n:",n,"offset",offset,"i",i,"data:",yi1-yi0,"\n")    # Only for testing
    }
    sig2 = 0.5/(M-1)*ysum                                        # Allan variance for individual series
    sig = sqrt(sig2)                                             # Allan deviation for individual series
#    cat("sig:",sig,"\n")                                        # Only for testing
    sigma = sigma + sig
  }
  sigma = sigma / (offset + 1)                                   # Allan deviation for specific Tau
  tau = tau0 * 2^n
  erg[n+1,1] = tau
  erg[n+1,2] = sigma
}

xmax = 10^(ceiling(log10(max(erg[,1]))))
plot(erg[,1], erg[,2], xlim=c(1,xmax), log ="xy", type = "b",
     main = "Allan deviation", xlab="Tau [s]", ylab = "Sigma", cex.main=2)

res = data.frame(erg)
colnames(res) = c('Tau','Sigma')
write_xlsx(res,path="Allan_results.xlsx",col_names=T)
