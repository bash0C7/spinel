# spinel: not-cruby -- what a 32-bit board with newlib answers for each
# method docs/newlib.md lists as behaving differently.
require "socket"
require "io/console"

def check(label)
  r = yield
  puts "#{label}: #{r.inspect}"
rescue Exception => e
  puts "#{label}: #{e.class}: #{e.message}"
end

check("Integer#size") { 1.size }
check("literal 4_000_000_000") { 4_000_000_000 }
check("literal 0xdeadbeef") { 0xdeadbeef }
check("2**31 - 1 + 1") { x = 2**31 - 1; x + 1 }
check("IO::Buffer.map") { IO::Buffer.map(File.open("map.bin", "w+"), 16) }
check("Signal.trap") { Signal.trap("INT") { puts "trapped" }; :ok }
check("Fiber.new.resume") { Fiber.new { 1 }.resume }
check("Array#each.next") { [1, 2].each.next }
check("Range#each.next") { (1..3).each.next }
check("each_slice.next") { (1..6).each_slice(2).next }
check("Enumerator.new.next") { Enumerator.new { |y| y << 1 }.next }
check("Enumerator.new.take") { Enumerator.new { |y| y << 1 }.take(1) }
check("IO#raw") { STDIN.raw { 1 } }
check("IO#noecho") { STDIN.noecho { 1 } }
check("Socket.new") { Socket.new(:INET, :STREAM) }
check("Socket.getaddrinfo") { Socket.getaddrinfo("example.com", 80) }
check("Dir.entries") { Dir.entries("/") }
check("Dir.new") { Dir.new("/") }

def nest(n)
  begin
    n.zero? ? :bottom : nest(n - 1)
  rescue ArgumentError
    :rescued
  end
end
check("begin nested 8") { nest(8) }
check("begin nested 40") { nest(40) }
puts "not reached"
