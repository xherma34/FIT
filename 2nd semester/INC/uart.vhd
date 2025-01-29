-- uart.vhd: UART controller - receiving part
-- Author(s): xherma34
--
library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_unsigned.all;
-------------------------------------------------
entity UART_RX is
port(
    CLK       :  in std_logic;
    RST       :  in std_logic;
    DIN       :  in std_logic;
    DOUT      :  out std_logic_vector(7 downto 0);
    DOUT_VLD  :  out std_logic
);
end UART_RX;

-------------------------------------------------
architecture behavioral of UART_RX is

signal readBool  :  std_logic;
signal printer   :  std_logic;
signal counter   :  integer range 0 to 20; 
signal CMB       :  integer range 0 to 10; --countMsgBits
begin
  
    FSM: entity work.UART_FSM(behavioral)--propojeni portu s uart_fsm
    port map (
        CLK 	    => CLK,
        RST 	    => RST,
        DIN 	    => DIN,
        readBool   => readBool,
        printer   => printer
    );
  
 process(CLK, RST) begin 
    if rising_edge(CLK) then
      if printer = '1' then
        DOUT_VLD  <= '1';
        counter   <= 0;
        CMB <= 0;
      else 
        DOUT_VLD <= '0';
      end if; 
      
      if readBool = '1' then  --FSM ma nastaveny bool pro cteni na 1
          counter <= counter + 1; --pricita podle clocku
        if counter = 15 then
          counter <= 0;
          CMB     <= CMB + 1;
        end if;
        if counter = 0 then
          case CMB is
            when 0      => DOUT(0) <= DIN;
            when 1      => DOUT(1) <= DIN;
            when 2      => DOUT(2) <= DIN;
            when 3      => DOUT(3) <= DIN;
            when 4      => DOUT(4) <= DIN;
            when 5      => DOUT(5) <= DIN;
            when 6      => DOUT(6) <= DIN;
            when 7      => DOUT(7) <= DIN;
            when others => null; 
          end case;        
        end if;
      end if;
      
    end if;
  end process;
end behavioral;


