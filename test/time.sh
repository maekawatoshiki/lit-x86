echo "lang: C"
time -p ./fibo > /dev/null

echo "lang: Java"
time -p java fibo > /dev/null

echo "lang: Ruby"
time -p ruby test.rb > /dev/null

echo "lang: Python"
time -p python test.py > /dev/null

echo "lang: Lua"
time -p lua test.lua > /dev/null

echo "lang: Lua(Compile)"
time -p lua luac.out > /dev/null

echo "lang: Lit"
time -p ../lit test.rb > /dev/null
