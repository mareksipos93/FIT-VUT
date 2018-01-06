-- fsm.vhd: Finite State Machine
-- Author(s): Marek Sipos, xsipos03
--
library ieee;
use ieee.std_logic_1164.all;
-- ----------------------------------------------------------------------------
--                        Entity declaration
-- ----------------------------------------------------------------------------
entity fsm is
port(
   CLK         : in  std_logic;
   RESET       : in  std_logic;

   -- Input signals
   KEY         : in  std_logic_vector(15 downto 0);
   CNT_OF      : in  std_logic;

   -- Output signals
   FSM_CNT_CE  : out std_logic;
   FSM_MX_MEM  : out std_logic;
   FSM_MX_LCD  : out std_logic;
   FSM_LCD_WR  : out std_logic;
   FSM_LCD_CLR : out std_logic
);
end entity fsm;

-- ----------------------------------------------------------------------------
--                      Architecture declaration
-- ----------------------------------------------------------------------------
architecture behavioral of fsm is
   -- vsechny stavy
   type t_state is (TEST1, TEST2, TEST3A, TEST3B, TEST4A, TEST4B, TEST5A, TEST5B, TEST6A, TEST6B, TEST7A, TEST7B, TEST8A, TEST8B, TEST9A, TEST9B, TEST10A, TEST10B, CONFIRM, FAIL, PRINT_FAIL, PRINT_SUCCESS, FINISH);
   signal present_state, next_state : t_state;

