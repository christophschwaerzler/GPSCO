#################################################################
#################################################################
# Datachecks                                                    #
#                                                               #
#                                                               #
# Christoph Schwaerzler, OE1CGS                                 #
# 06.12.2023                                                    #
#################################################################
#################################################################

library(readxl)
library(writexl)

replace = TRUE                             # Replace outlier by random value (TRUE) or zero (FALSE)

delta = 5                                  # Maximum allowed difference from median for inliers                                             # Data outside are classified as outliers

# Read Excel-File 'Data.xlsx'
setwd("/mnt/mc1cifs/RCO/Tests/SHT/Allan Deviation/")
data<-read_xlsx("Data.xlsx", col_names = FALSE, col_types = "numeric")
data$...2 = data$...1
colnames(data) = c('In','Out')

clean  = FALSE                              # Flag indicating that data has to be replaced (FALSE)
                                            # or is totally clean (TRUE)


leng_data = nrow(data)
medi_data = median(data$Out)

min_data = medi_data - delta
max_data = medi_data + delta

changes    = 0

while (clean == FALSE){
  clean = TRUE

  for(i in 1:leng_data){
    if(((data$Out[i]<min_data)|(data$Out[i]>max_data))&(data$Out[i] != 0)){
      clean = FALSE
      changes = changes + 1
      data$Out[i] = 0
      if(replace == TRUE){
        j = sample(1:leng_data, 1)
        data$Out[i] = data$Out[j]
      }
    }
  }
}

cat("Number of adjusted data:     ", changes)
cat("Percentage of adjusted data: ", changes/leng_data*100, " %")

write_xlsx(data,path="Data_checked.xlsx",col_names=F)
