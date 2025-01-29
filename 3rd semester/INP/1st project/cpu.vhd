-- cpu.vhd: Simple 8-bit CPU (BrainLove interpreter)
-- Copyright (C) 2021 Brno University of Technology,
--                    Faculty of Information Technology
-- Author(s): DOPLNIT
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
   DATA_WDATA : out std_logic_vector(7 downto 0); -- ram[DATA_ADDR] <- DATA_WDATA pokud DATA_EN='1'
   DATA_RDATA : in std_logic_vector(7 downto 0);  -- DATA_RDATA <- ram[DATA_ADDR] pokud DATA_EN='1'
   DATA_WREN  : out std_logic;                    -- cteni z pameti (DATA_WREN='0') / zapis do pameti (DATA_WREN='1')
   DATA_EN    : out std_logic;                    -- povoleni cinnosti
   
   -- vstupni port
   IN_DATA   : in std_logic_vector(7 downto 0);   -- IN_DATA obsahuje stisknuty znak klavesnice pokud IN_VLD='1' a IN_REQ='1'
   IN_VLD    : in std_logic;                      -- data platna pokud IN_VLD='1'
   IN_REQ    : out std_logic;                     -- pozadavek na vstup dat z klavesnice
   
   -- vystupni port
   OUT_DATA : out  std_logic_vector(7 downto 0);  -- zapisovana data
   OUT_BUSY : in std_logic;                       -- pokud OUT_BUSY='1', LCD je zaneprazdnen, nelze zapisovat,  OUT_WREN musi byt '0'
   OUT_WREN : out std_logic                       -- LCD <- OUT_DATA pokud OUT_WE='1' a OUT_BUSY='0'
 );
end cpu;


-- ----------------------------------------------------------------------------
--                      Architecture declaration
-- ----------------------------------------------------------------------------
architecture behavioral of cpu is

-- --------------------------------STAVY----------------------------------
type states is (
	s_idle, 
	s_fetch, s_decode, 								--								nacteni/dekodovani instrukce
	s_ptr_inc, s_ptr_dec, 							--">"		"<"				inkrementace/dekrementace pointeru
	s_val_inc0, s_val_dec0, 						--"+" 	"-"				inkrementace/dekrementace hodnoty
	s_val_inc1, s_val_dec1,
	s_val_inc2, s_val_dec2,
	s_while_b0, s_while_e0, s_while_break0,	--"["		"]"	"~"		while begin/end/break
	s_while_b1, s_while_e1, s_while_break1,
	s_while_b2, s_while_e2, s_while_break2,
	s_while_b3, s_while_e3,
	s_print0, s_load0,								--"." 	","				putchar/getchar
	s_print1, s_load1,
	s_null,												--"null"						konec programu
	s_none												--""							vse ostatni
);
	signal curr_state : states;
	signal next_state : states;

-- ------------------------------KOMPONENTY-------------------------------
	-- PC: Programovy citac
	signal pc_inc : std_logic;
	signal pc_dec : std_logic;
	signal pc_reg : std_logic_vector(11 downto 0);  --CODE_ADDR(12b)
	
	-- CNT: Korektni urceni zacatku/konce prikazu while
	signal cnt_inc : std_logic;
	signal cnt_dec : std_logic;
	signal cnt_reg : std_logic_vector(11 downto 0);	
	
	-- PTR: Ukazatel do pameti 
	signal ptr_inc : std_logic;
	signal ptr_dec : std_logic;
	signal ptr_reg : std_logic_vector(9 downto 0);	--DATA_ADDR(10b)
	
	--MUX: 
	signal mux_sel 		: std_logic_vector(1 downto 0);
	signal mux_wr_data	: std_logic_vector(7 downto 0);


begin

