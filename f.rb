require "File"

id = file_open "p.rb" 
puts file_read id
file_close id
