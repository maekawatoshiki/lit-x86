echo "lang: C"
time ./fibo > /dev/null

echo "lang: Java"
time java fibo > /dev/null

echo "lang: Ruby"
time ruby test.rb > /dev/null

echo "lang: Lit"
time ../lit test.rb > /dev/null
