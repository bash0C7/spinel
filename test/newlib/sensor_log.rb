module SensorLog
  Stats = Struct.new(:count, :min, :max, :sum)

  class Malformed < StandardError
  end

  NAMES = ["temp", "hum", "press"]

  def self.synth(seed, n)
    s = seed & 0xFFFF
    out = +""
    i = 0
    while i < n
      s = (s * 75 + 74) % 65537
      name = NAMES[s % 3]
      if s % 17 == 0
        out << "#{name},oops\n"
      elsif s % 23 == 0
        out << "#{name}\n"
      else
        whole = s % 400 - 100
        out << "#{name},#{whole / 10}.#{(s >> 3) % 10}\n"
      end
      i += 1
    end
    out
  end

  def self.parse_value(text)
    raise Malformed, "missing value" if text.empty?
    raise Malformed, "bad number #{text.inspect}" unless text.match?(/\A-?\d+\.\d+\z/)
    text.to_f
  end

  def self.report(log, window)
    stats = {}
    recent = []
    errors = []
    lineno = 0
    log.each_line do |line|
      lineno += 1
      name, field = line.chomp.split(",", 2)
      begin
        v = parse_value(field || "")
        st = stats[name] ||= Stats.new(0, v, v, 0.0)
        st.count += 1
        st.min = v if v < st.min
        st.max = v if v > st.max
        st.sum += v
        recent << v
        recent.shift if recent.size > window
      rescue Malformed => e
        errors << "line #{lineno}: #{name}: #{e.message}"
      end
    end
    out = +""
    stats.keys.sort.each do |k|
      st = stats[k]
      out << format("%-5s n=%3d min=%7.2f max=%7.2f avg=%7.2f\n", k, st.count, st.min, st.max, st.sum / st.count)
    end
    out << "moving avg (last #{recent.size}): #{(recent.sum / recent.size).round(3)}\n"
    out << "errors: #{errors.size}\n"
    errors.first(3).each { |e| out << "  #{e}\n" }
    out
  end

  def self.crc16(data)
    crc = 0xFFFF
    data.each_byte do |b|
      crc ^= b << 8
      8.times do
        crc = (crc & 0x8000) != 0 ? ((crc << 1) ^ 0x1021) & 0xFFFF : (crc << 1) & 0xFFFF
      end
    end
    crc
  end
end

24.times do |r|
  log = SensorLog.synth(1000 + r * 37, 150)
  rep = SensorLog.report(log, 8)
  printf("round %d log crc=%04x report crc=%04x\n", r, SensorLog.crc16(log), SensorLog.crc16(rep))
  print rep if r < 2 || r == 23
end
["23.5", "12.x", ""].each do |t|
  begin
    printf("parse_value %s -> %.3f\n", t.inspect, SensorLog.parse_value(t))
  rescue => e
    printf("parse_value %s raised %s: %s\n", t.inspect, e.class, e.message)
  end
end
