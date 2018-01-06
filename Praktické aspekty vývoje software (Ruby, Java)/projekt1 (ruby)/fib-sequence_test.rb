# Nazev: Fibbonaciho posloupnost - jednotkove testy
# Autor: Marek Sipos (xsipos03)
# Popis: Tyto jednoduche jednotkove testy jsou navrzeny pro kontrolu metod
#        implementace Fibbonaciho posloupnosti dle fib-sequence.rb
#        Celkem 6 testu, kazdy zkouma jinou metodu a jeden test zkouma spravne
#        provedeni pocatecni inicializace.
#

require 'test/unit'
require_relative 'fib-sequence'

class FibonacciSequenceTest < Test::Unit::TestCase
  def setup
    @fibObject = FibonacciSequence.new
  end
  
  # do testu budou zahrnuty metody s prefixem "test_"

  # TEST inicializace
  def test_init
    # Inicializovane prvni dva prvky sekvence
    assert_not_nil(@fibObject[0])
    assert_not_nil(@fibObject[1])
    # 0 a 1 jako zacinajici dva prvky sekvence
    assert_equal(@fibObject[0], 0)
    assert_equal(@fibObject[1], 1)
  end
  
  # TEST resetovani ukazatele na sekvenci (IDX)
  def test_reset
    @fibObject.reset
    # current_idx musi vracet pro -1 hodnotu nil
    assert_nil(@fibObject.current_idx)
  end
  
  # TEST posunu v sekvenci
  def test_next
    @fibObject.next
    assert_not_nil(@fibObject[0])
    assert_equal(@fibObject.current_idx, 0)
    10.times { @fibObject.next }
    assert_not_nil(@fibObject[10])
    assert_equal(@fibObject.current_idx, 10)
  end

  # TEST navratu aktualniho cisla
  def test_current
    assert_nil(@fibObject.current)
    # 0 1 1 2 3
    5.times { @fibObject.next }
    assert_equal(@fibObject.current, 3)
  end

  # TEST navratu aktualniho indexu
  def test_current_idx
    assert_nil(@fibObject.current_idx)
    5.times { @fibObject.next }
    assert_equal(@fibObject.current_idx, 4)
  end

  # TEST indexace
  def test_getbyindex
    assert_nil(@fibObject[-1])
    assert_equal(@fibObject[1], 1)
    assert_equal(@fibObject.current_idx, 1)
    5.times { @fibObject.next }
    assert_equal(@fibObject.current_idx, 6)
    # 0 1 1 2 3 5 8 13 21 34 55
    assert_equal(@fibObject[10], 55)
    assert_equal(@fibObject.current_idx, 10)
  end

end