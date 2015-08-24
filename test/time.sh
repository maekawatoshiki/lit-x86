echo "lang: C"
time ./fibo > /dev/null

echo "lang: Java"
time java fibo > /dev/null

echo "lang: Ruby"
time ruby test.rb > /dev/null

echo "lang: Python"
time python test.py > /dev/null

echo "lang: Lua"
time lua test.lua > /dev/null

echo "lang: Lua(Compile)"
time lua luac.out > /dev/null

echo "lang: Lit"
time ../lit test.rb > /dev/null
