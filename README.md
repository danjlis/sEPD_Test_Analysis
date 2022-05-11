# sEPD_Test_Analysis
The analysis code for the sEPD testing at CU Boulder:

# File Management:
The Line test and Full scan are both run on the babar1008 tower in the Nagle Laboratory. There are 18 total files that will be run over in the analysis for each sector. These files will all be placed in the directory below. 

./data/s##/

# Running the analysis:
Then go to the ./code/ directory. Change the sector number in the line_config.config and the full_config.config. After, do the following commands:

root Line_Test_Analysis.C

root Full_Test_Analysis.C

root

[0] .L Make_Health_Sheet.C

[1] Make_Health_Sheet(#) // with the argument being the sector number without the header 's'.

# Results
The results of these analyses will be placed in the ./Results/s##/ directory.
