-- cpu.vhd: Simple 8-bit CPU (BrainLove interpreter)
-- Copyright (C) 2016 Brno University of Technology,
--                    Faculty of Information Technology
-- Author(s): Marek Sipos (xsipos03)
--            2 BIT
--            INP Project 2
--

library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;

-- ----------------------------------------------------------------------------
--                        Entity declaration
-- ----------------------------------------------------------------------------
entity cpu is
 port (
   CLK   : in std_logic;  -- hodinovy signal
   RESET : in std_logic;  -- asynchronni reset procesoru
   EN    : in std_logic;  -- povoleni cinnosti procesoru
 
   -- synchronni pamet ROM
   CODE_ADDR : out std_logic_vector(11 downto 0); -- adresa do pameti
   CODE_DATA : in std_logic_vector(7 downto 0);   -- CODE_DATA <- rom[CODE_ADDR] pokud CODE_EN='1'
   CODE_EN   : out std_logic;                     -- povoleni cinnosti
   
   -- synchronni pamet RAM
   DATA_ADDR  : out std_logic_vector(9 downto 0); -- adresa do pameti
   DATA_WDATA : out std_logic_vector(7 downto 0); -- mem[DATA_ADDR] <- DATA_WDATA pokud DATA_EN='1'
   DATA_RDATA : in std_logic_vector(7 downto 0);  -- DATA_RDATA <- ram[DATA_ADDR] pokud DATA_EN='1'
   DATA_RDWR  : out std_logic;                    -- cteni (1) / zapis (0)
   DATA_EN    : out std_logic;                    -- povoleni cinnosti
   
   -- vstupni port
   IN_DATA   : in std_logic_vector(7 downto 0);   -- IN_DATA <- stav klavesnice pokud IN_VLD='1' a IN_REQ='1'
   IN_VLD    : in std_logic;                      -- data platna
   IN_REQ    : out std_logic;                     -- pozadavek na vstup data
   
   -- vystupni port
   OUT_DATA : out  std_logic_vector(7 downto 0);  -- zapisovana data
   OUT_BUSY : in std_logic;                       -- LCD je zaneprazdnen (1), nelze zapisovat
   OUT_WE   : out std_logic                       -- LCD <- OUT_DATA pokud OUT_WE='1' a OUT_BUSY='0'
 );
end cpu;


-- ----------------------------------------------------------------------------
--                      Architecture declaration
-- ----------------------------------------------------------------------------
architecture behavioral of cpu is

    type states is
    (
        S_BEGIN,
        S_DECODE,
        S_ACTION,
        S_NEXT,
        S_END
    );

    type instructions is 
    (
        I_RMOVE,     -- >   ptr--
        I_LMOVE,     -- <   ptr++
        I_INCR,      -- +   *ptr++
        I_DECR,      -- -   *ptr--
        I_WHILE1,    -- [   while start
        I_WHILE2,    -- ]   while end
        I_PRINT,     -- .   putchar(*ptr)
        I_READ,      -- ,   *ptr = getchar()
        I_TMPTO,     -- $   tmp = *ptr
        I_TMPFROM,   -- !   *ptr = tmp
        I_END,       -- (return)
        I_UNKNOWN    -- ?   // comment
    );

    signal state        :  states;                      -- actual state (most important)
    signal substate     :  std_logic_vector (2 downto 0);   -- substate (to divide state to smaller states)
    signal instruction  :  instructions;                -- current instruction (action is taken in S_ACTION state)

    -- [     REGISTERS     ] --

    signal pc      :     std_logic_vector (11 downto 0);    -- instruction pointer (ROM) 4096 x 8 b
    signal ptr     :     std_logic_vector (9 downto 0);     -- memory pointer (RAM) 1024 x 8 b
    signal tmp     :     std_logic_vector (7 downto 0);     -- temporary (8b)
    
    signal pc_add  :     std_logic;                     -- 1 = add, 0 = subtract
    signal ptr_add :     std_logic;                     -- 1 = add, 0 = subtract

    signal pc_en   :     std_logic;                     -- enable R/W
    signal ptr_en  :     std_logic;                     -- enable R/W


