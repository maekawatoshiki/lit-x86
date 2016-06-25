# this sample code reads examples/fibo.rb
require "File"

f_id = File::open "examples/fibo.rb" "r"
contents = File::read f_id # read all lines
puts contents
File::close f_id # close absolutely

