require 'test/unit'
require_relative 'equation'

class EquationTest < Test::Unit::TestCase
  # diskriminant je 0 (jedno reseni v realnych cislech)
  def test_DiscriminantEq0
    assert_equal( [-1], Equation.solve_quadratic(1, 2, 1) )
  end
  
  # diskriminant vetsi nez 0 (dve reseni v real. cislech)
  def test_DiscriminantGeq0
    assert_equal( [-1, -1.5], Equation.solve_quadratic(2, 5, 3) )
  end
  
  # diskriminant mensi nez 0 (komplexni cisla nepocitame)
  def test_DiscriminantLeq0
    assert_equal( nil, Equation.solve_quadratic(1, 1, 1) )
  end
  
  # pokud a je 0
  def test_Aeq0
    assert_equal( [-5/2.to_f], Equation.solve_quadratic(0, 2, 5) )
  end
  
  # pokud a i b je 0
  def test_Aeq0Beq0
    assert_equal( nil, Equation.solve_quadratic(0, 0, 5) )
  end
end