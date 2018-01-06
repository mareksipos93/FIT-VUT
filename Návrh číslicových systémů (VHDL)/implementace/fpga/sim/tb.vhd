-- testbench.vhd: Testbench file
-- Author(s): Tomas Martinek <martinto at fit.vutbr.cz>
--
library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;
use ieee.std_logic_textio.all;
use ieee.numeric_std.all;
use std.textio.all;

-- ----------------------------------------------------------------------------
--                        Entity declaration
-- ----------------------------------------------------------------------------
entity testbench is
end entity testbench;

-- ----------------------------------------------------------------------------
--                      Architecture declaration
-- ----------------------------------------------------------------------------
architecture behavioral of testbench is

   signal smclk   : std_logic;
   signal aclk    : std_logic;
   signal fclk    : std_logic;
   signal ledf    : std_logic;
   signal spi_clk : std_logic;
   signal spi_cs  : std_logic;
   signal spi_di  : std_logic;
   signal spi_do  : std_logic;
   signal spi_fpga_cs : std_logic;
   signal kin     : std_logic_vector(3 downto 0);
   signal kout    : std_logic_vector(3 downto 0);
   signal le      : std_logic;
   signal lrs     : std_logic;
   signal lrw     : std_logic;
   signal ld      : std_logic_vector(7 downto 0);
   signal ra      : std_logic_vector(14 downto 0);
   signal rd      : std_logic_vector(7 downto 0);
   signal rdqm    : std_logic;
   signal rcs     : std_logic;
   signal rras    : std_logic;
   signal rcas    : std_logic;
   signal rwe     : std_logic;
   signal rclk    : std_logic;
   signal rcke    : std_logic;
   signal p3m     : std_logic_vector(7 downto 0);
   signal afbus   : std_logic_vector(11 downto 0);
   signal x       : std_logic_vector(45 downto 0);

   constant smclkper  : time := 135.6 ns;
   signal key     : character := ' ';

-- ----------------------------------------------------------------------------
--                      Architecture body
-- ----------------------------------------------------------------------------
begin

uut : entity work.fpga
port map(
   -- hodiny
   SMCLK   => smclk,
   ACLK    => aclk,
   FCLK    => fclk,
   -- LED
   LEDF    => ledf,
   -- SPI
   SPI_CLK => spi_clk,
   SPI_CS  => spi_cs,
   SPI_DI  => spi_di,
   SPI_DO  => spi_do,
   SPI_FPGA_CS => spi_fpga_cs,
   -- klavesnice 4x4
   KIN     => kin,
   KOUT    => kout,
   -- LCD displej
   LE      => le,
   LRS     => lrs,
   LRW     => lrw,
   LD      => ld,
   -- SDRAM
   RA      => ra,
   RD      => rd,
   RDQM    => rdqm,
   RCS     => rcs,
   RRAS    => rras,
   RCAS    => rcas,
   RWE     => rwe,
   RCLK    => rclk,
   RCKE    => rcke,
   -- P3M
   P3M     => p3m,
   -- AFBUS
   AFBUS   => afbus,
   -- PC interface
   X       => x       
);

-- -------------------------------------------------------------
-- keyboard simulation component
KB_U : entity work.keyboard 
port map (
   KEY      => key,
   KIN      => kin,
   KOUT     => kout
);

-- -------------------------------------------------------------
-- LCD simulation component
LCD_U : entity work.lcd
port map (
   LRS      => lrs,
   LRW      => lrw,
   LE       => le,
   LD       => ld
);

-- -------------------------------------------------------------
-- smclkf clock generator
smclkfgen : process
begin
   smclk <= '1';
   wait for smclkper/2;
   smclk <= '0';
   wait for smclkper/2;
end process;

-- ----------------------------------------------------------------------------
--                         Main testbench process
-- ----------------------------------------------------------------------------
tb : process

procedure keypress(k: in character) is
begin
   key <= k;
   wait for 7 us;
   key <= ' ';
   wait for 7 us;
end procedure keypress;

begin
   wait for 40 us;
   keypress('1');
   keypress('2');
   keypress('3');
   keypress('#');

   wait;
end process;

end architecture behavioral;

