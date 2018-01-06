/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package calculators;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;
import static org.junit.Assert.*;

/**
 * 
 * @author Václav Stehlík, Marek Šipoš, Kristián Škrobánek, Tomáš Štveráček
 */
public class mathTest {
    
    public mathTest() {
    }
    
    @BeforeClass
    public static void setUpClass() {
    }
    
    @AfterClass
    public static void tearDownClass() {
    }
    
    @Before
    public void setUp() {
    }
    
    @After
    public void tearDown() {
    }

    /**
     * Test of add method, of class math.
     */
    @Test
    public void testAdd() {
        System.out.println("add");
        double a = 5.0;
        double b = 10.0;
        math useMath = new math();
        double addResult = 15.0;
        double result = useMath.add(a, b);
        assertEquals(addResult, result, 0.0);
    }

    /**
     * Test of subtract method, of class math.
     */
    @Test
    public void testSubtract() {
        System.out.println("subtract");
        double a = 5.0;
        double b = 10.0;
        math useMath = new math();
        double subResult = -5.0;
        double result = useMath.subtract(a, b);
        assertEquals(subResult, result, 0.0);
    }

    /**
     * Test of multiply method, of class math.
     */
    @Test
    public void testMultiply() {
        System.out.println("multiply");
        double a = 10.5;
        double b = 4.0;
        math useMath = new math();
        double mulResult = 42.0;
        double result = useMath.multiply(a, b);
        assertEquals(mulResult, result, 0.0);
    }

    /**
     * Test of divide method, of class math.
     */
    @Test
    public void testDivide() {
        System.out.println("divide");
        double a = 25.0;
        double b = 5.0;
        math useMath = new math();
        double divResult = 5.0;
        double result = useMath.divide(a, b);
        assertEquals(divResult, result, 0.0);
    }
    
    /**
     * Test of factorial method, of class math.
     */
    @Test
    public void testFactorial() {
        System.out.println("factorial");
        double a = 5.0;
        math useMath = new math();
        double facResult = 120.0;
        double result = useMath.factorial(a);
        assertEquals(facResult, result, 0.0);
    }

    /**
     * Test of sqrt method, of class math.
     */
    @Test
    public void testSquareRoot() {
        System.out.println("sqrt");
        double a = 9.0;
        math useMath = new math();
        double sqrtResult = 3.0;
        double result = useMath.squareRoot(a);
        assertEquals(sqrtResult, result, 0.0);
    }

    /**
     * Test of power method, of class math.
     */
    @Test
    public void testPower() {
        System.out.println("power");
        double a = 2.0;
        double exp = 5.0;
        math useMath = new math();
        double powResult = 32.0;
        double result = useMath.power(a, exp);
        assertEquals(powResult, result, 0.0);
    }
    
}