begin
-- -------------------------------------------------------
sync_logic : process(RESET, CLK)
begin
   if (RESET = '1') then
      present_state <= TEST1;
   elsif (CLK'event AND CLK = '1') then
      present_state <= next_state;
   end if;
end process sync_logic;

-- -------------------------------------------------------
next_state_logic : process(present_state, KEY, CNT_OF)
begin
   case (present_state) is
   -- - - - - - - - - - - - - - - - - - - - - - -
   when TEST1 =>
      next_state <= TEST1;
      -- predcasne zmackl krizek
      if (KEY(15) = '1') then
         next_state <= PRINT_FAIL;
      -- zmackl spravne cislo
      elsif (KEY(4) = '1') then
         next_state <= TEST2;
      -- zmackl neco jineho, vime ze to je spatne
      elsif (KEY(15 downto 0) /= "0000000000000000") then
         next_state <= FAIL;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when TEST2 =>
      next_state <= TEST2;
      -- predcasne zmackl krizek
      if (KEY(15) = '1') then
         next_state <= PRINT_FAIL;
      -- zmackl spravne cislo (1. vetev)
      elsif (KEY(0) = '1') then
         next_state <= TEST3A;
      -- zmackl spravne cislo (2. vetev)
      elsif (KEY(8) = '1') then
         next_state <= TEST3B;
      -- zmackl neco jineho, vime ze to je spatne
      elsif (KEY(15 downto 0) /= "0000000000000000") then
         next_state <= FAIL;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when TEST3A =>
      next_state <= TEST3A;
      -- predcasne zmackl krizek
      if (KEY(15) = '1') then
         next_state <= PRINT_FAIL;
      -- zmackl spravne cislo
      elsif (KEY(8) = '1') then
         next_state <= TEST4A;
      -- zmackl neco jineho, vime ze to je spatne
      elsif (KEY(15 downto 0) /= "0000000000000000") then
         next_state <= FAIL;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when TEST3B =>
      next_state <= TEST3B;
      -- predcasne zmackl krizek
      if (KEY(15) = '1') then
         next_state <= PRINT_FAIL;
      -- zmackl spravne cislo
      elsif (KEY(7) = '1') then
         next_state <= TEST4B;
      -- zmackl neco jineho, vime ze to je spatne
      elsif (KEY(15 downto 0) /= "0000000000000000") then
         next_state <= FAIL;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when TEST4A =>
      next_state <= TEST4A;
      -- predcasne zmackl krizek
      if (KEY(15) = '1') then
         next_state <= PRINT_FAIL;
      -- zmackl spravne cislo
      elsif (KEY(9) = '1') then
         next_state <= TEST5A;
      -- zmackl neco jineho, vime ze to je spatne
      elsif (KEY(15 downto 0) /= "0000000000000000") then
         next_state <= FAIL;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when TEST4B =>
      next_state <= TEST4B;
      -- predcasne zmackl krizek
      if (KEY(15) = '1') then
         next_state <= PRINT_FAIL;
      -- zmackl spravne cislo
      elsif (KEY(4) = '1') then
         next_state <= TEST5B;
      -- zmackl neco jineho, vime ze to je spatne
      elsif (KEY(15 downto 0) /= "0000000000000000") then
         next_state <= FAIL;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when TEST5A =>
      next_state <= TEST5A;
      -- predcasne zmackl krizek
      if (KEY(15) = '1') then
         next_state <= PRINT_FAIL;
      -- zmackl spravne cislo
      elsif (KEY(1) = '1') then
         next_state <= TEST6A;
      -- zmackl neco jineho, vime ze to je spatne
      elsif (KEY(15 downto 0) /= "0000000000000000") then
         next_state <= FAIL;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when TEST5B =>
      next_state <= TEST5B;
      -- predcasne zmackl krizek
      if (KEY(15) = '1') then
         next_state <= PRINT_FAIL;
      -- zmackl spravne cislo
      elsif (KEY(6) = '1') then
         next_state <= TEST6B;
      -- zmackl neco jineho, vime ze to je spatne
      elsif (KEY(15 downto 0) /= "0000000000000000") then
         next_state <= FAIL;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when TEST6A =>
      next_state <= TEST6A;
      -- predcasne zmackl krizek
      if (KEY(15) = '1') then
         next_state <= PRINT_FAIL;
      -- zmackl spravne cislo
      elsif (KEY(2) = '1') then
         next_state <= TEST7A;
      -- zmackl neco jineho, vime ze to je spatne
      elsif (KEY(15 downto 0) /= "0000000000000000") then
         next_state <= FAIL;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when TEST6B =>
      next_state <= TEST6B;
      -- predcasne zmackl krizek
      if (KEY(15) = '1') then
         next_state <= PRINT_FAIL;
      -- zmackl spravne cislo
      elsif (KEY(2) = '1') then
         next_state <= TEST7B;
      -- zmackl neco jineho, vime ze to je spatne
      elsif (KEY(15 downto 0) /= "0000000000000000") then
         next_state <= FAIL;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when TEST7A =>
      next_state <= TEST7A;
      -- predcasne zmackl krizek
      if (KEY(15) = '1') then
         next_state <= PRINT_FAIL;
      -- zmackl spravne cislo
      elsif (KEY(3) = '1') then
         next_state <= TEST8A;
      -- zmackl neco jineho, vime ze to je spatne
      elsif (KEY(15 downto 0) /= "0000000000000000") then
         next_state <= FAIL;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when TEST7B =>
      next_state <= TEST7B;
      -- predcasne zmackl krizek
      if (KEY(15) = '1') then
         next_state <= PRINT_FAIL;
      -- zmackl spravne cislo
      elsif (KEY(0) = '1') then
         next_state <= TEST8B;
      -- zmackl neco jineho, vime ze to je spatne
      elsif (KEY(15 downto 0) /= "0000000000000000") then
         next_state <= FAIL;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when TEST8A =>
      next_state <= TEST8A;
      -- predcasne zmackl krizek
      if (KEY(15) = '1') then
         next_state <= PRINT_FAIL;
      -- zmackl spravne cislo
      elsif (KEY(2) = '1') then
         next_state <= TEST9A;
      -- zmackl neco jineho, vime ze to je spatne
      elsif (KEY(15 downto 0) /= "0000000000000000") then
         next_state <= FAIL;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when TEST8B =>
      next_state <= TEST8B;
      -- predcasne zmackl krizek
      if (KEY(15) = '1') then
         next_state <= PRINT_FAIL;
      -- zmackl spravne cislo
      elsif (KEY(9) = '1') then
         next_state <= TEST9B;
      -- zmackl neco jineho, vime ze to je spatne
      elsif (KEY(15 downto 0) /= "0000000000000000") then
         next_state <= FAIL;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when TEST9A =>
      next_state <= TEST9A;
      -- predcasne zmackl krizek
      if (KEY(15) = '1') then
         next_state <= PRINT_FAIL;
      -- zmackl spravne cislo
      elsif (KEY(6) = '1') then
         next_state <= TEST10A;
      -- zmackl neco jineho, vime ze to je spatne
      elsif (KEY(15 downto 0) /= "0000000000000000") then
         next_state <= FAIL;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when TEST9B =>
      next_state <= TEST9B;
      -- predcasne zmackl krizek
      if (KEY(15) = '1') then
         next_state <= PRINT_FAIL;
      -- zmackl spravne cislo
      elsif (KEY(4) = '1') then
         next_state <= TEST10B;
      -- zmackl neco jineho, vime ze to je spatne
      elsif (KEY(15 downto 0) /= "0000000000000000") then
         next_state <= FAIL;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when TEST10A =>
      next_state <= TEST10A;
      -- predcasne zmackl krizek
      if (KEY(15) = '1') then
         next_state <= PRINT_FAIL;
      -- zmackl spravne cislo, ted musi potvrdit krizkem
      elsif (KEY(1) = '1') then
         next_state <= CONFIRM;
      -- zmackl neco jineho, vime ze to je spatne
      elsif (KEY(15 downto 0) /= "0000000000000000") then
         next_state <= FAIL;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when TEST10B =>
      next_state <= TEST10B;
      -- predcasne zmackl krizek
      if (KEY(15) = '1') then
         next_state <= PRINT_FAIL;
      -- zmackl spravne cislo, ted musi potvrdit krizkem
      elsif (KEY(4) = '1') then
         next_state <= CONFIRM;
      -- zmackl neco jineho, vime ze to je spatne
      elsif (KEY(15 downto 0) /= "0000000000000000") then
         next_state <= FAIL;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when CONFIRM =>
      next_state <= CONFIRM;
      -- potvrdil krizkem, vse ok
      if (KEY(15) = '1') then
         next_state <= PRINT_SUCCESS;
      -- zmackl neco jineho, zadal moc znaku, to je spatne
      elsif (KEY(15 downto 0) /= "0000000000000000") then
         next_state <= FAIL;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when FAIL =>
      next_state <= FAIL;
      -- potvrdil krizkem, vypiseme chybu
      if (KEY(15) = '1') then
         next_state <= PRINT_FAIL;
      -- jinak plati next_state <= FAIL, cekame na krizek
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when PRINT_FAIL =>
      next_state <= PRINT_FAIL;
      -- potvrdil krizkem, restart
      if (KEY(15) = '1') then
         next_state <= FINISH;
      -- jinak plati next_state <= PRINT_FAIL, cekame na krizek
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when PRINT_SUCCESS =>
      next_state <= PRINT_SUCCESS;
      -- potvrdil krizkem, restart
      if (KEY(15) = '1') then
         next_state <= FINISH;
      -- jinak plati next_state <= PRINT_SUCCESS, cekame na krizek
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when others =>
      next_state <= TEST1;
   end case;
end process next_state_logic;

-- -------------------------------------------------------
output_logic : process(present_state, KEY)
begin
   -- clock enable
   FSM_CNT_CE     <= '0';
   -- vyber pameti
   FSM_MX_MEM     <= '0';
   -- vyber vstupu
   FSM_MX_LCD     <= '0';
   -- zapis znaku na display
   FSM_LCD_WR     <= '0';
   -- reset displaye
   FSM_LCD_CLR    <= '0';

   case (present_state) is
   -- - - - - - - - - - - - - - - - - - - - - - -
   when PRINT_FAIL =>
      -- pristup odepren
      FSM_MX_MEM     <= '0';
      FSM_CNT_CE     <= '1';
      -- chceme zapsat z ROM
      FSM_MX_LCD     <= '1';
      FSM_LCD_WR     <= '1';
   -- - - - - - - - - - - - - - - - - - - - - - -
   when PRINT_SUCCESS =>
      -- pristup povolen
      FSM_MX_MEM     <= '1';
      FSM_CNT_CE     <= '1';
      -- chceme zapsat z ROM
      FSM_MX_LCD     <= '1';
      FSM_LCD_WR     <= '1';
   -- pruchodem skrz FINISH se resetuje display
   when FINISH =>
      FSM_LCD_CLR    <= '1';
   -- - - - - - - - - - - - - - - - - - - - - - -
   when others =>
      -- jakykoliv vstup krome krizku se vykresluje
      if (KEY(14 downto 0) /= "000000000000000") then
         FSM_LCD_WR     <= '1';
      -- krizek resetuje display
      elsif (KEY(15) = '1') then
         FSM_LCD_CLR    <= '1';
      end if;
   end case;
end process output_logic;

end architecture behavioral;