-- ------------------------------PC-------------------------------
	pc_cntr: process(RESET, CLK, pc_inc, pc_dec)
	begin
		if(RESET = '1') then
			pc_reg <= (others => '0'); 
		elsif(CLK'event) and (CLK = '1') then
			if(pc_inc = '1') then
				pc_reg <= pc_reg + 1;  --inkrementace
			elsif(pc_dec = '1') then
				pc_reg <= pc_reg - 1;  --dekrementace
			end if;
		end if;
	end process;
		
	CODE_ADDR <= pc_reg; --Predam data z pc registru na ROM
		
-- ------------------------------CNT------------------------------
	cnt_cntr: process(RESET, CLK, cnt_inc, cnt_dec)
	begin
		if(RESET = '1') then
			cnt_reg <= (others => '0');
		elsif(CLK'event) and (CLK = '1') then
			if(cnt_inc = '1') then
				cnt_reg <= cnt_reg +1;	--inkrementace
			elsif(cnt_dec = '1') then
				cnt_reg <= cnt_reg -1;	--dekrementace
			end if;
		end if;
	end process;
	
-- ------------------------------PTR------------------------------
	ptr_cntr: process(RESET, CLK, ptr_inc, ptr_dec)
	begin
		if(RESET = '1') then
			ptr_reg <= (others => '0');
		elsif(CLK'event) and (CLK = '1') then
			if(ptr_inc = '1') then
				ptr_reg <= ptr_reg + 1;  --inkrementace
			elsif(ptr_dec = '1') then
				ptr_reg <= ptr_reg - 1;  --dekrementace
			end if;
		end if;
	end process;
	
	DATA_ADDR <= ptr_reg;
	
-- ------------------------------MUX-------------------------------

	OUT_DATA <= DATA_RDATA;
	
	mux: process(CLK, RESET, mux_sel)
	begin
		if(RESET = '1') then
			mux_wr_data <= (others => '0');
		elsif(CLK'event) and (CLK = '1') then
			case mux_sel is
				when "00" =>
					mux_wr_data <= IN_DATA;				-- Write hodnoty z IN_DATA
				when "01" =>
					mux_wr_data <= DATA_RDATA + 1;		-- Write hodnoty IN_DATA posunuty o 1
				when "10" =>
					mux_wr_data <= DATA_RDATA - 1;		-- Write hodnoty IN_DATA posunuty o -1
				when others =>
					mux_wr_data <= (others => '0');
				end case;
		end if;
	end process;
	
	DATA_WDATA <= mux_wr_data;
	
-- ------------------------------FSM-------------------------------
	fsm_currs_logic: process(RESET, CLK, EN)
	begin
		if(RESET = '1') then
			curr_state <= s_idle;						--Pri resetu nastavujeme pocatecni stav
		elsif(CLK'event) and (CLK = '1') then		--Pokud je nabezna hrana clocku a je povolena cinnost procesoru
			if(EN = '1') then								--tak nastavime do current statu next state
				curr_state <= next_state;
			end if;
		end if;
	end process;

	fsm_nexts_logic: process(OUT_BUSY, IN_VLD, CODE_DATA, cnt_reg, DATA_RDATA, curr_state)
	begin
		-- Inicializace -> vynulovani vsech vystupnich signalu 
		pc_inc 		<= '0';
		pc_dec 		<= '0';
		ptr_inc 		<= '0';
		ptr_dec		<= '0';
		cnt_inc		<= '0';
		cnt_dec		<= '0';
		IN_REQ		<= '0';
		OUT_WREN		<= '0';
		CODE_EN		<= '0';
		DATA_WREN	<= '0';
		DATA_EN		<= '0';
		mux_sel		<= "00";
		
		case curr_state is
			when s_idle =>
				next_state <= s_fetch;
				
			when s_fetch =>
				CODE_EN <= '1';
				DATA_EN <= '1';
				DATA_WREN <= '0';
				next_state <= s_decode;
				
			when s_decode =>
				case CODE_DATA is
					when X"3E" =>
						next_state <= s_ptr_inc;
					when X"3C" =>
						next_state <= s_ptr_dec;
					when X"2B" =>
						next_state <= s_val_inc0;
					when X"2D" =>
						next_state <= s_val_dec0;
					when X"2E" =>
						next_state <= s_print0;
					when X"2C" =>
						next_state <= s_load0;
					when X"5B" =>
						next_state <= s_while_b0;
					when X"5D" =>
						next_state <= s_while_e0;
					when X"7E" =>
						next_state <= s_while_break0;
					when X"00" =>
						next_state <= s_null;
					when others =>
						next_state <= s_none;
				end case;
			-- --------PTR--------
			when s_ptr_inc =>
				ptr_inc <= '1';
				pc_inc <= '1';
				next_state <= s_fetch;				
			when s_ptr_dec =>
				ptr_dec <= '1';
				pc_inc <= '1';
				next_state <= s_fetch;
			-- --------VAL INC--------
			when s_val_inc0 =>
				mux_sel <= "01";
				next_state <= s_val_inc1;
			when s_val_inc1 =>
				DATA_EN <= '1';
				DATA_WREN <= '1';
				pc_inc <= '1';
				next_state <= s_fetch;
			-- --------VAL DEC--------			
			when s_val_dec0 =>
				mux_sel <= "10";	
				next_state <= s_val_dec1;
			when s_val_dec1 =>
				DATA_EN <= '1';
				DATA_WREN <= '1';
				pc_inc <= '1';
				next_state <= s_fetch;
			-- --------PRINT--------
			when s_print0 =>
				if(OUT_BUSY = '1') then
					DATA_EN <= '1';
					DATA_WREN <= '0';
					next_state <= s_print1;
				else
					OUT_WREN <= '1';
					pc_inc <= '1';
					next_state <= s_fetch;
				end if;
			when s_print1 =>
				DATA_EN <= '1';
				DATA_WREN <= '0';
				next_state <= s_print0;
			-- --------LOAD------------
			when s_load0 =>
				IN_REQ <= '1';
				mux_sel <= "00";
				next_state <= s_load1;
			when s_load1 =>
				if (IN_VLD = '1') then
					DATA_EN <= '1';
					DATA_WREN <= '1';
					pc_inc <= '1';
					next_state <= s_fetch;
				else 
					next_state <= s_load0;
				end if;
			-- --------WHILE START------------
			when s_while_b0 =>
				pc_inc <= '1';
				next_state <= s_while_b1;
				
			when s_while_b1 =>
				if(DATA_RDATA = "00000000") then		-- Pokud je hodnota prvni bunky 0
					next_state <= s_while_b2;
				else
					next_state <= s_fetch;				-- Kdyz se prvni bunke != 0, tak zpracovavam instrukci
				end if;
				
			when s_while_b2 =>
					CODE_EN <= '1';						-- Zapnu CODE_EN, abych mohl v dalsim stavu rozeznavat instrukci
					next_state <= s_while_b3;
				
			when s_while_b3 =>
				if(CODE_DATA = X"5D") then				--Pokud narazim na ']' posunu se o instrukci dal a jdu do fetche 
					next_state <= s_fetch;				-- -> Skipnul jsem vsechny instrukce uvnitr cyklu a vysel z nej
					pc_inc <= '1';
				else
					next_state <= s_while_b1;			-- Pokud jsem nenarazil na ']' posouvam se v instrukcich dokud
				end if;										-- na nej nenarazim
					
			-- --------WHILE END------------			
			when s_while_e0 =>	-- 1->0
				if(DATA_RDATA = "00000000") then		--Jestlize jsem skoncil s poslednim chodem whilu a hodnota prvni bunky je 0
					pc_inc <= '1';							-- prictu pc (posunu se na dalsi instrukci) vyskocim z while cyklu a fetchuji instrukci
					next_state <= s_fetch;
				else 
					pc_dec <= '1';							-- Jestlize nejsem na konci smycky, posunu pc zpet
					next_state <= s_while_e1;	
				end if;
				
			when s_while_e1 =>					
				CODE_EN <= '1';							--Zapnu CODE_EN abych mohl v dalsim stavu rozeznavat instrukci
				next_state <= s_while_e2;	
			
			when s_while_e2 =>					
				if(CODE_DATA = X"5B") then				--Pokud jsem nasel "[" posunu se na prvni instrukci v cyklu
					pc_inc <= '1';							
					next_state <= s_fetch;				--Instrukci posilam do fetch
				else
					next_state <= s_while_e0;			--Jestlize jsem jeste nedosel na "[" opakuji proces, dokud
				end if;										-- na ni nenarazim
					
			-- --------WHILE BREAK------------	
			when s_while_break0 =>
				pc_inc <= '1';
				CODE_EN <= '1';							--Zapnu CODE_EN abych mohl v dalsim stavu rozeznavat instrukci
				next_state <= s_while_break1;
				
			when s_while_break1 =>	
				if(CODE_DATA = X"5D") then				--Pokud jsem narazil na "]" tak jdu do fetche
					next_state <= s_fetch;
				else
					next_state <= s_while_break2;		--Pokud jsem jeste nenarazil na "]" jdu do dalsiho stavu, kde se posunu o instrukci a 
				end if;										-- vracim se zpet, dokud nenactu "]"
			
			when s_while_break2 =>
				CODE_EN <= '1';
				pc_inc <= '1';
				next_state <= s_while_break1;
				
			-- ------NULL------
			when s_null =>
				next_state <= s_null;
				
			-- -------OSTATNI ZNAKY-----
			when s_none =>
				pc_inc <= '1';
				next_state <= s_fetch;
			when others =>
				next_state <= s_null;
		end case;
	end process;


end behavioral;
 

