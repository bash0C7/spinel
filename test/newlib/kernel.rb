module NewlibKernel
  #: (String) -> String
  def self.ulaw_decode(src)
    n = src.bytesize
    out = "\0" * (n * 2)
    i = 0
    while i < n
      u = (~src.getbyte(i)) & 0xFF
      t = ((u & 0x0F) << 3) + 0x84
      t = t << ((u & 0x70) >> 4)
      v = ((u & 0x80) != 0 ? (0x84 - t) : (t - 0x84)) & 0xFFFF
      out.setbyte(i * 2, v & 0xFF)
      out.setbyte(i * 2 + 1, v >> 8)
      i += 1
    end
    out
  end

  #: (Integer, Integer, Integer, Array[Integer]) -> String
  def self.glyph16(w, fg, bg, rows)
    out = "\0" * (w * 32)
    j = 0
    y = 0
    while y < 16
      bit = w - 1
      while bit >= 0
        c = ((rows[y] >> bit) & 1) == 1 ? fg : bg
        out.setbyte(j, (c >> 8) & 0xFF)
        out.setbyte(j + 1, c & 0xFF)
        j += 2
        bit -= 1
      end
      y += 1
    end
    out
  end

  #: (Integer, Integer) -> Integer
  def self.spin(seed, n)
    s = seed & 0x7FFF
    i = 0
    while i < n
      s = (s * 75 + 74) & 0x7FFF
      i += 1
    end
    s
  end

  #: (Integer) -> Integer
  def self.must_pos(n)
    raise ArgumentError, "needs a positive number below #{4_000_000_000}" if n <= 0
    n
  end
end
