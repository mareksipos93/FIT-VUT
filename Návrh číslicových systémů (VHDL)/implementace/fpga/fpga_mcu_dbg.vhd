-- fpga_mcu_dbg.vhd: FPGA comunication with MCU for debuging purposes
-- Author(s): Lukas Kekely <ikekely at fit.vutbr.cz>
--
library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_unsigned.all;
library UNISIM;
use UNISIM.vcomponents.all;

-- ----------------------------------------------------------------------------
--                        Entity declaration
-- ----------------------------------------------------------------------------
entity fpga_mcu_dbg is
port(
   CLK         : in  std_logic;
   RESET       : in  std_logic;
   SPI_CLK     : in  std_logic;
   SPI_FPGA_CS : in  std_logic;
   SPI_DI      : out std_logic;
   SPI_DO      : in  std_logic;
   DO          : out std_logic_vector(15 downto 0);
   DO_RESET    : out std_logic;
   DO_DV       : out std_logic;
   DATA        : in  std_logic_vector(7 downto 0);
   WRITE       : in  std_logic;
   CLEAR       : in  std_logic
);
end entity;

-- ----------------------------------------------------------------------------
--                      Architecture declaration
-- ----------------------------------------------------------------------------
architecture behavioral of fpga_mcu_dbg is

   signal ispi_cs      : std_logic;
   signal ispi_di      : std_logic;
   signal ispi_di_req  : std_logic;
   signal ispi_do      : std_logic;
   signal ispi_do_vld  : std_logic;

   signal mcu_addr     : std_logic_vector(15 downto 0) := (others => '0');
   signal mcu_do       : std_logic_vector(7 downto 0);
   signal mcu_di       : std_logic_vector(7 downto 0);
   signal mcu_we       : std_logic;
   signal mcu_we_dut   : std_logic;
   signal mcu_we_ctrl  : std_logic;
   signal mcu_re       : std_logic;

   signal bram_we      : std_logic;
   signal bram_di      : std_logic_vector(7 downto 0);
   signal bram_addr    : std_logic_vector(15 downto 0) := (others => '0');
   alias bram_addr_cnt : std_logic_vector(5 downto 0) is bram_addr(5 downto 0);

begin

SPICTRL_U : entity work.spi_ctrl
port map (
   RST     => RESET,
   CLK     => CLK,
   SPI_CLK => SPI_CLK,
   SPI_CS  => SPI_FPGA_CS,
   SPI_MOSI=> SPI_DO,
   SPI_MISO=> SPI_DI,
   DI      => ispi_di,
   DI_req  => ispi_di_req,
   DO      => ispi_do,
   DO_vld  => ispi_do_vld,
   CS      => ispi_cs
);

SPIADC_U : entity work.spi_adc
generic map (
   ADDR_WIDTH => 16,
   DATA_WIDTH => 8,
   ADDR_OUT_WIDTH => 6,
   BASE_ADDR  => 16#0000#,
   DELAY      => 0
) port map (
   CLK      => CLK,
   CS       => ispi_cs,
   DO       => ispi_do,
   DO_vld   => ispi_do_vld,
   DI       => ispi_di,
   DI_req   => ispi_di_req,
   ADDR     => mcu_addr(5 downto 0),
   DATA_OUT => mcu_do,
   DATA_IN  => mcu_di,
   WRITE_EN => mcu_we,
   READ_EN  => mcu_re
);
mcu_we_ctrl <= mcu_we and mcu_addr(0);
mcu_we_dut  <= mcu_we and not mcu_addr(0);

KEY_DEC_P : process(mcu_do)
begin
   case mcu_do(3 downto 0) is
      when X"0" => DO <= "0000000000000001";
      when X"1" => DO <= "0000000000000010";
      when X"2" => DO <= "0000000000000100";
      when X"3" => DO <= "0000000000001000";
      when X"4" => DO <= "0000000000010000";
      when X"5" => DO <= "0000000000100000";
      when X"6" => DO <= "0000000001000000";
      when X"7" => DO <= "0000000010000000";
      when X"8" => DO <= "0000000100000000";
      when X"9" => DO <= "0000001000000000";
      when X"A" => DO <= "0000010000000000";
      when X"B" => DO <= "0000100000000000";
      when X"C" => DO <= "0001000000000000";
      when X"D" => DO <= "0010000000000000";
      when X"E" => DO <= "0100000000000000";
      when X"F" => DO <= "1000000000000000";
      when others => DO <= (others => '0');
   end case;
end process;
DO_DV    <= mcu_we_dut;
DO_RESET <= mcu_we_ctrl;

RAMB_U : RAMB16_S9_S9
port map (
   DOA => open,
   DOB => mcu_di,
   DOPA => open,
   DOPB => open,
   ADDRA => bram_addr(10 downto 0),
   ADDRB => mcu_addr(10 downto 0),
   CLKA => CLK,
   CLKB => CLK,
   DIA => bram_di,
   DIB => (others => '0'),
   DIPA => "0",
   DIPB => "0",
   ENA => '1',
   ENB => '1',
   SSRA => '0',
   SSRB => '0',
   WEA => bram_we,
   WEB => '0'
);
bram_di <= DATA when WRITE='1' else X"0A";
bram_we <= WRITE or CLEAR;

BRAM_ADDR_CNT_P : process(CLK)
begin
   if CLK'event and CLK='1' then
      if mcu_we_ctrl='1' then
         bram_addr_cnt <= (others => '0');
      elsif bram_we='1' then
         bram_addr_cnt <= bram_addr_cnt+1;
      end if;
   end if;
end process;

end architecture behavioral;

