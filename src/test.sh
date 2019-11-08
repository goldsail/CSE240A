make
for filename in ../traces/*.bz2; do
    echo "$filename"
    bunzip2 -kc "$filename" | ./predictor --gshare:16
done
make clean
