for i in {0..30}
do

    sed -i -e "/SECTOR:/ s/: .*/: "$i"/" line_config.config
    sed -i -e "/SECTOR:/ s/: .*/: "$i"/" full_config.config

    root -b -q -l Full_Test_Analysis.C
    root -b -q -l Line_Test_Analysis.C
    
    root -b -q -l Make_Health_Sheet.C\("$i"\)
done

