#!/usr/bin/ruby

a_size = 1000

a = (0 ... a_size).map{ rand(1000) }

File.open("test.in", "w+") do |file|
  file.write("#{a_size}\n")
  file.write(a.join(" "))
  file.write("\n")
end

File.open("test.out", "w+") do |file|
  file.write(a.sort.join(" "))
  file.write(" \n")
end
