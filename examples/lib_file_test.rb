# this sample code reads examples/fibo.rb
require "File"

f_id = File_open "examples/fibo.rb" "r"
contents = File_read f_id # read all lines
puts contents
File_close f_id # close absolutely

