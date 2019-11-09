make
for filename in ../traces/*.bz2; do
    echo "$filename"
    bunzip2 -kc "$filename" | ./predictor --gshare:16
    bunzip2 -kc "$filename" | ./predictor --tournament:16:10:10
    bunzip2 -kc "$filename" | ./predictor --custom
done
make clean
