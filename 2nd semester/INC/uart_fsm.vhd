-- uart_fsm.vhd: UART controller - finite state machine
-- Author(s): xherma34
--
library ieee;
use ieee.std_logic_1164.all;
-------------------------------------------------
entity UART_FSM is
port(
   CLK      : in std_logic;
   RST      : in std_logic;
   DIN      : in std_logic;
   readBool : out std_logic;
   printer  : out std_logic
   );
end entity UART_FSM;

-------------------------------------------------
architecture behavioral of UART_FSM is
signal counter   : integer range 0 to 24;  --Counter pro orientaci podle clocku
signal CMB       : integer range 0 to 8;   --CountMsgBits

type state_type is (CATCH_START_BIT, AT_START_BIT, GET_DATA, CATCH_STOP_BIT, VALID_DATA); 
signal state: state_type := CATCH_START_BIT; 
begin 

  process(CLK, RST) begin 
    
    if RST = '1' then --reset state
      state <= CATCH_START_BIT;
      readBool <= '0';
      counter <= 0;
      CMB <= 0;
      printer <= '0';
    elsif rising_edge(CLK) then
      
      if state = GET_DATA then --Get data stav
        readBool <= '1';
      else
        readBool <= '0';
      end if; 
      
      if state = CATCH_START_BIT then --Stav kdy cekam nez chytim prvni bit
        if DIN = '0' then
          state <= AT_START_BIT;
          counter <= counter + 1;
        end if;
      end if;
        
      if state = AT_START_BIT then --Stav kdy jsem chytil prvni bit 
        counter <= counter + 1;
        if counter = 23 then 
          state <= GET_DATA;
          counter <= 0;
        end if;
      end if;
      
      if state = GET_DATA then --stav kdy prijmam data
        counter <= counter + 1;
        if counter = 15 then
          counter <= 0;
          CMB <= CMB + 1;
        end if;
        if CMB = 7 then       
          CMB <= 0;
          state <= CATCH_STOP_BIT;
          counter <= 0;
        end if;
      end if;
      
      if state = CATCH_STOP_BIT then --Stav kdy chytam stop bit
        counter <= counter + 1;
          if counter = 8 then
            state <= VALID_DATA;
          end if; 
      end if;
      
      if state = VALID_DATA then --Stav kdy davam vedet, ze se ma zacit print slova
        state <= CATCH_START_BIT;
        counter <= 0;
        CMB <= 0;
        printer <= '1';
      else 
        printer <= '0';
      end if;
      
    end if;
  end process;
end behavioral;
