-- top_level.vhd: Application - Access Terminal
-- Copyright (C) 2006 Brno University of Technology,
--                    Faculty of Information Technology
-- Author(s): Tomas Martinek martinto at fit.vutbr.cz
--
-- LICENSE TERMS
--
-- Redistribution and use in source and binary forms, with or without
-- modification, are permitted provided that the following conditions
-- are met:
-- 1. Redistributions of source code must retain the above copyright
--    notice, this list of conditions and the following disclaimer.
-- 2. Redistributions in binary form must reproduce the above copyright
--    notice, this list of conditions and the following disclaimer in
--    the documentation and/or other materials provided with the
--    distribution.
-- 3. All advertising materials mentioning features or use of this software
--    or firmware must display the following acknowledgement:
--
--      This product includes software developed by the University of
--      Technology, Faculty of Information Technology, Brno and its
--      contributors.
--
-- 4. Neither the name of the Company nor the names of its contributors
--    may be used to endorse or promote products derived from this
--    software without specific prior written permission.
--
-- This software or firmware is provided ``as is'', and any express or implied
-- warranties, including, but not limited to, the implied warranties of
-- merchantability and fitness for a particular purpose are disclaimed.
-- In no event shall the company or contributors be liable for any
-- direct, indirect, incidental, special, exemplary, or consequential
-- damages (including, but not limited to, procurement of substitute
-- goods or services; loss of use, data, or profits; or business
-- interruption) however caused and on any theory of liability, whether
-- in contract, strict liability, or tort (including negligence or
-- otherwise) arising in any way out of the use of this software, even
-- if advised of the possibility of such damage.
--
-- $Id$
--
--
library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;

architecture main of fpga is

   signal cnt_reset        : std_logic_vector(3 downto 0) := (others => '0');
   signal cnt_reset_ce     : std_logic;

   signal cnt_mem          : std_logic_vector(3 downto 0);
   signal cnt_mem_ce       : std_logic;
   signal cnt_mem_ld       : std_logic;
   signal cnt_mem_of       : std_logic;
   signal kb_data_out      : std_logic_vector(15 downto 0);
   
   signal mem_ok_do        : std_logic_vector(7 downto 0);
   signal mem_bad_do       : std_logic_vector(7 downto 0);
   signal mx_mem_do        : std_logic_vector(7 downto 0);
   signal mx_lcd_data      : std_logic_vector(7 downto 0);

   signal fsm_key          : std_logic_vector(15 downto 0);
   signal fsm_reset        : std_logic;
   signal fsm_cnt_ce       : std_logic;
   signal fsm_mx_mem       : std_logic;
   signal fsm_mx_lcd       : std_logic;
   signal fsm_lcd_wr       : std_logic;
   signal fsm_lcd_clr      : std_logic;

   signal mcu_data_out     : std_logic_vector(15 downto 0);
   signal mcu_data_vld     : std_logic;
   signal dbg_reset        : std_logic;

   -- nastaveni alias nazvu pro signaly CLK a RESET
   alias CLK               : std_logic is SMCLK;
   alias RESET             : std_logic is cnt_reset_ce;

begin
-- Nepouzite signaly ---------------------------------------------------
   X        <= (others => 'Z');

   RA       <= (others => 'Z');
   RDQM     <= 'Z';
   RCS      <= 'Z';
   RRAS     <= 'Z';
   RCAS     <= 'Z';
   RWE      <= 'Z';
   RCLK     <= 'Z';
   RCKE     <= 'Z';

   LEDF     <= 'Z';

