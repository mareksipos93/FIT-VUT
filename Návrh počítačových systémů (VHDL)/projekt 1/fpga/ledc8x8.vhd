-- INP PROJEKT 1
-- Autor: Marek Sipos (xsipos03)
-- 23. rijna 2016

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.std_logic_arith.all;
use IEEE.std_logic_unsigned.all;

entity ledc8x8 is
    port (
        SMCLK: in std_logic; -- hodinovy signal
        RESET: in std_logic; -- reset pro asynchronni inicializaci
        ROW: out std_logic_vector (7 downto 0); -- radek displaye (1 = aktivni)
        LED: out std_logic_vector (0 to 7) -- jednotlive LEDky na radku (0 = sviti)
    );
end ledc8x8;

architecture main of ledc8x8 is
    signal letter: std_logic; -- ktera iniciala se prave zobrazuje (0 = krestni, 1 = prijmeni)
    signal row_cnt: std_logic_vector(7 downto 0); -- citac pro deleni frekvence na prepinani radku (SMCLK/2^8 Hz)
    signal letter_cnt: std_logic_vector(21 downto 0); -- citac pro deleni frekvence na prepinani znaku (SMCLK/2^22 = 1.76 Hz)
    signal row_temp: std_logic_vector (7 downto 0); -- container pro ROW hodnoty, aby se mohly cist
begin

-- inkrementace citacu pro deleni frekvenci
cnt_add: process(RESET, SMCLK)
begin
    if (SMCLK'event) and (SMCLK='1') then
        if (RESET='1') then
            row_cnt <= "00000000";
            letter_cnt <= "0000000000000000000000";
        else
            row_cnt <= row_cnt + 1;
            letter_cnt <= letter_cnt + 1;
        end if;
    end if;
end process cnt_add;

-- iterace mezi radky (klam lidskeho oka)
row_set: process(RESET, SMCLK)
begin
    if (SMCLK'event) and (SMCLK='1') then
        if (RESET='1') then
            row_temp <= "00000001";
        else
            if (row_cnt="11111111") then
                case row_temp is
                    when "00000001" => row_temp <= "00000010";
                    when "00000010" => row_temp <= "00000100";
                    when "00000100" => row_temp <= "00001000";
                    when "00001000" => row_temp <= "00010000";
                    when "00010000" => row_temp <= "00100000";
                    when "00100000" => row_temp <= "01000000";
                    when "01000000" => row_temp <= "10000000";
                    when "10000000" => row_temp <= "00000001";
                    when others => null;
                end case;
            end if;
        end if;
        ROW <= row_temp;
    end if;
end process row_set;

-- prepinani mezi pismeny
letter_switch: process(RESET, SMCLK)
begin
    if (SMCLK'event) and (SMCLK='1') then
        if (RESET='1') then
            letter <= '0';
        else
            if (letter_cnt="1111111111111111111111") then
                case letter is
                    when '0' => letter <= '1';
                    when '1' => letter <= '0';
                    when others => null;
                end case;
            end if;
        end if;
    end if;
end process letter_switch;

-- nastavovani LEDek
led_set: process(RESET, SMCLK)
begin
    if (SMCLK'event) and (SMCLK='1') then
        if (RESET='1') then
            LED <= "00000000";
        else
            if (letter='0') then
                case row_temp is
                    when "00000001" => LED <= "11111111";
                    when "00000010" => LED <= "10111101";
                    when "00000100" => LED <= "10011001";
                    when "00001000" => LED <= "10100101";
                    when "00010000" => LED <= "10111101";
                    when "00100000" => LED <= "10111101";
                    when "01000000" => LED <= "10111101";
                    when "10000000" => LED <= "11111111";
                    when others => null;
                end case;
            else
                case row_temp is
                    when "00000001" => LED <= "11111111";
                    when "00000010" => LED <= "11000011";
                    when "00000100" => LED <= "10111101";
                    when "00001000" => LED <= "11001111";
                    when "00010000" => LED <= "11110011";
                    when "00100000" => LED <= "10111101";
                    when "01000000" => LED <= "11000011";
                    when "10000000" => LED <= "11111111";
                    when others => null;
                end case;
            end if;
        end if;
    end if;
end process;

end main;
