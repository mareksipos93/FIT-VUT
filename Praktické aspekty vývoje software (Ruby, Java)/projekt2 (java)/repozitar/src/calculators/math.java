
package calculators;

/**
 * Tato trida je pomocna knihovna metod pro grafickou kalkulacku
 * @author Václav Stehlík, Marek Šipoš, Kristián Škrobánek, Tomáš Štveráček
 */

public class math {

    // Error flags
    private boolean tooLarge = false;
    private boolean negativeSqrt = false;
    private boolean divideByZero = false;
    
    /**
     * Tato metoda resetuje kontrolni flagy
     */
    private void resetFlags() {
        tooLarge = false;
        negativeSqrt = false;
        divideByZero = false;
    }

    /**
     * Zavolanim teto metody zjistime, jestli cislo je prilis velke
     * @return vraci hodnotu true, kdyz je cislo moc velke, false, kdyz je to v poradku 
     */
    public boolean isTooLarge() {
        return tooLarge;
    }

    /**
     * Zavolanim teto metody zjistime, jestli pod odmocninou je zaporne cislo
     * @return vraci hodnotu true, kdyz je cislo pod odmocninou zaporne, false, kdyz je to v poradku 
     */
    public boolean isNegativeSqrt() {
        return negativeSqrt;
    }

    /**
     * Zavolanim teto metody zjistime, jestli hrozilo deleni nulou
     * @return vraci hodnotu true, kdyz delime nulou, false, kdyz je to v poradku 
     */
    public boolean isDivideByZero() {
        return divideByZero;
    }

    /**
     * Tato metoda pricte k prvnimu cislu druhe
     * @param a prvni cislo souctu
     * @param b druhe cislo souctu
     * @return vraci soucet dvou cisel
     */
    public double add(double a, double b) {

        resetFlags();
        
        double result;
        
        result = a + b;
        
        if (result == Double.POSITIVE_INFINITY || result == Double.NEGATIVE_INFINITY) {
            tooLarge = true;
        }

        return result;
    }

    /**
     * Tato metoda odecte od prvniho cisla druhe cislo
     * @param a prvni cislo rozdilu
     * @param b druhe cislo rozdilu
     * @return vraci rozdil dvou cisel
     */
    public double subtract(double a, double b) {

        resetFlags();
        
        double result;
        
        result = a - b;
        
        if (result == Double.POSITIVE_INFINITY || result == Double.NEGATIVE_INFINITY) {
            tooLarge = true;
        }

        return result;
    }

    /**
     * Tato metoda nasobi prvni cislo druhym cislem
     * @param a prvni cislo soucinu
     * @param b druhe cislo soucinu
     * @return vraci soucin dvou cisel
     */
    public double multiply(double a, double b) {

        resetFlags();
        
        double result;
  
        result = a * b;
        
        if (result == Double.POSITIVE_INFINITY || result == Double.NEGATIVE_INFINITY) {
            tooLarge = true;
        }

        return result;
    }

    /**
     * Tato metoda podeli prvni zadane cislo druhym zadanym cislem
     * @param a delenec
     * @param b delitel
     * @return vraci podil dvou cisel
     */
    public double divide(double a, double b) {

        resetFlags();
        
        double result;
        
        result = a / b;
        
        if (result == Double.POSITIVE_INFINITY || result == Double.NEGATIVE_INFINITY) {
            tooLarge = true;
        }

        if (b == 0) {
            divideByZero = true;
        }

        return result;
    }
    
    /**
     * Tato metoda udela mocninu cisla o zakladu a a exponentu exp
     * @param a zaklad mocniny
     * @param exp exponent
     * @return vraci umocnene cislo
     */
    public double power(double a, double exp) {
        
        resetFlags();
        
        // Anything to zero power is always one
        if (exp == 0) {
            return 1;
        }

        double result = 1;
        
        // Make exponent positive, must be natural
        if (exp < 0) {
            exp *= -1;
        }
        
        // Cut decimal part of exponent, must be natural
        exp = Math.floor(exp);
        
        for (int i = 0; i < exp; i++) {
            result = result * a;
            if (result == Double.POSITIVE_INFINITY || result == Double.NEGATIVE_INFINITY) {
                tooLarge = true;
                return result;
            }
        }

        return result;
    }

    /**
     * Tato metoda udela faktorial ze zakladu
     * @param a zaklad faktorialu
     * @return vraci faktorial zadaneho cisla
     */
    public double factorial(double a) {
        
        resetFlags();
        
        // Factorial zero or one is always zero or one
        if (a == 0 || a == 1) {
            return a;
        }
        
        double result = 1;
        
        // Make input positive, must be natural
        if (a < 0) {
            a *= -1;
        }
        
        // Cut decimal part of input, must be natural
        a = Math.floor(a);
        
        for (double i = a; i > 1; i--) {
            result *= i;
            
            if (result == Double.POSITIVE_INFINITY || result == Double.NEGATIVE_INFINITY) {
                tooLarge = true;
                return result;
            }
        }
        
        return result;
    }

    /**
     * Tato metoda udela druhou odmocninu ze zadaneho cisla
     * @param a zaklad druhe odmociny
     * @return vraci druhou odmocninu ze zadaneho cisla
     */
    public double squareRoot(double a) {
        
        resetFlags();
        
        double result = a / 2;
        double last;

        if (a == 0) {
            return 0;
        }
        else if (a < 0) {
            negativeSqrt = true;
            return a;
        }
        do {
            last = result;
            result = (last + (a / last)) / 2;

        } while ((last - result) != 0);

        return result;
    }
    
}
