# Nazev: Fibbonaciho posloupnost - rekurzivni
# Autor: Marek Sipos (xsipos03)
# Popis: Tento jednoduchy program kopiruje chovani puvodni implementace
#        Fibbonaciho posloupnosti s drobnou upravou - pri pristupu k prvku
#        posloupnosti je prvek vzdy znovu vypocitan nezavisle na tom, zda jiz
#        vypocitan byl. Navic je vypocet proveden velmi neefektivni rekurzivni
#        metodou, jejiz delka trvani roste s kazdym clenem geometrickou radou.
#
#        Program splnuje jednotkove testy fib-sequence_test.rb, jelikoz nijak
#        nezasahuje do puvodniho prostredi programu. Je pouze naimplementovana
#        nova neefektivni metoda, skrz kterou jsou vypocty smerovany.
#

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
      f = recursive(@idx)
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
    
    return recursive(@idx)
  end
  
  # rekurzivni vypocet Fibbonaciho poosloupnosti
  def recursive(n)
    return n if n <= 1
    
    return recursive(n-1)+recursive(n-2)
  end
end