-- Generator signalu RESET -------------------------------------------
cnt_reset_ce <= '1' when (cnt_reset < "0100") else '0';
process(CLK)
begin
   if (CLK'event AND CLK = '1') then
      if (cnt_reset_ce = '1') then
         cnt_reset <= cnt_reset + 1;
      end if;
   end if;
end process;

-- Citac adresy pro LCD display ---------------------------------------
cnt_mem_ce <= fsm_cnt_ce or fsm_lcd_clr;
cnt_mem_ld <= fsm_lcd_clr;
process(RESET, CLK)
begin
   if (RESET = '1') then
      cnt_mem <= (others => '0');
   elsif (CLK'event AND CLK = '1') then
      if (cnt_mem_ce = '1') then
         if (cnt_mem_ld = '1') then
            cnt_mem <= "0000";
         else
            cnt_mem <= cnt_mem + 1;
         end if;
      end if;
   end if;
end process;

cnt_mem_of <= '1' when (cnt_mem = "1111") else '0';

-- Instance kontroleru klavesnice -------------------------------------
kbc_u : entity work.keyboard_controller_high
-- pragma translate off
generic map(
   -- Pouze pro ucely simulaci se zkrati interval mezi ctenim stavu 
   -- klavesnice. Pri synteze se pouzije defaultni hodnota nastavena 
   -- v radici.
   READ_INTERVAL => 32
)
-- pragma translate on
port map(
   -- Hodiny, Reset
   CLK        => CLK,
   RST        => RESET,

   -- Stisknute klavesy
   DATA_OUT   => kb_data_out,
      
   -- Signaly klavesnice 
   KB_KIN     => KIN,
   KB_KOUT    => KOUT
);

-- Instance kontroleru LCD -------------------------------------------
lcdc_u : entity work.lcd_ctrl_high
port map(
   CLK         => CLK,
   RESET       => RESET,

   -- user interface
   DATA        => mx_lcd_data,
   WRITE       => fsm_lcd_wr,
   CLEAR       => fsm_lcd_clr,

   -- lcd interface
   LRS         => LRS,
   LRW         => LRW,
   LE          => LE,
   LD          => LD
);

-- Instance pameti ROM ------------------------------------------------
ROM_OK : entity work.rom_memory
generic map(
   -- text "Pristup povolen"
   INIT        => X"5072697374757020706F766F6C656E20"
)
port map(
   ADDR        => cnt_mem,
   DATA_OUT    => mem_ok_do 
);

-- Instance pameti ROM ------------------------------------------------
ROM_BAD : entity work.rom_memory
generic map(
   -- text "Pristup odepren"
   INIT        => X"50726973747570206F64657072656E20"
)
port map(
   ADDR        => cnt_mem,
   DATA_OUT    => mem_bad_do 
);

-- Instance ridiciho automatu -----------------------------------------
FSM_U : entity work.fsm
port map(
   CLK         => CLK,
   RESET       => fsm_reset,

   -- Input signals
   KEY         => fsm_key,
   CNT_OF      => cnt_mem_of,

   -- Output signals
   FSM_CNT_CE  => fsm_cnt_ce,
   FSM_MX_MEM  => fsm_mx_mem,
   FSM_MX_LCD  => fsm_mx_lcd,
   FSM_LCD_WR  => fsm_lcd_wr,
   FSM_LCD_CLR => fsm_lcd_clr
);

-- multiplexor vystupu z pameti
mx_mem_do <= mem_ok_do when(fsm_mx_mem='1') else 
             mem_bad_do;

-- multiplexor dat pro lcd display
mx_lcd_data <= mx_mem_do when(fsm_mx_lcd='1') else 
               X"2a"; -- znak "*"

-- multiplexor dat pro FSM
fsm_key   <= kb_data_out when mcu_data_vld='0' else mcu_data_out;
fsm_reset <= RESET or dbg_reset;


-- DEBUG komunikace s MCU ---------------------------------------------
MCUDBG_U : entity work.fpga_mcu_dbg
port map (
   RESET       => RESET,
   CLK         => CLK,
   SPI_CLK     => SPI_CLK,
   SPI_FPGA_CS => SPI_FPGA_CS,
   SPI_DO      => SPI_DO,
   SPI_DI      => SPI_DI,
   DO          => mcu_data_out,
   DO_RESET    => dbg_reset,
   DO_DV       => mcu_data_vld,
   DATA        => mx_lcd_data,
   WRITE       => fsm_lcd_wr,
   CLEAR       => fsm_lcd_clr
);

end main;