begin

    -- ================================== --

    -- [     PC_REG [ROM]    ] --
    pc_reg: process(CLK, RESET, pc_en, pc_add)
    begin
        if (RESET = '1') then
            pc <= (others => '0');
        else
            if (pc_en = '1' and CLK'event and CLK = '1') then
                if (pc_add = '1') then
                    pc <= pc + 1;
                else
                    pc <= pc - 1;
                end if;
            end if;
        end if;
    end process;

    CODE_ADDR <= pc; -- entity interaction OUT [ROM] - SYNC !!!!!

    -- ================================== --

    -- [     PTR_REG [RAM]     ] --
    ptr_reg: process(CLK, RESET, ptr_en, ptr_add)
    begin
        if (RESET = '1') then
            ptr <= (others => '0');
        else
            if (ptr_en = '1' and CLK'event and CLK = '1') then
                if (ptr_add = '1') then
                    ptr <= ptr + 1;
                else
                    ptr <= ptr - 1;
                end if;
            end if;
        end if;
    end process;

    DATA_ADDR <= ptr; -- entity interaction OUT [RAM] - SYNC !!!!!

    -- ================================== --

    -- TO READ INSTRUCTION
    -- 1. use pc_add + pc_en to increment CODE_ADDR
    -- 2. enable CODE_EN
    -- 3. read CODE_DATA
    -- 4. disable CODE_EN

    -- ================================== --

    yntel: process(CLK, RESET, EN, CODE_DATA, DATA_RDATA, IN_DATA, IN_VLD, OUT_BUSY, state, substate, instruction, tmp)
    begin
        if (RESET = '1') then
            state <= S_BEGIN;
            substate <= "000";
            instruction <= I_UNKNOWN;
            tmp <= (others => '0');
            pc_en <= '0';
            pc_add <= '1';
            ptr_en <= '0';
            ptr_add <= '1';
            CODE_EN <= '0';
            DATA_EN <= '0';
            OUT_WE <= '0';
            IN_REQ <= '0';
            DATA_WDATA <= (others => '0');
            DATA_RDWR <= '0';
            OUT_DATA <= (others => '0');
        else
            if (EN = '1' and CLK'event and CLK = '1') then
                case state is
                    when S_BEGIN =>
                        if (substate = "000") then
                            CODE_EN <= '1'; -- read first CODE_DATA instruction
                            pc_en <= '0';
                            substate <= "001";
                        else
                            CODE_EN <= '0';
                            substate <= "000";
                            state <= S_DECODE;
                        end if;
                    when S_DECODE =>
                        CODE_EN <= '0';
                        case CODE_DATA is
                            when "00111110" =>
                                instruction <= I_RMOVE;
                                state <= S_ACTION;
                            when "00111100" =>
                                instruction <= I_LMOVE;
                                state <= S_ACTION;
                            when "00101011" =>
                                instruction <= I_INCR;
                                state <= S_ACTION;
                            when "00101101" =>
                                instruction <= I_DECR;
                                state <= S_ACTION;
                            when "01011011" =>
                                instruction <= I_WHILE1;
                                state <= S_ACTION;
                            when "01011101" =>
                                instruction <= I_WHILE2;
                                state <= S_ACTION;
                            when "00101110" =>
                                instruction <= I_PRINT;
                                state <= S_ACTION;
                            when "00101100" =>
                                instruction <= I_READ;
                                state <= S_ACTION;
                            when "00100100" =>
                                instruction <= I_TMPTO;
                                state <= S_ACTION;
                            when "00100001" =>
                                instruction <= I_TMPFROM;
                                state <= S_ACTION;
                            when "00000000" =>
                                instruction <= I_END;
                                state <= S_ACTION;
                            when others =>
                                instruction <= I_UNKNOWN;
                                state <= S_NEXT;
                        end case;
                    when S_ACTION =>
                        case instruction is
                            when I_RMOVE =>
                                -- 0. increase ptr
                                -- N. NEXT
                                ptr_add <= '1';
                                ptr_en <= '1';
                                state <= S_NEXT;
                            when I_LMOVE =>
                                -- 0. decrease ptr
                                -- N. NEXT
                                ptr_add <= '0';
                                ptr_en <= '1';
                                state <= S_NEXT;
                            when I_INCR =>
                                -- 0. read to fill DATA_RDATA
                                -- 1. put increased to DATA_WDATA
                                -- N. write & NEXT
                                case substate is
                                    when "000" =>
                                        DATA_RDWR <= '1';
                                        DATA_EN <= '1';
                                        substate <= "001";
                                    when "001" =>
                                        DATA_WDATA <= DATA_RDATA + 1;
                                        DATA_RDWR <= '0';
                                        state <= S_NEXT;
                                    when others =>
                                        null;
                                end case;
                            when I_DECR =>
                                -- 0. read to fill DATA_RDATA
                                -- 1. put decreased to DATA_WDATA
                                -- N. write & NEXT
                                case substate is
                                    when "000" =>
                                        DATA_RDWR <= '1';
                                        DATA_EN <= '1';
                                        substate <= "001";
                                    when "001" =>
                                        DATA_WDATA <= DATA_RDATA - 1;
                                        DATA_RDWR <= '0';
                                        state <= S_NEXT;
                                    when others =>
                                        null;
                                end case;
                            when I_WHILE1 =>
                                -- 0. read to fill DATA_RDATA
                                -- 1. if RAM is zero goto 2 else goto N
                                -- 2. increase instr pointer
                                -- 3. read with CODE_EN
                                -- 4. check CODE_DATA for ], if ] goto N else goto 2
                                -- N. NEXT
                                case substate is
                                    when "000" =>
                                        DATA_RDWR <= '1';
                                        DATA_EN <= '1';
                                        substate <= "001";
                                    when "001" =>
                                        DATA_EN <= '0';
                                        if (DATA_RDATA = "00000000") then
                                            substate <= "010";
                                        else
                                            state <= S_NEXT;
                                        end if;
                                    when "010" =>
                                        pc_add <= '1';
                                        pc_en <= '1';
                                        substate <= "011";
                                    when "011" =>
                                        pc_en <= '0';
                                        CODE_EN <= '1';
                                        substate <= "100";
                                    when "100" =>
                                        CODE_EN <= '0';
                                        if (CODE_DATA = "01011101") then
                                            state <= S_NEXT;
                                        else
                                            substate <= "010";
                                        end if;
                                    when others =>
                                        null;
                                end case;
                            when I_WHILE2 =>
                                -- 0. read to fill DATA_RDATA
                                -- 1. if RAM is zero goto N else goto 2
                                -- 2. decrease instr pointer
                                -- 3. read with CODE_EN
                                -- 4. check CODE_DATA for [, if [ goto N else goto 2
                                -- N. NEXT
                                case substate is
                                    when "000" =>
                                        DATA_RDWR <= '1';
                                        DATA_EN <= '1';
                                        substate <= "001";
                                    when "001" =>
                                        DATA_EN <= '0';
                                        if (DATA_RDATA = "00000000") then
                                            state <= S_NEXT;
                                        else
                                            substate <= "010";
                                        end if;
                                    when "010" =>
                                        pc_add <= '0';
                                        pc_en <= '1';
                                        substate <= "011";
                                    when "011" =>
                                        pc_en <= '0';
                                        CODE_EN <= '1';
                                        substate <= "100";
                                    when "100" =>
                                        CODE_EN <= '0';
                                        if (CODE_DATA = "01011011") then
                                            state <= S_NEXT;
                                        else
                                            substate <= "010";
                                        end if;
                                    when others =>
                                        null;
                                end case;
                            when I_PRINT =>
                                -- 0. read to fill DATA_RDATA
                                -- 1. prepare OUT_DATA and try if not busy (if busy goto 3, else goto N)
                                -- 2. wait until not busy
                                -- N. print it & NEXT
                                case substate is
                                    when "000" =>
                                        DATA_RDWR <= '1';
                                        DATA_EN <= '1';
                                        substate <= "001";
                                    when "001" =>
                                        OUT_DATA <= DATA_RDATA;
                                        DATA_EN <= '0';
                                        if (OUT_BUSY = '0') then
                                            OUT_WE <= '1';
                                            state <= S_NEXT;
                                        else
                                            substate <= "010";
                                        end if;
                                    when "010" =>
                                        if (OUT_BUSY = '0') then
                                            OUT_WE <= '1';
                                            state <= S_NEXT;
                                        end if;
                                    when others =>
                                        null;
                                end case;
                            when I_READ =>
                                -- 0. enable IN_REQ to read
                                -- 1. wait until read something and then save it
                                -- N. NEXT
                                case substate is
                                    when "000" =>
                                        IN_REQ <= '1';
                                        substate <= "001";
                                    when "001" =>
                                        if (IN_VLD = '1') then
                                            DATA_WDATA <= IN_DATA;
                                            DATA_RDWR <= '0';
                                            DATA_EN <= '1';
                                            state <= S_NEXT;
                                        end if;
                                    when others =>
                                        null;
                                end case;
                            when I_TMPTO =>
                                -- 0. read to fill DATA_RDATA
                                -- 1. put read data in temp
                                -- N. NEXT
                                case substate is
                                    when "000" =>
                                        DATA_RDWR <= '1';
                                        DATA_EN <= '1';
                                        substate <= "001";
                                    when "001" =>
                                        tmp <= DATA_RDATA;
                                        DATA_EN <= '0';
                                        state <= S_NEXT;
                                    when others =>
                                        null;
                                end case;
                            when I_TMPFROM =>
                                -- 0. save temp
                                -- N. NEXT
                                DATA_RDWR <= '0';
                                DATA_WDATA <= tmp;
                                DATA_EN <= '1';
                                state <= S_NEXT;
                            when I_END =>
                                state <= S_END;
                            when others =>
                                state <= S_NEXT;
                        end case;
                    when S_NEXT =>
                        substate <= "000";
                        pc_add <= '1';
                        pc_en <= '1';
                        ptr_en <= '0';
                        DATA_EN <= '0';
                        OUT_WE <= '0';
                        IN_REQ <= '0';
                        state <= S_BEGIN;
                    when S_END =>
                        pc_en <= '0';
                        ptr_en <= '0';
                        CODE_EN <= '0';
                        DATA_EN <= '0';
                        OUT_WE <= '0';
                        IN_REQ <= '0';
                    when others =>
                        null;
                end case;
            end if;
        end if;


    end process;

end behavioral;
 
