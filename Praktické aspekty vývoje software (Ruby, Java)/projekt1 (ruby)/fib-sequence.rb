# indexovani clenu posloupnosti vychazi z http://en.wikipedia.org/wiki/Fibonacci_sequence
# tj. prvni clen ma index 0

class FibonacciSequence
  # konstruktor, hodnoty prvnich dvou clenu lze nastavit (implicitne 0, 1)
  def initialize( f0 = 0, f1 = 1 )
    @seq = [f0, f1]
    self.reset
  end
  
  # nastavi radu do vychoziho stavu 
  def reset
    @idx = -1
    
    return true
  end
  
  # vrati dalsi fibonacciho cislo
  def next
    @idx += 1
    
    if @seq[@idx].nil?
      f = @seq[@idx - 1] + @seq[@idx - 2]
      @seq[@idx] = f
      return f
    else
      return @seq[@idx]
    end
  end
  
  # aktualni fibonacciho cislo
  def current
    return @idx >= 0 ? @seq[@idx] : nil
  end
  
  # aktualni index (vraci nil, pokud se jeste nezacalo generovat)
  def current_idx
    return @idx >= 0 ? @idx : nil
  end
  
  # vrati fibonacciho cislo s danym indexem
  def [](n)
    return nil if n < 0
    return @seq[n] if n <= @idx
    
    while @idx < n
      self.next
    end
    
    return self.current
  end
end