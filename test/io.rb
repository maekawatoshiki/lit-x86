s:string = Array(1024)

f = File.open("test/io_test", "r")
printf "%s\n",  File.read(s, 1024, f)
File.close(f)

output "Hello"
puts " world!"

