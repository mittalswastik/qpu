binary="./vqemod"

for i in {1..200}; do
    for num in 1 2 3 4 5; do
        csv="equalruns.csv"
        output=`$binary $num $num`
        csv_entry="$num,6,$output"
        echo "$csv_entry" >> $csv
        echo "appended $csv_entry to $csv"

#         csv="allruns_single.csv"
#         output=`$binary 1 $num`
#         csv_entry="1,$num,$output"
#         echo "$csv_entry" >> $csv
#         echo "appended $csv_entry to $csv"
    done
done


