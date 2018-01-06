# Nazev: Resitel kvadraticke rovnice
# Autor: Marek Sipos (xsipos03)
# Popis: Tento jednoduchy program resi kvadratickou rovnici zadanou absolutnimi
#        cleny a, b, c. V pripade, ze je nejaky clen nulovy, program spravne
#        zareaguje a pripadne provede vypocet linearni rovnice, nebo vrati
#        hodnotu nil v pripade, ze dany koren nebude v realnem oboru definovan.
#
#        Program splnuje soustavu testu equation_test.rb
#

class Equation 
  def self.solve_quadratic(a, b, c)
	  # ax2 + bx + c = 0
    
    # aby nebylo provedeno celociselne deleni
    a = a.to_f
    b = b.to_f
    c = c.to_f
    
    # linearni rovnice
    if a == 0
      if b == 0
        return nil
      else
        return [-c/b]
      end
    # kvadraticka rovnice
    else
      dis = b*b-4*a*c
        
      if dis == 0
        return [-b/2*a]
      elsif dis > 0
        return [(-b+Math.sqrt(dis))/(2*a), (-b-Math.sqrt(dis))/(2*a)]
      else
        return nil
      end # end inner if
    end # end outer if
  end # end method
end # end class